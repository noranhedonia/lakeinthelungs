#include "internal.h"

u32 lake_worker_thread_index(void)
{
    lake_san_assert(g_bedrock != nullptr, LAKE_FRAMEWORK_REQUIRED, nullptr);

    sys_thread_id self;
    /* TODO implement a hashmap lookup for the worker thread index */
#if defined(LAKE_PLATFORM_UNIX)
    self = (sys_thread_id)pthread_self();
    for (s32 i = 0; i < g_bedrock->thread_count; i++)
        if (pthread_equal(g_bedrock->threads[i], self)) return i;
#elif defined(LAKE_PLATFORM_WINDOWS)
    self = (sys_thread_id)GetCurrentThreadId();
    for (s32 i = 0; i < g_bedrock->thread_count; i++)
        if (self == g_bedrock->threads[i]) return i;
#endif /* LAKE_PLATFORM_UNIX */
    return 0;
}

char const *lake_fiber_name(void)
{
    struct tls *tls = get_thread_local_storage();
    return g_bedrock->fibers[tls->fiber_in_use].work.details.name;
}

/* XXX a lot of false sharing around index 0 is expected, there probably should be a better
 * way to acquire a free fiber than this primitive method - but it works so i don't care now */
extern usize get_free_fiber(void)
{
    for (s32 i = 0; i < g_bedrock->fiber_count; i++) {
        /* double lock helps CPUs that have a weak memory model, ARM should 
         * go 2x to 3x faster using double lock, if it is the bottleneck */
        usize fiber_idx = lake_atomic_read_explicit(&g_bedrock->free[i], lake_memory_model_relaxed);
        if (fiber_idx == FIBER_INVALID) continue;

        fiber_idx = lake_atomic_read_explicit(&g_bedrock->free[i], lake_memory_model_acquire);
        if (fiber_idx == FIBER_INVALID) continue;

        usize expected = fiber_idx;
        if (lake_atomic_compare_exchange_weak_explicit(&g_bedrock->free[i], &expected, 
                FIBER_INVALID, lake_memory_model_release, lake_memory_model_relaxed))
        {
            return fiber_idx;
        }
    }
    return FIBER_INVALID;
}

static void update_free_and_waiting(struct tls *tls)
{
    if (tls->fiber_old == (u32)FIBER_INVALID) return;

    usize const fiber_idx = tls->fiber_old & tls_mask;

    /* a thread that added the fiber to the free list is the same as the one freeing it */
    if (tls->fiber_old & tls_to_free)
        lake_atomic_write_explicit(&g_bedrock->free[fiber_idx], fiber_idx, lake_memory_model_relaxed);

    /* wait threshold needs to be thread synced, so a CPU fence is welcome */
    if (tls->fiber_old & tls_to_wait)
        lake_atomic_write_explicit(&g_bedrock->waiting[fiber_idx], fiber_idx, lake_memory_model_release);

    tls->fiber_old = (u32)FIBER_INVALID;
}

static LAKE_NORETURN void LAKECALL the_work(sptr raw_tls);

static usize acquire_next_fiber(void)
{
    usize fiber_idx = FIBER_INVALID;

    for (s32 i = 0; i < g_bedrock->fiber_count; i++) {
        /* double lock helps CPUs that have a weak memory model, ARM should 
         * go 2x to 3x faster using double lock, if it is the bottleneck */
        usize fiber_waiting = lake_atomic_read_explicit(&g_bedrock->waiting[i], lake_memory_model_relaxed);
        if (fiber_waiting == FIBER_INVALID) continue;

        fiber_waiting = lake_atomic_read_explicit(&g_bedrock->waiting[i], lake_memory_model_acquire);
        if (fiber_waiting == FIBER_INVALID) continue;

        struct fiber *fiber = &g_bedrock->fibers[fiber_waiting];
        s32 finished = 1;
        atomic_usize *counter = fiber->wait_counter;

        if (counter) {
            usize left = lake_atomic_read(counter);
            finished = (!left);
        }
        if (!finished) continue;

        usize expected = fiber_waiting;
        if (lake_atomic_compare_exchange_weak_explicit(&g_bedrock->waiting[i], &expected,
                FIBER_INVALID, lake_memory_model_release, lake_memory_model_relaxed))
        {
            fiber_idx = i;
            break;
        }
    }
    if (fiber_idx == FIBER_INVALID) {
        struct work data;
        if (lake_mpmc_dequeue_t(&g_bedrock->work_queue.ring, work_queue_node, &data)) {
            while (fiber_idx == FIBER_INVALID)
                fiber_idx = get_free_fiber();

            struct fiber *fiber = &g_bedrock->fibers[fiber_idx];
            fiber->work = data;

            /* make_fcontext requires the top of the stack, as it grows downwards */
            u8 *stack = &g_bedrock->stack[(fiber_idx + 1) * g_bedrock->stack_size];
            make_fiber_context(&fiber->context, the_work, stack, g_bedrock->stack_size);
        }
    }
    return fiber_idx;
}

