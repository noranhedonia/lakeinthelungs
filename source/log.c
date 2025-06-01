#include <lake/bedrock/log.h>

#include <stdlib.h>
#include <stdio.h>

enum log_hints {
    log_hint_with_colors        = (1u << 0),
    log_hint_with_context       = (1u << 1),
    log_hint_with_timestamps    = (1u << 2),
    log_hint_with_threading     = (1u << 3),
};

static s32 g_status = LAKE_SUCCESS;
static s32 g_log_level = 4;
static u32 g_log_hints = log_hint_with_colors;

void lake_flush_log(bool yield)
{
    (void)yield;
    /* TODO */
}

void lake_printv_(
    s32         level, 
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt,
    va_list     args)
{
    /* TODO */
    (void)level;
    (void)fn;
    (void)file;
    (void)line;
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    fflush(stdout);
}

void lake_print_(
    s32         level, 
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    lake_printv_(level, fn, file, line, fmt, args);
    va_end(args);
}

void lake_logv_(
    s32         level,
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt,
    va_list     args)
{
    if (level > g_log_level) return;
    lake_printv_(level, fn, file, line, fmt, args);
}

void lake_log_(
    s32         level, 
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt, ...)
{
    if (level > g_log_level) return;

    va_list args;
    va_start(args, fmt);
    lake_printv_(level, fn, file, line, fmt, args);
    va_end(args);
}

lake_assert_status lake_assert_log_(
    s32         status,
    char const *condition,
    char const *fn,
    char const *file,
    s32         line,
    char const *fmt, ...)
{
    /* TODO */
    lake_exit_status(status);
    lake_print_(-4, fn, file, line, "Assertion! `%s`.", condition);
    if (fmt != nullptr) {
        va_list args;
        va_start(args, fmt);
        lake_printv_(-4, fn, file, line, fmt, args);
        va_end(args);
    }
    return lake_assert_status_trap;
}

s32 lake_log_set_level(s32 level)
{
    s32 current = g_log_level;
    g_log_level = level;
    return current;
}

s32 lake_log_get_level(void)
{
    return g_log_level;
}

bool lake_log_enable_colors(bool enabled)
{
    bool prev = g_log_hints & log_hint_with_colors;
    enabled ? (g_log_hints |= log_hint_with_colors) : (g_log_hints &= (~log_hint_with_colors));
    return prev;
}

bool lake_log_enable_context(bool enabled)
{
    bool prev = g_log_hints & log_hint_with_context;
    enabled ? (g_log_hints |= log_hint_with_context) : (g_log_hints &= (~log_hint_with_context));
    return prev;
}

bool lake_log_enable_timestamps(bool enabled)
{
    bool prev = g_log_hints & log_hint_with_timestamps;
    enabled ? (g_log_hints |= log_hint_with_timestamps) : (g_log_hints &= (~log_hint_with_timestamps));
    return prev;
}

bool lake_log_enable_threading(bool enabled)
{
    bool prev = g_log_hints & log_hint_with_threading;
    enabled ? (g_log_hints |= log_hint_with_threading) : (g_log_hints &= (~log_hint_with_threading));
    return prev;
}

s32 lake_exit_status(s32 status)
{
    s32 prev = g_status;
    g_status = status;
    return prev;
}

void lake_abort_(
    s32         status, 
    char const *fn, 
    char const *file, 
    s32         line)
{
    if (!status && g_status != LAKE_SUCCESS)
        status = g_status;

    if (status != LAKE_SUCCESS)
        lake_log_(-4, fn, file, line, "Forced exit, status: %d.", status);

    lake_flush_log(true);
    exit(status);
}
