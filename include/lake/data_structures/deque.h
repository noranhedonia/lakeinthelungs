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
#include <lake/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum : s32 {
    lake_deque_no_shrink = 0, 
    lake_deque_shrink_if_empty,
    lake_deque_shrink_at_one_fifth,
};

/** Double-ended queue. `head` and `tail` are indices of the first and last items in deque.
 *  `len` is the distance between head and tail. `cap` is the total capacity of `v`. `min` 
 *  is the initial capacity of the deque. `shrink` is a flag to specify shrink behaviour.
 *  When shrinking, `min` is the smallest size. */
typedef struct lake_deque {
    void *v;
    s32 head, tail, len, cap, min, shrink;
} lake_deque;

/** Define a custom deque type. */
#define lake_deque_t(T) \
    union { lake_deque deq; T *v; }

#define lake_deque_new(min, shrink) \
    (lake_deque){ nullptr, 0, 0, 0, 0, (min), (shrink) }

/** Resize the deque, allocate atleast bytes*n for future resources.
 *  TODO custom allocator? */
LAKEAPI lake_result LAKECALL 
lake_deque_resize_w_dbg(
    lake_deque *deq, 
    s32         stride, 
    s32         align, 
    s32         n, 
    char const *type);    

/** Resize the deque, typed. */
#define lake_deque_resize_t(deq, T, n) \
    lake_deque_resize_w_dbg(deq, lake_ssizeof(T), lake_salignof(T), n, "deque<"#T">")

/** Initialize the allocation, typed.
 *  TODO custom allocator? */
#define lake_deque_init_t(deq, T, n, min, shrink) \
    do { \
        *(deq) = lake_deque_new(min, shrink); \
        lake_result __res = lake_deque_resize_t(deq, T, n); \
        lake_dbg_assert(__res == LAKE_SUCCESS, __res, "deque<"#T">"); \
        (void)__res; \
    } while(0)

/** Release the allocation and clear the array.
 *  TODO custom allocator? */
#define lake_deque_fini(deq) \
    do { \
        if ((deq)->v) __lake_free((deq)->v); \
        *(deq) = lake_deque_new(0, 0); \
    } while(0)

typedef enum lake_deque_op : s32 {
    lake_deque_op_push = 0,
    lake_deque_op_pop,
    lake_deque_op_shift,
    lake_deque_op_unshift,
} lake_deque_op;

/* Commit an operation on the deque, returns an index. */
LAKE_NONNULL_ALL LAKE_HOT_FN
LAKEAPI s32 LAKECALL 
lake_deque_op_w_dbg(
    lake_deque     *deq, 
    s32             stride, 
    s32             align, 
    lake_deque_op   op,
    char const     *type);

#define lake_deque_op_t(deq, T, op) \
    lake_deque_op_w_dbg((deq), lake_ssizeof(T), lake_salignof(T), (op), "deque<"#T">")

/** Protect the operation using an atomic spinlock. */
#define lake_deque_locked_op(q, T, item, spinlock, op) \
    ({ \
        s32 ___idx; \
        lake_spinlock_acquire(spinlock); \
        ___idx = op(q, T, item); \
        lake_spinlock_release(spinlock); \
        ___idx; \
    })

/** Add an item to end of deque.
 *  @returns non-negative value on success. */
#define lake_deque_push_v(q, T, item) \
    ({ \
        s32 __idx = lake_deque_op_t(&(q).deq, T, lake_deque_op_push); \
        if (__idx >= 0) \
            (q).v[__idx] = (item); \
        __idx; \
    })
#define lake_deque_push_v_locked(q, T, item, spinlock) \
    lake_deque_locked_op(q, T, item, spinlock, lake_deque_push_v)
    
/** Add an item to beginning of deque.
 *  @returns non-negative value on success. */
#define lake_deque_unshift_v(q, T, item) \
    ({ \
        s32 __idx = lake_deque_op_t(&(q).deq, T, lake_deque_op_unshift); \
        if (__idx >= 0) \
            (q).v[__idx] = (item); \
        __idx; \
    })
#define lake_deque_unshift_v_locked(q, T, item, spinlock) \
    lake_deque_locked_op(q, T, item, spinlock, lake_deque_unshift_v)

/** Dequeue an item from end of deque. */
#define lake_deque_pop_v(q, T, out_item) \
    ({ \
        s32 __idx = lake_deque_op_t(&(q).deq, T, lake_deque_op_pop);  \
        if (__idx >= 0) \
            *(out_item) = (q).v[__idx]; \
        __idx; \
    })
#define lake_deque_pop_v_locked(q, T, out_item, spinlock) \
    lake_deque_locked_op(q, T, out_item, spinlock, lake_deque_pop_v)

/** Dequeue an item from beginning of deque. */
#define lake_deque_shift_v(q, T, out_item) \
    ({ \
        s32 __idx = lake_deque_op_t(&(q).deq, T, lake_deque_op_shift);  \
        if (__idx >= 0) \
            *(out_item) = (q).v[__idx]; \
        __idx; \
    })
#define lake_deque_shift_v_locked(q, T, out_item, spinlock) \
    lake_deque_locked_op(q, T, out_item, spinlock, lake_deque_shift_v)

/** Get item from beginning of deque, deque is unchanged. */
#define lake_deque_first_v(q, T, out_item) \
    ({ \
        s32 __idx = 0; \
        if ((q).deq.len > 0) { \
            __idx = (q).deq.head; \
            *(out_item) = (q).v[__idx]; \
        } \
        __idx; \
    })

/** Get item from end of deque, deque is unchanged. */
#define lake_deque_last_v(q, T, out_item) \
    ({ \
        s32 __idx = 0; \
        if ((q).deq.len > 0) { \
            __idx = (q).deq.tail == 0 \
                ? (q).deq.cap : (q).deq.tail; \
            *(out_item) = (q).v[--__idx]; \
        } \
        __idx; \
    })

#define lake_deque_reset(deq) \
    ({ \
        void *__v = (deq).v; \
        (deq) = lake_deque_new((deq).min, (deq).shrink); \
        (deq).v = __v; \
    })

#define lake_deque_len(deq)   ((deq).len)
#define lake_deque_cap(deq)   ((deq).cap)
#define lake_deque_empty(deq) ((deq).len == 0)
#define lake_deque_empty_v(q) ((q).deq.len == 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
