#include "bedrock_impl.h"
#include <lake/math/bits.h>

/** We use a bitmap to represent our blocks of free memory. One bit in the bitmap 
 *  is one block of LAKE_TAGGED_HEAP_BLOCK_SIZE, with one byte we represent 8 blocks. */
#define IMPL_HEAP_BITMAP_TEMPLATE(FUNC, OPERATION)                                  \
    LAKE_HOT_FN LAKE_NONNULL_ALL                                                    \
    void LAKECALL FUNC##_heap_bitmap(                                               \
        atomic_u8  *bitmap,                                                         \
        usize const offset,                                                         \
        usize const size)                                                           \
{                                                                                   \
    if (size == 0) return;                                                          \
    usize const position   = __position_from_block(offset);                         \
    usize const head_index = __index_from_position(position);                       \
    usize const count      = 1 + __index_from_page(size);                           \
    usize const tail_index = head_index + count - 1;                                \
                                                                                    \
    u8 bitmask = ~((1 << (position & 0x07)) - 1);                                   \
    for (usize i = 0; i < count; i++) {                                             \
        usize index = head_index + i;                                               \
                                                                                    \
        /* XOR'ing the bitmask will leave blocks beyond the range untouched */      \
        if (index == tail_index)                                                    \
            bitmask ^= ~((1 << ((position + count) & 0x07)) - 1);                   \
                                                                                    \
        /* set the blocks as in use (bits set to 0) or as free (bits set to 1) */   \
        OPERATION;                                                                  \
        bitmask = 0xFF;                                                             \
    }                                                                               \
}
/** Sets a range of blocks in the heap as in use. */
IMPL_HEAP_BITMAP_TEMPLATE(acquire, lake_atomic_and_explicit(&bitmap[index], ~bitmask, lake_memory_model_acquire))

/** Sets a range of blocks in the heap as free to take. */
IMPL_HEAP_BITMAP_TEMPLATE(release, lake_atomic_or_explicit(&bitmap[index], bitmask, lake_memory_model_release))

/** A single block allocation (LAKE_TAGGED_HEAP_BLOCK_SIZE). We don't own the growth 
 *  sync here and NEVER wait on it, instead the sync value is passed in as the ceiling 
 *  for bitmap query. If a free block is found, we commit it to the bitmap immediately. */
LAKE_HOT_FN LAKE_PURE_FN
static usize LAKECALL find_free_block(usize const offset, usize const ceiling)
{
    if (offset >= ceiling) return 0lu;

    atomic_u8 *bitmap = g_bedrock->bitmap;
    usize const index = __index_from_page(offset);
    usize const range = 1 + __index_from_page(ceiling) - index;

    /* try until we either succeed or hit the ceiling */
    for (;;) {
        usize bits = lake_ffsbit((u8 const *)&bitmap[index], range);

        /* `lake_ffsbit()` returns a 1-based value, or 0 if no bits are set.
         * This is also why later we must decrement the bits value. */
        if (!bits) return 0lu;

        /* The `__position_from_index` will align the value of `pos`
         * to a byte boundary (index is: `pos >> 3lu`). */
        bits += __position_from_index(index) - 1; /* decrement to get a 0-based value */

        /* we may have overshot on edging (!! holy moly) bytes */
        if (lake_unlikely(bits >= __position_from_block(ceiling)))
            return 0lu;

        /* try to acquire the block */
        u8 const bitmask = (1u << (bits & 0x07));
        u8 const prev = lake_atomic_and_explicit(&bitmap[__index_from_position(bits)], ~bitmask, lake_memory_model_acquire);

        /* Double check to avoid any possibility of data races. If this is true, 
         * then the bitmask wrote to a block already in use. Nothing changed, 
         * and we can't claim this block for ourselves. */
        if ((prev & bitmask) == bitmask) 
            return __block_from_position(bits);
    }
    LAKE_UNREACHABLE;
}

/** Tries to find a range of contiguous memory blocks to fulfill a larger request.
 *  If this function was called, it's assumed we own the sync counter. */
