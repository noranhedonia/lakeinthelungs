#pragma once

#include <lake/bedrock.h>
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
    lake_work_details       details;
    atomic_usize           *work_left;
};
typedef lake_mpmc_node_t(struct work) work_queue_node;

struct region {
    usize           v;
    struct region  *next;
    usize           offset;
    usize           alloc;
};

struct drifter_cursor {
    struct drifter_cursor  *prev;
    struct region          *tail;
    usize                   offset;
};

struct drifter {
    struct region          *head;
    struct region          *tail_page;
    struct drifter_cursor  *tail_cursor;
};

struct tls {
    fcontext                home_context;
    u32                     fiber_in_use;
    u32                     fiber_old;
};

struct fiber {
    struct work             work;
    fcontext                context;
    lake_work_chain         wait_counter;
    struct drifter_cursor   cursor;
    struct drifter          drifter;
};

struct tagged_heap {
    LAKE_ATOMIC(lake_heap_tag)  tag;
    lake_spinlock               spinlock;
    struct region               head;
    struct region              *tail;
};

struct bedrock {
    lake_mpmc_ring_t(work_queue_node) work_queue;
    struct tls             *tls;
    atomic_usize            tls_sync;
    lake_work_details      *ends;
    
    /* TODO a hashamp to acquire worker thread index */
    sys_thread_id          *threads;
    struct fiber           *fibers;
    atomic_usize           *waiting;
    atomic_usize           *free;
    atomic_usize           *locks;
    s32                     thread_count;
    s32                     fiber_count;

    atomic_u8              *heap_bitmap;
    atomic_usize            growth_sync;

    struct tagged_heap      roots;
    struct tagged_heap    **tagged_heaps;
    atomic_usize            tagged_heap_tail;
    s32                     tagged_heap_count;

    u8                     *stack;
    usize                   stack_size;
    usize                   budget;
    usize                   page_size;
    atomic_usize            commitment;
};
/** Address to this global variable is offset 0 in our virtual map. */
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

/* position - a bit position, within the memory bitmap represents a single 256 KiB 
 *            (RIVEN_BLOCK_SIZE) block of memory using a single bit.
 *    index - an index value to access a byte within the bitmap (uint8_t per index).
 *    block - a memory offset in bytes, represents the range of a single block (1 block is 256 KiB).
 *     page - a memory offset in bytes, represents the range of a single "page" (1 page is 2 MiB, 
 *            this translates into: 1 hugepage, 1 byte worth of blocks in the bitmap). */
#define __index_from_position(val) (val >> 3lu)
#define __position_from_index(val) (val << 3lu)
#define __block_from_position(val) (val << 21lu) /* 21 because 2 MiB */
#define __position_from_block(val) (val >> 21lu)
#define __index_from_range(val)    (__position_from_block(val))

LAKE_HOT_FN LAKE_NONNULL_ALL
extern void LAKECALL acquire_heap_bitmap(atomic_u8 *bitmap, usize const position, usize const size);

LAKE_HOT_FN LAKE_NONNULL_ALL
extern void LAKECALL release_heap_bitmap(atomic_u8 *bitmap, usize const position, usize const size);

LAKE_HOT_FN LAKE_PURE_FN
extern usize LAKECALL acquire_blocks(usize const block_aligned);

/** Returns an index to a free fiber and grabs it's lock. */
LAKE_HOT_FN LAKE_PURE_FN
extern usize get_free_fiber(void);

LAKE_FORCE_INLINE
struct tls *get_thread_local_storage(void)
{ return &g_bedrock->tls[lake_worker_thread_index()]; }

/** Entry point for the worker threads, defined at `work.c`. */
extern void *LAKECALL dirty_deeds_done_dirt_cheap(void *raw_tls);

/** Map virtual memory space without physical memory commitment.
 *  Before memory is used, must be commited with `sys_madvise()`. */
extern void *LAKECALL sys_mmap(usize page_aligned, usize hugepage_size);

/** Unmap virtual memory space and release physical resources. */
extern void LAKECALL sys_munmap(void *mapped, usize size_page_aligned);

/** Control state and commitment of physical resources. Offset and size must be page aligned. */
extern bool LAKECALL sys_madvise(void *mapped, usize offset, usize size, bool commit_or_release);

/** Read system info about the CPU. */
extern void LAKECALL sys_cpuinfo(s32 *out_threads, s32 *out_cores, s32 *out_packages);

/** Read system info about RAM. */
extern void LAKECALL sys_meminfo(usize *out_total_ram, usize *out_page_size);

/** Read system info about support of huge pages. */
extern void LAKECALL sys_hugetlbinfo(usize *out_hugepage_size, usize ceiling);

/** Dump the stack trace into an output stream. */
extern void LAKECALL sys_dump_stack_trace(void *stream);

/** Creates and runs a thread, saves it's identifier to the given thread handle. */
extern void LAKECALL sys_thread_create(sys_thread_id *out_thread, void *(*procedure)(void *), void *argument);

/** Destroys and jois a thread. */
extern void LAKECALL sys_thread_destroy(sys_thread_id thread);

/** Joins a thread. It will wait for the thread to finish it's work before continuing. */
extern void LAKECALL sys_thread_join(sys_thread_id thread);

/** Set thread affinity for an array of worker threads. */
extern void LAKECALL sys_thread_affinity(u32 thread_count, sys_thread_id const *threads, u32 cpu_count, u32 begin_cpu_idx);
