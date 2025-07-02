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
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum : s32 {
    LAKE_DEQUE_NO_SHRINK = 0, 
    LAKE_DEQUE_SHRINK_IF_EMPTY,
    LAKE_DEQUE_SHRINK_AT_ONE_FIFTH,
};

/** Double-ended queue. `head` and `tail` are indices of the first and last items in deque.
 *  `len` is the distance between head and tail. `cap` is the total capacity of `v`. `min` 
 *  is the initial capacity of the deque. `shrink` is a flag to specify shrink behaviour.
 *  When shrinking, `min` is the smallest size. */
typedef struct lake_deque {
    void *v;
    s32 head, tail, len, cap, min, shrink;
} lake_deque;

/** Zero out the deque. */
#define lake_deque_clear(min, shrink) \
    (lake_deque){ nullptr, 0, 0, 0, 0, (min), (shrink) }

/** Reset data in deque. */
#define lake_deque_reset(deq) \
    ({ *(deq) = (lake_deque){(deq)->v, 0, 0, 0, (deq)->cap, (deq)->min, (deq)->shrink}; })

#define lake_deque_len(deq)     ((deq)->len)
#define lake_deque_cap(deq)     ((deq)->cap)
#define lake_deque_empty(deq)   ((deq)->len == 0)

/** Resize the deque, allocate atleast stride*n for future resources. */
#define __lake_deque_resize_w_dbg(deq, stride, align, n, type_name, allocator, deallocator) \
    ({ \
        lake_san_assert(stride != 0 && (n) > 0, LAKE_INVALID_PARAMETERS, "deque<"type_name">"); \
        void *__v = allocator((stride) * (n), (align)); \
        if ((deq)->len) { \
            s32 const part1 = (deq)->head + (deq)->len <= (deq)->cap ? (deq)->len : (deq)->cap - (deq)->head; \
            s32 const part2 = (deq)->len - part1; \
            lake_memcpy(__v, lake_elem((deq)->v, (stride), (deq)->head), (stride) * part1); \
            if (part2) lake_memcpy(lake_elem(__v, (stride), part1), (deq)->v, (stride) * part2); \
        } \
        if ((deq)->v) deallocator((deq)->v); \
        (deq)->v = __v; \
        (deq)->head = 0; \
        (deq)->tail = (deq)->len; \
        (deq)->cap = n; \
    }) 
#define lake_deque_resize_w_dbg(deq, stride, align, n, type_name, ...) \
    __lake_deque_resize_w_dbg(deq, stride, align, n, type_name, __VA_ARGS__)
