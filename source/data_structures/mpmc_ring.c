#include <lake/data_structures/mpmc_ring.h>

LAKEAPI LAKE_NONNULL_ALL void LAKECALL
lake_mpmc_init_w_dbg(
    lake_mpmc_ring *ring, 
    s32             node_count, 
    void           *nodes,
    s32 const       stride,
    char const     *type_name)
{
    lake_dbg_assert(lake_is_pow2(node_count), LAKE_INVALID_PARAMETERS, "mpmc_ring<%s> node count must be a power of 2.", type_name);

    ring->buffer_mask = node_count - 1l;
    ring->buffer = nodes;
    for (ssize i = 0l; i < node_count; i++)
        lake_atomic_write((atomic_ssize *)lake_elem(nodes, stride, i), i);

    lake_atomic_write(&ring->enqueue_pos, 0l); \
    lake_atomic_write(&ring->dequeue_pos, 0l); \
}

bool lake_mpmc_rotate(
    lake_mpmc_ring     *ring,
    atomic_ssize       *in_or_out,
    s32 const           stride,
    s32 const           pos_delta,
    lake_mpmc_result   *out_result) 
{
    atomic_ssize *sequence;
    ssize pos = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);

    u8 *raw = (u8 *)ring->buffer;
    for (;;) {
        void *check = (void *)(uptr)(raw + stride * (pos & ring->buffer_mask));
        sequence = (atomic_ssize *)lake_elem(ring->buffer, stride, pos & ring->buffer_mask);
        lake_dbg_assert(check == sequence, LAKE_PANIC, "check: %p and sequence: %p", check, sequence);
        ssize seq = lake_atomic_read_explicit(sequence, lake_memory_model_acquire);
        sptr diff = (sptr)seq - (sptr)(pos + pos_delta);

        if (diff == 0) {
            if (lake_atomic_compare_exchange_weak_explicit(in_or_out, &pos , pos + 1,
                    lake_memory_model_relaxed, lake_memory_model_relaxed)) 
            {
                *out_result = (lake_mpmc_result){ .node = (void *)sequence, .pos = pos };
                return true;
            }
        } else if (diff < 0) {
            /* it's empty */
            *out_result = (lake_mpmc_result){ .node = nullptr, .pos = 0 };
            return false;
        } else {
            pos = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);
        }
    }
    LAKE_UNREACHABLE;
}
