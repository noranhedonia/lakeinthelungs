#pragma once

/** @file lake/bedrock/compiler.h
 *  @brief TODO docs
 */
#define LAKE_VERSION_NUM_MAJOR(VER)    ((VER) / 10000000)
#define LAKE_VERSION_NUM_MINOR(VER)    (((VER) / 10000) % 1000)
#define LAKE_VERSION_NUM_REVISION(VER) ((VER) % 10000)
#define LAKE_VERSION_NUM(MA, MI, REV)  ((MA) * 10000000 + (MI) * 10000 + (REV))

#if !defined(LAKE_NDEGUG) && defined(NDEBUG)
    #define LAKE_NDEBUG 1
#elif !defined(LAKE_DEBUG) && defined(DEBUG)
    #define LAKE_DEBUG 1
#endif

/** Including system's headers inside `extern "C"` { ... } is not safe, as system 
 *  headers may have C++ code in them, and C++ code inside extern "C" leads to 
 *  syntactically incorrect code. 
 *
 *  This is because putting code inside extern "C" won't make __cplusplus define 
 *  go away, that is, the system header being included thinks it is free to 
 *  use C++ as it sees fits. 
 *
 *  Including non-system headers inside extern "C" is not safe either, because 
 *  non-system headers end up including system headers, hence fall in the above
 *  case too. Conclusion, includes inside extern "C" are simply not portable.
 *
 *  This template helps surface these issues. */
#ifdef __cplusplus
template<class T> class _IncludeInsideExternCNotPortable;
#endif

