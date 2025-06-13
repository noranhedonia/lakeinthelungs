#include "internal.h"

struct bedrock *g_bedrock = nullptr;

#define HEAP_BLOCK_SIZE (256lu*1024)

static LAKE_NORETURN void LAKECALL d4c_love_train(void *stub)
{
    (void)stub;
    struct tls *tls = get_thread_local_storage();
    struct fiber *old_fiber = &g_bedrock->fibers[tls->fiber_in_use];

    if (tls == &g_bedrock->tls[0]) 
        for (s32 i = 1; i < g_bedrock->thread_count; sys_thread_join(g_bedrock->threads[i++]));

    tls->fiber_old = tls->fiber_in_use | tls_to_free;
    jump_fiber_context(tls, &old_fiber->context, &tls->home_context);
    LAKE_UNREACHABLE;
}

struct application {
    PFN_lake_framework  main;
    void               *argument;
    lake_framework     *framework;
};

static LAKE_NORETURN void LAKECALL funny_valentine(void *raw_app)
{
    struct application *app = (struct application *)raw_app;
    app->main(app->argument, app->framework);

    /* tell all threads to die, type shit */
    for (s32 i = 0; i < g_bedrock->thread_count; i++) {
        g_bedrock->ends[i].procedure = d4c_love_train;
        g_bedrock->ends[i].argument = nullptr;
        g_bedrock->ends[i].name = "bedrock/ends";
    }
    lake_submit_work_and_yield(g_bedrock->thread_count, g_bedrock->ends);
    LAKE_UNREACHABLE;
}

