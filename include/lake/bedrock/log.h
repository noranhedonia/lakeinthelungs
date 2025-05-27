#pragma once

/** @file lake/bedrock/log.h 
 *  @brief Operations for tracing, asserting and logging messages.
 *
 *  The logger runs optimally within the framework, but serves the basic purpose
 *  of logging into the command line output
 */
#include <lake/bedrock/types.h>

#ifndef LAKE_FUNCTION
    #define LAKE_FUNCTION __func__
#endif
#ifndef LAKE_FILE 
    #define LAKE_FILE __FILE__
#endif
#ifndef LAKE_LINE
    #define LAKE_LINE __LINE__
#endif

#if (defined(__cplusplus) && (__cplusplus >= 201103L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L))
    #define lake_static_assert(x, desc) static_assert(x, desc)
#elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)) || defined(LAKE_CC_GNUC_VERSION)
    /* For GCC 4.6+ it will work but it may throw a warning:
     * warning: ISO C99 does not support '_Static_assert' [-Wpedantic] */
    #define lake_static_assert(x, desc) _Static_assert(x, desc)
#endif
#ifndef lake_static_assert
    #define lake_static_assert(x, desc) /* blank */
#endif

/* unreachable */
#if defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
    #define LAKE_UNREACHABLE __builtin_unreachable()
#elif defined(LAKE_CC_MSVC_VERSION)
    #define LAKE_UNREACHABLE __assume(false)
#else
    #define LAKE_UNREACHABLE lake_assert(0, LAKE_PANIC, "Unreachable code branch, shouldn't get here!");
#endif

/* assumption */
#if LAKE_HAS_BUILTIN(__builtin_assume)
    #define LAKE_ASSUME(x) __builtin_assume(x)
#elif defined(LAKE_CC_MSVC_VERSION)
    #define LAKE_ASSUME(x) __assume(x)
#else
    #define LAKE_ASSUME(x) do { if(!(x)) { LAKE_UNREACHABLE; } } while(LAKE_NULL_WHILE_LOOP_CONDITION)
#endif

#if !defined(LAKE_LOG_0) || defined(LAKE_LOG_1) || defined(LAKE_LOG_2) || defined(LAKE_LOG_3)
    #if !defined(LAKE_NDEBUG)
        #define LAKE_LOG_3 /* enable all tracing in debug mode */
    #else
        #define LAKE_LOG_0 /* enable infrequent tracing in release mode */
    #endif
#endif /* LOG LEVELS */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Aborts the game and returns the given code. In case where exitcode is 0 (lake_result_success),
 *  the exitcode may be overwritten if ever `lake_exit_status()` was called. This function may 
 *  perform a logging operation before it exits. */
LAKE_NORETURN 
LAKEAPI void LAKECALL 
lake_abort_(
    s32         status, 
    char const *fn, 
    char const *file, 
    s32         line);
#define lake_abort(status) lake_abort_(status, LAKE_FUNCTION, LAKE_FILE, LAKE_LINE)

/** Overwrites the return exitcode of the game process, whenever `lake_abort_()` will be called.
 *  @return Current exit status. */
LAKEAPI s32 LAKECALL 
lake_exit_status(
    s32 status);

/** Enqueues an internal job to flush all gathered log data to registered sinks.
 *  If yield is true, the flush will run immediately instead of asynchronously.
 *
 *  If the framework is not initialized, this function is ignored. */
LAKEAPI void LAKECALL 
lake_flush_log(
    bool yield);

/** Always log a message with a level prefix. */
LAKE_HOT_FN
LAKEAPI void LAKECALL
lake_print_(
    s32         level, 
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt,
    ...) LAKE_PRINTF(5,6);

/** As `lake_print_`, but directly accepts a variable argument list. */
LAKE_HOT_FN
LAKEAPI void LAKECALL
lake_printv_(
    s32         level, 
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt,
    va_list     args);

/** Macro helper for `lake_print_()`. */
#define lake_print(level, ...)          lake_print_(level, LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, __VA_ARGS__)
/** Macro helper for `lake_printv_()`. */
#define lake_printv(level, fmt, args)   lake_printv_(level, LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, fmt, args)

/** Log message with a level prefix, will be ignored if the level is larger than the current set log level. */
LAKE_HOT_FN 
LAKEAPI void LAKECALL 
lake_log_(
    s32         level, 
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt,
    ...) LAKE_PRINTF(5,6);

/** As `lake_log_`, but directly accepts a variable argument list. */
LAKE_HOT_FN
LAKEAPI void LAKECALL
lake_logv_(
    s32         level,
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt,
    va_list     args);

/** Macro helper for `lake_log_()`. */
#define lake_log(level, ...)            lake_log_(level, LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, __VA_ARGS__)
/** Macro helper for `lake_logv_()`. */
#define lake_logv(level, fmt, args)     lake_logv_(level, LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, fmt, args)

/** Tracing. Used for logging of infrequent events. */
#define lake_trace_(fn, file, line, ...) lake_log_(0, fn, file, line, __VA_ARGS__)
#define lake_trace(...) lake_trace_(LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, __VA_ARGS__)

/** Warning. Used when an issue occurs, but operation is successful. */
#define lake_warn_(fn, file, line, ...) lake_log_(-1, fn, file, line, __VA_ARGS__)
#define lake_warn(...) lake_warn_(LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, __VA_ARGS__)

