#pragma once

/** @file lake/data_structures/map.h 
 *  @brief TODO docs 
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef u64           lake_map_data;
typedef lake_map_data lake_map_key;
typedef lake_map_data lake_map_value;

typedef struct lake_bucket_entry {
    lake_map_key                key;
    lake_map_value              value;
    struct lake_bucket_entry   *next;
} lake_bucket_entry;

typedef struct lake_bucket {
    lake_bucket_entry          *first;
} lake_bucket;

typedef struct lake_map {
    lake_bucket                *buckets;
    u32                         bucket_count;
    u32                         count : 26;
    u32                         bucket_shift : 6;
} lake_map;

typedef struct lake_map_iter {
    lake_map const             *map;
    lake_bucket                *bucket;
    lake_bucket_entry          *entry;
    lake_map_data              *res;
} lake_map_iter;

#ifdef __cplusplus
}
#endif /* __cplusplus */