static void bedrock_init(lake_framework *framework)
{
    usize ram_budget, page_size, huge_page_size = 0;
    sys_meminfo(&ram_budget, &page_size);
    if (framework->hints.memory_budget == 0)
        framework->hints.memory_budget = lake_align(((ram_budget) & ~(HEAP_BLOCK_SIZE - 1)), 8lu * HEAP_BLOCK_SIZE);

    s32 cpu_count = 0;
    sys_cpuinfo(&cpu_count, nullptr, nullptr);
    if (framework->hints.worker_thread_count == 0 || framework->hints.worker_thread_count > (u32)cpu_count)
        framework->hints.worker_thread_count = cpu_count;

    if (framework->hints.huge_page_size == 0)
        framework->hints.huge_page_size = 4lu*1024*1024;
    sys_hugetlbinfo(&huge_page_size, framework->hints.huge_page_size);
    if (huge_page_size != framework->hints.huge_page_size)
        framework->hints.huge_page_size = huge_page_size;

    if (framework->hints.tagged_heap_count == 0)
        framework->hints.tagged_heap_count = 32;
    if (framework->hints.fiber_stack_size == 0)
        framework->hints.fiber_stack_size = 64lu * 1024;
    if (framework->hints.fiber_count == 0)
        framework->hints.fiber_count = 96 + 4 * framework->hints.worker_thread_count;
    if (framework->hints.log2_work_count == 0)
        framework->hints.log2_work_count = 11; /* 2048 */
    if (framework->hints.frames_in_flight < 2)
        framework->hints.frames_in_flight = 2;
    framework->timer_start = lake_rtc_counter();

    work_queue_node *work_nodes = nullptr;
    struct region *roots_pages = nullptr;
    usize const roots_page_count = 8;

    usize const bedrock_bytes           = lake_align(sizeof(struct bedrock), LAKE_CACHELINE_SIZE);
    usize const work_count              = 1lu << framework->hints.log2_work_count;
    usize const work_nodes_bytes        = lake_align(sizeof(work_queue_node) * work_count, 16);
    usize const roots_pages_bytes       = lake_align(sizeof(struct region) * roots_page_count, 16);
    usize const tls_bytes               = lake_align(sizeof(struct tls) * framework->hints.worker_thread_count, 16);
    usize const ends_bytes              = lake_align(sizeof(lake_work_details) * framework->hints.worker_thread_count, 16);
    usize const threads_bytes           = lake_align(sizeof(sys_thread_id) * framework->hints.worker_thread_count, 16);
    usize const fibers_bytes            = lake_align(sizeof(struct fiber) * framework->hints.fiber_count, 16);
    usize const waiting_bytes           = lake_align(sizeof(atomic_usize) * framework->hints.fiber_count, 16);
    usize const free_bytes              = lake_align(sizeof(atomic_usize) * framework->hints.fiber_count, 16);
    usize const locks_bytes             = lake_align(sizeof(atomic_usize) * framework->hints.fiber_count, 16);
    usize const heap_bytes              = lake_align(sizeof(struct tagged_heap), 16);
    usize const tagged_heap_bytes       = heap_bytes * framework->hints.tagged_heap_count;
    usize const tagged_heap_array_bytes = lake_align(sizeof(struct tagged_heap *) * framework->hints.tagged_heap_count, 16);
    usize const block_count             = __position_from_block(framework->hints.memory_budget); 
    usize const heap_bitmap_bytes       = lake_align(__index_from_position(block_count), 16);
    usize const stack_bytes             = lake_align(framework->hints.fiber_stack_size, 16);
    usize const stack_heap_bytes        = stack_bytes * framework->hints.fiber_count;

    usize const roots_bytes = 
        bedrock_bytes +
        work_nodes_bytes +
        roots_pages_bytes +
        tls_bytes +
        ends_bytes +
        threads_bytes +
        fibers_bytes +
        waiting_bytes +
        free_bytes +
        locks_bytes +
        tagged_heap_bytes +
        tagged_heap_array_bytes +
        heap_bitmap_bytes +
        stack_heap_bytes;
    usize const roots_block_aligned = lake_align(roots_bytes, LAKE_TAGGED_HEAP_BLOCK_SIZE);
    usize const commitment = lake_min(lake_align(roots_block_aligned, framework->hints.huge_page_size), framework->hints.memory_budget);

    g_bedrock = sys_mmap(framework->hints.memory_budget, framework->hints.huge_page_size);
    if (g_bedrock == nullptr || !sys_madvise(g_bedrock, 0u, commitment, true)) {
        lake_fatal("Can't map internal framework memory.");
        lake_abort(LAKE_ERROR_MEMORY_MAP_FAILED);
    }
    lake_memset(g_bedrock, 0u, commitment);

    g_bedrock->thread_count = framework->hints.worker_thread_count;
    g_bedrock->fiber_count = framework->hints.fiber_count;
    g_bedrock->tagged_heap_count = framework->hints.tagged_heap_count;
    g_bedrock->budget = framework->hints.memory_budget;
    g_bedrock->page_size = framework->hints.huge_page_size;
    lake_atomic_init(&g_bedrock->commitment, commitment);
    g_bedrock->stack_size = stack_bytes;

    u8 *raw = (u8 *)g_bedrock;
    usize o = bedrock_bytes;

    work_nodes = (work_queue_node *)&raw[o]; 
    o += work_nodes_bytes;
    roots_pages = (struct region *)&raw[o]; 
    o += roots_pages_bytes;
    g_bedrock->tls = (struct tls *)&raw[o]; 
    o += tls_bytes;
    g_bedrock->ends = (lake_work_details *)&raw[o]; 
    o += ends_bytes;
    g_bedrock->threads = (sys_thread_id *)&raw[o]; 
    o += threads_bytes;
    g_bedrock->fibers = (struct fiber *)&raw[o]; 
    o += fibers_bytes;
    g_bedrock->waiting = (atomic_usize *)&raw[o]; 
    o += waiting_bytes;
    g_bedrock->free = (atomic_usize *)&raw[o]; 
    o += free_bytes;
    g_bedrock->locks = (atomic_usize *)&raw[o]; 
    o += locks_bytes;
    g_bedrock->tagged_heaps = (struct tagged_heap **)&raw[o];
    o += tagged_heap_array_bytes;
    for (s32 i = 0; i < g_bedrock->tagged_heap_count; i++) {
        g_bedrock->tagged_heaps[i] = (struct tagged_heap *)&raw[o];
        o += heap_bytes;
    }
    g_bedrock->heap_bitmap = (atomic_u8 *)&raw[o];
    o += heap_bitmap_bytes;
    g_bedrock->stack = (u8 *)&raw[o];
    o += stack_heap_bytes;

    /* bits set to 1 means free blocks */
    usize const roots_bitmap_end = __index_from_range(roots_block_aligned);
    lake_memset((void *)&g_bedrock->heap_bitmap[roots_bitmap_end], 0xff, heap_bitmap_bytes);

    g_bedrock->roots.tail = &g_bedrock->roots.head;
    for (u32 i = 0; i < roots_page_count; i++)
        g_bedrock->roots.tail = g_bedrock->roots.tail->next = &roots_pages[i];
    g_bedrock->roots.head.v = 0lu;
    g_bedrock->roots.head.offset = roots_bytes;
    g_bedrock->roots.head.alloc = roots_block_aligned;
    g_bedrock->roots.tail = &g_bedrock->roots.head;

    lake_dbg_assert(!(((sptr)work_nodes)                & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)roots_pages)               & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->tls)            & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->ends)           & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->threads)        & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->fibers)         & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->waiting)        & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->free)           & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->locks)          & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->tagged_heaps)   & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->heap_bitmap)    & 15), LAKE_PANIC, nullptr);
    lake_dbg_assert(!(((sptr)g_bedrock->stack)          & 15), LAKE_PANIC, nullptr);

    lake_mpmc_init_t(&g_bedrock->work_queue.ring, work_queue_node, work_count, work_nodes);

    for (s32 i = 0; i < g_bedrock->fiber_count; i++) {
        lake_atomic_init(&g_bedrock->free[i], i);
        lake_atomic_init(&g_bedrock->locks[i], FIBER_INVALID);
        lake_atomic_init(&g_bedrock->waiting[i], FIBER_INVALID);
    }
    g_bedrock->tls[0].fiber_in_use = (u32)get_free_fiber();
