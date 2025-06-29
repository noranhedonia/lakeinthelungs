#pragma once

/** @file lake/data_structures/dagraph.h 
 *  @brief Directed acyclic graph optimized for parallelism.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/darray.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef u32 lake_dagraph_id;

typedef struct lake_dagraph_node {
    lake_dagraph_id    *read;
    lake_dagraph_id    *write;
    PFN_lake_work       work;
    void               *context;
    lake_dagraph_id     read_count;
    lake_dagraph_id     write_count;
    lake_dagraph_id     id;
} lake_dagraph_node;

typedef struct lake_dagraph {
    /** Node storage. */
    lake_darray_t(lake_dagraph_node)    nodes;
    /** Topologically sorted nodes. */
    lake_dagraph_id                    *topology;
    /** Valid entries in sorted topology. */
    lake_dagraph_id                     topology_count;
} lake_dagraph;

#ifdef __cplusplus
}
#endif /* __cplusplus */