/** Error. Used when an issue occurs, and operation fails. */
#define lake_error_(fn, file, line, ...) lake_log_(-2, fn, file, line, __VA_ARGS__)
#define lake_error(...) lake_error_(LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, __VA_ARGS__)

/** Fatal. Used when an issue occurs, and the application cannot continue. */
#define lake_fatal_(fn, file, line, ...) lake_log_(-3, fn, file, line, __VA_ARGS__)
#define lake_fatal(...) lake_fatal_(LAKE_FUNCTION, LAKE_FILE, LAKE_LINE, __VA_ARGS__)

#if !defined(LAKE_LOG_0) 
    #if defined(LAKE_LOG_3)
        #define lake_dbg_3(...) lake_log(3, __VA_ARGS__)

        #ifndef LAKE_LOG_2
        #define LAKE_LOG_2
        #endif
    #endif /* LAKE_LOG_3 */
    #if defined(LAKE_LOG_2)
        #define lake_dbg_2(...) lake_log(2, __VA_ARGS__)

        #ifndef LAKE_LOG_1
        #define LAKE_LOG_1
        #endif
    #endif /* LAKE_LOG_2 */
    #if defined(LAKE_LOG_1)
        #define lake_dbg_1(...) lake_log(1, __VA_ARGS__)
    #endif /* LAKE_LOG_1 */
#else
#undef lake_trace
#define lake_trace(...)
#define lake_dbg_1(...)
#define lake_dbg_2(...)
#define lake_dbg_3(...)
#endif /* LAKE_LOG_0 */

typedef enum : s32 {
    lake_assert_status_continue =  0,  /**< Continue the application as normal, may be error prone. */
    lake_assert_status_trap     = -1,  /**< Invoke a debugger breakpoint. */
    lake_assert_status_abort    = -2,  /**< Abort the application. */
} lake_assert_status;

/** Logs a failed assertion. Returns an innate code for how to handle the runtime.
 *  This function shares the framework implications of `lake_log_()`. */
LAKEAPI lake_assert_status LAKECALL 
lake_assert_log_(
    s32         status,
    char const *condition,
    char const *fn,
    char const *file,
    s32         line,
    char const *fmt,
    ...) LAKE_PRINTF(6,7);

#if defined(LAKE_NDEBUG) && !defined(LAKE_KEEP_ASSERT)
#define lake_assert(condition, error_code, ...) ((void)0)
#else
#define lake_assert(condition, error_code, ...)                 \
    do {                                                        \
        if (lake_unlikely(!(condition))) {                      \
            char const *fn = LAKE_FUNCTION, *file = LAKE_FILE;  \
            s32 line = LAKE_LINE;                               \
            lake_assert_status o = lake_assert_log_(error_code, \
                #condition, fn, file, line, __VA_ARGS__);       \
                                                                \
            if (o == lake_assert_status_trap) {                 \
                lake_debugtrap();                               \
            } else if (o == lake_assert_status_abort) {         \
                lake_abort_(error_code, fn, file, line);        \
            }                                                   \
        }                                                       \
    } while(0,0)
#endif /* LAKE_KEEP_ASSERT */

/** Debug assert. Is only valid in debug mode (ignores LAKE_KEEP_ASSERT). */
#ifndef LAKE_NDEBUG
#define lake_dbg_assert(condition, error_code, ...) lake_assert(condition, error_code, __VA_ARGS__)
#else
#define lake_dbg_assert(condition, error_code, ...) ((void)0)
#endif

/** Sanitize assert. Is only valid in sanitized mode (ignores LAKE_KEEP_ASSERT). */
#ifdef LAKE_SANITIZE
#define lake_san_assert(condition, error_code, ...) lake_assert(condition, error_code, __VA_ARGS__) 
#else
#define lake_san_assert(condition, error_code, ...) ((void)0)
#endif

/** Enable or disable log for a given levels. The provided level corresponds
 *  to the desired tracing level. Accepted range of values is:
 *      4 - all tracing enabled,
 *      0 - disable all debug and infrequent tracing 
 *     -1 - disable warnings 
 *     -2 - disable errors 
 *     -3 - disable asserts and all logs 
 *  @return Previous log level. */
LAKEAPI s32 LAKECALL 
lake_log_set_level(
    s32 level);

/** @return Current log level. */
LAKEAPI s32 LAKECALL 
lake_log_get_level(void);

/** Enable or disable tracing with colors. By default colors are enabled.
 *  @return Previous color setting. */
LAKEAPI bool LAKECALL 
lake_log_enable_colors(
    bool enabled);

/** Enable or disable information about the call context, e.g. function name, file and line of code.
 *  @return Previous call context setting. */
LAKEAPI bool LAKECALL
lake_log_enable_context(
    bool enabled);

/** Enable or disable logging timestamps. By default timestamps are disabled.
 *  Note that enabling timestamps introduces overhead to the loggin operation.
 *  @return Previous timestamp setting. */
LAKEAPI bool LAKECALL 
lake_log_enable_timestamps(
    bool enabled);

/** Enable or disable logging thread info. By default this is disabled. 
 *  This will log the system thread id and the worker thread index.
 *  @return Previous threading setting. */
LAKEAPI bool LAKECALL 
lake_log_enable_threading(
    bool enabled);

#ifdef __cplusplus
}
#endif /* __cplusplus */
