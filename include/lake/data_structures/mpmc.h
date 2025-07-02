#pragma once

/** @file lake/data_structures/mpmc.h
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Define a node with a custom data type. */
#define lake_mpmc_t(T) \
    struct { atomic_ssize sequence; T data; }

/* A commonly used node type. */
typedef lake_mpmc_t(ssize) lake_mpmc_node;

/** The MPMC ring buffer is limited to a buffer size that is a power of two.
 *  This data structure was not designed for dynamic growth */
typedef struct LAKE_CACHELINE_ALIGNMENT lake_mpmc {
    void                   *buffer;
    ssize                   buffer_mask;
    u8                  pad0[LAKE_CACHELINE_SIZE - sizeof(uptr) - sizeof(ssize)];

    atomic_ssize            enqueue_pos;
    u8                  pad1[LAKE_CACHELINE_SIZE - sizeof(atomic_ssize)];

    atomic_ssize            dequeue_pos;
    u8                  pad2[LAKE_CACHELINE_SIZE - sizeof(atomic_ssize)];
} lake_mpmc;

/** Used to enqueue or dequeue data. */
typedef struct lake_mpmc_result {
    void                   *node;
    ssize                   pos;
} lake_mpmc_result;

/** Initializes the MPMC ring buffer. Nodes memory must be externally managed.
 *  Node count must be a power of 2, stride must be sizeof(T). */
#define lake_mpmc_init_w_dbg(mpmc, node_count, nodes, stride, type_name) \
    ({ \
        lake_dbg_assert(lake_is_pow2(node_count), LAKE_INVALID_PARAMETERS, "mpmc<"#type_name">"); \
        (mpmc)->buffer_mask = (ssize)node_count - 1; \
        (mpmc)->buffer = nodes; \
        for (ssize iter = 0; iter < (ssize)node_count; iter++) \
            lake_atomic_write((atomic_ssize *)lake_elem(nodes, stride, iter), iter); \
        lake_atomic_write(&(mpmc)->enqueue_pos, 0); \
        lake_atomic_write(&(mpmc)->dequeue_pos, 0); \
    })
#define lake_mpmc_init_t(mpmc, T, node_count, nodes) \
    lake_mpmc_init_w_dbg(mpmc, node_count, nodes, lake_ssizeof(T), #T)

/** Either enqueue into or dequeue from the ring buffer. */
LAKEAPI LAKE_NONNULL_ALL LAKE_HOT_FN
bool LAKECALL lake_mpmc_rotate(
    lake_mpmc          *mpmc,
    atomic_ssize       *in_or_out,
    s32 const           stride,
    s32 const           pos_delta,
    lake_mpmc_result   *out_result);

/** The producer. The data within cells is persistent, so submissions can be made from the stack. */
#define lake_mpmc_enqueue_t(mpmc, T, submit) \
    ({ \
        union { lake_mpmc_result result; T *node; } query; \
        bool __success = lake_mpmc_rotate(mpmc, &(mpmc)->enqueue_pos, lake_ssizeof(T), 0, &query.result); \
        if (__success) { \
            lake_memcpy(&query.node->data, submit, sizeof(lake_typeof(*(submit)))); \
            lake_atomic_write_explicit(&query.node->sequence, query.result.pos + 1, lake_memory_model_release); \
        }; \
        __success; \
    })

/** The consumer. It is enough to pass a pointer onto the output instead of copying the data. */
#define lake_mpmc_dequeue_t(mpmc, T, submit) \
    ({ \
        union { lake_mpmc_result result; T *node; } query; \
        bool __success = lake_mpmc_rotate(mpmc, &(mpmc)->dequeue_pos, lake_ssizeof(T), 1, &query.result); \
        if (__success) { \
            lake_memcpy(submit, &query.node->data, sizeof(lake_typeof(*(submit)))); \
            lake_atomic_write_explicit(&query.node->sequence, query.result.pos + (mpmc)->buffer_mask + 1, lake_memory_model_release); \
        }; \
        __success; \
    })

#ifdef __cplusplus
}
#endif /* __cplusplus */
