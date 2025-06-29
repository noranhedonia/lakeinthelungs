#include <lake/sorceress.h>

/* must be a power of 2 */
#define MAX_PIPELINE_WORK_COUNT 8

static void LAKECALL perform_encore_movements(
    sorceress_encore_composer  *composer,
    sorceress_encore           *encore, 
    sorceress_control_flags    *control)
{
    lake_defer_begin;
    sorceress_composition const *live_composition = &encore->live_composition;
    sorceress_composition *write_composition = composer->composition;
    sorceress_control_flags flags = *control & ~(sorceress_control_flag_compose_movements);

    /* An empty current transition means there is no engine state, and resolving details is simpler. */
    bool const do_init = !lake_popcnt((u8 const *)live_composition, sizeof(sorceress_composition));
    /* An empty transition structure equals `sorceress_control_flag_exit_app` and will destroy all state. */
    bool const do_fini = !lake_popcnt((u8 const *)write_composition, sizeof(sorceress_composition));
    if (do_fini) flags |= sorceress_control_flag_exit_app;
    /* If neither initialization nor finalization, we'll only touch whatever detail has changed. */
    bool const do_update = !do_init && !do_fini;

    /* I'd like to not hold onto our temporary stuff here. ;3 */
    lake_drift_push();
    lake_defer({ 
        lake_drift_pop(); 
        encore->live_composition = *write_composition;
        *control = flags;
    });

    /* XXX if init, destroying state can be skipped */
    /* XXX if fini, creating state can be skipped */
    /* TODO DO THE THING MAN */

    /* TODO calculate and compare control sums for the current and pending transitions */
    (void)do_update;
    if (do_update) flags |= sorceress_control_flag_exit_app; // XXX

    /* TODO disable "fallback_to_any" inside modules, as it's intended to be a fail safe for either 
     * initialization or for hot-reloading custom modules. */

    if (do_fini) {

    } 
    if (do_fini || do_update) lake_defer_return;

    /* TODO init prototype */

    lake_defer_return;
}

enum : s8 {
    PIPELINE_GAMEPLAY_STAGE_IDX = 0,
    PIPELINE_RENDERING_STAGE_IDX,
    PIPELINE_GPUEXEC_STAGE_IDX,
    PIPELINE_STAGE_COUNT,
};

