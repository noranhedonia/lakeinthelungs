#pragma once

/** @file lake/data_structures/switch_list.h 
 *  @brief Interleaved linked list for storing mutually exclusive values.
 *
 *  TODO docs 
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/darray.h>
#include <lake/data_structures/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_switch_node {
    u32 next, prev;
} lake_switch_node;

typedef struct lake_switch_page {
    lake_darray_t(lake_switch_node) nodes;
    lake_darray_t(u64)              values; 
} lake_switch_page;

typedef struct lake_switch {
    lake_map                        hdrs;
    lake_darray_t(lake_switch_page) pages;
} lake_switch;

#ifdef __cplusplus
}
#endif /* __cplusplus */
