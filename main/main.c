#define LAKE_IN_THE_LUNGS_MAIN
#include "main.h"

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER 1024

#define PIPELINE_WORK_COUNT (8)
#define PIPELINE_WORK_MASK  (PIPELINE_WORK_COUNT - 1)
#define GAMEPLAY_STAGE_INDEX  0
#define RENDERING_STAGE_INDEX 1
#define GPUEXEC_STAGE_INDEX   2

#define MAX_MOON_DEVICES    4

static lake_result prototype_init(struct a_moonlit_walk *amw)
{
    lake_result result = LAKE_SUCCESS;

    amw->hadal.adapter = hadal_interface_impl_wayland(amw->framework);
    amw->soma.adapter = soma_interface_impl_pipewire(amw->framework);
    amw->moon.adapter = moon_interface_impl_vulkan(amw->framework);

    if (!amw->hadal.v || !amw->soma.v || !amw->moon.v) {
        result = LAKE_ERROR_INITIALIZATION_FAILED;
        lake_exit_status(result);
        return result;
    }

    u32 moon_device_count = 0;
    amw->moon.interface->list_device_details(amw->moon.adapter, &moon_device_count, nullptr); 

    moon_device_details const *moon_device_details[MAX_MOON_DEVICES];
    amw->moon.interface->list_device_details(amw->moon.adapter, &moon_device_count, moon_device_details); 

    moon_device_assembly moon_device_work = MOON_DEVICE_ASSEMBLY_INIT;
    moon_device_work.name = (lake_small_string){ .str = "primary", .len = sizeof("primary"), };
    moon_device_work.device_idx = 0; /* pick the first device as our main */

    // result = amw->moon.interface->device_assembly(amw->moon.adapter, &moon_device_work, &amw->primary_device);
    // if (result != LAKE_SUCCESS) {
    //     lake_exit_status(result);
    //     return result;
    // }

    /* TODO:
     * - create window
     * - create swapchain */

    return result;
}

static void prototype_fini(struct a_moonlit_walk *amw)
{
    // if (amw->primary_device.v)
    //     lake_dec_refcnt(&amw->primary_device.header->refcnt, amw->primary_device.v, (PFN_lake_work)amw->moon.interface->device_zero_refcnt);
    if (amw->moon.v)
        lake_dec_refcnt(&amw->moon.header->refcnt, amw->moon.v, amw->moon.header->zero_refcnt);
    if (amw->soma.v)
        lake_dec_refcnt(&amw->soma.header->refcnt, amw->soma.v, amw->soma.header->zero_refcnt);
    if (amw->hadal.v)
        lake_dec_refcnt(&amw->hadal.header->refcnt, amw->hadal.v, amw->hadal.header->zero_refcnt);
}