LAKE_HOT_FN LAKE_PURE_FN LAKE_NONNULL_ALL
static usize LAKECALL find_free_blocks_range(
        usize const   offset,
        usize const   request, 
        usize const   ceiling, 
        atomic_usize *sync)
{
    atomic_u8 const *bitmap = g_bedrock->bitmap; /* we won't write to it here */
    usize const blocks = __position_from_block(request);
    usize const head_index = __index_from_page(offset);
    usize const tail_index = __index_from_page(ceiling);

    /* early checks to invalidate the request */
    if (head_index > tail_index || !blocks || blocks < __position_from_block((ceiling - offset)))
        return 0lu;

    usize candidate = 0lu;
    usize current = 0lu;

    /* query the bitmap */
    for (usize i = head_index; i <= tail_index; i++) {
        /* a byte represents a range of 8 blocks */
        u8 const byte = lake_atomic_read(&bitmap[i]);

        /* a full free byte is convenient */
        if (byte == 0xff) {
            if (current == 0lu)
                candidate = __position_from_index(i);
            current += 8;
            if (current >= blocks) return __block_from_position(candidate);
        } else if (byte != 0) {
            u8 bitmask = 0xff;
            usize bit = 0;

            /* check individual blocks */
            while (bitmask && bit < 8) {
                if (byte & bitmask) {
                    if (current == 0lu)
                        candidate = __position_from_index(i) + bit;
                    current++;
                    bitmask >>= 1;
                    bit++;
                } else {
                    lake_atomic_write(sync, __block_from_position(__position_from_index(i)));
                    current = 0lu;
                    bitmask = 0lu;
                }
                if (current >= blocks) return __block_from_position(candidate);
            }
        } else {
            /* currently queried free memory range does not satisfy our request */
            lake_atomic_write(sync, __page_from_index(i));
            current = 0lu;
        }
    }
    /* either out of host memory or the memory is fragmented... */
    return 0lu;
}

usize acquire_blocks(usize const block_aligned)
{
    usize const roots_end = g_bedrock->roots.head.alloc;
    usize commitment = lake_atomic_read(&g_bedrock->commitment);

    atomic_usize *sync = &g_bedrock->growth_sync;
    for (;;) {
        usize const sync_value = lake_atomic_read_explicit(sync, lake_memory_model_acquire);

        /** We can greatly simplify the allocation process if we only need to acquire 
         *  a single block of memory for resources (2 MiB). Then it is enough to query 
         *  just one single bit that is free, flip it's value immediately, and in this 
         *  process we can avoid any locks between different heaps. */
        if (block_aligned == LAKE_TAGGED_HEAP_BLOCK_SIZE) {
            usize const ceiling = sync_value ? lake_min(sync_value, commitment) : commitment;
            usize const offset = find_free_block(roots_end, ceiling);
            if (offset) {
                return offset;
            }
            if (sync_value) continue;
        }
        usize expected = 0lu;
        if (!lake_atomic_compare_exchange_weak_explicit(sync, &expected,
                roots_end, lake_memory_model_relaxed, lake_memory_model_relaxed))
        { /* another thread owns the growth sync */
            continue;
        }
        usize offset = 0;

        /* in case it changed */
        commitment = lake_atomic_read_explicit(&g_bedrock->commitment, lake_memory_model_acquire);
        /** This step can be skipped in case we came here with the implication, that there is 
         *  not enough free commited memory right now to satisfy even a minimal allocation. */
        if (block_aligned > LAKE_TAGGED_HEAP_BLOCK_SIZE) {
            /* When it's a non-zero value, we can satisfy the allocation from existing resources. */
            offset = find_free_blocks_range(roots_end, block_aligned, commitment, sync);
            if (offset && offset + block_aligned <= commitment) {
                /* write changes and drop the sync */
                acquire_heap_bitmap(g_bedrock->bitmap, offset, block_aligned);
                lake_atomic_write_explicit(sync, 0lu, lake_memory_model_release);
                return offset;
            }
        }
        if (!offset) offset = commitment;

        lake_atomic_write_explicit(sync, offset, lake_memory_model_release);
        usize new_ceiling = block_aligned - (commitment - offset);

        /* we must commit memory for new resources */
        if (lake_unlikely(new_ceiling + commitment > g_bedrock->budget || 
            !sys_madvise((void *)g_bedrock, commitment, new_ceiling, true)))
        {
            /* try one last time... */
            new_ceiling = LAKE_TAGGED_HEAP_BLOCK_SIZE - (commitment - offset);
            if (lake_unlikely(new_ceiling + commitment > g_bedrock->budget || 
                !sys_madvise((void *)g_bedrock, commitment, new_ceiling, true))) 
            {
                lake_error("Can't commit new resources allocation: %s.", 
                        (new_ceiling + commitment > g_bedrock->budget)
                        ? "framework budget drained" : "out of host memory");
                lake_atomic_write_explicit(sync, 0lu, lake_memory_model_release);
                return 0lu;
            }
        }
        acquire_heap_bitmap(g_bedrock->bitmap, offset, block_aligned);
        lake_atomic_write_explicit(&g_bedrock->commitment, new_ceiling + commitment, lake_memory_model_release);
        lake_atomic_write_explicit(sync, 0lu, lake_memory_model_release);
        return offset;
    }
    LAKE_UNREACHABLE;
}

