#pragma once

#include <lake/compiler.h>
#include <lake/platforms.h>

/* To enable analysis, set these environment variables before running meson:
 *     export CC=clang
 *     export CFLAGS="-DLAKE_THREAD_SAFETY_ANALYSIS -Wthread-safety" */
#if defined(LAKE_THREAD_SAFETY_ANALYSIS) && defined(LAKE_CC_CLANG_VERSION) && (!defined(SWIG))
    #define _LAKE_THREAD_SAFETY_ATTRIBUTE(x) __attribute__((x))
#else
    #define _LAKE_THREAD_SAFETY_ATTRIBUTE(x)
#endif

#define LAKE_THREAD_SAFETY_CAPABILITY(X)               _LAKE_THREAD_SAFETY_ATTRIBUTE(capability(x))
#define LAKE_THREAD_SAFETY_SCOPED_CAPABILITY           _LAKE_THREAD_SAFETY_ATTRIBUTE(scoped_lockable)
#define LAKE_THREAD_SAFETY_GUARDED_BY(X)               _LAKE_THREAD_SAFETY_ATTRIBUTE(guarded_by(x))
#define LAKE_THREAD_SAFETY_PT_GUARDED_BY(X)            _LAKE_THREAD_SAFETY_ATTRIBUTE(pt_guarded_by(x))
#define LAKE_THREAD_SAFETY_ACQUIRED_BEFORE(X)          _LAKE_THREAD_SAFETY_ATTRIBUTE(acquired_before(x))
#define LAKE_THREAD_SAFETY_ACQUIRED_AFTER(X)           _LAKE_THREAD_SAFETY_ATTRIBUTE(acquired_after(x))
#define LAKE_THREAD_SAFETY_REQUIRES(X)                 _LAKE_THREAD_SAFETY_ATTRIBUTE(requires_capability(x))
#define LAKE_THREAD_SAFETY_REQUIRES_SHARED(X)          _LAKE_THREAD_SAFETY_ATTRIBUTE(requires_shared_capability(x))
#define LAKE_THREAD_SAFETY_ACQUIRE(X)                  _LAKE_THREAD_SAFETY_ATTRIBUTE(acquire_capability(x))
#define LAKE_THREAD_SAFETY_ACQUIRE_SHARED(X)           _LAKE_THREAD_SAFETY_ATTRIBUTE(acquire_shared_capability(x))
#define LAKE_THREAD_SAFETY_RELEASE(X)                  _LAKE_THREAD_SAFETY_ATTRIBUTE(release_capability(x))
#define LAKE_THREAD_SAFETY_RELEASE_SHARED(X)           _LAKE_THREAD_SAFETY_ATTRIBUTE(release_shared_capability(x))
#define LAKE_THREAD_SAFETY_RELEASE_GENERIC(X)          _LAKE_THREAD_SAFETY_ATTRIBUTE(release_generic_capability(x))
#define LAKE_THREAD_SAFETY_TRY_ACQUIRE(X)              _LAKE_THREAD_SAFETY_ATTRIBUTE(try_acquire_capability(x))
#define LAKE_THREAD_SAFETY_TRY_ACQUIRE_SHARED(X)       _LAKE_THREAD_SAFETY_ATTRIBUTE(try_acquire_shared_capability(x))
#define LAKE_THREAD_SAFETY_EXCLUDES(X)                 _LAKE_THREAD_SAFETY_ATTRIBUTE(locks_excluded(x))
#define LAKE_THREAD_SAFETY_ASSERT_CAPABILITY(X)        _LAKE_THREAD_SAFETY_ATTRIBUTE(assert_capability(x))
#define LAKE_THREAD_SAFETY_ASSERT_SHARED_CAPABILITY(X) _LAKE_THREAD_SAFETY_ATTRIBUTE(assert_shared_capability(x))
#define LAKE_THREAD_SAFETY_RETURN_CAPABILITY(X)        _LAKE_THREAD_SAFETY_ATTRIBUTE(lock_returned(x))
#define LAKE_NO_THREAD_SAFETY_ANALYSIS                 _LAKE_THREAD_SAFETY_ATTRIBUTE(no_thread_safety_analysis)