/* GNUC */
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define LAKE_CC_GNUC_VERSION LAKE_VERSION_NUM(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define LAKE_CC_GNUC_VERSION LAKE_VERSION_NUM(__GNUC__, __GNUC_MINOR__, 0)
#endif
#if defined(LAKE_CC_GNUC_VERSION)
    #define LAKE_CC_GNUC_VERSION_CHECK(MA, MI, REV) (LAKE_CC_GNUC_VERSION >= LAKE_VERSION_NUM(MA, MI, REV))
#else
    #define LAKE_CC_GNUC_VERSION_CHECK(MA, MI, REV) (0)
#endif

/* MSVC */
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define LAKE_CC_MSVC_VERSION LAKE_VERSION_NUM(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define LAKE_CC_MSVC_VERSION LAKE_VERSION_NUM(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 100)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define LAKE_CC_MSVC_VERSION LAKE_VERSION_NUM(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif
#if !defined(LAKE_CC_MSVC_VERSION)
    #define LAKE_CC_MSVC_VERSION_CHECK(MA, MI, REV) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define LAKE_CC_MSVC_VERSION_CHECK(MA, MI, REV) (_MSC_FULL_VER >= ((MA * 10000000) + (MI * 100000) + (REV)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define LAKE_CC_MSVC_VERSION_CHECK(MA, MI, REV) (_MSC_FULL_VER >= ((MA * 1000000) + (MI * 10000) + (REV)))
#else
    #define LAKE_CC_MSVC_VERSION_CHECK(MA, MI, REV) (_MSC_VER >= ((MA * 100) + (MI)))
#endif

/* MINGW */
#if defined(__MINGW64__)
    #define LAKE_CC_MINGW_VERSION LAKE_VERSION_NUM(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, __MINGW64_VERSION_BUGFIX)
#elif defined(__MINGW32__)
    #define LAKE_CC_MINGW_VERSION LAKE_VERSION_NUM(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#endif
#if defined(LAKE_CC_MINGW_VERSION)
    #define LAKE_CC_MINGW_VERSION_CHECK(MA, MI, REV) (LAKE_CC_MINGW_VERSION >= LAKE_VERSION_NUM(MA, MI, REV))
#else
    #define LAKE_CC_MINGW_VERSION_CHECK(MA, MI, REV) (0)
#endif

/* EMSCRIPTEN */
#if defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #define LAKE_CC_EMSCRIPTEN_VERSION LAKE_VERSION_NUM(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#endif
#if defined(LAKE_CC_EMSCRIPTEN_VERSION)
    #define LAKE_CC_EMSCRIPTEN_VERSION_CHECK(MA, MI, REV) (LAKE_CC_EMSCRIPTEN_VERSION >= LAKE_VERSION_NUM(MA, MI, REV))
#else
    #define LAKE_CC_EMSCRIPTEN_VERSION_CHECK(MA, MI, REV) (0)
#endif

/* ARMC */
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define LAKE_CC_ARM_VERSION LAKE_VERSION_NUM(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define LAKE_CC_ARM_VERSION LAKE_VERSION_NUM(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif
#if defined(LAKE_CC_ARM_VERSION)
    #define LAKE_CC_ARM_VERSION_CHECK(MA, MI, REV) (LAKE_CC_ARM_VERSION >= LAKE_VERSION_NUM(MA, MI, REV))
#else
    #define LAKE_CC_ARM_VERSION_CHECK(MA, MI, REV) (0)
#endif

/* CLANG */
#if defined(__clang__) && !defined(LAKE_CC_CLANG_VERSION)
    #if __has_warning("-Wmissing-designated-field-initializers")
        #define LAKE_CC_CLANG_VERSION 190000
    #elif __has_warning("-Woverriding-option")
        #define LAKE_CC_CLANG_VERSION 180000
    #elif __has_attribute(unsafe_buffer_usage)  /* no new warnings in 17.0 */
        #define LAKE_CC_CLANG_VERSION 170000
    #elif __has_attribute(nouwtable)  /* no new warnings in 16.0 */
        #define LAKE_CC_CLANG_VERSION 160000
    #elif __has_warning("-Warray-parameter")
        #define LAKE_CC_CLANG_VERSION 150000
    #elif __has_warning("-Wbitwise-instead-of-logical")
        #define LAKE_CC_CLANG_VERSION 140000
    #elif __has_warning("-Waix-compat")
        #define LAKE_CC_CLANG_VERSION 130000
    #elif __has_warning("-Wformat-insufficient-args")
        #define LAKE_CC_CLANG_VERSION 120000
    #elif __has_warning("-Wimplicit-const-int-float-conversion")
        #define LAKE_CC_CLANG_VERSION 110000
    #elif __has_warning("-Wmisleading-indentation")
        #define LAKE_CC_CLANG_VERSION 100000
    #elif defined(__FILE_NAME__)
        #define LAKE_CC_CLANG_VERSION 90000
    #elif __has_warning("-Wextra-semi-stmt") || __has_builtin(__builtin_rotateleft32)
        #define LAKE_CC_CLANG_VERSION 80000
    /* For reasons unknown, Xcode 10.3 (Apple LLVM version 10.0.1) is apparently
     * based on Clang 7, but does not support the warning we test.
     * See https://en.wikipedia.org/wiki/Xcode#Toolchain_versions and
     * https://trac.macports.org/wiki/XcodeVersionInfo. */
    #elif __has_warning("-Wc++98-compat-extra-semi") || \
        (defined(__apple_build_version__) && __apple_build_version__ >= 10010000)
        #define LAKE_CC_CLANG_VERSION 70000
    #elif __has_warning("-Wpragma-pack")
        #define LAKE_CC_CLANG_VERSION 60000
    #elif __has_warning("-Wbitfield-enum-conversion")
        #define LAKE_CC_CLANG_VERSION 50000
    #elif __has_attribute(diagnose_if)
        #define LAKE_CC_CLANG_VERSION 40000
    #elif __has_warning("-Wcomma")
        #define LAKE_CC_CLANG_VERSION 39000
    #elif __has_warning("-Wdouble-promotion")
        #define LAKE_CC_CLANG_VERSION 38000
    #elif __has_warning("-Wshift-negative-value")
        #define LAKE_CC_CLANG_VERSION 37000
    #elif __has_warning("-Wambiguous-ellipsis")
        #define LAKE_CC_CLANG_VERSION 36000
    #else
        #define LAKE_CC_CLANG_VERSION 1
    #endif
#endif
#if defined(LAKE_CC_CLANG_VERSION)
    #define LAKE_CC_CLANG_VERSION_CHECK(MA, MI, REV) (LAKE_CC_CLANG_VERSION >= ((MA * 10000) + (MI * 1000) + (REV)))
#else
    #define LAKE_CC_CLANG_VERSION_CHECK(MA, MI, REV) (0)
#endif

/* GCC */
#if (defined(LAKE_CC_GNUC_VERSION) || defined(LAKE_CC_MINGW_VERSION)) && \
    !defined(LAKE_CC_CLANG_VERSION) && \
    !defined(LAKE_CC_ARM_VERSION) && \
    !defined(LAKE_CC_PGI_VERSION) && \
    !defined(LAKE_CC_ARM_VERSION) && \
    !defined(LAKE_CC_IBM_VERSION) && \
    !defined(LAKE_CC_INTEL_VERSION) && \
    !defined(__COMPCERT__)
    #define LAKE_CC_GCC_VERSION LAKE_CC_GNUC_VERSION
#endif
#if defined(LAKE_CC_GCC_VERSION)
    #define LAKE_CC_GCC_VERSION_CHECK(MA, MI, REV) (LAKE_CC_GCC_VERSION >= LAKE_VERSION_NUM(MA, MI, REV))
#else
    #define LAKE_CC_GCC_VERSION_CHECK(MA, MI, REV) (0)
#endif

#ifdef __has_attribute
    #define LAKE_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
    #define LAKE_HAS_ATTRIBUTE(x) (0)
#endif
#ifdef __has_builtin
    #define LAKE_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define LAKE_HAS_BUILTIN(x) (0)
#endif
#ifdef __has_extension
    #define LAKE_HAS_EXTENSION(x) __has_extension
#else
    #define LAKE_HAS_EXTENSION(x) (0)
#endif
#ifdef __has_feature
    #define LAKE_HAS_FEATURE(x) __has_feature(x)
#else
    #define LAKE_HAS_FEATURE(x) (0)
#endif
#ifdef __has_include
    #define LAKE_HAS_INCLUDE(x) __has_include(x)
#else
    #define LAKE_HAS_INCLUDE(x) (0)
#endif
#ifdef __has_warning
    #define LAKE_HAS_WARNING(x) __has_warning(x)
#else
    #define LAKE_HAS_WARNING(x) (0)
#endif
#if !defined(LAKE_FORCE_INLINE) && !defined(LAKE_FORCE_NOINLINE)
    #if defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
        #define LAKE_FORCE_INLINE static __attribute__((always_inline)) inline
        #define LAKE_FORCE_NOINLINE __attribute__(noinline)
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define LAKE_FORCE_INLINE __forceinline
        #define LAKE_FORCE_NOINLINE __declspec(noinline)
    #else
        #define LAKE_FORCE_INLINE static inline
        #define LAKE_FORCE_NOINLINE
    #endif
#endif

#if defined(LAKE_CC_MSVC_VERSION)
    /** Visual studio will complain if we define the `inline` keyword, but actually it only supports the keyword in C++. */
    #if !defined(_ALLOW_KEYWORD_MACROS)
        #define _ALLOW_KEYWORD_MACROS
    #endif

    /** MSVC has a `__restrict` keyword, but it also has a `__declspec(restrict)` modifier, so it is impossible
     *  to define a `restrict` macro without interfering with the latter. Furthermore the MSVC standard library 
     *  uses __declspec(restrict) under the _CRTRESTRICT macro. For now resolve this issue by redefining _CRTRESTRICT. */
    #include <crtdefs.h>
    #undef _CRTRESTRICT
    #define _CRTRESTRICT
#endif /* MSVC */
#ifndef inline
    #ifdef __cplusplus
        /* supports the inline keyword */
    #elif defined(LAKE_CC_GNUC_VERSION)
        #define inline __inline__
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define inline __inline
    #elif defined(LAKE_CC_INTEL_VERSION)
        /* the intel compiler should support inline keyword */
    #elif (__STDC_VERSION__ >= 199901L)
        /* C99 supports inline keyword */
    #else
        #define inline
    #endif
#endif /* inline */
/* http://cellperformance.beyond3d.com/articles/2006/05/demystifying-the-restrict-keyword.html */
#ifndef restrict
    #if (__STDC_VERSION__ >= 199901L) && !defined(__cplusplus)
        /* C99 */
    #elif defined(LAKE_CC_GNUC_VERSION)
        #define restrict __restrict__
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define restrict __restrict
    #else
        #define restrict
    #endif
#endif /* restrict */

#if LAKE_HAS_BUILTIN(__builtin_offsetof)
    #define lake_offsetof(ST, M) __builtin_offsetof(ST, M)
#else
    #define lake_offsetof(ST, M) ((size_t)((uint8_T *)&((ST *)0)->M - (uint8_t *)0))
#endif
#ifndef __cplusplus
    #define lake_static_cast(T, V)      ((T)(V))
    #define lake_const_cast(T, V)       ((T)(V))
    #define lake_reinterpret_cast(T, V) ((T)((uintptr_t)(V)))
#else
    #define lake_static_cast(T, V)      (static_cast<T>(V))
    #define lake_const_cast(T, V)       (const_cast<T>(V))
    #define lake_reinterpret_cast(T, V) (reinterpret_cast<T>(V))
#endif

/** Likely and unlikely branches. */
#if !defined(lake_likely) && !defined(lake_unlikely)
    #if defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
        #define lake_likely(x)   __builtin_expect(!!(x), 1)
        #define lake_unlikely(x) __builtin_expect(!!(x), 0)
    #else
        #define lake_likely(x)   (x)
        #define lake_unlikely(x) (x)
    #endif
#endif

/** The compiler declares a type for you. */
#if !defined(lake_typeof)
    #if defined(LAKE_CC_MSVC_VERSION)
        #define lake_typeof(x) decltype((x))
    #elif defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
        #define lake_typeof(x) __typeof__((x)) 
    #else /* may generate errors depending on the compiler */
        #define lake_typeof(x) typeof((x))
    #endif
#endif

#ifndef lake_may_alias
    #if defined(LAKE_CC_GNUC_VERSION)
        #define lake_may_alias __attribute__((__may_alias__))
        #define LAKE_HAS_MAY_ALIAS 1
    #elif defined(LAKE_CC_MSVC_VERSION)
        /* MSVC does not enforce strict aliasing rules, as is the case with GCC or Clang. */
        #define lake_may_alias
        #define LAKE_HAS_MAY_ALIAS 1
    #else
        #define lake_may_alias
        #define LAKE_HAS_MAY_ALIAS 0
    #endif
#endif

/** The function never returns. */
#if LAKE_HAS_ATTRIBUTE(noreturn)
    #define LAKE_NORETURN __attribute__((noreturn))
#elif defined(LAKE_CC_MSVC_VERSION)
    #define LAKE_NORETURN __declspec(noreturn)
#else
    #define LAKE_NORETURN
#endif

/** Function is a cold spot and will be optimized for size. */
#if LAKE_HAS_ATTRIBUTE(cold)
#define LAKE_COLD_FN __attribute__((cold))
#else
#define LAKE_COLD_FN
#endif

/** Function is a hot spot and will be optimized for speed. */
#if LAKE_HAS_ATTRIBUTE(hot)
#define LAKE_HOT_FN __attribute__((hot))
#else
#define LAKE_HOT_FN
#endif

/** Function has no side-effects. */
#if LAKE_HAS_ATTRIBUTE(pure)
#define LAKE_PURE_FN __attribute__((pure))
#else
#define LAKE_PURE_FN
#endif

/** Function has no side-effects, return value depends on arguments only.
 *  Must not take pointer parameters, must not return NULL. */
#if LAKE_HAS_ATTRIBUTE(const)
#define LAKE_CONST_FN __attribute__((const))
#else
#define LAKE_CONST_FN
#endif

/** Function never returns NULL. */
#if LAKE_HAS_ATTRIBUTE(returns_nonnull)
#define LAKE_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define LAKE_RETURNS_NONNULL
#endif

/** Function must be called with NULL as the last argument (for varargs functions). */
#if LAKE_HAS_ATTRIBUTE(sentinel)
#define LAKE_SENTINEL_FN __attribute__((sentinel))
#else
#define LAKE_SENTINEL_FN
#endif

/** Symbol is meant to be possibly unused. */
#if LAKE_HAS_ATTRIBUTE(unused)
#define LAKE_UNUSED __attribute__((unused))
#else
#define LAKE_UNUSED
#endif

/** Symbol should be emitted even if it appears to be unused. */
#if LAKE_HAS_ATTRIBUTE(used)
#define LAKE_USED __attribute__((used))
#else
#define LAKE_USED
#endif

/** Function or type is deprecated and should not be used. */
#if LAKE_HAS_ATTRIBUTE(deprecated)
#define LAKE_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define LAKE_DEPRECATED(msg) #warning msg
#endif

/** Function parameters at specified positions must not be NULL. */
#if LAKE_HAS_ATTRIBUTE(nonnull)
#define LAKE_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define LAKE_NONNULL(...)
#endif

/** All pointer parameters must not be NULL. */
#if LAKE_HAS_ATTRIBUTE(nonnull)
#define LAKE_NONNULL_ALL __attribute__((nonnull))
#else
#define LAKE_NONNULL_ALL
#endif

/** The return value of this function must not be ignored. */
#if LAKE_HAS_ATTRIBUTE(warn_unused_result)
#define LAKE_NODISCARD __attribute__((warn_unused_result))
#else
#define LAKE_NODISCARD
#endif

/** Function takes a printf-style format string and variadic arguments. */
#if LAKE_HAS_ATTRIBUTE(format)
#define LAKE_PRINTF(fmt, va) __attribute__((format(__printf__, fmt, va)))
#else
#define LAKE_PRINTF(fmt, va)
#endif

/** The pointer returned by this function cannot alias any other pointer valid when the function returns. */
#if LAKE_HAS_ATTRIBUTE(malloc)
#define LAKE_MALLOC __attribute__((malloc))
#else
#define LAKE_MALLOC
#endif

/** Hints to the compiler that a statement that falls through to another case label is intentional. */
#if LAKE_HAS_ATTRIBUTE(fallthrough)
#define LAKE_FALLTHROUGH __attribute__((fallthrough))
#else
#define LAKE_FALLTHROUGH
#endif

#if LAKE_HAS_ATTRIBUTE(packed)
#define LAKE_PACKED __attribute__((packed))
#else
#define LAKE_PACKED
#endif
