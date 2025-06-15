#pragma once

/** @file lake/tagged_heap.h
 *  @brief A tagged heap memory allocator.
 *
 *  This is the backend allocator, and the only one allowed to map new virtual memory.
 *  It has control over physical resources of the host system. It's a block allocator,
 *  combined with arenas to allocate resources of any size.
 *
 *  A block is of size 256 KiB and this is the minimal allocation size for internal use 
 *  by the tagged heaps. These blocks live under a shared tag, where any unique value 
 *  desribes an unique lifetime frequency. There is no free(ptr) interface for these 
 *  allocations, instead all blocks under a tag must be freed together. Calling 
 *  `lake_thfree(tag)` is enough to release resources, so they can be reused later.
 */
#include <lake/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A minimum block size for the tagged heap is 256 KiB. */
#define LAKE_TAGGED_HEAP_BLOCK_SIZE (2lu*1024lu*1024lu)

/** An unique value describes an unique resource lifetime.
 *
 *  A value of 0 is a roots tag, that cannot EVER be freed by the user.
 *  The memory under this heap is tied to the lifetime of the bedrock framework,
 *  so the memory will be freed only after returning from `lake_in_the_lungs()`. */
typedef u32 lake_heap_tag;

/** A linear allocation under a tagged heap. The only way to release this memory 
 *  is to free the entire heap via `lake_thfree(tag)`. */
LAKEAPI LAKE_HOT_FN LAKE_MALLOC
void *LAKECALL lake_thalloc(lake_heap_tag tag, usize size, usize align);

#define lake_thalloc_t(tag, T) \
    lake_reinterpret_cast(T *, lake_thalloc(tag, sizeof(T), alignof(T)))
#define lake_thalloc_n(tag, T, n) \
    lake_reinterpret_cast(T *, lake_thalloc(tag, sizeof(T) * (n), alignof(T)))

typedef struct lake_thblock_range {
    void *memory;
    usize alloc;
} lake_thblock_range;

/** Allows the user to acquire a whole ass memory block (or a range of contiguous blocks)
 *  for themselve, they are then responsible for managing this memory. This allocation 
 *  is valid until `lake_thfree(tag)` is called - any access after is undefined behaviour.
 *  The request will be aligned to nearest block size (LAKE_TAGGED_HEAP_BLOCK_SIZE). */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
lake_thblock_range lake_thblock(lake_heap_tag tag, usize request)
{ 
    usize const page_aligned = lake_align(request, LAKE_TAGGED_HEAP_BLOCK_SIZE);
    void *range = lake_thalloc(tag, page_aligned, LAKE_TAGGED_HEAP_BLOCK_SIZE);
    if (range) {
        return (lake_thblock_range){ .memory = range, .alloc = page_aligned };
    }
    return (lake_thblock_range){ .memory = nullptr, .alloc = 0lu };
}

/** Forces release of resources used by a matching tagged heap. 
 *  Tag can now be reused with a new lifetime. */
LAKEAPI LAKE_HOT_FN
void LAKECALL lake_thfree(lake_heap_tag tag);

typedef enum lake_thadvise_mode : u8 {
    /** Request the commitment of physical resources, limited by RAM available
     *  and by the budget limit set at framework initialization. */
    lake_thadvise_commit        = (1u << 0),
    /** Request the release of physical resources, freeing them to the system.
     *  Only unused memory can be released, fragmented heap may prevent this. */
    lake_thadvise_release       = (1u << 1),
    /** If set, the request may be resolved incomplete without warnings.
     *  Otherwise the request will be ignored unless it can be fulfilled exact. */
    lake_thadvise_suboptimal    = (1u << 2),
} lake_thadvise_mode;

/** Advises on commitment of host memory resources. 
 *  The request size will be block aligned (256 KiB). */
LAKEAPI usize LAKECALL lake_thadvise(usize request, lake_thadvise_mode mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