struct tls *fiber_search(struct tls *tls, fcontext *context)
{
    struct fiber *old = nullptr;
    atomic_usize *wait_counter = nullptr;
    
    if ((tls->fiber_old != (u32)FIBER_INVALID) && (tls->fiber_old & tls_to_wait)) {
        usize const fiber_idx = tls->fiber_old & tls_mask;
        old = &g_bedrock->fibers[fiber_idx];
        wait_counter = old->wait_counter;
    }

    for (;;) {
        usize fiber_idx = acquire_next_fiber();

        if (fiber_idx != FIBER_INVALID) {
            struct fiber *fiber = &g_bedrock->fibers[fiber_idx];
            tls->fiber_in_use = (u32)fiber_idx;

            /* inherit information from the last fiber */
            if (old != nullptr) {
                if (old->drifter.head) 
                    fiber->drifter = old->drifter;
                fiber->logger.depth = 1 + old->logger.depth;
                if (old->logger.should_flush)
                    flush_logger(&old->logger);
                fiber->logger.buf = old->logger.buf;
            }
            return (struct tls *)jump_fiber_context(tls, context, &fiber->context);
        }

        /* Race condition fix.
         *
         * Context needs to wait until a set of jobs are done. The jobs finish *before* 
         * a new context to swap to is found - there's no new jobs. The context swap code 
         * deadlocks looking for a new job to swap to, when no jobs may arrive. Meanwhile
         * the "to be swapped" context is waiting to be run, but cannot as it hasn't been 
         * swapped out yet (in order to be picked up by the wait list). */
        if (wait_counter) {
            usize count = lake_atomic_read_explicit(wait_counter, lake_memory_model_relaxed);

            if (!count) {
                /* variable `tls->fiber_in_use` still points to the "to waitlist" fiber */
                tls->fiber_old = (u32)FIBER_INVALID;
                return tls;
            }
        }
    }
    LAKE_UNREACHABLE;
}

void *LAKECALL dirty_deeds_done_dirt_cheap(void *raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;

    /* we need to wait for the main thread to be ready */
    while (!lake_atomic_read_explicit(&g_bedrock->tls_sync, lake_memory_model_acquire)){/* spin */};

    tls->fiber_old = (u32)FIBER_INVALID;
    tls = fiber_search(tls, &tls->home_context);

    update_free_and_waiting(tls);
    return nullptr;
}

