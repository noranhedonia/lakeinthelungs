#pragma once

#include <lake/bedrock/compiler.h>
#include <lake/bedrock/platforms.h>
#include <lake/bedrock/sanitize.h>
#include <lake/bedrock/arch.h>
#include <lake/bedrock/alignment.h>
#include <lake/bedrock/types.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/complex.h>
#include <lake/bedrock/endian.h>
#include <lake/bedrock/magic.h>
#include <lake/bedrock/time.h>
#include <lake/bedrock/simd.h>
#include <lake/bedrock/work.h>
#include <lake/bedrock/log.h>

#ifdef __cplusplus
#include <cstdlib>
#include <cstring>
#else
#include <stdlib.h>
#include <string.h>
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* rename them, but exploit compiler optimizations */
#define lake_memset     memset
#define lake_memcpy     memcpy
#define lake_memcmp     memcmp
#define lake_memrchr    memrchr
#define lake_strlen     strlen
#define lake_strncmp    strncmp
#define lake_strncpy    strncpy
#define lake_strchrnul  strchrnul
#define lake_strchr     strchr
#define lake_strrchr    strrchr

#define lake_zero(mem)  lake_memset(&(mem), 0, sizeof((mem)))
#define lake_zerop(mem) lake_memset((mem), 0, sizeof(*(mem)))
#define lake_zeroa(mem) lake_memset((mem), 0, sizeof((mem)))

/** Aligned OS allocator, used temporarily until i implement other strategies 
 *  for memory allocation, and how to integrate them with my fiber scheduler.
 *  They will later be replaced, and code will be refactored. TODO */
LAKE_HOT_FN 
LAKEAPI void *LAKECALL 
__lake_malloc(
    usize size, 
    usize align);

#define __lake_malloc_t(T)    lake_reinterpret_cast(T *, __lake_malloc(sizeof(T), alignof(T)))
#define __lake_malloc_n(T, n) lake_reinterpret_cast(T *, __lake_malloc(sizeof(T) * (n), alignof(T)))

/** Reallocates memory allocated from `__lake_malloc()`. */
LAKE_HOT_FN 
LAKEAPI void *LAKECALL 
__lake_realloc(
    void *ptr, 
    usize size, 
    usize align);

#define __lake_realloc_t(ptr, T)    __lake_realloc(ptr, sizeof(T), alignof(T))
#define __lake_realloc_n(ptr, T, n) __lake_realloc(ptr, sizeof(T) * (n), alignof(T))

/** Frees memory allocated from `__lake_malloc()`. */
LAKE_HOT_FN 
LAKEAPI void LAKECALL 
__lake_free(
    void *ptr);

/** Open a shared library. */
LAKE_NONNULL_ALL 
LAKEAPI void *LAKECALL 
lake_open_library(
    char const *libname);

/** Close a shared library. */
LAKE_NONNULL_ALL 
LAKEAPI void LAKECALL 
lake_close_library(
    void *library);

/** Get a procedure address from a shared library. */
LAKE_NONNULL_ALL LAKE_HOT_FN
LAKEAPI void *LAKECALL
lake_get_proc_address(
    void       *library,
    char const *procname);

/** Information about the application, given at main. After the framework initializes, it is immutable. */
typedef struct lake_framework {
    char const     *engine_name;
    char const     *app_name;
    u32             build_engine_ver;
    u32             build_app_ver;
    struct {
        /** Sets a hard limit on the physical resources the framework is allowed to use. If 0, default 
         *  will be the system's total RAM memory. This budget is used to reserve virtual address space. */
        usize       memory_budget;
        /** Target size for hugetlb entries. If 0, default will be the default huge page size (usually 2MB).
         *  If set at a non-zero value, this will serve as the limit - the lowest valid page size will be picked.
         *  Whenever the huge page size could not be resolved, normal page size is set (usually 4096KB). */
        u32         huge_page_size;
        /** Number of threads to create. If 0, default will be the system's CPU count. 
         *  Worker threads have CPU affinity, thus we don't allow to create more threads than CPUs available. */
        u32         worker_thread_count;
        /** Every fiber will have a stack of this size. If 0, default will be 64KB. */
        u32         fiber_stack_size;
        /** Number of fibers to create. If 0, default will be 96 + 4 * worker_thread_count. */
        u32         fiber_count;
        /** The job queue will be of this size: (1u << log2_job_count). If 0, default will be 10 (1024). */
        u32         log2_work_count;
        /** Explicit debug tools will be enabled, may be limited on release/NDEBUG builds.
         *  By default disabled, unless LAKE_SANITIZE is defined. Value -1 disables always. */
        s32         use_debug_instruments;
    } hints;
    u64             timer_start;
} lake_framework;

