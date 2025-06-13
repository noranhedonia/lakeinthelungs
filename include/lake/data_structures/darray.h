#pragma once

/** @file lake/data_structures/darray.h 
 *  @brief A set of macros for managing dynamically-allocated arrays.
 *  
 *  It removes the tedium of managing realloc'd arrays with pointer, size, 
 *  and allocated size, with support for the custom allocators.
 */
#include <lake/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A dynamic array expected to change in size. */
typedef struct lake_darray {
    void *v;        /**< The dynamic array of data. */
    s32   size;     /**< The current size of the array. */
    s32   alloc;    /**< Size of the allocation for the array. */
} lake_darray;

/** Define a custom darray type. */
#define lake_darray_t(T) \
    union { lake_darray da; T *v; }

/** Initialize the dynamic array, allocate atleast bytes*n for future resources.
 *  TODO custom allocator? */
LAKEAPI void LAKECALL 
lake_darray_init_w_dbg(
    lake_darray    *da, 
    s32             stride, 
    s32             align, 
    s32             n, 
    char const     *type);    
#define lake_darray_init_t(da, T, n) \
    lake_darray_init_w_dbg(da, lake_ssizeof(T), lake_salignof(T), n, "darray<"#T">")

/** Release the allocation and clear the array.
 *  TODO custom allocator? */
#define lake_darray_fini(da) \
    do { \
        if ((da)->v) __lake_free((da)->v); \
        *(da) = (lake_darray){ nullptr, 0, 0 }; \
    } while(0)

#define lake_darray_size(da)    ((da)->size)
#define lake_darray_alloc(da)   ((da)->alloc)
#define lake_darray_empty(da)   ((da)->size == 0)
#define lake_darray_clear(da)   do {((da)->size = 0); } while(0)
#define lake_darray_pop(da)     do {((da)->size--); } while(0)
#define lake_darray_pop_n(da,n) do {((da)->size = lake_min(0, (da)->size - n)); } while(0)

#define lake_darray_elem_t(da, T, idx) \
    lake_reinterpret_cast(T *, lake_elem_t((da)->v, T, idx))

#define lake_darray_last_t(da, T) \
    lake_reinterpret_cast(T *, lake_elem_t((da)->v, T, (da)->size - 1))

#define lake_darray_beyond_t(da, T) \
    lake_reinterpret_cast(T *, lake_elem_t((da)->v, T, (da)->size))

#define lake_darray_first_t(da, T) \
    lake_reinterpret_cast(T *, (da)->v)

#define lake_darray_elem(va, idx) (&(va).v[idx])
#define lake_darray_last(va)      (&(va).v[(va).da.size - 1])
#define lake_darray_first(va)     ((va).v)

/** Copy all data inside the dynamic array.
 *  TODO custom allocator?
 *  @return a copy of the array. */
LAKEAPI lake_darray LAKECALL
lake_darray_copy(
    lake_darray const  *da, 
    s32                 stride, 
    s32                 align);
#define lake_darray_copy_t(da, T) \
    lake_darray_copy(da, lake_ssizeof(T), lake_salignof(T))

/** If possible, try to free memory unused memory.
 *  If the array is empty, the entire array will be released.
 *  TODO custom allocator? */
LAKEAPI void LAKECALL
lake_darray_reclaim(
    lake_darray    *da, 
    s32             stride, 
    s32             align);
#define lake_darray_reclaim_t(da, T) \
    lake_darray_reclaim(da, lake_ssizeof(T), lake_salignof(T))

/** Resize the dynamic array to a minimum size of (bytes*n).
 *  If the requested size is smaller than the current alloc, no change is done.
 *  TODO custom allocator? */
LAKEAPI void LAKECALL
lake_darray_resize(
    lake_darray    *da, 
    s32             stride, 
    s32             align, 
    s32             n);
#define lake_darray_resize_t(da, T, n) \
    lake_darray_resize(da, lake_ssizeof(T), lake_salignof(T), n)

/** Insert items into the end of the array.
 *  @return pointer into the array where items begin. */
LAKE_FORCE_INLINE
void *lake_darray_append_items(
    lake_darray    *da, 
    void const     *items, 
    s32             stride, 
    s32             align, 
    s32             n)
{
    s32 size = da->size;
    if (size + n - 1 >= da->alloc)
        lake_darray_resize(da, stride, align, size + n);
    da->size = size + n;
    return lake_memcpy(lake_elem(da->v, stride, size), items, stride * n);
}

#define lake_darray_append_n(da, T, items, n) \
    ({ \
        T *__item = lake_reinterpret_cast(T *, lake_darray_append_items(da, items, lake_ssizeof(T), lake_salignof(T), n)); \
        __item; \
    })

#define lake_darray_append_t(da, T, item) \
    ({ \
        T *__item = lake_reinterpret_cast(T *, lake_darray_append_items(da, item, lake_ssizeof(T), lake_salignof(T), 1)); \
        __item; \
    })


#define lake_darray_append_v_locked(va, T, item, n, spinlock) \
    ({ \
        T *___item; \
        lake_spinlock_acquire(spinlock); \
        ___item = lake_reinterpret_cast(T *, lake_darray_append_items(&(va).da, item, lake_ssizeof(T), lake_salignof(T), n)); \
        lake_spinlock_release(spinlock); \
        ___item; \
    })

/** Insert items into the array, starting from the index item.
 *  Existing data beginning from index will be moved upwards, keeping order.
 *  @return pointer into the array where items begin. */
LAKE_FORCE_INLINE
void *lake_darray_insert_items(
    lake_darray    *da, 
    void const     *items, 
    s32             stride, 
    s32             align, 
    s32             n, 
    s32             idx)
{
    s32 size = da->size;
    if (size + n - 1 >= da->alloc)
        lake_darray_resize(da, stride, align, size + n);
    da->size = size + n;
    /* don't leave the range */
    idx = lake_min(idx, size);
    lake_memmove(lake_elem(da->v, stride, idx + n), 
                 lake_elem(da->v, stride, idx), stride * (size - idx));
    return lake_memcpy(lake_elem(da->v, stride, idx), items, stride * n);
}

#define lake_darray_insert_n(da, T, items, n, idx) \
    ({ \
        T *__item = lake_reinterpret_cast(T *, lake_darray_insert_items(da, items, lake_ssizeof(T), lake_salignof(T), n, idx)); \
        __item; \
    })

#define lake_darray_insert_t(da, T, item, idx) \
    ({ \
        T *__item = lake_reinterpret_cast(T *, lake_darray_insert_items(da, item, lake_ssizeof(T), lake_salignof(T), 1, idx)); \
        __item; \
    })

#define lake_darray_insert_v_locked(va, T, item, n, spinlock) \
    ({ \
        T *___item; \
        lake_spinlock_acquire(spinlock); \
        ___item = lake_reinterpret_cast(T *, lake_darray_insert_items(&(va).da, item, lake_ssizeof(T), lake_salignof(T), n)); \
        lake_spinlock_release(spinlock); \
        ___item; \
    })

/** Remove items from the array, starting from the index item.
 *  Existing data beginning from index will be moved downwards, keeping order. */
LAKE_FORCE_INLINE
void lake_darray_remove_ordered_items(
    lake_darray    *da, 
    s32             stride, 
    s32             n, 
    s32             idx)
{
    s32 new_size = da->size = da->size - n;
    if (idx == new_size)
        return;
    lake_memmove(lake_elem(da->v, stride, idx),
                 lake_elem(da->v, stride, idx + n), stride * (new_size - idx));
}

#define lake_darray_remove_ordered_n(da, T, n, idx) \
    lake_darray_remove_ordered_items(da, lake_ssizeof(T), n, idx)

#define lake_darray_remove_ordered_t(da, T, idx) \
    lake_darray_remove_ordered_items(da, lake_ssizeof(T), 1, idx)

#define lake_darray_remove_ordered_v_locked(va, T, n, spinlock) \
    ({ \
        lake_spinlock_acquire(spinlock); \
        lake_darray_remove_ordered_items(&(va).da, lake_ssizeof(T), n, idx)); \
        lake_spinlock_release(spinlock); \
    })

