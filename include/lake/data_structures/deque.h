#pragma once

/** @file lake/data_structures/deque.h 
 *  @brief Type-preserving resizing circular double-ended queue.
 *
 *  This implementation is using a resizing circular buffer. At steay state,
 *  deque operations can proceed perpetually without reallocations. The initial
 *  capacity must be specified and is a lower bound when shrinking. Buffer capacity 
 *  is doubled at enqueue to a full deque. Shrink behaviour choices are never shrink,
 *  shrink to minimum when the queue is empty, or shrink by hald when the queue is 
 *  at 20% of capacity. Operation names are in Ruby style ;3.
 */
#include <lake/bedrock/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum : s32 {
    lake_deque_no_shrink = 0, 
    lake_deque_shrink_if_empty,
    lake_deque_shrink_at_one_fifth,
};

/** Double-ended queue. */
typedef struct lake_deque {
    void *v;
    s32 head, tail, len, cap, min, shrink;
} lake_deque;

/** Define a custom deque type. */
#define lake_deque_t(T) \
    union { lake_deque deq; T *v; }

#ifdef __cplusplus
}
#endif /* __cplusplus */
