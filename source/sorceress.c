#include <lake/modules/sorceress.h>
#ifdef LAKE_SORCERESS

enum : s8 {
    PIPELINE_GAMEPLAY_STAGE_IDX = 0,
    PIPELINE_RENDERING_STAGE_IDX,
    PIPELINE_GPUEXEC_STAGE_IDX,
    PIPELINE_STAGE_COUNT,
};

FN_LAKE_BEDROCK_MAIN(sorceress_bedrock_main, PFN_lake_interface_impl impl)
{
    /* To avoid division when calculating the delta time. */
    f64 const dt_freq_reciprocal = 1.0/(f64)lake_rtc_frequency();

    /* The current CPU timeline and real-time clock counters used to calculate the delta time 
     * for a given iteration. Timeline is only incremented if any new work enters the pipeline. */
    u64 timeline = 0, time_last = 0, time_now = lake_rtc_counter();

    /* Controls the gameloop and engine state transitions. */
    sorceress_control_flags control = sorceress_control_flag_should_compose;

    /* Holds all engine and game state, lets setup it right now. */
    sorceress_interface sorceress = { .impl = impl(bedrock), };
    if (sorceress.v == nullptr) { 
        lake_exit_status(LAKE_ERROR_INITIALIZATION_FAILED); 
        return; 
    }
    /* This additional loop is responsible for commiting any changes to the engine state, that here we'll 
     * be calling "movements". These may include init or fini of all engine systems, hot-reloading dynamic 
     * modules and systems that use them as dependencies, user changing settings that normally would require 
     * a reboot, or for any sanitary work to resolve critical issues during game runtime. */
    do {lake_drift_push();

        /* Will be acquired from PFN_sorceress_acquire_work. */
        u32 work_count = 0;
        /* A job system chain for every pipeline stage. We want to make sure that no stage runs two 
         * instances of itself at a time. With a chain the stages can be synchronized by a context 
         * switch instead of busy-waiting for the stage to finish, then the thread will yield back 
         * to the job system and help with remaining work, before safely continuing the gameloop. */
        lake_work_chain work_chains[PIPELINE_STAGE_COUNT] = { nullptr, nullptr, nullptr };

        /* Job details per pipeline stage. They will run asynchronously, the responsibility 
         * of the gameloop is to keep them synchronized and to wait for them to end whenever
         * unsafe state transitions were issued. We set them up here in case the interface 
         * has reloaded it's implementation during movements. */
        lake_work_details work_details[PIPELINE_STAGE_COUNT];
        work_details[PIPELINE_GAMEPLAY_STAGE_IDX] = (lake_work_details){
            .procedure = (PFN_lake_work)sorceress.interface->stage_gameplay,
            .name = "sorceress::gameplay",
        };
        work_details[PIPELINE_RENDERING_STAGE_IDX] = (lake_work_details){
            .procedure = (PFN_lake_work)sorceress.interface->stage_rendering,
            .name = "sorceress::rendering",
        };
        work_details[PIPELINE_GPUEXEC_STAGE_IDX] = (lake_work_details){
            .procedure = (PFN_lake_work)sorceress.interface->stage_gpuexec,
            .name = "sorceress::gpuexec",
        };
        /* Holds per-frame work that will be fed forward between pipeline stages. We can assume 
         * every stage to safely run in parallel whenever they run on an unique instance of work. 
         * The gameloop will cycle between these structures when new work enters the pipeline. 
         * GPUexec runs on work N-2, rendering on work N-1, and gameplay runs on work N. 
         * When a work leaves the pipeline, it is considered done and will be recycled. */
        struct sorceress_work_impl *work_impl = nullptr;

        /* No need here for implementation details of what work will be done, just the public header.
         * So the responsibility to provide a work implementation was shifted to the application.
         * Acquire an opaque array and calculate offsets to access specific work on the fly. */
        usize const work_stride = sorceress.interface->acquire_work(sorceress.impl, &work_count, &work_impl);
        /* Assert correctness of the work array. */
        lake_dbg_assert(work_impl && lake_is_pow2(work_stride) && lake_is_pow2(work_count), LAKE_ERROR_MEMORY_MAP_FAILED, 
            "Acquired work array is invalid. Work count (%u), stride (%lu) and address (%p).", work_count, work_stride, work_impl);

        /* These will be cycled, and set as the arguments before a submit to the job system. */
        sorceress_work gameplay = { .impl = lake_elem(work_impl, work_stride, timeline & (work_count-1)) };
        sorceress_work rendering = { .impl = nullptr };
        sorceress_work gpuexec = { .impl = nullptr };

        /* The gameloop. Until there is work inside the pipeline, we'll continue. */
        while (gameplay.v || rendering.v || gpuexec.v) {
            lake_work_details  *work;       /* Points to the work details of currently processed stage. */
            lake_work_chain    *chain;      /* Points to the chain owned by currently processed stage. */
            sorceress_work      resolve;    /* Points to the instance of work data to be fed forward. */

#define RUN_PIPELINE_STAGE(stage, idx, ...) \
            work = &work_details[idx]; \
            chain = &work_chains[idx]; \
            resolve.v = work->argument; \
            work->argument = stage.v; \
            if (resolve.v) { \
                lake_yield(*chain); \
                *chain = nullptr; \
                control |= resolve.header->control; \
                __VA_ARGS__ \
            } if (stage.v)

            /* GPUEXEC     timeline N-2 */
            RUN_PIPELINE_STAGE(gpuexec, PIPELINE_GPUEXEC_STAGE_IDX, sorceress.interface->end_of_pipe(resolve.impl); )
                { lake_submit_work(1, work, chain); }

            /* RENDERING   timeline N-1 */
            RUN_PIPELINE_STAGE(rendering, PIPELINE_RENDERING_STAGE_IDX)
                { lake_submit_work(1, work, chain); }

            /* GAMEPLAY    timeline N   */
            RUN_PIPELINE_STAGE(gameplay, PIPELINE_GAMEPLAY_STAGE_IDX) {
                time_last = time_now;
                time_now = lake_rtc_counter();
                lake_frame_time_record(bedrock->timer_start, time_now, dt_freq_reciprocal);
                lake_frame_time_print(10.f);
                gameplay.header->timeline = timeline++;
                gameplay.header->dt = ((f64)(time_now - time_last) * dt_freq_reciprocal);
                control |= gameplay.header->control = sorceress.interface->begin_of_pipe(gameplay.impl);
                lake_submit_work(1, work, chain);
            }
#undef RUN_PIPELINE_STAGE
            /* Cycle the pipeline work, or begin a transition. */
            gpuexec = rendering; 
            rendering = gameplay;
            gameplay.v = !(control & sorceress_control_flag_should_exit_or_compose) 
                ? lake_elem(work_impl, work_stride, timeline & (work_count-1)) : nullptr;
        } 
        /* End of gameloop. Wait for any remaining work to finish. */
        for (u32 i = 0; i < PIPELINE_STAGE_COUNT; i++) { 
            lake_work_chain chain = work_chains[i];

            if (chain != nullptr) {
                sorceress_work resolve = { .v = work_details[i].argument };
                lake_yield(work_chains[i]); 
                control |= resolve.header->control;
                sorceress.interface->end_of_pipe(resolve.impl);
            }
            work_chains[i] = nullptr;
        }
        sorceress.interface->release_work(work_count, work_impl);
        lake_drift_pop();

        sorceress.interface->invoke_movements(sorceress.impl, &control);
    } while (!(control & sorceress_control_flag_should_exit));

    f32 const median = lake_frame_time_median();
    lake_trace("Last recorded frame time: %.3f ms (%.0f FPS).", 1000.f*median, 1.f/median);
    sorceress_interface_unref(sorceress);
}
#endif /* LAKE_SORCERESS */
