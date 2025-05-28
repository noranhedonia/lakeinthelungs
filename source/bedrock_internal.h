#pragma once

#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/mpmc_ring.h>

#define FIBER_INVALID (SIZE_MAX)

/** Thread-local storage. */
struct tls;

/** A fiber context, it holds saved CPU registers and pointers required to perform a CPU context switch. */
typedef void *fcontext;

/** Implemented in assembly, ofc/nfc - original/new fiber context, preserve_fpu - procedure. */
LAKE_HOT_FN
extern sptr jump_fcontext(fcontext *ofc, fcontext nfc, sptr vp, s32 preserve_fpu);

/** Implemented in assembly, sp - top of stack pointer. */
LAKE_HOT_FN LAKE_NONNULL_ALL
extern fcontext make_fcontext(void *sp, usize size, void (*fn)(sptr));

#if defined(LAKE_PLATFORM_UNIX)
#include <unistd.h>
#include <pthread.h>
typedef pthread_t sys_thread_id;
#elif defined(LAKE_PLATFORM_WINDOWS)
#include <process.h>
typedef DWORD sys_thread_id;
#endif /* LAKE_PLATFORM_UNIX */

enum tls_flags : u32 {
    tls_in_use = 0u,
    tls_to_free = 0x40000000u,
    tls_to_wait = 0x80000000u,
    tls_mask    = ~(tls_to_free | tls_to_wait),
};

struct work {
    lake_work_details   details;
    atomic_usize       *work_left;
};

struct tls {
    fcontext            home_context;
    u32                 fiber_in_use;
    u32                 fiber_old;
};

struct fiber {
    struct work         work;
    fcontext            context;
    atomic_usize       *wait_counter;
};

struct bedrock {
    lake_mpmc_ring      work_queue;

    struct tls         *tls;
    atomic_usize        tls_sync;
    lake_work_details  *ends;
    
    sys_thread_id      *threads;
    s32                 thread_count;

    s32                 fiber_count;
    struct fiber       *fibers;
    atomic_usize       *waiting;
    atomic_usize       *free;
    atomic_usize       *locks;

    u8                 *stack;
    usize               stack_size;

    usize               budget;
    usize               page_size;
    atomic_usize        commitment;
};
extern struct bedrock *g_bedrock;

LAKE_FORCE_INLINE LAKE_NONNULL_ALL
sptr jump_fiber_context(struct tls *tls, fcontext *from, fcontext *to)
{
    lake_san_assert(*from != *to, LAKE_INVALID_PARAMETERS, "Can't switch fiber context to itself.");
    return jump_fcontext(from, *to, (sptr)tls, 1);
}

LAKE_FORCE_INLINE LAKE_NONNULL_ALL
void make_fiber_context(fcontext *context, void (*procedure)(sptr), void *stack, usize stack_bytes)
{
    *context = make_fcontext(stack, stack_bytes, procedure);
}

/** Returns an index to a free fiber and grabs it's lock. */
extern usize get_free_fiber(void);

LAKE_FORCE_INLINE
struct tls *get_thread_local_storage(void)
{ return &g_bedrock->tls[lake_worker_thread_index()]; }

/** Entry point for the worker threads, defined at `work.c`. */
extern void *LAKECALL dirty_deeds_done_dirt_cheap(void *raw_tls);

/** Map virtual memory space without physical memory commitment.
 *  Before memory is used, must be commited with `sys_madvise()`. */
extern void *sys_mmap(usize page_aligned, usize hugepage_size);

/** Unmap virtual memory space and release physical resources. */
extern void sys_munmap(void *mapped, usize size_page_aligned);

enum sys_madvise_mode : s32 {
    sys_madvise_mode_commit = 0,
    sys_madvise_mode_release,
};

/** Control state and commitment of physical resources. Offset and size must be page aligned. */
extern bool sys_madvise(void *mapped, usize offset, usize size, enum sys_madvise_mode mode);

/** Read system info about the CPU. */
extern void sys_cpuinfo(s32 *out_threads, s32 *out_cores, s32 *out_packages);

/** Read system info about RAM. */
extern void sys_meminfo(usize *out_total_ram, usize *out_page_size);

/** Read system info about support of huge pages. */
extern void sys_hugetlbinfo(usize *out_hugepage_size, usize ceiling);

/** Dump the stack trace into an output stream. */
extern void sys_dump_stack_trace(void *stream);

/** Creates and runs a thread, saves it's identifier to the given thread handle. */
extern void sys_thread_create(sys_thread_id *out_thread, void *(*procedure)(void *), void *argument);

/** Destroys and jois a thread. */
extern void sys_thread_destroy(sys_thread_id thread);

/** Joins a thread. It will wait for the thread to finish it's work before continuing. */
extern void sys_thread_join(sys_thread_id thread);

/** Set thread affinity for an array of worker threads. */
extern void sys_thread_affinity(u32 thread_count, sys_thread_id const *threads, u32 cpu_count, u32 begin_cpu_idx);