/** Remove items from the array, starting from the index item.
 *  Last items in the array will be moved unordered, to pop the array.
 *  Should be faster than the ordered counterpart. */
LAKE_FORCE_INLINE
void lake_darray_remove_items(
    lake_darray    *da, 
    s32             stride, 
    s32             n, 
    s32             idx)
{
    s32 new_size = da->size = da->size - n;
    if (idx == new_size)
        return;
    lake_memcpy(lake_elem(da->v, stride, idx),
                lake_elem(da->v, stride, new_size), stride * n);
}

#define lake_darray_remove_n(da, T, n, idx) \
    lake_darray_remove_items(da, lake_ssizeof(T), n, idx)

#define lake_darray_remove_t(da, T, idx) \
    lake_darray_remove_items(da, lake_ssizeof(T), 1, idx)

#define lake_darray_remove_v_locked(va, T, n, spinlock) \
    ({ \
        lake_spinlock_acquire(spinlock); \
        lake_darray_remove_items(&(va).da, lake_ssizeof(T), n, idx)); \
        lake_spinlock_release(spinlock); \
    })

/** Union procedure, to traverse over all elements in a dynamic array. 
 *  The `iter` is declared as a pointer to an item. */
#define lake_darray_foreach_v(va, T, iter) \
    for (T *(iter) = &(va).v[0]; (iter) < &(va).v[(va).da.size]; (iter)++)

/** Union procedure, to traverse over all elements in a dynamic array in reverse.
 *  The `iter` is declared as a pointer to an item. */
#define lake_darray_foreach_reverse_v(va, T, iter) \
    for (T *(iter) = &(va).v[(va).da.size]; (iter)-- > &(va).v[0]; )

#ifdef __cplusplus
}
#endif /* __cplusplus */
