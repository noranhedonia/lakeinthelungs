#pragma once

/** @file lake/work.h 
 *  @brief A fiber-based job system.
 *
 *  This fiber-based job system is implemented based on ideas presented by Naughty Dog 
 *  in the GDC2015 talk "Parallelizing the Naughty Dog Engine using Fibers". 
 *
 *      [Naughty Dog Video]
 *      http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
 *
 *      [Naughty Dog Slides]
 *      http://twvideo01.ubm-us.net/o1/vault/gdc2015/presentations/Gyrling_Christian_Parallelizing_The_Naughty.pdf
 *
 *  Since the job system is multithreaded, the work is not guaranteed to be executed 
 *  in the order that it was submitted. Work runs in parallel, we'll, that's the deal.
 *  Yielding jobs doesn't "block" in the traditional sense, instead the current context 
 *  of execution is switched with a fiber that contains details of a new job to run. 
 *  Only when all work of a single submission is done, a yielding fiber may resume.
 *
 *  Context switching is very CPU specific. This functionality is implemented in assembly
 *  for every architecture and platform ABI that should be supported. A great deal of help 
 *  in implementing this is the source code of Boost C++ fiber context library. It has most 
 *  of this stuff figured out, with some documentation for different ABI's in place for a 
 *  lot of different architectures.
 *
 *      [Boost::Context]
 *      https://github.com/boostorg/context
 *
 *      [Fibers, Oh My!]
 *      https://graphitemaster.github.io/fibers/
 *
 *  Because the fiber stacks are created as a large flat array, there is NO protection from 
 *  a stack overflow. It will just walk into the neighbouring fiber's stack and corrupt it.
 *
 *  Fundamental to the fiber code is thread local storage (TLS). It's used as a way for the 
 *  system to communicate between jobs. Instead of using an OS or compiler provided TLS,
 *  I'm rolling out my own using a hash lookup on the current thread id. That is because:
 *
 *  - '__thread' on GCC is a GCC specific extension.
 *  - '__declspec(thread)' on windows only supports statically linked libraries.
 *  - 'thread_local' in C++11 is.. C++. I do mostly C here.
 *  - 'TlsAlloc' is uh, just too much work. Lazy motherfucker..
 *  - '_Thread_local' in C11 is nice, but the compiler support for C11 threads is questionable.
 *
 *  A job is defined with the following function signature: void function(void *). The typedef 
 *  `PFN_lake_work` can be used to cast compatible functions of different argument type into 
 *  a pointer that the job system may accept. Work is submitted by providing an array of details 
 *  for every job to run as `struct lake_work[]`. A `lake_work_chain` is an atomic counter 
 *  used for implicit context switches on yielding jobs.
 *
 *  Some notes:
 *
 *  - A deadlock happens if the MPMC ring buffer is full and running from only one thread. 
 *    This will also happen when dequeuing from an empty buffer, when running a single thread.
 *    The reason being, that the waiting for enqueue/dequeue thread may never leave the loop,
 *    when there is no other thread to try to empty/fill it. This should not really be an issue 
 *    unless stress testing the job system on single core environments.
 *
 *  - The free and wait list implementations are pretty naive. There should be a better lockless 
 *    way of accessing them as opposed to looping over them. I suspect there is a lot of cache 
 *    churn and false sharing around index 0 between lots of threads.
 *
 *  - May get into improving context switching branch prediction, if this can improve speed: 
 *    http://www.crystalclearsoftware.com/soc/coroutine/coroutine/linuxasm.html
 */
#include <lake/sanitize.h>
#include <lake/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Defines a job procedure. */
typedef void (LAKECALL *PFN_lake_work)(void *userdata);

/** Declares a procedure with a job-compatible signature, can be cast into `PFN_lake_work`. */
#define PFN_LAKE_WORK(pfn, arg) typedef void (LAKECALL *pfn)(arg)
/** Declares a job, can be cast into `PFN_lake_work`. */
#define FN_LAKE_WORK(fn, arg) void LAKECALL fn(arg)

/** Details of the job description. */
typedef struct lake_work_details {
    PFN_lake_work   procedure;  /**< Job to run. */
    void           *argument;   /**< Data for the job. */
    const char     *name;       /**< A fiber will adopt this name for profiling. */
} lake_work_details;

