#include "bedrock_impl.h"

static struct region *LAKECALL construct_drift_region(usize const block_aligned)
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
static void *LAKECALL drifter_allocation(struct drifter *d, usize size, usize align)
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

void *lake_drift_allocate(usize size, usize align)
{
    struct tls *tls = get_thread_local_storage();
    struct fiber *f = &g_bedrock->fibers[tls->fiber_in_use];
    struct drifter *d = &f->drifter;
    return drifter_allocation(d, size, align);
}

void *lake_drift_alias(usize size, usize align)
{
    struct tls *tls = get_thread_local_storage();
    struct fiber *f = &g_bedrock->fibers[tls->fiber_in_use];
    struct drifter *d = &f->drifter;
    struct region *tail = d->tail_page;
    usize offset = 0;
    void *ret = drifter_allocation(d, size, align); 

    if (lake_likely(tail != nullptr))
        offset = d->tail_page->offset;
    if (lake_likely(!tail || tail == d->tail_page)) {
        d->tail_page->offset = offset;
    } else {
        d->tail_page->offset = 0;
    }
    return ret;
}

void _lake_drift_scratch(s32 depth)
{
    struct tls *tls = get_thread_local_storage();
    struct fiber *f = &g_bedrock->fibers[tls->fiber_in_use];
    struct drifter *d = &f->drifter;

    if (depth == _lake_drift_scratch_push) {
        struct drifter_cursor *cursor = (struct drifter_cursor *)
            drifter_allocation(d, sizeof(struct drifter_cursor), alignof(struct drifter_cursor));
        cursor->tail = d->tail_page;
        cursor->prev = d->tail_cursor;
        cursor->offset = d->tail_page->offset;
        d->tail_cursor = cursor;
    } else if (depth == _lake_drift_scratch_pop) {
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
