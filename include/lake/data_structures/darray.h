#pragma once

/** @file lake/data_structures/darray.h 
 *  @brief A set of macros for managing dynamically-allocated arrays.
 *  
 *  It removes the tedium of managing realloc'd arrays with pointer, size, 
 *  and allocated size, with support for the custom allocators.
 */
#include <lake/bedrock/bedrock.h>

#include <lake/math/bits.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A dynamic array expected to change in size. */
typedef struct lake_darray {
    void *v;
    s32   len, cap;
} lake_darray;

/** Zero out the darray. */
#define lake_darray_clear() \
    (lake_darray){ .v = nullptr, .len = 0, .cap = 0 }

/** Reset data in darray. */
#define lake_darray_reset(da) ({ (da)->len = 0; })

#define lake_darray_len(da)     ((da)->len)
#define lake_darray_cap(da)     ((da)->cap)
#define lake_darray_empty(da)   ((da)->len == 0)

/** Resize the darray, allocate atleast stride*n for future resources. */
#define __lake_darray_resize_w_dbg(da, stride, align, n, type_name, allocator, deallocator) \
    ({ \
        lake_san_assert(stride != 0 && (n) > 0, LAKE_INVALID_PARAMETERS, "darray<"type_name">"); \
        s32 __len = n; \
        if (__len < (da)->len) \
            __len = (da)->len; \
        if ((da)->len == 0) { \
            (da)->v = allocator((stride) * (n), (align)); \
            (da)->len = 0; \
            (da)->cap = n; \
        } else if ((da)->cap != __len) { \
            (da)->cap = (stride) * (n); \
            void *__v = lake_memcpy(allocator((da)->cap, (align)), (da)->v, (da)->len); \
            deallocator((da)->v); \
            (da)->v = __v; \
        } \
    }) 
#define lake_darray_resize_w_dbg(da, stride, align, n, type_name, ...) \
    __lake_darray_resize_w_dbg(da, stride, align, n, type_name, __VA_ARGS__)
#define lake_darray_resize_t(da, T, n, ...) \
    __lake_darray_resize_w_dbg(da, lake_ssizeof(T), lake_salignof(T), n, #T, __VA_ARGS__)

/** Initialize the darray allocation. */
#define lake_darray_init_w_dbg(da, stride, align, n, type_name, ...) \
    ({ \
        *(da) = lake_darray_clear(); \
        __lake_darray_resize_w_dbg(da, stride, align, n, type_name, __VA_ARGS__); \
    })
#define lake_darray_init_t(da, T, n, ...) \
    lake_darray_init_w_dbg(da, lake_ssizeof(T), lake_salignof(T), n, #T, __VA_ARGS__)

/** Release the allocation and clear the array, allows optional free((da)->v). */
#define __lake_darray_fini(da, allocator, deallocator) \
    ({ \
        if ((da)->v) deallocator((da)->v); \
        *(da) = lake_darray_clear(); \
    })
#define lake_darray_fini(da, ...) \
    __lake_darray_fini(da, __VA_ARGS__)

#define lake_darray_first_t(da, T) \
    lake_reinterpret_cast(T *, (da)->v)

#define lake_darray_at_t(da, T, idx) \
    lake_reinterpret_cast(T *, lake_elem_t((da)->v, T, idx))

#define lake_darray_last_t(da, T) \
    lake_reinterpret_cast(T *, lake_elem_t((da)->v, T, (da)->len - 1))

#define lake_darray_beyond_t(da, T) \
    lake_reinterpret_cast(T *, lake_elem_t((da)->v, T, (da)->len))

#define lake_darray_pop(da) \
    ({ (da)->len--; })
#define lake_darray_popn(da, n) \
    ({ (da)->len = lake_max(0, (da)->len - n); })

/* PRIVATE: lock */
#define __lake_darray_op_locked(da, ACQ, REL, lock, op, ...) \
    ({ \
         void *__item; \
         ACQ(lock); \
         __item = op(da, __VA_ARGS__); \
         REL(lock); \
         __item; \
    })

/** Insert items after the end of the darray.
 *  @return Pointer to array where appended items begin. */
#define lake_darray_append_w_dbg(da, stride, align, n, items, type_name, ...) \
    ({ \
        s32 __idx = (da)->len; \
        if (__idx + n > (da)->cap) \
            lake_darray_resize_w_dbg(da, stride, align, __idx + n, type_name, __VA_ARGS__); \
        (da)->len = __idx + n; \
        lake_memcpy(lake_elem((da)->v, stride, __idx), items, (stride) * n); \
    })
#define lake_darray_append_w_spinlock(da, T, n, items, lock, ...) \
    ({ lake_reinterpret_cast(T *, __lake_darray_op_locked(da, lake_spinlock_acquire, lake_spinlock_release, lock, \
            lake_darray_append_w_dbg, lake_ssizeof(T), lake_salignof(T), n, items, #T, __VA_ARGS__)); })

#define lake_darray_append_t(da, T, n, items, ...) \
    ({ lake_reinterpret_cast(T *, lake_darray_append_w_dbg(da, lake_ssizeof(T), lake_salignof(T), n, items, #T, __VA_ARGS__)); })

/** Insert items into the array, starting at given index.
 *  Existing data will be moved upwards.
 *  @return Pointer to array where inserted items begin. */
#define lake_darray_insert_w_dbg(da, stride, align, n, items, at, type_name, ...) \
    ({ \
        s32 const __idx = (da)->len; \
        if (__idx + n > (da)->cap) \
            lake_darray_resize_w_dbg(da, stride, align, __idx + n, type_name, __VA_ARGS__); \
        (da)->len = __idx + n; \
        s32 const __range = lake_min(at, __idx); \
        lake_memmove(lake_elem((da)->v, stride, __range + n), lake_elem((da)->v, stride, __range), (stride) * (__idx - __range)); \
    })
#define lake_darray_insert_w_spinlock(da, T, n, items, at, lock, ...) \
    ({ lake_reinterpret_cast(T *, __lake_darray_op_locked(da, lake_spinlock_acquire, lake_spinlock_release, lock, \
            lake_darray_insert_w_dbg, lake_ssizeof(T), lake_salignof(T), n, items, at, #T, __VA_ARGS__)); })

#define lake_darray_insert_t(da, T, n, items, at, ...) \
    ({ lake_reinterpret_cast(T *, lake_darray_insert_w_dbg(da, lake_ssizeof(T), lake_salignof(T), n, items, at, #T, __VA_ARGS__)); })

/** Traverse over all elements in a dynamic array. 
 *  The `iter` is declared as a pointer to an item. */
#define lake_darray_foreach_t(da, T, iter) \
    for (T *(iter) = lake_darray_first_t(da, T); (iter) < lake_darray_beyond_t(da, T); (iter)++)

/** Traverse over all elements in a dynamic array in reverse.
 *  The `iter` is declared as a pointer to an item. */
#define lake_darray_foreach_reverse_t(da, T, iter) \
    for (T *(iter) = lake_darray_last_t(da, T); (iter)-- > lake_darray_first_t(da, T); )

#ifdef __cplusplus
}
#endif /* __cplusplus */