FN_LAKE_BEDROCK_MAIN(sorceress_bedrock_main, sorceress_composition *composition)
{
    /* Holds all engine state. Non-read access to this state is generally unsafe,
     * writes are done on synchronization checkpoints from within this function, 
     * and hot handles (like the native window, swapchains or meshes) are passed
     * as copies into per-frame pipeline work structures. */
    sorceress_encore encore = {
        .bedrock = bedrock,
    };
    /* To avoid division when calculating the delta time. */
    f64 const dt_freq_reciprocal = 1.0/(f64)lake_rtc_frequency();
    /* The current CPU timeline and RTC counters used to calculate the delta time for a given iteration.
     * Timeline is only incremented if any new work enters the pipeline. */
    u64 timeline = 0, time_last = 0, time_now = lake_rtc_counter();

    /* A job system chain for every pipeline stage. We want to make sure that no stage runs two 
     * instances of itself at a time. With a chain the stages can be synchronized by a context 
     * switch instead of busy-waiting for the stage to finish, then the thread will yield back 
     * to the job system and help with remaining work, before safely continuing the gameloop. */
    lake_work_chain work_chains[PIPELINE_STAGE_COUNT] = { nullptr, nullptr, nullptr };

    /* Job details per pipeline stage. They will run asynchronously, the responsibility 
     * of the gameloop is to keep them synchronized and to wait for them to end whenever
     * unsafe state transitions were issued. */
    lake_work_details work_details[PIPELINE_STAGE_COUNT];
    work_details[PIPELINE_GAMEPLAY_STAGE_IDX] = (lake_work_details){
        .procedure = (PFN_lake_work)sorceress_pipeline_gameplay,
        .name = "sorceress::gameplay",
    };
    work_details[PIPELINE_RENDERING_STAGE_IDX] = (lake_work_details){
        .procedure = (PFN_lake_work)sorceress_pipeline_rendering,
        .name = "sorceress::rendering",
    };
    work_details[PIPELINE_GPUEXEC_STAGE_IDX] = (lake_work_details){
        .procedure = (PFN_lake_work)sorceress_pipeline_gpuexec,
        .name = "sorceress::gpuexec",
    };
    /* Controls the gameloop and engine state transitions. */
    sorceress_control_flags control = sorceress_control_flag_compose_movements;
    /* The encore composer holds onto movements and commits them during an engine state transition.
     * We'll grab the initial composition directly from `lake_main()`. The composer forms a directed 
     * acyclic graph to resolve dependencies between state transitions, and if possible runs these 
     * transitions in parallel. All changes to encore state are done this way. */
    sorceress_encore_composer encore_composer = {
        .composition = composition,
    };
    /* Holds per-frame work that will be fed forward the pipeline stages. We assume every 
     * stage can safely run in parallel, if it runs on an unique instance of work, and only
     * if safe access patterns are honored. The gameloop will cycle between these structures 
     * when new work enters the pipeline. GPUexec works on work N-2, rendering on work N-1,
     * and gameplay runs on work N. When a work leaves the pipeline, it can be reused. */
    sorceress_pipeline_work pipeline_work[MAX_PIPELINE_WORK_COUNT];
    lake_zeroa(pipeline_work);
    for (u32 i = 0; i < MAX_PIPELINE_WORK_COUNT; i++) {
        sorceress_pipeline_work *w = &pipeline_work[i];
        w->worker_thread_count = bedrock->hints.worker_thread_count;
        w->frames_in_flight = bedrock->hints.frames_in_flight;
        w->encore_composer = &encore_composer;
        w->encore = &encore;
        w->last_work = &pipeline_work[(i + (MAX_PIPELINE_WORK_COUNT-2)) & (MAX_PIPELINE_WORK_COUNT-1)];
        w->next_work = &pipeline_work[(i + MAX_PIPELINE_WORK_COUNT) & (MAX_PIPELINE_WORK_COUNT-1)];
        lake_arena_init(&w->arena, 4096);
    }
    /* This additional loop is responsible for commiting any changes to the engine state, that here we'll be 
     * calling "movements". These may include init or fini of all engine systems, hot-reloading dynamic modules 
     * and their associated state, or any sanitary work to resolve critical issues during a normal game runtime. */
    do {perform_encore_movements(&encore_composer, &encore, &control);

        /* Just ignore the gameloop if movements fail. */
        if (control & sorceress_control_flag_exit_or_compose) 
            continue;

        /* These will be cycled, and set as the arguments before a submit to the job system. */
        sorceress_pipeline_work *gameplay = &pipeline_work[timeline & (MAX_PIPELINE_WORK_COUNT-1)];
        sorceress_pipeline_work *rendering = nullptr;
        sorceress_pipeline_work *gpuexec = nullptr;

        /* XXX debug, delete later */
        s32 close_counter = 1024;

        /* The gameloop. Until there is work for the current timeline, we'll iterate over it. */
        while (gameplay || rendering || gpuexec) {
            lake_work_details       *work;
            lake_work_chain         *chain;
            sorceress_pipeline_work *resolve;

#define RUN_PIPELINE_STAGE(stage, idx, ...) \
            work = &work_details[idx]; \
            chain = &work_chains[idx]; \
            /* resolve points to the work that is to be fed forward */ \
            resolve = (sorceress_pipeline_work *)work->argument; \
            work->argument = (void *)stage; \
            if (resolve) { \
                lake_yield(*chain); *chain = nullptr; \
                control |= resolve->control & (sorceress_control_flag_exit_or_compose); \
                __VA_ARGS__ \
            } if (stage)

            /* GAMEPLAY - timeline N */
            RUN_PIPELINE_STAGE(gameplay, PIPELINE_GAMEPLAY_STAGE_IDX) {
                time_last = time_now;
                time_now = lake_rtc_counter();
                lake_frame_time_record(bedrock->timer_start, time_now, dt_freq_reciprocal);
                lake_frame_time_print(6000.f);
                gameplay->timeline = timeline++;
                gameplay->dt = ((f64)(time_now - time_last) * dt_freq_reciprocal);
                gameplay->camera = gameplay->last_work->camera;
                lake_submit_work(1, work, chain);
            }
            /* RENDERING - timeline N-1 */
            RUN_PIPELINE_STAGE(rendering, PIPELINE_RENDERING_STAGE_IDX)
                { lake_submit_work(1, work, chain); }

            /* GPUEXEC - timeline N-2 */
            RUN_PIPELINE_STAGE(gpuexec, PIPELINE_GPUEXEC_STAGE_IDX,
                /* resolve points to work that just left the pipeline */
                resolve->control = 0;
                lake_arena_reset(&gameplay->arena);
                for (s32 i = 0; i < encore.renderer.device_count; resolve->submits[i++].bitmask = 0u);
                resolve->present = (moon_device_present){0};
            /* end of resolve */
            ){ lake_submit_work(1, work, chain); }

#undef RUN_PIPELINE_STAGE
            /* XXX debug, delete later; */
            close_counter -= 1;
            if (close_counter <= 0) control |= sorceress_control_flag_exit_app;

            if (lake_atomic_read(&encore.video.primary_viewport.window.header->flags) & hadal_window_flag_should_close)
                control |= sorceress_control_flag_exit_app;

            /* Cycle the pipeline work, or begin a transition. */
            gpuexec = rendering; 
            rendering = gameplay;
            gameplay = (control & sorceress_control_flag_exit_or_compose)
                ? nullptr : &pipeline_work[timeline & (MAX_PIPELINE_WORK_COUNT-1)];
        } 
        for (u32 i = 0; i < PIPELINE_STAGE_COUNT; lake_yield(work_chains[i++]));
        /* If only a transition was requested, this loop will iterate to commit any engine state updates. */
    } while ((control & sorceress_control_flag_exit_or_compose) == sorceress_control_flag_compose_movements);

    f64 const median = lake_frame_time_median();
    lake_trace("Last recorded frame time: %.3f ms (%.0f FPS).", 1000.f*median, 1.f/median);

    /* Destroy all engine state before exit. */
    lake_zerop(composition);
    perform_encore_movements(&encore_composer, &encore, &control);

    /* Cleanup the mallocs I guess. */
    for (u32 i = 0; i < MAX_PIPELINE_WORK_COUNT; i++) {
        sorceress_pipeline_work *w = &pipeline_work[i];
        lake_arena_fini(&w->arena);
    }
}

s32 sorceress_compose_w_args(
    sorceress_composition *out_composition, 
    s32                    argc, 
    char const           **argv)
{
    /* Populate the transition details with a default configuration. */
    sorceress_composition compose = {};
    /* TODO read from a config file */

    /* TODO parse arguments */
    (void)argc;
    (void)argv;

    *out_composition = compose;
    return LAKE_SUCCESS;
}
