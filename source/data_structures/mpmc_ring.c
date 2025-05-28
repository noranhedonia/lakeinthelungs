#include <lake/data_structures/mpmc_ring.h>

void lake_mpmc_ring_init(
    lake_mpmc_ring     *ring,
    usize               node_count,
    lake_mpmc_node     *nodes,
    usize const         stride,
    void               *array)
{
    lake_dbg_assert(lake_is_pow2(node_count), LAKE_INVALID_PARAMETERS, "Ring buffer node count must be a power of 2.");

    u8 *raw = (u8 *)array;
    ring->buffer_mask = node_count - 1; /* ((1lu << log2_node_count) - 1) */
    ring->buffer = nodes;

    for (usize i = 0lu; i < node_count; i++) {
        lake_mpmc_node *node = &ring->buffer[i];
        lake_atomic_write(&node->sequence, i);
        node->data = (void *)&raw[i * stride];
    }
    lake_atomic_write(&ring->enqueue_pos, 0lu);
    lake_atomic_write(&ring->dequeue_pos, 0lu);
}

lake_mpmc_result lake_mpmc_ring_rotate(
    lake_mpmc_ring     *ring,
    atomic_usize       *in_or_out,
    u32 const           pos_delta)
{
    lake_mpmc_node *node;

    usize pos = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);
    for (;;) {
        node = &ring->buffer[pos & ring->buffer_mask];
        usize seq = lake_atomic_read_explicit(&node->sequence, lake_memory_model_acquire);
        sptr diff = (sptr)seq - (sptr)(pos + pos_delta);

        if (diff == 0) {
            if (lake_atomic_compare_exchange_weak_explicit(in_or_out, &pos , pos + 1,
                    lake_memory_model_relaxed, lake_memory_model_relaxed)) 
            {
                return (lake_mpmc_result){ .node = node, .pos = pos };
            }
        } else if (diff < 0) {
            /* it's empty */
            return (lake_mpmc_result){ .node = nullptr, .pos = 0 };
        } else {
            pos = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);
        }
    }
    LAKE_UNREACHABLE;
}
