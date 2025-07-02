#pragma once

/** @file lake/data_structures/switch.h 
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
    u32                 next, prev;
} lake_switch_node;

typedef struct lake_switch_page {
    lake_darray         nodes;      /**< darray<lake_switch_node> */
    lake_darray         values;     /**< darray<u64> */
} lake_switch_page;

typedef struct lake_switch {
    lake_map            hdrs;
    lake_darray         pages;      /**< darray<lake_switch_page> */
} lake_switch;

#ifdef __cplusplus
}
#endif /* __cplusplus */