#if LAKE_HAS_FEATURE(address_sanitizer)
    #ifndef LAKE_SANITIZE
        #define LAKE_SANITIZE 1
    #endif
    #define __SANITIZE_ADDRESS__ 1
#endif
#if LAKE_HAS_FEATURE(memory_sanitizer)
    #ifndef LAKE_SANITIZE
        #define LAKE_SANITIZE 1
    #endif
    #include <sanitizer/msan_interface.h>
    #ifndef LAKE_HAS_VALGRIND
        #define LAKE_HAS_VALGRIND 1
    #endif
    #define lake_san_undefined(a,len)           __msan_allocated_memory(a,len)
    #define lake_san_make_addressable(a,len)    MEM_UNDEFINED(a,len)
    #define lake_san_make_defined(a,len)        __msan_unpoison(a,len)
    #define lake_san_noaccess(a,len)            ((void)0)
    #define lake_san_check_addressable(a,len)   ((void)0)
    #define lake_san_check_defined(a,len)       __msan_check_mem_is_initialized(a,len)
    #define lake_san_get_vbits(a,b,len)         __msan_copy_shadow(b,a,len)
    #define lake_san_set_vbits(a,b,len)         __msan_copy_shadow(a,b,len)
    #define LAKE_REDZONE_SIZE 8
#elif defined(LAKE_HAS_VALGRIND)
    #ifndef LAKE_SANITIZE
        #define LAKE_SANITIZE 1
    #endif
    #include <valgrind/memcheck.h>
    #define lake_san_undefined(a,len)           VALGRIND_MAKE_MEM_UNDEFINED(a,len)
    #define lake_san_make_addressable(a,len)    MEM_UNDEFINED(a,len)
    #define lake_san_make_defined(a,len)        VALGRIND_MAKE_MEM_DEFINED(a,len)
    #define lake_san_noaccess(a,len)            VALGRIND_MAKE_MEM_NOACCESS(a,len)
    #define lake_san_check_addressable(a,len)   VALGRIND_CHECK_MEM_IS_ADDRESSABLE(a,len)
    #define lake_san_check_defined(a,len)       VALGRIND_CHECK_MEM_IS_DEFINED(a,len)
    #define lake_san_get_vbits(a,b,len)         VALGRIND_GET_VBITS(a,b,len)
    #define lake_san_set_vbits(a,b,len)         VALGRIND_SET_VBITS(a,b,len)
    #define LAKE_REDZONE_SIZE 8
#elif defined(__SANITIZE_ADDRESS__)
    #ifndef LAKE_SANITIZE
        #define LAKE_SANITIZE 1
    #endif
    #include <sanitizer/asan_interface.h>
    /* How to do manual poisoning:
     * https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning */
    #define lake_san_undefined(a,len)           ((void)0)
    #define lake_san_make_addressable(a,len)    ASAN_UNPOISON_MEMORY_REGION(a,len)
    #define lake_san_make_defined(a,len)        ((void)0)
    #define lake_san_noaccess(a,len)            ASAN_POISON_MEMORY_REGION(a,len)
    #define lake_san_check_addressable(a,len)   __asan_region_is_poisoned((void*) a,len)
    #define lake_san_check_defined(a,len)       ((void)0)
    #define lake_san_get_vbits(a,b,len)         ((void)0)
    #define lake_san_set_vbits(a,b,len)         ((void)0)
    #define LAKE_REDZONE_SIZE 8
#else
    #define lake_san_undefined(a,len)           ((void)0)
    #define lake_san_make_addressable(a,len)    ((void)0)
    #define lake_san_make_defined(a,len)        ((void)0)
    #define lake_san_noaccess(a,len)            ((void)0)
    #define lake_san_check_addressable(a,len)   ((void)0)
    #define lake_san_check_defined(a,len)       ((void)0)
    #define lake_san_get_vbits(a,b,len)         ((void)0)
    #define lake_san_set_vbits(a,b,len)         ((void)0)
    #define LAKE_REDZONE_SIZE 0
#endif /* valgrind */

#ifdef LAKE_HAS_VALGRIND
    #define LAKE_IF_VALGRIND(A,B) A
#else
    #define LAKE_IF_VALGRIND(A,B) B
#endif