/** We grab them from roots, so they are never released.
 *  This operation must never fail. */
static struct region *LAKECALL construct_tagged_heap_region(
        lake_heap_tag tag, 
        usize const block_aligned)
{
    u8 *raw = (u8 *)g_bedrock;
    struct tagged_heap *roots = &g_bedrock->roots;
    if (lake_likely(tag != 0)) lake_spinlock_acquire(&roots->spinlock);

    struct region *tail = roots->tail;
    constexpr usize page_bytes = sizeof(struct region);     
    usize aligned = lake_align(tail->offset, alignof(struct region));

    if (lake_unlikely(aligned + page_bytes > tail->alloc)) {
        usize block = acquire_blocks(block_aligned);
        lake_assert(block, LAKE_ERROR_OUT_OF_HOST_MEMORY, nullptr); 

        tail = roots->tail = tail->next = (struct region *)(void *)(uptr)(raw + block);
        *tail = (struct region){
            .v = block,
            .next = nullptr,
            .offset = page_bytes,
            .alloc = block_aligned,
        };
    } else {
        tail->offset = aligned + page_bytes;
    }
    if (lake_likely(tag != 0)) lake_spinlock_release(&roots->spinlock);
    return (struct region *)(void *)(uptr)(raw + tail->v + aligned);
}

