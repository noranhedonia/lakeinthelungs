#pragma once

#include <lake/compiler.h>
#include <lake/platforms.h>
#include <lake/sanitize.h>
#include <lake/arch.h>
#include <lake/alignment.h>
#include <lake/types.h>
#include <lake/atomic.h>
#include <lake/complex.h>
#include <lake/endian.h>
#include <lake/magic.h>
#include <lake/time.h>
#include <lake/simd.h>
#include <lake/work.h>
#include <lake/log.h>
#include <lake/defer.h>
#include <lake/tagged_heap.h>

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
#define lake_memmove    memmove
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
        /** Number of empty tagged heaps to prepare. This value only references to user defined tags. */
        u32         tagged_heap_count;
        /** Number of threads to create. If 0, default will be the system's CPU count. 
         *  Worker threads have CPU affinity, thus we don't allow to create more threads than CPUs available. */
        u32         worker_thread_count;
        /** Every fiber will have a stack of this size. If 0, default will be 64KB. */
        u32         fiber_stack_size;
        /** Number of fibers to create. If 0, default will be 96 + 4 * worker_thread_count. */
        u32         fiber_count;
        /** The job queue will be of this size: (1u << log2_job_count). If 0, default will be 10 (1024). */
        u32         log2_work_count;
        /** How many frames can the CPU get ahead of the GPU. Usually 2-4. */
        u32         frames_in_flight;
        /** Explicit debug tools will be enabled, may be limited on release/NDEBUG builds.
         *  By default disabled, unless LAKE_SANITIZE is defined. Value -1 disables always. */
        s32         enable_debug_instruments;
    } hints;
    u64             timer_start;
} lake_framework;

/** Used to implement different interfaces. */
typedef LAKE_NODISCARD void *(LAKECALL *PFN_lake_interface_impl)(void const *assembly);
#define FN_LAKE_INTERFACE_IMPL(T, fn, data) \
    LAKE_NODISCARD struct T##_impl *LAKECALL T##_interface_impl_##fn(data const *assembly)

/** Systems can use an atomic counter for references to itself. A reference count of 0
 *  or less means that the system can be safely destroyed, as it is no longer in use. */
typedef atomic_s32 lake_refcnt;

/** Increment a reference count. 
 *  @return previous refcnt. */
LAKE_FORCE_INLINE s32 lake_inc_refcnt(lake_refcnt *refcnt)
{ return lake_atomic_add_explicit(refcnt, 1, lake_memory_model_release); }

/** Decrement a reference count, run the zero refcnt callback inline.
 *  @return previous refcnt. */
LAKE_FORCE_INLINE s32 lake_dec_refcnt(lake_refcnt *refcnt, void *self, PFN_lake_work zero_refcnt) 
{
    s32 __prev = lake_atomic_sub_explicit(refcnt, 1, lake_memory_model_release);
    if (__prev <= 1) zero_refcnt(self);
    return __prev;
}

/** Quickly wrap a handle in the header view union. */ \
#define lake_impl_v(T, v) \
    (T){ .impl = v }

/** An interface header for other systems to inherit. Used to abstract over implementations. */
LAKE_CACHELINE_ALIGNMENT
typedef struct lake_interface_header {
    lake_framework const   *framework;
    /** Can be optionally used for shared contextualizing of internal state. */
    atomic_u32              flags;
    /** Tracks reference count to this interface. */
    lake_refcnt             refcnt;
    /** Will be called to destroy the interface, the work argument is self. */
    PFN_lake_work           zero_refcnt;
    /** An unique name of the interface implementation, can be colored. */
    lake_small_string       name;
} lake_interface_header;

/** Defines an union for the interface. */
#define LAKE_DECL_INTERFACE(I)                          \
    typedef union I##_interface {                       \
        lake_interface_header      *header;             \
        struct I##_interface_impl  *interface;          \
        struct I##_impl            *impl;               \
        void                       *v;                  \
    } I##_interface;                                    \
                                                        \
    LAKE_FORCE_INLINE s32 I##_interface_ref(I##_interface self) \
    { return lake_inc_refcnt(&self.header->refcnt); }   \
                                                        \
    LAKE_FORCE_INLINE s32 I##_interface_unref(I##_interface self) { \
        return lake_dec_refcnt(                         \
                &self.header->refcnt,                   \
                self.v,                                 \
                self.header->zero_refcnt);              \
    }

/** Defines an opaque handle with a private implementation. */
#define LAKE_DECL_HANDLE(T) \
    typedef struct T##_impl *T 

/** Defines an opaque handle with a public interface layout. */
#define LAKE_DECL_HANDLE_INTERFACED(T, parent)  \
    typedef union T {                           \
                            parent;             \
        struct T##_header  *header;             \
        struct T##_impl    *impl;               \
        void               *v;                  \
    } T; \

/** For a public interface, defines the following:
 *  - a `header` structure to access the known internal layout of an opaque handle,
 *  - a `v` union to view into the header and handle.
 *  - inline functions to increment and decrement the reference count. 
 *  - inline function to submit the zero_refcnt call into work for the job system.
 *  - inline function to get a pointer to the saved assembly structure. */
#define LAKE_IMPL_HANDLE_INTERFACED(T, parent, ...)                             \
    /** Header for `struct T##_impl`. */                                        \
    typedef struct T##_header {                                                 \
                        parent;                                                 \
        atomic_u32      flags;                                                  \
        lake_refcnt     refcnt;                                                 \
        PFN_lake_work   zero_refcnt;                                            \
        T##_assembly    assembly;                                               \
                        __VA_ARGS__                                             \
    } T##_header;                                                               \
                                                                                \
    LAKE_FORCE_INLINE s32 T##_ref(T self)                                       \
    { return lake_inc_refcnt(&self.header->refcnt); }                           \
                                                                                \
    LAKE_FORCE_INLINE s32 T##_ref_v(struct T##_impl *self)                      \
    { return T##_ref(lake_impl_v(T, self)); }                                   \
                                                                                \
    LAKE_FORCE_INLINE s32 T##_unref(T self) {                                   \
        return lake_dec_refcnt(                                                 \
                &self.header->refcnt,                                           \
                self.v,                                                         \
                self.header->zero_refcnt);                                      \
    }                                                                           \
                                                                                \
    LAKE_FORCE_INLINE s32 T##_unref_v(struct T##_impl *self)                    \
    { return T##_unref(lake_impl_v(T, self)); }                                 \
                                                                                \
    LAKE_FORCE_INLINE T##_assembly const *T##_read_asm(T self)                  \
    { return &self.header->assembly; }                                          \
                                                                                \
    LAKE_FORCE_INLINE T##_assembly const *T##_read_asm_v(struct T##_impl *self) \
    { return &lake_impl_v(T, self).header->assembly; }

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