#define lake_deque_resize_t(deq, T, n, ...) \
    __lake_deque_resize_w_dbg(deq, lake_ssizeof(T), lake_salignof(T), n, #T, __VA_ARGS__)

/** Initialize the deque allocation. */
#define lake_deque_init_w_dbg(deq, stride, align, n, type_name, min, shrink, ...) \
    ({ \
        *(deq) = lake_deque_clear(min, shrink); \
        lake_deque_resize_w_dbg(deq, stride, align, n, type_name, __VA_ARGS__); \
    })
#define lake_deque_init_t(deq, T, n, min, shrink, ...) \
    lake_deque_init_w_dbg(deq, lake_ssizeof(T), lake_salignof(T), n, #T, min, shrink, __VA_ARGS__)

/** Release the allocation and clear the array, allows optional free((deq)->v). */
#define __lake_deque_fini(deq, allocator, deallocator) \
    ({ \
        if ((deq)->v) deallocator((deq)->v); \
        *(deq) = lake_deque_clear(0, 0); \
    })
#define lake_deque_fini(deq, ...) __lake_deque_fini(deq, __VA_ARGS__)

/** Get item from beginning of deque, data is unchanged.
 *  @return Index of head, or -1 if empty. */
#define lake_deque_first_t(deq, T, out_item) \
    ({ \
        LAKE_UNUSED s32 __idx = -1; \
        if ((deq)->len > 0) __idx = (deq)->head; \
        if (__idx >= 0) lake_memcpy((out_item), lake_elem((deq)->v, lake_ssizeof(T), __idx), lake_ssizeof(T)); \
        __idx; \
    })

/** Get item from end of deque, data is unchanged.
 *  @return Index of tail, or -1 if empty. */
#define lake_deque_last_t(deq, T, out_item) \
    ({ \
        LAKE_UNUSED s32 __idx = -1; \
        if ((deq)->len > 0) __idx = ((deq)->tail == 0 ? (deq)->cap : (deq)->tail) - 1; \
        if (__idx >= 0) lake_memcpy((out_item), lake_elem((deq)->v, lake_ssizeof(T), __idx), lake_ssizeof(T)); \
        __idx; \
    })

/* PRIVATE: resize */
#define __lake_deque_resize_push_or_unshift(deq, stride, align, type_name, ...) \
    ({ \
        if ((deq)->len == (deq)->cap) \
            lake_deque_resize_w_dbg(deq, stride, align, (deq)->cap == 0 ? (deq)->min : (deq)->cap * 2, type_name, __VA_ARGS__); \
    })
#define __lake_deque_resize_pop_or_shift(deq, stride, align, type_name, ...) \
    ({ \
        if ((deq)->cap == (deq)->min) { \
            if ((deq)->shrink == LAKE_DEQUE_SHRINK_IF_EMPTY && (deq)->len == 1) { \
                lake_deque_resize_w_dbg(deq, stride, align, (deq)->min, type_name, __VA_ARGS__); \
            } else if ((deq)->shrink == LAKE_DEQUE_SHRINK_AT_ONE_FIFTH && (deq)->len*5 <= (deq)->cap) { \
                lake_deque_resize_w_dbg(deq, stride, align, (deq)->cap >> 1, type_name, __VA_ARGS__); \
            } \
        } \
    })
#define __lake_deque_resize_push    __lake_deque_resize_push_or_unshift
#define __lake_deque_resize_unshift __lake_deque_resize_push_or_unshift
#define __lake_deque_resize_pop     __lake_deque_resize_pop_or_shift
#define __lake_deque_resize_shift   __lake_deque_resize_pop_or_shift

/* PRIVATE: write */
#define __lake_deque_write_push(deq, stride, idx, item) \
    lake_memcpy(lake_elem((deq)->v, stride, idx), (item), stride)
#define __lake_deque_write_unshift(deq, stride, idx, item) \
    lake_memcpy(lake_elem((deq)->v, stride, idx), (item), stride)

#define __lake_deque_write_pop(deq, stride, idx, item) \
    lake_memcpy((item), lake_elem((deq)->v, stride, idx), stride)
#define __lake_deque_write_shift(deq, stride, idx, item) \
    lake_memcpy((item), lake_elem((deq)->v, stride, idx), stride)

/* PRIVATE: op */
#define __lake_deque_op_push(deq, idx) \
    ({ \
        idx = (deq)->tail++; \
        (deq)->tail %= (deq)->cap; \
        (deq)->len++; \
    })
#define __lake_deque_op_shift(deq, idx) \
    ({ \
        idx = (deq)->head++; \
        (deq)->head %= (deq)->cap; \
        (deq)->len--; \
    })
#define __lake_deque_op_pop(deq, idx) \
    ({ \
        (deq)->tail = ((deq)->tail == 0 ? (deq)->cap : (deq)->tail) - 1; \
        idx = (deq)->tail; \
        (deq)->len--; \
    })
#define __lake_deque_op_unshift(deq, idx) \
    ({ \
        (deq)->head = ((deq)->head == 0 ? (deq)->cap : (deq)->head) - 1; \
        idx = (deq)->head; \
        (deq)->len++; \
    })
#define __lake_deque_op(deq, stride, align, type_name, OP, item, ...) \
    ({ \
        LAKE_UNUSED s32 __idx = -1; \
        __lake_deque_resize_##OP(deq, stride, align, type_name, __VA_ARGS__); \
        if ((deq)->len != 0) __lake_deque_op_##OP(deq, __idx); \
        if (__idx >= 0) __lake_deque_write_##OP(deq, stride, __idx, item); \
        __idx; \
    })
#define __lake_deque_op_locked(deq, stride, align, type_name, ACQ, REL, lock, OP, item, ...) \
    ({ \
        LAKE_UNUSED s32 __idx = -1; \
        ACQ(lock); \
        __lake_deque_resize_##OP(deq, stride, align, type_name, __VA_ARGS__); \
        if ((deq)->len != 0) __lake_deque_op_##OP(deq, __idx); \
        if (__idx >= 0) __lake_deque_write_##OP(deq, stride, __idx, item); \
        REL(lock); \
        __idx; \
    })

/** Add an item to end of deque.
 *  @return Non-negative index on success. */
#define lake_deque_push_t(deq, T, item, ...) \
    __lake_deque_op(deq, lake_ssizeof(T), lake_salignof(T), #T, push, item, __VA_ARGS__)

/** Add an item to end of deque, protected by a non-recursive spinlock.
 *  @return Non-negative index on success. */
#define lake_deque_push_t_spinlock(deq, T, item, lock, ...) \
    __lake_deque_op_locked(deq, lake_ssizeof(T), lake_salignof(T), #T, \
            lake_spinlock_acquire, lake_spinlock_release, lock, push, item, __VA_ARGS__)

/** Add an item to beginning of deque.
 *  @return Non-negative index on success. */
#define lake_deque_unshift_t(deq, T, item, ...) \
    __lake_deque_op(deq, lake_ssizeof(T), lake_salignof(T), #T, unshift, item, ...)

/** Add an item to beginning of deque, protected by a non-recursive spinlock.
 *  @return Non-negative index on success. */
#define lake_deque_unshift_w_spinlock(deq, T, item, lock, ...) \
    __lake_deque_op_locked(deq, lake_ssizeof(T), lake_salignof(T), #T, \
            lake_spinlock_acquire, lake_spinlock_release, lock, unshift, item, __VA_ARGS__)

/** Dequeue an item from end of deque.
 *  @return Non-negative index on success. */
#define lake_deque_pop_t(deq, T, out_item, ...) \
    __lake_deque_op(deq, lake_ssizeof(T), lake_salignof(T), #T, pop, out_item, __VA_ARGS__)

/** Dequeue an item from end of deque, protected by non-recursive spinlock.
 *  @return Non-negative index on success. */
#define lake_deque_pop_w_spinlock(q, T, out_item, lock, ...) \
    __lake_deque_op_locked(deq, lake_ssizeof(T), lake_salignof(T), #T, \
            lake_spinlock_acquire, lake_spinlock_release, lock, pop, out_item, __VA_ARGS__)

/** Dequeue an item from beginning of deque.
 *  @return Non-negative index on success. */
#define lake_deque_shift_t(q, T, out_item, ...) \
    __lake_deque_op(deq, lake_ssizeof(T), lake_salignof(T), #T, shift, out_item, __VA_ARGS__)

/** Dequeue an item from beginning of deque, protected by non-recursive spinlock.
 *  @return Non-negative index on success. */
#define lake_deque_shift_w_spinlock(q, T, out_item, lock, ...) \
    __lake_deque_op_locked(deq, lake_ssizeof(T), lake_salignof(T), #T, \
            lake_spinlock_acquire, lake_spinlock_release, lock, shift, out_item, __VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */
