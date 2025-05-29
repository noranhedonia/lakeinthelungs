#define LAKE_IN_THE_LUNGS_MAIN
#include "main.h"

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER 1024

#define PIPELINE_WORK_COUNT (8)
#define PIPELINE_WORK_MASK  (PIPELINE_WORK_COUNT - 1)
#define GAMEPLAY_STAGE_INDEX  0
#define RENDERING_STAGE_INDEX 1
#define GPUEXEC_STAGE_INDEX   2

static lake_result prototype_init(struct a_moonlit_walk *amw)
{
    lake_result result = LAKE_SUCCESS;
    hadal_interface_assembly hadal_assembly = {
        .framework = amw->framework,
        .out_impl = &amw->hadal,
    };
    soma_interface_assembly soma_assembly = {
        .framework = amw->framework,
        .out_impl = &amw->soma,
    };
    moon_interface_assembly moon_assembly = {
        .framework = amw->framework,
        .out_impl = &amw->moon,
    };

    /* for now we don't care about fallbacks to other backends */
    lake_work_details work[3] = {
        { /* hadal (display) */
            .procedure = (PFN_lake_work)hadal_interface_assembly_wayland,
            .argument = &hadal_assembly,
            .name = "main/prototype_init/hadal",
        },
        { /* soma (audio) */
            .procedure = (PFN_lake_work)soma_interface_assembly_pipewire,
            .argument = &soma_assembly,
            .name = "main/prototype_init/soma",
        },
        { /* moon (graphics) */
            .procedure = (PFN_lake_work)moon_interface_assembly_vulkan,
            .argument = &moon_assembly,
            .name = "main/prototype_init/moon",
        },
    };
    lake_submit_work_and_yield(3, work);

    if (!amw->hadal.v || !amw->soma.v || !amw->moon.v) {
        result = LAKE_ERROR_INITIALIZATION_FAILED;
        lake_exit_status(result);
    }
    return result;
}

static void prototype_fini(struct a_moonlit_walk *amw)
{
    if (amw->moon.v)
        lake_refcnt_dec(&amw->moon.header->refcnt, amw->moon.v, amw->moon.header->destructor);
    if (amw->soma.v)
        lake_refcnt_dec(&amw->soma.header->refcnt, amw->soma.v, amw->soma.header->destructor);
    if (amw->hadal.v)
        lake_refcnt_dec(&amw->hadal.header->refcnt, amw->hadal.v, amw->hadal.header->destructor);
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

        lake_dbg_2("Gameloop entry on timeline: %lu.", timeline);
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
        lake_dbg_2("Gameloop leave on timeline: %lu.", timeline);
    } while (stage_hint == pipeline_stage_hint_restart_engine);

    prototype_fini(amw);

    dt = lake_frame_time_median();
    lake_trace("Last recorded frame time: %.3f ms (%.0f FPS).", 1000.f * dt, 1.f/dt);
}

s32 lake_main(lake_framework *framework, s32 argc, const char **argv)
{
    struct a_moonlit_walk amw = {0};

    (void)argc;
    (void)argv;

    lake_abort(lake_in_the_lungs((PFN_lake_framework)a_moonlit_walk__main, &amw, framework));
}