LAKE_HOT_FN LAKE_MALLOC
static void *LAKECALL allocate_from_tagged_heap(struct tagged_heap *th, usize size, u32 align)
{
    u8 *raw = (u8 *)g_bedrock;

    lake_heap_tag tag = lake_atomic_read(&th->tag);
    static char const *err = "Host memory failure, not enough resources to reserve %lu bytes of memory (aligned to %lu bytes, %lu MB) from tagged heap %X.";

    usize const block_aligned = lake_align(size, LAKE_TAGGED_HEAP_BLOCK_SIZE);

    lake_spinlock_acquire(&th->spinlock);
    if (lake_unlikely(th->head.alloc = 0lu)) {
        th->head.v = acquire_blocks(block_aligned);
        th->tail = &th->head;

        if (lake_unlikely(th->head.v == 0lu)) {
            lake_fatal(err, size, block_aligned, block_aligned >> 20, tag);
            lake_spinlock_release(&th->spinlock);
            return nullptr;
        }
        th->head.offset = size;
        th->head.alloc = block_aligned;
        lake_spinlock_release(&th->spinlock);
        return (void *)(uptr)(raw + th->head.v);
    }

    /* we can assume that there is no more free space than a single block
     * withing an existing tagged heap page */
    if (lake_likely(size < LAKE_TAGGED_HEAP_BLOCK_SIZE)) {
        for (struct region *page = &th->head; page != nullptr; page = page->next) {
            usize const aligned = lake_align(page->offset, align);

            if (page->alloc == 0) {
                th->tail = page;
                page->v = acquire_blocks(block_aligned); 

                if (lake_unlikely(page->v == 0lu)) {
                    lake_fatal(err, size, block_aligned, block_aligned >> 20, tag);
                    lake_spinlock_release(&th->spinlock);
                    return nullptr;
                }
                page->offset = aligned + size;
                page->alloc = block_aligned;
            } else if (aligned + size > page->alloc) {
                continue;
            }
            lake_spinlock_release(&th->spinlock);
            return (void *)(uptr)(raw + page->v + aligned);
        }
    }

    /* we could not yet satisfy the allocation, so grab a new arena page */
    struct region *next = construct_tagged_heap_region(tag, block_aligned);
    th->tail = th->tail->next = next;
    next->v = acquire_blocks(block_aligned); 

    if (lake_unlikely(next->v == 0lu)) {
        lake_error(err, size, block_aligned, block_aligned >> 20, tag);
        lake_spinlock_release(&th->spinlock);
        return nullptr;
    }
    next->offset = size;
    next->alloc = block_aligned;
    lake_spinlock_release(&th->spinlock);
    return (void *)(uptr)(raw + next->v);
}

void *lake_thalloc(lake_heap_tag tag, usize size, usize align)
{
    lake_dbg_assert(lake_is_pow2(align), LAKE_INVALID_PARAMETERS, "alignment must be a power of 2");

    if (lake_unlikely(size == 0 || align == 0))
        return nullptr;

    if (tag == 0) 
        return allocate_from_tagged_heap(&g_bedrock->roots, size, align);

    /* if a tag exists, it will be found here */
    usize tail = lake_atomic_read_explicit(&g_bedrock->tagged_heap_tail, lake_memory_model_acquire);
    for (usize i = 0; i < tail; i++) {
        struct tagged_heap *th = g_bedrock->tagged_heaps[i];
        if (tag == lake_atomic_read(&th->tag))
            return allocate_from_tagged_heap(th, size, align);
    }

    /* prepare a new tagged heap */
    for (;;) {
        if (lake_unlikely(tail >= (u32)g_bedrock->tagged_heap_count)) {
            lake_error("Reached maximum count of unique tagged heaps (%lu), can't satisfy" 
                    " the allocation for tag `%X`, as the heap does not yet exist.", tail, tag);
            return nullptr;
        }
        /* we are using the roots tag to identify free heaps */
        lake_heap_tag expected = 0;
        struct tagged_heap *th = g_bedrock->tagged_heaps[tail];

        if (lake_atomic_compare_exchange_weak_explicit(&th->tag, &expected, tag,
                lake_memory_model_release, lake_memory_model_relaxed))
        {
            lake_atomic_add_explicit(&g_bedrock->tagged_heap_tail, 1lu, lake_memory_model_release);
            return allocate_from_tagged_heap(th, size, align);
        }
        tail = lake_atomic_read_explicit(&g_bedrock->tagged_heap_tail, lake_memory_model_acquire);
    }
    LAKE_UNREACHABLE;
}

