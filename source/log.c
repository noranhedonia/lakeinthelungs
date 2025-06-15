#include "internal.h"

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
static u32 g_log_hints = log_hint_with_colors | log_hint_with_threading;

void flush_logger(struct logger *l)
{
    FILE *s = stdout;
    lake_spinlock_acquire(&l->flush_lock);
    fprintf(s, "%s", l->buf.v);
    l->buf.len = 0;
    l->should_flush = false;
    lake_spinlock_release(&l->flush_lock);
    fflush(s);
}

void lake_forced_flush_all_loggers(void)
{
    if (lake_unlikely(g_bedrock == nullptr)) return;

    for (s32 i = 0; i < g_bedrock->thread_count; i++) {
        struct tls *tls = &g_bedrock->tls[i];
        if (tls->fiber_in_use == (u32)FIBER_INVALID) continue;
        struct logger *l = &g_bedrock->fibers[tls->fiber_in_use].logger;
        if (l->should_flush) flush_logger(l);
    }
}

#define COLOR_BLACK   "\033[30m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[97m"
#define COLOR_GREY    "\033[90m"
#define COLOR_NORMAL  "\033[0m"

#define isdigit(c) (c >= '0' && c <= '9')
#define isalpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define appendlit(b, s) \
    lake_strbuf_appendstrn(b, s, (s32)lake_lengthof(s))

static void colorize_buf(char *msg, bool enable_colors, lake_strbuf *buf)
{
    lake_dbg_assert(msg && buf, LAKE_PANIC, nullptr);

    char *ptr, ch, prev = '\0';
    char is_str = '\0';
    bool is_num = false;
    bool is_var = false;
    bool override_color = false;
    bool auto_color = true;
    bool dont_append = false;

    for (ptr = msg; (ch = *ptr); ptr++) {
        dont_append = false;

        if (!override_color) {
            if (is_num && !isdigit(ch) && !isalpha(ch) && (ch != '.') && (ch != '%')) {
                if (enable_colors) appendlit(buf, COLOR_NORMAL);
                is_num = false;
            }

            if (is_str && (is_str == ch) && prev != '\\') {
                is_str = '\0';
            } else if (((ch == '\'') || (ch == '"')) && !is_str && !isalpha(prev) && (prev != '\\')) {
                if (enable_colors) appendlit(buf, COLOR_CYAN);
                is_str = ch;
            }

            if ((isdigit(ch) || (ch == '%' && isdigit(prev)) || 
                (ch == '-' && isdigit(ptr[1]))) && !is_num && !is_str && !is_var && 
                !isalpha(prev) && !isdigit(prev) && (prev != '_' && (prev != '.')))
            {
                if (enable_colors) appendlit(buf, COLOR_GREEN);
                is_num = true;
            }

            if (is_var && !isalpha(ch) && !isdigit(ch) && ch != '_') {
                if (enable_colors) appendlit(buf, COLOR_NORMAL);
                is_var = false;
            }

            if (!is_str && !is_var && ch == '$' && isalpha(ptr[1])) {
                if (enable_colors) appendlit(buf, COLOR_CYAN);
                is_var = true;
            }
        }

        if (!is_var && !is_str && !is_num && ch == '#' && ptr[1] == '[') {
            bool is_color = true;
            override_color = true;

            /* custom colors */
            if (!lake_strncmp(&ptr[2], "]", lake_lengthof("]"))) {
                auto_color = false;
            } else if (!lake_strncmp(&ptr[2], "green]", lake_lengthof("green]"))) {
                if (enable_colors) appendlit(buf, COLOR_GREEN);
            } else if (!lake_strncmp(&ptr[2], "red]", lake_lengthof("red]"))) {
                if (enable_colors) appendlit(buf, COLOR_RED);
            } else if (!lake_strncmp(&ptr[2], "blue]", lake_lengthof("blue]"))) {
                if (enable_colors) appendlit(buf, COLOR_BLUE);
            } else if (!lake_strncmp(&ptr[2], "magenta]", lake_lengthof("magenta]"))) {
                if (enable_colors) appendlit(buf, COLOR_MAGENTA);
            } else if (!lake_strncmp(&ptr[2], "cyan]", lake_lengthof("cyan]"))) {
                if (enable_colors) appendlit(buf, COLOR_CYAN);
            } else if (!lake_strncmp(&ptr[2], "yellow]", lake_lengthof("yellow]"))) {
                if (enable_colors) appendlit(buf, COLOR_YELLOW);
            } else if (!lake_strncmp(&ptr[2], "grey]", lake_lengthof("grey]"))) {
                if (enable_colors) appendlit(buf, COLOR_GREY);
            } else if (!lake_strncmp(&ptr[2], "white]", lake_lengthof("white]"))) {
                if (enable_colors) appendlit(buf, COLOR_WHITE);
            } else if (!lake_strncmp(&ptr[2], "normal]", lake_lengthof("normal]"))) {
                if (enable_colors) appendlit(buf, COLOR_NORMAL);
            } else if (!lake_strncmp(&ptr[2], "reset]", lake_lengthof("reset]"))) {
                override_color = false;
                if (enable_colors) appendlit(buf, COLOR_NORMAL);
            } else {
                is_color = false;
                override_color = false;
            }

            if (is_color) {
                ptr += 2;
                while ((ch = *ptr) != ']') ptr++;
                dont_append = true;
            }
            if (!auto_color) override_color = true;
        }

        if (ch == '\n') {
            if (is_num || is_str || is_var || override_color) {
                if (enable_colors) appendlit(buf, COLOR_NORMAL);
                override_color = false;
                is_num = false;
                is_str = false;
                is_var = false;
            }
        }

        if (!dont_append) lake_strbuf_appendstrn(buf, ptr, 1);

        if (!override_color && enable_colors && (((ch == '\'') || (ch == '"')) && !is_str))
            appendlit(buf, COLOR_NORMAL);

        prev = ch;
    }

    if (enable_colors && (is_num || is_str || is_var || override_color))
        appendlit(buf, COLOR_NORMAL);
    buf->v[buf->len] = '\0';
}

