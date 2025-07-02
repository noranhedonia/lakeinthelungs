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

typedef u64 lake_dagraph_id;
#define LAKE_DAGRAPH_ID_INVALID (UINT64_MAX)

typedef u32 lake_dagraph_ctx;

/** An edge represents a dependency. */
typedef u32 lake_dagraph_edge;
typedef enum lake_dagraph_edge_bits : lake_dagraph_edge {
    lake_dagraph_edge_strong       = (1u << 0),
    lake_dagraph_edge_weak         = (1u << 1),
    lake_dagraph_edge_data         = (1u << 2),
} lake_dagraph_edge_bits;

/** A node represents a state, a change, a pass, some movement. */
typedef struct LAKE_ALIGNMENT(64) lake_dagraph_node {
    /** Other nodes can use it to refer this node. */
    lake_dagraph_id             id;
    /** A context, what this node represents internally. */
    lake_dagraph_ctx            ctx;
    /** Atomic reference count for tracking parallel execution. */
    lake_refcnt                 refcnt;
    /** Nodes this node depends on (a strong/weak link, data flow). */
    lake_dagraph_id            *dependencies;
    /** A flag describing the edge, matching dependencies. */
    lake_dagraph_edge          *edges;
    /** Nodes that directly depend on this node. */
    lake_dagraph_id            *dependents;
    /** Function implementing this node. */
    PFN_lake_work               work;
    /** Data for work above. */
    void                       *payload;
    /** Counts for the arrays above. */
    u32                         dependencies_count, dependents_count;
} lake_dagraph_node;

/** Directed acyclic graph container, sub-graphs can be constructed from existing graphs. */
typedef struct lake_dagraph {
    lake_darray                 nodes;              /**< darray<lake_dagraph_node> */
    lake_darray                 execution_order;    /**< darray<lake_dagraph_id> */
} lake_dagraph;

#ifdef __cplusplus
}
#endif /* __cplusplus */