static LAKE_NORETURN void LAKECALL the_work(sptr raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;

    update_free_and_waiting(tls);
    struct fiber *fiber = &g_bedrock->fibers[tls->fiber_in_use];

    fiber->cursor.tail = fiber->drifter.tail_page;
    fiber->cursor.offset = fiber->cursor.tail ? fiber->drifter.tail_page->offset : 0lu;
    fiber->cursor.prev = fiber->drifter.tail_cursor;
    fiber->drifter.tail_cursor = &fiber->cursor;
    fiber->logger.buf = (lake_strbuf){0};
    fiber->logger.tail_cursor = nullptr;
    fiber->logger.should_flush = false;

    for (;;) { /* do the work */
        fiber->work.details.procedure(fiber->work.details.argument);

        if (fiber->logger.should_flush) 
            flush_logger(&fiber->logger);
        /* release unnecessary resources */
        if (fiber->drifter.head != nullptr) {
            for (struct region *page = fiber->drifter.tail_page->next; page != nullptr; page = page->next)
                if (page->alloc) release_heap_bitmap(g_bedrock->bitmap, page->v, page->alloc);
            if (fiber->cursor.tail) {
                fiber->drifter.tail_page = fiber->cursor.tail;
                fiber->drifter.tail_page->offset = fiber->cursor.offset;
            }
            fiber->drifter.tail_page->next = nullptr;
        }

        /* decrement the chain */
        if (fiber->work.work_left) {
            usize last = lake_atomic_sub(fiber->work.work_left, 1lu);
            lake_dbg_assert(last > 0, LAKE_PANIC, nullptr);

            /* try to reuse the fiber */
            if (last > 1 && lake_mpmc_dequeue_t(
                    &g_bedrock->work_queue.ring, 
                    work_queue_node, 
                    &fiber->work)) 
                continue;
        }
        fiber->drifter.tail_cursor = fiber->cursor.prev;
        /* if we own the drifter, destroy it */
        if (fiber->drifter.tail_cursor == nullptr && fiber->drifter.head) {
            struct region *page = fiber->drifter.head;
            release_heap_bitmap(g_bedrock->bitmap, page->v, page->alloc);
            fiber->drifter = (struct drifter){0};
        }

        tls = get_thread_local_storage();
        struct fiber *old = &g_bedrock->fibers[tls->fiber_in_use];
        tls->fiber_old = tls->fiber_in_use | tls_to_free;

        /* yield the fiber */
        fiber_search(tls, &old->context);
        LAKE_UNREACHABLE;
    }
    LAKE_UNREACHABLE;
}

lake_work_chain lake_acquire_chain_n(usize initial_value)
{
    lake_san_assert(g_bedrock != nullptr, LAKE_FRAMEWORK_REQUIRED, nullptr);
    for (;;) {
        /* as with acquiring the fibers, this method of traversing through the locks 
         * is primitive, it may be a point of optimization to revisit this later */
        for (s32 i = 0; i < g_bedrock->fiber_count; i++) {
            atomic_usize *lock = &g_bedrock->locks[i];

            if (lake_atomic_read(lock) == FIBER_INVALID) {
                usize expected = FIBER_INVALID;

                if (lake_atomic_compare_exchange_weak_explicit(lock, &expected,
                        initial_value, lake_memory_model_relaxed, lake_memory_model_relaxed))
                {
                    return (lake_work_chain)lock;
                }
            }
        }
    }
    LAKE_UNREACHABLE;
}

void lake_submit_work(
    u32                      work_count, 
    lake_work_details const *work, 
    lake_work_chain         *out_chain)
{
    atomic_usize *to_use = nullptr;
    lake_san_assert(g_bedrock != nullptr, LAKE_FRAMEWORK_REQUIRED, nullptr);

    if (out_chain) {
        *out_chain = lake_acquire_chain_n(work_count);
        to_use = (atomic_usize *)*out_chain;
    }

    for (u32 i = 0; i < work_count; i++) {
        struct work submit = { .details = work[i], .work_left = to_use };

        while (!lake_mpmc_enqueue_t(&g_bedrock->work_queue.ring, work_queue_node, &submit)) {
            lake_dbg_assert(false, LAKE_ERROR_OUT_OF_RANGE, 
                "Failed to submit work into the work queue at: %u/%u.", i, work_count);
        }
    }
}

void lake_yield(lake_work_chain chain)
{
    usize wait_value = 0;
    lake_san_assert(g_bedrock != nullptr, LAKE_FRAMEWORK_REQUIRED, nullptr);

    if (chain) {
        wait_value = lake_atomic_read(chain);
        lake_dbg_assert(wait_value != FIBER_INVALID, LAKE_ERROR_OUT_OF_DATE, "The work chain has expired.");
    }
    if (wait_value) {
        struct tls *tls = get_thread_local_storage();
        struct fiber *old = &g_bedrock->fibers[tls->fiber_in_use];
        if (old->logger.should_flush)
            flush_logger(&old->logger);

        old->wait_counter = chain;
        tls->fiber_old = tls->fiber_in_use | tls_to_wait;
        tls = fiber_search(tls, &old->context);
        update_free_and_waiting(tls);
    }
    if (chain) lake_atomic_write_explicit(chain, FIBER_INVALID, lake_memory_model_release);
}