void lake_log_from_critical_path(s32 level, char const *fmt, ...)
{
    if (level > g_log_level) return;

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    fflush(stderr);
}

#ifdef LAKE_PLATFORM_WINDOWS
    #define LOG_FILENAME(cstr) (strrchr(cstr, '\\') ? strrchr(cstr, '\\') + 1 : cstr)
#else
    #define LOG_FILENAME(cstr) (strrchr(cstr, '/') ? strrchr(cstr, '/') + 1 : cstr)
#endif

void lake_printv_(
    s32         level, 
    char const *file, 
    s32         line, 
    char const *fmt,
    va_list     args)
{
    if (lake_unlikely(g_bedrock == nullptr)) {
        lake_log_from_critical_path(level, fmt, args);
        return;
    }
    u32 const thread_idx = lake_worker_thread_index();
    struct tls *tls = &g_bedrock->tls[thread_idx];
    struct fiber *f = &g_bedrock->fibers[tls->fiber_in_use];
    struct logger *l = &f->logger;

    time_t t;
    struct tm *tm;
    char timestamp[35];
    va_list args_copy;
    va_copy(args_copy, args);

    bool use_color = g_log_hints & log_hint_with_colors;
    bool write_context = g_log_hints & log_hint_with_context;
    bool write_threading = g_log_hints & log_hint_with_threading;
    bool write_timestamp = g_log_hints & log_hint_with_timestamps;
    s32 n = 32 + vsnprintf(nullptr, 0, fmt, args_copy);

    if (write_timestamp) {
        t = time(nullptr);
        tm = localtime(&t);
        timestamp[strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm)] = '\0';
        n += snprintf(nullptr, 0, "%s ", timestamp);
    }
