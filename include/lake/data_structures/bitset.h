#pragma once

/** @file lake/data_structures/bitset.h 
 *  @brief TODO docs 
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/darray.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    u64 *v;
    s32  size;
    s32  alloc;
} lake_bitset;

#ifdef __cplusplus
}
#endif /* __cplusplus */
