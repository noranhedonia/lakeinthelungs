#pragma once

/** @file lake/data_structures/mpmc_ring.h
 *  @brief Multiple-producer multiple-consumer ring buffer.
 *
 *  The MPMC ring buffer was implemented from the multiple producer, multiple consumer (MPMC) 
 *  queue described by Dmitry Vyuko on 1024cores. [High Speed Atomic MPMC Queue]
 *
 *  http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 *
 *  Read and write operations on an atomic object are free from data races. However, if one thread 
 *  writes to it, all cache lines occupied by the object are invalidated. If another thread is reading
 *  from an unrelated object that shares the same cache line, it incures unnecesary overhead. This is 
 *  called false sharing, and we pad our MPMC ring buffer to avoid that. 
 */
#include <lake/bedrock/bedrock.h>

typedef struct lake_mpmc_node {
    void           *data;
    atomic_usize    sequence;
} lake_mpmc_node;

/** The MPMC ring buffer is limited to a buffer size that is a power of two */
typedef struct LAKE_CACHELINE_ALIGNMENT lake_mpmc_ring {
    lake_mpmc_node *buffer;
    usize           buffer_mask;
    u8          pad0[LAKE_CACHELINE_SIZE - sizeof(lake_mpmc_node *) - sizeof(usize)];

    atomic_usize    enqueue_pos;
    u8          pad1[LAKE_CACHELINE_SIZE - sizeof(atomic_usize)];

    atomic_usize    dequeue_pos;
    u8          pad2[LAKE_CACHELINE_SIZE - sizeof(atomic_usize)];
} lake_mpmc_ring;

typedef struct lake_mpmc_result {
    usize           pos;
    lake_mpmc_node *node;
} lake_mpmc_result;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The ring buffer's memory and data array must be managed externally. Stride is the size of an 
 *  element in the array for node data. The array should be of size `node_count * stride` bytes. */
LAKE_NONNULL_ALL 
LAKEAPI void LAKECALL
lake_mpmc_ring_init(
    lake_mpmc_ring     *ring,
    usize               node_count,
    lake_mpmc_node     *nodes,
    usize const         stride,
    void               *array);

/** Either enqueue into or dequeue from the ring buffer. */
LAKE_NONNULL_ALL LAKE_HOT_FN
LAKEAPI lake_mpmc_result LAKECALL 
lake_mpmc_ring_rotate(
    lake_mpmc_ring *ring,
    atomic_usize   *in_or_out,
    u32 const       pos_delta);

/** The producer. The data within cells is persistent, so submissions can be made from the stack.
 *  Everytime a enqueue happens to a cell, existing data is discarded (collisions won't happen). */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
bool lake_mpmc_ring_enqueue(
    lake_mpmc_ring *ring, 
    usize const     stride, 
    void *restrict  submit)
{
    lake_mpmc_result result = lake_mpmc_ring_rotate(ring, &ring->enqueue_pos, 0);
    if (result.node) {
        lake_memcpy(result.node->data, submit, stride);
        lake_atomic_write_explicit(&result.node->sequence, result.pos + 1, lake_memory_model_release);
        return true;
    }
    return false;
}

/** The consumer. It is enough to pass a pointer onto the output instead of copying the data. */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
bool lake_mpmc_ring_dequeue(
    lake_mpmc_ring *ring, 
    usize const     stride, 
    void *restrict  output)
{
    lake_mpmc_result result = lake_mpmc_ring_rotate(ring, &ring->dequeue_pos, 1);
    if (result.node) {
        lake_memcpy(output, result.node->data, stride);
        lake_atomic_write_explicit(&result.node->sequence, result.pos + ring->buffer_mask + 1, lake_memory_model_release);
        return true;
    }
    return false;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
