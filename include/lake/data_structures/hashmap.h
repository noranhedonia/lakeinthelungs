#pragma once

/** @file lake/data_structures/hashmap.h 
 *  @brief TODO docs 
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/darray.h>
#include <lake/data_structures/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_hashmap_bucket {
    lake_darray_t(lake_map_key)     keys;
    lake_darray_t(lake_map_value)   values;
} lake_hashmap_bucket;

typedef struct lake_hashmap {
    PFN_lake_hash_value_op          hash;
    PFN_lake_compare_op             compare;
    s32                             key_size;
    s32                             value_size;
    lake_map                        impl;
} lake_hashmap;

typedef struct lake_hashmap_iter {
    lake_hashmap_bucket            *bucket;
    ssize                           index;
    lake_map_iter                   impl;
} lake_hashmap_iter;

typedef struct lake_hashmap_result {
    void                           *v;
    void                           *key;
    u64                             hash;
} lake_hashmap_result;

#ifdef __cplusplus
}
#endif /* __cplusplus */
