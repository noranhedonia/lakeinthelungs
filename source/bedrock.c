#include "bedrock_internal.h"

struct bedrock *g_bedrock = nullptr;

#define HEAP_BLOCK_SIZE (256lu*1024)

static LAKE_NORETURN void LAKECALL d4c_love_train(void *stub)
{
    (void)stub;
    struct tls *tls = get_thread_local_storage();
    struct fiber *old_fiber = &g_bedrock->fibers[tls->fiber_in_use];

    if (tls == &g_bedrock->tls[0]) for (s32 i = 1; i < g_bedrock->thread_count; i++) 
        sys_thread_join(g_bedrock->threads[i++]);

    tls->fiber_old = tls->fiber_in_use | tls_to_free;
    jump_fiber_context(tls, &old_fiber->context, &tls->home_context);
    LAKE_UNREACHABLE;
}

struct application {
    PFN_lake_framework  main;
    void               *argument;
    lake_framework     *framework;
};

static LAKE_NORETURN void LAKECALL funny_valentine(void *raw_app)
{
    struct application *app = (struct application *)raw_app;
    app->main(app->argument, app->framework);

    /* tell all threads to die, type shit */
    for (s32 i = 0; i < g_bedrock->thread_count; i++) {
        g_bedrock->ends[i].procedure = d4c_love_train;
        g_bedrock->ends[i].argument = nullptr;
        g_bedrock->ends[i].name = "bedrock/ends";
    }
    lake_submit_work_and_yield(g_bedrock->thread_count, g_bedrock->ends);
    LAKE_UNREACHABLE;
}

s32 lake_in_the_lungs(
    PFN_lake_framework main, 
    void              *userdata,
    lake_framework    *framework)
{
    usize ram_budget, page_size, huge_page_size = 0;
    sys_meminfo(&ram_budget, &page_size);
    if (framework->hints.memory_budget == 0)
        framework->hints.memory_budget = lake_align(((ram_budget) & ~(HEAP_BLOCK_SIZE - 1)), 8lu * HEAP_BLOCK_SIZE);

    s32 cpu_count = 0;
    sys_cpuinfo(&cpu_count, nullptr, nullptr);
    if (framework->hints.worker_thread_count == 0 || framework->hints.worker_thread_count > (u32)cpu_count)
        framework->hints.worker_thread_count = cpu_count;

    if (framework->hints.huge_page_size == 0)
        framework->hints.huge_page_size = 4lu*1024*1024;
    sys_hugetlbinfo(&huge_page_size, framework->hints.huge_page_size);
    if (huge_page_size != framework->hints.huge_page_size)
        framework->hints.huge_page_size = huge_page_size;

    if (framework->hints.fiber_stack_size == 0)
        framework->hints.fiber_stack_size = 64lu * 1024;
    if (framework->hints.fiber_count == 0)
        framework->hints.fiber_count = 96 + 4 * framework->hints.worker_thread_count;
    if (framework->hints.log2_work_count == 0)
        framework->hints.log2_work_count = 11; /* 2048 */

    { /* setup the framework */
        struct work    *work_array = nullptr;
        lake_mpmc_node *work_nodes = nullptr;
        usize bedrock_bytes     = lake_align(sizeof(struct bedrock), LAKE_CACHELINE_SIZE);
        usize work_count        = 1lu << framework->hints.log2_work_count;
        usize work_array_bytes  = lake_align(sizeof(struct work) * work_count, 16);
        usize work_nodes_bytes  = lake_align(sizeof(lake_mpmc_node) * work_count, 16);
        usize tls_bytes         = lake_align(sizeof(struct tls) * framework->hints.worker_thread_count, 16);
        usize ends_bytes        = lake_align(sizeof(lake_work_details) * framework->hints.worker_thread_count, 16);
        usize threads_bytes     = lake_align(sizeof(sys_thread_id) * framework->hints.worker_thread_count, 16);
        usize fibers_bytes      = lake_align(sizeof(struct fiber) * framework->hints.fiber_count, 16);
        usize waiting_bytes     = lake_align(sizeof(atomic_usize) * framework->hints.fiber_count, 16);
        usize free_bytes        = lake_align(sizeof(atomic_usize) * framework->hints.fiber_count, 16);
        usize locks_bytes       = lake_align(sizeof(atomic_usize) * framework->hints.fiber_count, 16);
        usize stack_bytes       = lake_align(framework->hints.fiber_stack_size, 16);
        usize stack_heap_bytes  = stack_bytes * framework->hints.fiber_count;

        usize total_bytes = 
            bedrock_bytes +
            work_array_bytes +
            work_nodes_bytes +
            tls_bytes +
            ends_bytes +
            threads_bytes +
            fibers_bytes +
            waiting_bytes +
            free_bytes +
            locks_bytes +
            stack_heap_bytes;
        usize commitment = lake_min(lake_align(total_bytes, framework->hints.huge_page_size), framework->hints.memory_budget);

        g_bedrock = sys_mmap(framework->hints.memory_budget, framework->hints.huge_page_size);
        lake_assert(g_bedrock != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, "Can't map internal memory for the framework.");

        if (!sys_madvise(g_bedrock, 0u, commitment, sys_madvise_mode_commit)) {
            lake_fatal("Internal memory mapped, but could not be commited into.");
            return LAKE_PANIC;
        }
        lake_memset(g_bedrock, 0u, commitment);

        u8 *raw = (u8 *)g_bedrock;
        usize o = bedrock_bytes;
    
        work_array          = (struct work *)       &raw[o]; o += work_array_bytes;
        work_nodes          = (lake_mpmc_node *)    &raw[o]; o += work_nodes_bytes;
        g_bedrock->tls      = (struct tls *)        &raw[o]; o += tls_bytes;
        g_bedrock->ends     = (lake_work_details *) &raw[o]; o += ends_bytes;
        g_bedrock->threads  = (sys_thread_id *)     &raw[o]; o += threads_bytes;
        g_bedrock->fibers   = (struct fiber *)      &raw[o]; o += fibers_bytes;
        g_bedrock->waiting  = (atomic_usize *)      &raw[o]; o += waiting_bytes;
        g_bedrock->free     = (atomic_usize *)      &raw[o]; o += free_bytes;
        g_bedrock->locks    = (atomic_usize *)      &raw[o]; o += locks_bytes;
        g_bedrock->stack    = (u8 *)&raw[o];

        lake_san_assert(!(((sptr)work_array)        & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)work_nodes)        & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.tls)     & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.ends)    & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.threads) & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.fibers)  & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.waiting) & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.free)    & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.locks)   & 15), LAKE_PANIC, nullptr);
        lake_san_assert(!(((sptr)g_bedrock.stack)   & 15), LAKE_PANIC, nullptr);

        g_bedrock->thread_count = framework->hints.worker_thread_count;
        g_bedrock->fiber_count = framework->hints.fiber_count;
        g_bedrock->budget = framework->hints.memory_budget;
        g_bedrock->page_size = framework->hints.huge_page_size;
        g_bedrock->commitment = commitment;
        g_bedrock->stack_size = stack_bytes;

        lake_mpmc_ring_init(&g_bedrock->work_queue, work_count, work_nodes, sizeof(struct work), work_array);
    }
    framework->timer_start = lake_rtc_counter();

    for (s32 i = 0; i < g_bedrock->fiber_count; i++) {
        lake_atomic_write(&g_bedrock->free[i], i);
        lake_atomic_write(&g_bedrock->locks[i], FIBER_INVALID);
        lake_atomic_write(&g_bedrock->waiting[i], FIBER_INVALID);
    }
    g_bedrock->tls[0].fiber_in_use = (u32)get_free_fiber();