/** An atomic counter bound to a work submit. If used within a call to the job system,
 *  this chain is used to "wait" for the work to finish. A fiber that yields while waiting 
 *  for the submit to return, instead of blocking or busy-waiting, will implicitly perform 
 *  a context switch. This synchronization work is completely hidden from the user. */
typedef atomic_usize *lake_work_chain;

/** Acquire an external chain not bound to any valid work. When this chain is given to 
 *  `lake_yield()`, the fiber will wait until this chain is released by another thread. */
LAKEAPI LAKE_HOT_FN
lake_work_chain LAKECALL lake_acquire_chain_n(usize initial_value);
#define lake_acquire_chain() lake_acquire_chain_n(1)

/** Release a chain acquired externally to a work submission. */
LAKE_FORCE_INLINE void lake_release_chain(lake_work_chain chain)
{ lake_atomic_write_explicit(chain, 0lu, lake_memory_model_release); }

/** Returns an index of the worker thread the current fiber is running on. This index 
 *  can be used to access an array of per-thread data structures. The index is acquired 
 *  by a hash lookup of the worker thread ID. If this function is called from a thread 
 *  that does not run on a fiber (e.g. outside of the framework), 0 is returned. */
LAKEAPI LAKE_HOT_FN LAKE_PURE_FN 
u32 LAKECALL lake_worker_thread_index(void);

/** Submits `work_count` of work to the job queue, using details provided by the array of `work`.
 *  This function will return IMMEDIATELY, and the given work will be resolved in the background 
 *  running on different worker threads. If `out_chain` is not nullptr, it will be set to a value 
 *  that the user can use to synchronize with the work to finish. */
LAKEAPI LAKE_THREAD_SAFETY_ACQUIRE_SHARED(3) LAKE_NONNULL(2) LAKE_HOT_FN 
void LAKECALL lake_submit_work(
    u32                      work_count, 
    lake_work_details const *work, 
    lake_work_chain         *out_chain);

/** If chain is not NULL, the fiber will yield and won't resume until the completion of work 
 *  that is chained. Otherwise if no chain is given, then the fiber may or may not yield to 
 *  the job system before returning. The chain becomes invalidated and any more yields will 
 *  be asserted, as they indicate innapropriate synchronization work. */
LAKEAPI LAKE_THREAD_SAFETY_RELEASE_SHARED(1) LAKE_HOT_FN 
void LAKECALL lake_yield(lake_work_chain chain);

/** Combines the effects of `lake_submit()` and `lake_yield()` into a single call.
 *  This function does not return until the completion of all submitted work. */
LAKE_FORCE_INLINE void lake_submit_work_and_yield(
    u32                      work_count, 
    lake_work_details const *work)
{
    lake_work_chain chain = nullptr;
    lake_submit_work(work_count, work, &chain);
    lake_yield(chain);
}

/** Allocate transient resources with an automatic release. Their lifetime is tighly 
 *  tied with a fiber (or an explicit scope from usercode), will be preserved at yield, 
 *  and then released after the fiber is done with it's work. Because of the nature of 
 *  these allocations, they are stupidly fast, free from memory leaks (on correct use), 
 *  they form a hierarchy of lifetime scope, are lockless and completely thread safe. */
LAKEAPI LAKE_HOT_FN LAKE_MALLOC
void *LAKECALL lake_drift(usize size, usize align);

#define lake_drift_t(T) \
    lake_reinterpret_cast(T *, lake_drift(sizeof(T), alignof(T)))
#define lake_drift_n(T, n) \
    lake_reinterpret_cast(T *, lake_drift(sizeof(T) * (n), alignof(T)))

enum : s32 {
    __lake_drift_depth_op_entry__ = 0,
    __lake_drift_depth_op_leave__,
};

/** Enter or leave the current drift scope. Drift allocations are only valid inside the 
 *  scope they were called in, where a fiber context is an implicit scope. By calling this 
 *  function an userspace scope may be created or completed, this works recursively. It's 
 *  only thread safe if a matching number of entries and leaves was called from within 
 *  the work function, before the fiber returns to it's home context. Yields are safe. */
LAKEAPI LAKE_HOT_FN 
void LAKECALL lake_drift_depth_op(s32 depth);

/** Nest a new scope. */
#define lake_drift_push() lake_drift_depth_op(__lake_drift_depth_op_entry__)

/** Leave the current scope. */
#define lake_drift_pop() lake_drift_depth_op(__lake_drift_depth_op_leave__)

#ifdef __cplusplus
}
#endif /* __cplusplus */
