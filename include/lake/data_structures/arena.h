#pragma once

/** @file lake/data_structures/arena.h 
 *  @brief Arena allocator.
 *
 *  TODO docs.
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

typedef struct lake_arena {
    lake_arena_page        *head;
    lake_arena_page        *tail;
} lake_arena;

#define LAKE_DEFAULT_ARENA_PAGE_SIZE 4096

/** Allocate a new arena page, use any Bedrock allocator. */
#define __lake_arena_page_new(n, allocator, deallocator) \
    ({ \
        lake_arena_page *out = nullptr; \
        usize const actual_size = sizeof(lake_arena_page) + (usize)n; \
        u8 *raw = allocator(actual_size, alignof(lake_arena_page)); \
        if (raw) { \
            out = (lake_arena_page *)&raw[n]; \
            *out = (lake_arena_page){ .v = raw, .alloc = n }; \
        } \
        out; \
    })
#define lake_arena_page_new(n, ...) \
    __lake_arena_page_new(n, __VA_ARGS__)

/** Initialize the arena with a single page of given size. */
#define lake_arena_init(a, n, ...) \
    ({ (a)->tail = (a)->head = lake_arena_page_new(n, __VA_ARGS__); })

/** Allocate a new page for the arena. */
#define lake_arena_append(a, n, ...) \
    ({ \
        lake_arena_page *tail; \
        for (tail = (a)->tail; tail->next; tail = tail->next); \
        tail->next = lake_arena_page_new(n, __VA_ARGS__); \
    })

/** Release any used memory (if a free(ptr) function is given) and zero the arena. */
#define __lake_arena_fini(a, allocator, deallocator) \
    ({ \
        lake_arena_page *p = (a)->head; \
        while (p) { \
            lake_arena_page *next = p->next; \
            deallocator(p); \
            p = next; \
        } \
        *(a) = (lake_arena){0}; \
    })
#define lake_arena_fini(a, ...) \
    __lake_arena_fini(a, __VA_ARGS__)

/** Reset the offsets without releasing memory pages. */
#define lake_arena_reset(a) \
    ({ \
        for (lake_arena_page *p = (a)->head; p; p = p->next){ p->offset = 0; } \
        (a)->tail = (a)->head; \
    })

/** Save a scratch of the arena to later restore a checkpoint. */
#define lake_arena_read_scratch(a) \
    (lake_arena_scratch){ .tail = (a)->tail, .offset = (a)->tail ? (a)->tail->offset : 0 }

/** Write a previously read scratch into the arena. If the scratch was obtained 
 *  from a different arena, this will result in undefined behaviour. */
#define lake_arena_write_scratch(a, val) \
    ({ \
        (a)->tail = (val).tail; \
        if ((a)->tail) (a)->tail->offset = (val).offset; \
    })

/** Allocates memory from the arena. Uses the given allocator for growth. */
#define lake_arena_allocate(a, size, align, ...) \
    ({ \
        lake_san_assert(lake_is_pow2(align), LAKE_ERROR_NOT_PERMITTED, "Alignment must be a power of 2."); \
        void *__out = nullptr; \
        if ((a)->tail == nullptr) { \
            s32 __capacity = LAKE_DEFAULT_ARENA_PAGE_SIZE; \
            lake_san_assert((a)->head == nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, "Missuse of the arena allocator, tail is null but head not."); \
            if (__capacity < size) __capacity = size; \
            (a)->tail = (a)->head = lake_arena_page_new(__capacity, __VA_ARGS__); \
        } \
        if ((a)->head != nullptr) { \
            for (lake_arena_page *tail = (a)->tail; tail != nullptr; tail = tail->next) { \
                s32 const __aligned = lake_align(tail->offset, align); \
                s32 const __offset = __aligned + size; \
                if (__offset > tail->alloc) { \
                    (a)->tail = tail; \
                    continue; \
                } \
                tail->offset = __offset; \
                __out = (void *)(uptr)(tail->v[__aligned]); \
                break; \
            } \
            if (__out == nullptr) { \
                s32 __capacity = LAKE_DEFAULT_ARENA_PAGE_SIZE; \
                if (__capacity < size) __capacity = size; \
                (a)->tail = (a)->tail->next = lake_arena_page_new(__capacity, __VA_ARGS__); \
                lake_san_assert((a)->tail != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, "Host memory failure."); \
                __out = (void *)(uptr)((a)->tail->v); \
            } \
        } \
        __out; \
    })

/** Implements lake_arena_allocate, uses drifter as the page allocator. */
LAKEAPI LAKE_NONNULL_ALL LAKE_HOT_FN
void *LAKECALL lake_arena_w_drifter(lake_arena *a, s32 size, s32 align);

/** Implements lake_arena_allocate, uses machina as the page allocator. */
LAKEAPI LAKE_NONNULL_ALL LAKE_HOT_FN
void *LAKECALL lake_arena_w_machina(lake_arena *a, s32 size, s32 align);

#ifdef __cplusplus
}
#endif /* __cplusplus */
