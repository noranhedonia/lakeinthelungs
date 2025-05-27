#pragma once

/** @file lake/data_structures/darray.h 
 *  @brief A set of macros for managing dynamically-allocated arrays.
 *  
 *  It removes the tedium of managing realloc'd arrays with pointer, size, 
 *  and allocated size, with support for the custom allocators.
 */
#include <lake/bedrock/types.h>

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

/* TODO */

#ifdef __cplusplus
}
#endif /* __cplusplus */