#define set_level_string(cstr) { level_str = cstr; n += lake_lengthof(cstr); }
    char const *level_str = "#[normal]";
    if (level >= 4) {
        set_level_string("#[normal]jrnl")
    } else if (level > 0) {
        set_level_string("#[magenta] dbg")
    } else if (level == 0) {
        set_level_string("#[green]info")
    } else if (level == -2) {
        set_level_string("#[yellow]warn")
    } else if (level == -3) {
        set_level_string("#[red] err")
    } else if (level == -4) {
        set_level_string("#[blue] ftl")
    }
#undef set_level_string

    char const *threading_fmt = "#[grey]%2u %d:@%s";
    if (write_threading)
        n += snprintf(nullptr, 0, threading_fmt, thread_idx, l->depth, f->work.details.name);

    char const *context_fmt = "#[grey]%6d:%s";
    if (write_context)
        n += snprintf(nullptr, 0, context_fmt, line, file);

    if (lake_unlikely(!l->buf.alloc)) {
        usize const default_log_size = 8192 + n;
        l->tail_cursor = f->drifter.tail_cursor;
        l->buf.v = lake_drift(default_log_size, 1);
        l->buf.alloc = default_log_size;
        l->buf.v[0] = '\0';
    } else if (l->buf.len + n > l->buf.alloc) {
        flush_logger(l);
    }
    char *msg_nocolor = lake_drift_alias(n, 1);
    s32 o = 0;

    if (write_timestamp) 
        o += snprintf(msg_nocolor + o, n-o, "%s ", timestamp);
    if (level >= -4)
        o += snprintf(msg_nocolor + o, n-o, "%s#[normal]: ", level_str);
    if (write_threading) 
        o += snprintf(msg_nocolor + o, n-o, threading_fmt, thread_idx, l->depth, f->work.details.name);
    if (write_context)
        o += snprintf(msg_nocolor + o, n-o, context_fmt, line, LOG_FILENAME(file));
    if (write_context || write_threading)
        o += snprintf(msg_nocolor + o, n-o, "#[normal]: ");
    o += vsnprintf(msg_nocolor + o, n-o, fmt, args);
    o += snprintf(msg_nocolor + o, n-o, "\n");

    lake_spinlock_acquire_relaxed(&l->flush_lock);
    colorize_buf(msg_nocolor, use_color, &l->buf);

    if (level == -4)
        sys_dump_stack_trace(&l->buf);

    lake_spinlock_release_relaxed(&l->flush_lock);
    l->should_flush = true;
}

void lake_print_(
    s32         level, 
    char const *file, 
    s32         line, 
    char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    lake_printv_(level, file, line, fmt, args);
    va_end(args);
}

void lake_logv_(
    s32         level,
    char const *file, 
    s32         line, 
    char const *fmt,
    va_list     args)
{
    if (level > g_log_level) return;
    lake_printv_(level, file, line, fmt, args);
}

void lake_log_(
    s32         level, 
    char const *file, 
    s32         line, 
    char const *fmt, ...)
{
    if (level > g_log_level) return;

    va_list args;
    va_start(args, fmt);
    lake_printv_(level, file, line, fmt, args);
    va_end(args);
}

lake_assert_status lake_assert_log_(
    s32         status,
    char const *condition,
    char const *file,
    s32         line,
    char const *fmt, ...)
{
    lake_exit_status(status);
    lake_print_(-4, file, line, "Assertion! `#[cyan]%s#[normal]`.", condition);

    if (fmt != nullptr) {
        va_list args;
        va_start(args, fmt);
        lake_printv_(-4, file, line, fmt, args);
        va_end(args);
    }
    lake_forced_flush_all_loggers();
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
    char const *file, 
    s32         line)
{
    if (!status && g_status != LAKE_SUCCESS)
        status = g_status;

    if (status != LAKE_SUCCESS)
        lake_print_(-4, file, line, "Forced exit, status: %d.", status);

    lake_forced_flush_all_loggers();
    exit(status);
}