#if defined(LAKE_PLATFORM_UNIX)
    g_bedrock->threads[0] = (sys_thread_id)pthread_self();
#elif defined(LAKE_PLATFORM_WINDOWS)
    g_bedrock->threads[0] = (sys_thread_id)GetCurrentThreadId();
#endif /* LAKE_PLATFORM_UNIX */

    lake_atomic_write_explicit(&g_bedrock->tls_sync, 0lu, lake_memory_model_release);
    for (s32 i = 1; i < g_bedrock->thread_count; i++) {
        struct tls *tls = &g_bedrock->tls[i];
        tls->fiber_in_use = (u32)FIBER_INVALID;
        sys_thread_create(&g_bedrock->threads[i], dirty_deeds_done_dirt_cheap, (void *)tls);
    }
    sys_thread_affinity(g_bedrock->thread_count, g_bedrock->threads, cpu_count, 0);
    lake_atomic_write_explicit(&g_bedrock->tls_sync, 1lu, lake_memory_model_release);
}

s32 lake_in_the_lungs(
    PFN_lake_framework main, 
    void              *userdata,
    lake_framework    *framework)
{
    bedrock_init(framework);

    struct application app = {
        .main = main,
        .argument = userdata,
        .framework = framework,
    };
    lake_work_details work = {
        .procedure = funny_valentine,
        .argument = (void *)&app,
        .name = "lake/in_the_lungs",
    };

    lake_submit_work(1, &work, nullptr);
    dirty_deeds_done_dirt_cheap((void *)&g_bedrock->tls[0]);
    /* won't resume until the application returns */

    sys_munmap(g_bedrock, g_bedrock->budget);
    g_bedrock = nullptr;

    /* returns the last recorded error code and resets the internal status value */
    return lake_exit_status(LAKE_SUCCESS);
}