/** Defines an opaque handle with private implementation. */
#define LAKE_DECL_HANDLE(T) \
    typedef struct T##_impl *T 

#define LAKE_DECL_INTERFACE(T)                  \
    typedef union T##_interface {               \
        lake_interface_header      *header;     \
        struct T##_adapter_impl    *adapter;    \
        struct T##_interface_impl  *interface;  \
        void                       *v;          \
    } T##_interface;

/** Used to implement different interfaces. */
typedef LAKE_NODISCARD void *(LAKECALL *PFN_lake_interface_impl)(void const *assembly);
#define FN_LAKE_INTERFACE_IMPL(T, fn, data) \
    LAKE_NODISCARD struct T##_adapter_impl *LAKECALL T##_interface_impl_##fn(data const *assembly)

/** Systems can use an atomic counter for references to itself. A reference count of 0
 *  or less means that the system can be safely destroyed, as it is no longer in use. */
typedef atomic_s32 lake_refcnt;

LAKE_FORCE_INLINE LAKE_NONNULL_ALL
s32 lake_inc_refcnt(lake_refcnt *refcnt)
{ return lake_atomic_add_explicit(refcnt, 1, lake_memory_model_release); }

LAKE_FORCE_INLINE LAKE_NONNULL_ALL
s32 lake_dec_refcnt(lake_refcnt *refcnt, void *self, PFN_lake_work zero_ref_callback)
{ 
    s32 prev = lake_atomic_sub_explicit(refcnt, 1, lake_memory_model_release); 
    if (prev <= 1) zero_ref_callback(self);
    return prev;
}

/** An interface header for other systems to inherit. Used to abstract over implementations. */
LAKE_CACHELINE_ALIGNMENT
typedef struct lake_interface_header {
    lake_framework const   *framework;
    /** XXX For future use (and as a padding). */
    atomic_u32              flags;
    /** Tracks reference count to this interface. */
    lake_refcnt             refcnt;
    /** Will be called to destroy the interface, the work argument is self. */
    PFN_lake_work           zero_refcnt;
    /** An unique name of the interface implementation, can be colored. */
    lake_small_string       name;
} lake_interface_header;

/** Entry point defined by an application. */
typedef void (LAKECALL *PFN_lake_framework)(void *userdata, lake_framework const *framework);
#define FN_LAKE_FRAMEWORK(fn, work) \
    void LAKECALL fn(work, lake_framework const *framework)

/** Setups the framework from given hints, and passes the configuration back to an application 
 *  defined entry point. The initialization will process hints given in the `lake_framework` 
 *  structure, and on limits of the host machine. If hints are zeroed out, sane default values 
 *  will be provided. After init is done, a fiber calls into the `main` procedure, passing 
 *  in the self-defined userdata and immutable framework configuration.
 *
 *  A fiber-aware job queue with a capacity of (1 << log2_work_count is created. The queue 
 *  has NO concept of priority, so there is no job stealing either. `worker_thread_count` 
 *  system threads are created, each one locked to a CPU core. Every fiber of `fiber_count`
 *  has it's own stack region of `fiber_stack_size`. Virtual memory is mapped for internal 
 *  use (TODO and later for custom memory allocators) with a hard limit of `memory_budget`.
 *  The memory budget is aligned to a hugetlb page entry, and if no limit is provided the 
 *  virtual map is set to the amount of RAM in the host system. The `huge_page_size` works 
 *  as a ceiling, and the largest possible value under that will be used for the mapping
 *  (usually 2MB), and if huge pages are not available or the given hint is not valid, this 
 *  will default to the standard page size (usually 4096 bytes).
 *
 *  The framework and internal systems described above are all valid, until the application 
 *  returns from the `main` procedure. */
LAKE_NONNULL(1,3)
LAKEAPI s32 LAKECALL
lake_in_the_lungs(
    PFN_lake_framework main, 
    void              *userdata,
    lake_framework    *framework);

#ifdef __cplusplus
}
#endif /* __cplusplus */
