#pragma once

/** @file lake/bedrock/drifter.h 
 *  @brief A fiber-aware linear allocator.
 *
 *  Every fiber internally owns a drifter, that works like an arena allocator
 *  automatically releasing resources on expected scheduling points or explicitly
 *  paired push and pop operations. Memory allocations done this way have no 
 *  synchronization overhead, have no free(ptr) interface, are safe from fiber 
 *  migrating between threads, behaving just like a stack on heap memory, and 
 *  are stupidly fast - thus are ideal for transient resources, whose lifetime 
 *  is just for the duration of an algorithm. A drifter can allocate memory of 
 *  any size, where best performance and lock-less mapping is for allocations 
 *  smaller than 2 MiB, as we use same allocation strategies used in tagged heaps.
 *
 *  The scheduler is responsible for managing the allocated resources, and so 
 *  if a function leaves from it's fiber scope (e.g. returning from a yield),
 *  the memory allocated inside this scope becomes invalid.
 */
#include <lake/bedrock/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Allocate transient resources with an automatic release. Their lifetime is tighly 
 *  tied with a fiber (or an explicit scope from usercode), will be preserved at yield, 
 *  and then released after the fiber is done with it's work. Because of the nature of 
 *  these allocations, they are stupidly fast, free from memory leaks (on correct use), 
 *  they form a hierarchy of lifetime scope, are lockless and completely thread safe. */
LAKEAPI LAKE_HOT_FN LAKE_MALLOC
void *LAKECALL lake_drift_allocate(usize size, usize align);

#define lake_drift_allocate_t(T) \
    lake_reinterpret_cast(T *, lake_drift_allocate(sizeof(T), alignof(T)))
#define lake_drift_allocate_n(T, n) \
    lake_reinterpret_cast(T *, lake_drift_allocate(sizeof(T) * (n), alignof(T)))

/** Expected to be used in macro allocator expansions. */
#define lake_drifter lake_drift_allocate, (void)

/** An alias will not append to the drifters offset. This call will be similar to 
 *  calling push, drift and pop in sequence. It's generally unsafe if for the lifetime 
 *  of this resource another drift allocation happens, which can happen internally, 
 *  thus aliasing an allocation should happen only in a controlled scope. */
LAKEAPI LAKE_HOT_FN
void *LAKECALL lake_drift_alias(usize size, usize align);

enum : s32 {
    _lake_drift_scratch_push = 0,
    _lake_drift_scratch_pop,
};

/** Enter or leave the current drift scope. Drift allocations are only valid inside the 
 *  scope they were called in, where a fiber context is an implicit scope. By calling this 
 *  function an userspace scope may be created or completed, this works recursively. It's 
 *  only thread safe if a matching number of entries and leaves was called from within 
 *  the work function, before the fiber returns to it's home context. Yields are safe. */
LAKEAPI LAKE_HOT_FN 
void LAKECALL _lake_drift_scratch(s32 op);

/** Nest a new scope. */
#define lake_drift_push() _lake_drift_scratch(_lake_drift_scratch_push)

/** Leave the current scope. */
#define lake_drift_pop() _lake_drift_scratch(_lake_drift_scratch_pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */
