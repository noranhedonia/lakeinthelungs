#pragma once

/** @file lake/riven.h
 *  @brief ECS (Entity-component-system).
 *
 *  TODO docs. 
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** IDs are what can be added to an entity. It represents an entity or a pair,
 *  and can have optional id flags. */
typedef u64 riven_id;

/** An entity identifier. Entity IDs consist out of an index unique to the entity 
 *  in the lower 32 bits, and a reference counter in the upper 32 bits. When these 
 *  IDs are recycled, it's generation count is increased. This causes recycled IDs 
 *  to be very large (over 4 billion - 4GiB), which is normal. */
typedef riven_id riven_entity;

/** A type is a list of (component) IDs. They are used to communicate the type of 
 *  an entity. In ECS an entity can have multiple components, which is why an ECS 
 *  type consists of a vector of IDs.
 *
 *  The component IDs of a type are sorted, which ensures that it doesn't matter 
 *  in which order components are added to an entity. 
 *
 *  Entities are grouped together by type in the ECS storage in tables. The storage 
 *  has exactly one table per unique type that is created by the application, that 
 *  stores all entities and components for that type. 
 *  This is also referred to as an archetype. */
typedef struct riven_type {
    riven_id   *v;      /**< Array with IDs. */
    s32         count;  /**< Number of elements in array. */
} riven_type;

/** The encore is a container for all ECS data and supporting features. Applications 
 *  can have multiple encores, though in most cases will only need one - this is still 
 *  usefull for testing. Encores are isolated from each other, and can have separate 
 *  sets of systems, components, modules etc. */
typedef struct riven_encore riven_encore;

/** A stage enables modification while iterating and from multiple threads. */
typedef struct riven_stage riven_stage;

/** A table stores entities and components for an unique type. */
typedef struct riven_table riven_table;

/** A term is a single element in a query. */
typedef struct riven_term riven_term;

/** A query returns entities matching a list of constraints. */
typedef struct riven_query riven_query;

/** An observer is a system that is invoked when an event matches its query.
 *  Observers allow applications to respond to specific events, such as adding 
 *  or removing a component. Observers are created by both specifying a query 
 *  and a list of event kinds that should be listened for. Observers only 
 *  trigger when the source of the event matches the full observer query. */
typedef struct riven_observer riven_observer;

/** An observable produces events that can be listened for by an observer.
 *  Only encores and queries should be observable. */
typedef struct riven_observable riven_observable;

/** Type used for iterating over iterable vector objects. These iterators provide 
 *  the applications with info about the currently iterated result, and store any 
 *  state required for the iteration process. */
typedef struct riven_iter riven_iter;

/** A ref is a fast way to fetch a component for a specific entity. References are a 
 *  faster alternative to repeatedly acquiring the same entity/component combination.
 *  Refs achieve this performance by caching internal data structures associated with 
 *  the entity and component on the ref object that otherwise would have to be looked up. */
typedef struct riven_ref riven_ref;

/** Type hooks are callbacks associated with component lifecycle events. Typical examples
 *  of lifecycle events are construction, destruction, copying and moving of components. */
typedef struct riven_type_hooks riven_type_hooks;

/** Contains information about a component type, such as its size, alignment and type hooks. */
typedef struct riven_type_info riven_type_info;

/** Information about an entity like its table and row. */
typedef struct riven_record riven_record;

/** Information about a component id like type info and tables with the id. */
typedef struct riven_component_record riven_component_record;

/** Information about a table record defining it's archetype. */
typedef struct riven_table_record riven_table_record;

/** A polymorph object has a variable list of capabilities, determined by a mixin table. 
 *  Encores, stages and queries can be polymorphed. Functions that accept an riven_poly 
 *  argument can accept objects of these types. If the object does not have the requested 
 *  mixin the engine will throw an assertion.
 *
 *  A poly/mixin framework enables partially overlapping features to be implemented once, 
 *  and enables objects of different types to interact with each other depending on what 
 *  mixins they have, rather than their type. Additionally, every poly object has a header 
 *  that enables the API to do sanity checks on the input arguments. */
typedef void riven_poly;

/** Type that stores poly mixins. */
typedef struct riven_mixins riven_mixins;

/** Header for riven_poly objects. */
typedef struct riven_header {
    s32             type;       /**< Magic number indicating of which type is an object. */
    lake_refcnt     refcnt;     /**< Reference count for handles. */
    riven_mixins   *mixins;     /**< Table with offsets to optional mixins. */
} riven_header;;

/** Function prototype for runnables (systems, observers). The run callback overrides the default 
 *  behaviour for iterating through the results of a runnable object. The default runnable iterates
 *  the iterator, and calls an PFN_riven_iter_work for each returned result. */
PFN_LAKE_WORK(PFN_riven_run_work, riven_iter *it);
#define FN_RIVEN_RUN_WORK(fn) \
    FN_LAKE_WORK(fn, riven_iter *it)

/** Function prototype for iterables. A system may invoke a callback multiple times, 
 *  typically once for each matched table. */