#if defined(LAKE_PLATFORM_UNIX)
    g_bedrock->threads[0] = (sys_thread_id)pthread_self();
#elif defined(LAKE_PLATFORM_WINDOWS)
    g_bedrock->threads[0] = (sys_thread_id)GetCurrentThreadId();
#endif /* LAKE_PLATFORM_UNIX */

    lake_atomic_write_explicit(&g_bedrock->tls_sync, 0lu, lake_memory_model_release);
    for (s32 i = 1; i < g_bedrock->thread_count; i++) {
        struct tls *tls = &g_bedrock->tls[i];
        tls->fiber_in_use = (u32)FIBER_INVALID;
        sys_thread_create(&g_bedrock->threads[i], dirty_deeds_done_dirt_cheap, (void *)tls);
    }
    sys_thread_affinity(g_bedrock->thread_count, g_bedrock->threads, cpu_count, 0);
    lake_atomic_write_explicit(&g_bedrock->tls_sync, 1lu, lake_memory_model_release);

    struct application app = {
        .main = main,
        .argument = userdata,
        .framework = framework,
    };
    lake_work_details work = {
        .procedure = funny_valentine,
        .argument = (void *)&app,
        .name = "lake/in_the_lungs",
    };

    lake_submit_work(1, &work, nullptr);
    dirty_deeds_done_dirt_cheap((void *)&g_bedrock->tls[0]);
    /* won't resume until the application returns */
    sys_munmap(g_bedrock, g_bedrock->budget);
    g_bedrock = nullptr;

    /* returns the last recorded error code and resets the internal status value */
    return lake_exit_status(LAKE_SUCCESS);
}
