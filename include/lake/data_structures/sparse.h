#pragma once

/** @file lake/data_structures/sparse.h 
 *  @brief Sparse set data structure.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/darray.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_sparse {
    /** Dense array with indices to sparse array. The dense array stores both alive and dead
     *  sparse indices. The `count` member keeps track of which sparse indices are alive. */
    lake_darray             dense;      /**< darray<u64> */
    lake_darray             pages;      /**< darray<internal>, chunks with sparse arrays and data. */
    s32                     elem_size;  /**< Size of an element in the sparse array. */
    s32                     count;      /**< Number of alive entries in the dense array. */
    u64                     max_idx;    /**< Local max index, if no global was set. */
} lake_sparse;

#ifdef __cplusplus
}
#endif /* __cplusplus */
