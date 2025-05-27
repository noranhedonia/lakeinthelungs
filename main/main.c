#define LAKE_IN_THE_LUNGS_MAIN
#include "main.h"

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER 1024

#define PIPELINE_WORK_COUNT (8)
#define PIPELINE_WORK_MASK  (PIPELINE_WORK_COUNT - 1)
#define GAMEPLAY_STAGE_INDEX  0
#define RENDERING_STAGE_INDEX 1
#define GPUEXEC_STAGE_INDEX   2

static FN_LAKE_WORK(a_moonlit_walk__main, struct a_moonlit_walk *amw)
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

                lake_frame_time_record(amw->timer_start, time_now, dt_freq_reciprocal);
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

    /* TODO destroy the game state and framework */

    dt = lake_frame_time_median();
    lake_trace("Last recorded frame time: %.3f ms (%.0f FPS).", 1000.f * dt, 1.f/dt);
}

s32 lake_main(s32 argc, const char **argv)
{
    struct a_moonlit_walk amw = {0};

    (void)argc;
    (void)argv;

    lake_abort(lake_in_the_lungs((PFN_lake_work)a_moonlit_walk__main, &amw));
}