static FN_LAKE_FRAMEWORK(a_moonlit_walk__main, struct a_moonlit_walk *amw)
{
    f64 dt = 0.0;
    f64 const dt_freq_reciprocal = 1.0f/(f64)lake_rtc_frequency();
    u64 timeline = 0, time_last = 0, time_now = 0;
    enum pipeline_stage_hint stage_hint;

    struct pipeline_work pipeline_work[PIPELINE_WORK_COUNT];
    lake_zeroa(pipeline_work);
    for (s32 i = 0; i < PIPELINE_WORK_COUNT; i++) {
        pipeline_work[i].amw = amw;
        pipeline_work[i].last_work = &pipeline_work[(i-1 + PIPELINE_WORK_COUNT) & PIPELINE_WORK_MASK];
        pipeline_work[i].last_work = &pipeline_work[(i+1 + PIPELINE_WORK_COUNT) & PIPELINE_WORK_MASK];
    }
    lake_work_details stages[3];
    stages[GAMEPLAY_STAGE_INDEX].procedure = (PFN_lake_work)a_moonlit_walk__gameplay;
    stages[GAMEPLAY_STAGE_INDEX].name = "main/gameplay";
    stages[RENDERING_STAGE_INDEX].procedure = (PFN_lake_work)a_moonlit_walk__rendering;
    stages[RENDERING_STAGE_INDEX].name = "main/rendering";
    stages[GPUEXEC_STAGE_INDEX].procedure = (PFN_lake_work)a_moonlit_walk__gpuexec;
    stages[GPUEXEC_STAGE_INDEX].name = "main/gpuexec";
    amw->framework = framework;

    if (prototype_init(amw) != LAKE_SUCCESS) {
        if (amw->hadal.v == nullptr)
            lake_error("Wayland prototype init failed.");
        if (amw->soma.v == nullptr)
            lake_error("PipeWire prototype init failed.");
        if (amw->moon.v == nullptr)
            lake_error("Vulkan prototype init failed.");
        prototype_fini(amw);
        return;
    }

    /* this additional loop controls engine state updates */
    do {stage_hint = pipeline_stage_hint_continue;
        /* TODO setup and update the game and engine state */
        s32 close_counter = DEBUG_CLOSE_COUNTER;

        struct pipeline_work *gameplay = pipeline_work;
        struct pipeline_work *rendering = nullptr;
        struct pipeline_work *gpuexec = nullptr;
        struct pipeline_work *resolve = nullptr;

        lake_dbg_3("Gameloop entry on timeline: %lu.", timeline);
        while (gameplay || rendering || gpuexec || resolve) {

            if (resolve) { /* timeline N-3 */
                lake_yield(resolve->chain);
                if (resolve->stage_hint != pipeline_stage_hint_continue)
                    stage_hint = resolve->stage_hint;
                resolve->chain = nullptr;
            }

            if (gpuexec) { /* timeline N-2 */
                lake_yield(gpuexec->chain);
                if (gpuexec->stage_hint != pipeline_stage_hint_continue)
                    stage_hint = gpuexec->stage_hint;
                gpuexec->chain = nullptr;

                stages[GPUEXEC_STAGE_INDEX].argument = gpuexec;
                lake_submit_work(1, &stages[GPUEXEC_STAGE_INDEX], &gpuexec->chain);
            }

            if (rendering) { /* timeline N-1 */
                lake_yield(rendering->chain);
                if (rendering->stage_hint != pipeline_stage_hint_continue)
                    stage_hint = rendering->stage_hint;
                rendering->chain = nullptr;

                stages[RENDERING_STAGE_INDEX].argument = rendering;
                lake_submit_work(1, &stages[RENDERING_STAGE_INDEX], &rendering->chain);
            }

            if (gameplay) { /* timeline N */
                time_last = time_now;
                time_now = lake_rtc_counter();
                dt = ((f64)(time_now - time_last) * dt_freq_reciprocal);

                lake_frame_time_record(framework->timer_start, time_now, dt_freq_reciprocal);
                lake_frame_time_print(1000.f);

                gameplay->timeline = timeline;
                gameplay->dt = dt;
                stages[GAMEPLAY_STAGE_INDEX].argument = gameplay;
                lake_submit_work(1, &stages[GAMEPLAY_STAGE_INDEX], &gameplay->chain);
            }

            /* FIXME debug, delete later */
            if (--close_counter <= 0) stage_hint = pipeline_stage_hint_save_and_exit;

            resolve = gpuexec;
            gpuexec = rendering;
            rendering = gameplay;
            gameplay = stage_hint == pipeline_stage_hint_continue 
                ? &pipeline_work[(timeline++) & (PIPELINE_WORK_COUNT - 1)] : nullptr;
        }
        lake_dbg_3("Gameloop leave on timeline: %lu.", timeline);
    } while (stage_hint == pipeline_stage_hint_restart_engine);

    prototype_fini(amw);

    dt = lake_frame_time_median();
    lake_trace("Last recorded frame time: %.3f ms (%.0f FPS), on %u worker threads.", 1000.f * dt, 1.f/dt, framework->hints.worker_thread_count);
}

s32 lake_main(lake_framework *framework, s32 argc, const char **argv)
{
    struct a_moonlit_walk amw = {0};

    (void)argv;
    framework->hints.worker_thread_count = (argc > 1 ? 1 : 0);
    framework->hints.fiber_stack_size = 128*1024; /* 128 KB bo vulkan daje dupy */

    lake_abort(lake_in_the_lungs((PFN_lake_framework)a_moonlit_walk__main, &amw, framework));
}
