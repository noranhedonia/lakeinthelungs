#pragma once

/** @file lake/data_structures/arena_allocator.h 
 *  @brief TODO docs 
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_arena_page {
    u8                     *v;
    struct lake_arena_page *next;    
    s32                     offset;
    s32                     alloc;
} lake_arena_page;

typedef struct lake_arena_scratch {
    lake_arena_page        *tail;
    ssize                   offset;
} lake_arena_scratch;

typedef struct lake_arena_allocator {
    lake_arena_page        *head;
    lake_arena_page        *tail;
} lake_arena_allocator;

#define LAKE_DEFAULT_ARENA_PAGE_SIZE 4096

/** Allocate a new arena page via __lake_malloc. */
LAKEAPI lake_arena_page *LAKECALL 
lake_arena_page_new(s32 page_size);

/** Initialize the arena with a single page of given size. 
 *  Newly created pages will inherit this size, unless the allocation is bigger. */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
void lake_arena_init(
    lake_arena_allocator   *a, 
    s32                     page_size)
{
    s32 capacity = LAKE_DEFAULT_ARENA_PAGE_SIZE;
    if (capacity < page_size) capacity = page_size;
    a->tail = a->head = lake_arena_page_new(capacity);
}

/** Release any used memory and zero the arena. */
LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_arena_fini(
    lake_arena_allocator   *a);

/** Reset the offsets without releasing memory pages. */
LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_arena_reset(
    lake_arena_allocator   *a);

/** Save a scratch of the arena to later restore it's state.
 *  If the arena is empty, scratch will be zeroed out. */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
void lake_arena_read_scratch(
    lake_arena_allocator   *a, 
    lake_arena_scratch     *out_scratch)
{
    *out_scratch = (lake_arena_scratch){ .tail = a->tail, .offset = a->tail->offset };
}

/** Write a previously read scratch into the arena. If the scratch was obtained 
 *  from a different arena, this will result in undefined behaviour. */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
void lake_arena_write_scratch(
    lake_arena_allocator   *a, 
    lake_arena_scratch     *scratch)
{
    a->tail = scratch->tail;
    a->tail->offset = scratch->offset;
}

LAKEAPI LAKE_NONNULL_ALL
void *LAKECALL lake_arena_alloc(
    lake_arena_allocator   *a,
    s32                     size,
    s32                     align);

#ifdef __cplusplus
}
#endif /* __cplusplus */
