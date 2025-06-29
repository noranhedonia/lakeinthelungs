#include <lake/data_structures/arena_allocator.h>

lake_arena_page *lake_arena_page_new(s32 page_size) {
    usize actual_size = sizeof(lake_arena_page) + (usize)page_size;
    u8 *raw = __lake_malloc(actual_size, 32);
    lake_arena_page *p = (lake_arena_page *)&raw[page_size];
    *p = (lake_arena_page){ .v = raw, .alloc = page_size };
    return p;
}

void lake_arena_fini(lake_arena_allocator *a)
{
    lake_arena_page *p = a->head;
    while (p) {
        lake_arena_page *next = p->next;
        __lake_free(p);
        p = next;
    }
    *a = (lake_arena_allocator){0};
}

void lake_arena_reset(lake_arena_allocator *a)
{
    for (lake_arena_page *p = a->head; p; p = p->next) p->offset = 0;
    a->tail = a->head;
}

void *lake_arena_alloc(lake_arena_allocator *a, s32 size, s32 align)
{
    lake_dbg_assert(lake_is_pow2(align), LAKE_ERROR_NOT_PERMITTED, nullptr);

    if (a->tail == nullptr) {
        s32 capacity = LAKE_DEFAULT_ARENA_PAGE_SIZE;
        lake_dbg_assert(a->head == nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, "Missuse of the arena allocator.");
        if (capacity < size) capacity = size;
        a->tail = a->head = lake_arena_page_new(capacity);
    }
    for (lake_arena_page *tail = a->tail; tail != nullptr; tail = tail->next) {
        s32 const aligned = lake_align(tail->offset, align);
        s32 const offset = aligned + size;

        if (offset > tail->alloc) {
            a->tail = tail;
            continue;
        }
        tail->offset = offset;
        return (void *)&tail->v[aligned];
    } 
    s32 capacity = LAKE_DEFAULT_ARENA_PAGE_SIZE;
    if (capacity < size) capacity = size;
    a->tail = a->tail->next = lake_arena_page_new(capacity);
    a->tail->offset = size;
    return (void *)a->tail->v;
}