static struct region *construct_drift_region(usize const block_aligned)
{
    u8 *raw = (u8 *)g_bedrock;
    usize block = acquire_blocks(block_aligned);
    lake_san_assert(block != 0lu, LAKE_ERROR_OUT_OF_HOST_MEMORY, nullptr);

    struct region *out = (struct region *)(void *)(uptr)(raw + block);
    out->v = block;
    out->offset = sizeof(struct region);
    out->alloc = block_aligned;
    out->next = nullptr;
    return out;
}

LAKE_HOT_FN LAKE_MALLOC
static void *LAKECALL drift_allocation(struct drifter *d, usize size, usize align)
{
    u8 *raw = (u8 *)g_bedrock;
    struct region *tail = d->tail_page;

    if (lake_unlikely(tail == nullptr)) {
        usize const block_aligned = lake_align(size + sizeof(struct region), LAKE_TAGGED_HEAP_BLOCK_SIZE);
        d->tail_page = d->head = tail = construct_drift_region(block_aligned);
        tail->offset += size;
        return (void *)(uptr)(raw + tail->v + sizeof(struct region));
    }
    lake_dbg_assert(tail->alloc > 0 && tail->v > 0, LAKE_PANIC, nullptr);

    usize aligned = lake_align(tail->offset, align);
    if (aligned + size > tail->alloc) {
        usize const block_aligned = lake_align(size + sizeof(struct region), LAKE_TAGGED_HEAP_BLOCK_SIZE);
        d->tail_page = d->tail_page->next = tail = construct_drift_region(block_aligned);
        tail->offset += size;
        return (void *)(uptr)(raw + tail->v + sizeof(struct region));
    }
    tail->offset = aligned + size;
    return (void *)(uptr)(raw + tail->v + aligned);
}

void *lake_drift(usize size, usize align)
{
    struct tls *tls = get_thread_local_storage();
    struct fiber *f = &g_bedrock->fibers[tls->fiber_in_use];
    struct drifter *d = &f->drifter;
    return drift_allocation(d, size, align);
}

void *lake_drift_alias(usize size, usize align)
{
    struct tls *tls = get_thread_local_storage();
    struct fiber *f = &g_bedrock->fibers[tls->fiber_in_use];
    struct drifter *d = &f->drifter;
    struct region *tail = d->tail_page;
    usize offset = 0;
    void *ret = drift_allocation(d, size, align); 

    if (lake_likely(tail != nullptr))
        offset = d->tail_page->offset;
    if (lake_likely(!tail || tail == d->tail_page)) {
        d->tail_page->offset = offset;
    } else {
        d->tail_page->offset = 0;
    }
    return ret;
}

void lake_drift_depth_op(s32 depth)
{
    struct tls *tls = get_thread_local_storage();
    struct fiber *f = &g_bedrock->fibers[tls->fiber_in_use];
    struct drifter *d = &f->drifter;

    if (depth == __lake_drift_depth_op_entry__) {
        struct drifter_cursor *cursor = (struct drifter_cursor *)
            drift_allocation(d, sizeof(struct drifter_cursor), alignof(struct drifter_cursor));
        cursor->tail = d->tail_page;
        cursor->prev = d->tail_cursor;
        cursor->offset = d->tail_page->offset;
        d->tail_cursor = cursor;
    } else if (depth == __lake_drift_depth_op_leave__) {
        struct drifter_cursor *cursor = d->tail_cursor;

        d->tail_cursor = cursor->prev;
        d->tail_page = cursor->tail;
        d->tail_page->offset = cursor->offset;

        if (f->logger.should_flush) 
            flush_logger(&f->logger);
        if (f->logger.tail_cursor == cursor) {
            f->logger.tail_cursor = nullptr;
            f->logger.buf = (lake_strbuf){0};
        }
        for (struct region *page = cursor->tail->next; page != nullptr; page = page->next)
            if (page->alloc) release_heap_bitmap(g_bedrock->bitmap, page->v, page->alloc);
        d->tail_page->next = nullptr;
#ifndef LAKE_NDEBUG
    } else {
        lake_debugtrap();
#endif /* LAKE_NDEBUG */
    }
}