PFN_LAKE_WORK(PFN_riven_iter_work, riven_iter *it);
#define FN_RIVEN_ITER_WORK(fn) \
    FN_LAKE_WORK(fn, riven_iter *it)

/** Function prototype for iterating an iterator. Stored inside initialized iterators.
 *  This allows an application to iterate an iterator without needing to know what created it. */
typedef bool (LAKECALL *PFN_riven_iter_next_work)(riven_iter *it);
#define FN_RIVEN_ITER_NEXT_WORK(fn) \
    bool LAKECALL fn(riven_iter *it)

/** Function prototype for releasing an iterator. */
PFN_LAKE_WORK(PFN_riven_iter_fini_work, riven_iter *it);
#define FN_RIVEN_ITER_FINI_WORK(fn) \
    FN_LAKE_WORK(fn, riven_iter *it)

/** Function prototype for comparing components. */
typedef s32 (LAKECALL *PFN_riven_order_by_work)(riven_entity e1, void const *data1, riven_entity e2, void const *data2);
#define FN_RIVEN_ORDER_BY_WORK(fn) \
    s32 LAKECALL fn(riven_entity e1, void const *data1, riven_entity e2, void const *data2)

/** Function prototype for sorting the entire table of components. */
typedef void (LAKECALL *PFN_riven_sort_table_work)(
        riven_encore           *encore, 
        riven_table            *table,
        riven_entity           *entities,
        void                   *data,
        s32                     size,
        s32                     lo,
        s32                     hi,
        PFN_riven_order_by_work order_by);
#define FN_RIVEN_SORT_TABLE_WORK(fn) \
    void LAKECALL fn(riven_encore *encore, riven_table *table, riven_entity *entities, \
            void *data, s32 size, s32 lo, s32 hi, PFN_riven_order_by_work order_by)

/** Function prototype for grouping tables in a query. */
typedef u64 (LAKECALL *PFN_riven_group_by_work)(riven_encore *encore, riven_table *table, riven_id group_id, void *ctx);
#define FN_RIVEN_GROUP_BY_WORK(fn) \
    u64 LAKECALL fn(riven_encore *encore, riven_table *table, riven_id group_id, void *ctx)

/** Function prototype for when a query creates a new group.
 *  `group_by_ctx` is from riven_query_desc. */
typedef void *(LAKECALL *PFN_riven_group_create_work)(riven_encore *encore, riven_id group_id, void *group_by_ctx);
#define FN_RIVEN_GROUP_CREATE_WORK(fn) \
    void *LAKECALL fn(riven_encore *encore, riven_id group_id, void *group_by_ctx)

/** Function prototype for when a query deletes an existing group.
 *  `group_ctx` is a return value from PFN_riven_group_work.
 *  `group_by_ctx` is from riven_query_desc. */
typedef void (LAKECALL *PFN_riven_group_delete_work)(riven_encore *encore, riven_id group_id, void *group_ctx, void *group_by_ctx);
#define FN_RIVEN_GROUP_DELETE_WORK(fn) \
    void LAKECALL fn(riven_encore *encore, riven_id group_id, void *group_ctx, void *group_by_ctx)

/** Function prototype for a constructor/destructor. */
typedef void (LAKECALL *PFN_riven_xtor)(void *ptr, s32 count, riven_type_info const *type_info);
#define FN_RIVEN_XTOR(fn) \
    void LAKECALL fn(void *ptr, s32 count, riven_type_info const *type_info)

/** Function prototype for when a component is copied into another component. */
typedef void (LAKECALL *PFN_riven_copy)(void *dst, void const *src, s32 count, riven_type_info const *type_info);
#define FN_RIVEN_COPY(fn) \
    void LAKECALL fn(void *dst, void const *src, s32 count, riven_type_info const *type_info)

/** Function prototype for when a component is moved to another component. */
typedef void (LAKECALL *PFN_riven_move)(void *dst, void *src, s32 count, riven_type_info const *type_info);
#define FN_RIVEN_MOVE(fn) \
    void LAKECALL fn(void *dst, void *src, s32 count, riven_type_info const *type_info)

/** Function prototype for comparing component instances. */
typedef s32 (LAKECALL *PFN_riven_cmp)(void const *a, void const *b, riven_type_info const *type_info);
#define FN_RIVEN_CMP(fn) \
    s32 LAKECALL fn(void const *a, void const *b, riven_type_info const *type_info)

/** Function prototype for equals operator hook. */
typedef s32 (LAKECALL *PFN_riven_equals)(void const *a, void const *b, riven_type_info const *type_info);
#define FN_RIVEN_EQUALS(fn) \
    s32 LAKECALL fn(void const *a, void const *b, riven_type_info const *type_info)

/** Function prototype for poly object destructor. */
PFN_LAKE_WORK(PFN_riven_poly_zero_refcnt, riven_poly *poly);
#define FN_RIVEN_POLY_ZERO_REFCNT(fn) \
    FN_LAKE_WORK(fn, riven_poly *poly)

#ifdef __cplusplus
}
#endif /* __cplusplus */