void lake_thfree(lake_heap_tag tag)
{
    usize const tail = lake_atomic_read_explicit(&g_bedrock->tagged_heap_tail, lake_memory_model_relaxed);
    lake_dbg_assert(tag != 0, LAKE_ERROR_NOT_PERMITTED, "roots tagged heap MUST NOT be freed");

    for (usize i = 0; i < tail; i++) {
        struct tagged_heap *th = g_bedrock->tagged_heaps[i];

        if (lake_atomic_compare_exchange_weak_explicit(&th->tag, &tag, 0u, 
                lake_memory_model_release, lake_memory_model_relaxed))
        {
            usize const actual_tail = lake_atomic_sub_explicit(
                    &g_bedrock->tagged_heap_tail, 1lu, lake_memory_model_release);
            /* swap the heap with the tail */
            g_bedrock->tagged_heaps[i] = g_bedrock->tagged_heaps[actual_tail];
            g_bedrock->tagged_heaps[actual_tail] = th;
            lake_spinlock_acquire(&th->spinlock);

            for (struct region *page = &th->head; page != nullptr; page = page->next) {
                if (!page->alloc) break;

                release_heap_bitmap(g_bedrock->bitmap, page->v, page->alloc);
                *page = (struct region){ .next = page->next };
            }
            th->tail = &th->head;
            lake_spinlock_release(&th->spinlock);
            return;
        }
    }
}

usize lake_thadvise(usize request, lake_thadvise_mode mode)
{
    if (request == 0) return LAKE_SUCCESS;
    lake_dbg_assert(g_bedrock != nullptr, LAKE_FRAMEWORK_REQUIRED, nullptr);

    if (!lake_is_pow2((mode & (lake_thadvise_commit | lake_thadvise_release))))
        return LAKE_ERROR_NOT_PERMITTED;

    atomic_usize *sync = &g_bedrock->growth_sync;
    usize const roots_end = g_bedrock->roots.head.alloc;
    bool const allow_suboptimal = (mode & lake_thadvise_suboptimal) ? true : false;

    usize expected = 0lu;
    while (!lake_atomic_compare_exchange_weak_explicit(sync, &expected, roots_end, lake_memory_model_acquire, lake_memory_model_relaxed))

    for (;;) {
        usize const page_aligned = lake_align(request, g_bedrock->page_size);
        usize const commitment = lake_atomic_read_explicit(&g_bedrock->commitment, lake_memory_model_relaxed);

        /* try to release physical resources */
        if (mode & lake_thadvise_release) {
            usize const offset = commitment - page_aligned;
            usize const index = __index_from_page(offset);
            usize const range = __index_from_page(page_aligned);
            usize const bits = (range + 1) * 8  /* full bytes of free blocks */
                - (index & 0x07)                /* trim head index bits from range */ 
                - ((index + range) & 0x07);     /* trim tail index bits from range */
            u64 const popcnt = lake_popcnt((u8 const *)g_bedrock->bitmap + index, range);

            bool success = false;
            if (popcnt >= bits && sys_madvise((void *)g_bedrock, offset, page_aligned, false)) {
                success = true;
            } else if (allow_suboptimal) {
                request >>= 1;
                if (request > g_bedrock->page_size)
                    continue;
            }

            lake_atomic_write_explicit(sync, 0lu, lake_memory_model_release);
            if (success) {
                lake_atomic_write_explicit(&g_bedrock->commitment, offset, lake_memory_model_release);
                return page_aligned;
            }
            return 0lu;

        /* try to commit physical resources */
        } else if (mode & lake_thadvise_commit) {
            usize const offset = find_free_blocks_range(roots_end, page_aligned, commitment, sync);
            if (offset) {
                /* there is enough free memory for this range */
                lake_atomic_write_explicit(sync, 0lu, lake_memory_model_release);
                return offset;
            }
            lake_atomic_write_explicit(sync, commitment, lake_memory_model_release);

            /* commit physical resources */
            bool success = false;
            if (sys_madvise((void *)g_bedrock, commitment, page_aligned, true)) {
                success = true;
            } else if (allow_suboptimal) {
                request >>= 1;
                if (request > g_bedrock->page_size)
                    continue;
            }
            if (success) {
                lake_atomic_write_explicit(&g_bedrock->commitment, commitment + page_aligned, lake_memory_model_release);
                lake_atomic_write_explicit(sync, 0lu, lake_memory_model_release);
                return page_aligned;
            }
            lake_atomic_write_explicit(sync, 0lu, lake_memory_model_release);
            return 0lu;
        } else {
            LAKE_UNREACHABLE;
        }
    }
    LAKE_UNREACHABLE;
}
