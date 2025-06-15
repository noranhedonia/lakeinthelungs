#define LAKE_IN_THE_LUNGS_MAIN
#include "a_moonlit_walk.h"

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER 1024

#define PIPELINE_WORK_COUNT   (8)
#define PIPELINE_WORK_MASK    (PIPELINE_WORK_COUNT - 1)
#define GAMEPLAY_STAGE_INDEX  0
#define RENDERING_STAGE_INDEX 1
#define GPUEXEC_STAGE_INDEX   2

#define MAX_MOON_DEVICES      8

static FN_LAKE_WORK(engine_init__platform, struct a_moonlit_walk *amw)
{
    amw->hadal.impl = hadal_interface_impl_wayland(amw->framework);
    if (!amw->hadal.impl)
        return;

    hadal_window_assembly const window_assembly = {
        .width = 1200,
        .height = 800,
        .flag_hints = hadal_window_flag_visible,
        .fullscreen.impl = nullptr,
        .name = { "prototype", lake_arraysize("prototype") },
    };
    amw->primary_window.impl = amw->hadal.interface->window_assembly(amw->hadal.impl, &window_assembly);
    if (amw->primary_window.v == nullptr) {
        hadal_interface_unref(amw->hadal);
    }
    lake_darray_append_t(&amw->windows.da, hadal_window, &amw->primary_window);
}

static FN_LAKE_WORK(engine_init__audio, struct a_moonlit_walk *amw)
{
    amw->soma.impl = soma_interface_impl_pipewire(amw->framework);
    if (!amw->soma.impl)
        return;
}

static FN_LAKE_WORK(engine_init__renderer, struct a_moonlit_walk *amw)
{
    amw->moon.impl = moon_interface_impl_vulkan(amw->framework);
    if (!amw->moon.impl)
        return;

    u32 device_count = 0;
    amw->moon.interface->list_device_details(amw->moon.impl, &device_count, nullptr); 

    moon_device_details const *device_details[MAX_MOON_DEVICES];
    amw->moon.interface->list_device_details(amw->moon.impl, &device_count, device_details); 

    moon_device_assembly device_assembly = MOON_DEVICE_ASSEMBLY_INIT;
    device_assembly.explicit_features |= moon_explicit_feature_vulkan_memory_model;
    device_assembly.name = (lake_small_string){ .str = "primary", .len = sizeof("primary"), };
    device_assembly.device_idx = 0; /* pick the first device as our main */

    lake_result result = amw->moon.interface->device_assembly(amw->moon.impl, &device_assembly, &amw->primary_device.impl);
    if (result != LAKE_SUCCESS) {
        lake_exit_status(result);
        moon_interface_unref(amw->moon);
    }
    lake_darray_append_t(&amw->devices.da, moon_device, &amw->primary_device);
}

static lake_result LAKECALL engine_init(struct a_moonlit_walk *amw)
{
    lake_result result = LAKE_SUCCESS;
    bool enable_imgui = amw->framework->hints.enable_debug_instruments;

    /* TODO implement deferred allocator */
    lake_darray_init_t(&amw->windows.da, hadal_window, 2);
    lake_darray_init_t(&amw->swapchains.da, moon_swapchain, 2);
    lake_darray_init_t(&amw->devices.da, moon_device, 2);
    lake_thadvise(32lu*1024lu*1024lu, lake_thadvise_commit | lake_thadvise_suboptimal);

    lake_work_details const init_work[3] = {
        { /* platform */
            .procedure = (PFN_lake_work)engine_init__platform,
            .argument = (void *)amw,
            .name = "main/init/platform",
        },
        { /* audio */
            .procedure = (PFN_lake_work)engine_init__audio,
            .argument = (void *)amw,
            .name = "main/init/audio",
        },
        { /* renderer */
            .procedure = (PFN_lake_work)engine_init__renderer,
            .argument = (void *)amw,
            .name = "main/init/renderer",
        },
    };
    lake_submit_work_and_yield(3, init_work);
    if (!amw->hadal.v || !amw->soma.v || !amw->moon.v) {
        result = LAKE_ERROR_INITIALIZATION_FAILED;
        return result;
    }
    result = amw->moon.interface->connect_to_display(amw->moon.impl, amw->hadal.impl);
    if (result != LAKE_SUCCESS)
        return result;
    // moon_swapchain_assembly const swapchain_assembly = {
    //     .native_window = amw->window.impl,
    //     .queue_type = moon_queue_type_main,
    //     .max_allowed_frames_in_flight = 4,
    //     .image_usage = moon_texture_usage_color_attachment,
    //     .present_mode = moon_present_mode_mailbox,
    //     .present_transform = moon_present_transform_identity,
    //     .surface_format_selector = nullptr,
    // };
    // moon_swapchain swapchain;
    // result = amw->moon.interface->swapchain_assembly(amw->primary_device.impl, &swapchain_assembly, &swapchain.impl);
    // if (result != LAKE_SUCCESS)
    //     return result;
    // lake_darray_append_t(&amw->swapchains.da, moon_swapchain, &swapchain);

    if (enable_imgui) {
        imgui_tools_assembly const imgui_assembly = {
            .device = amw->primary_device,
            .window = amw->primary_window,
            .format = moon_format_r8g8b8a8_srgb, /* TODO obtain it from the swapchain */
            .style = imgui_default_style_light,
        };
        result = imgui_tools_init(&imgui_assembly, &amw->imgui);
        if (result != LAKE_SUCCESS)
            return result;
    }
    return game_prototype_init(amw);
}

static void LAKECALL engine_fini_(struct a_moonlit_walk *amw)
{
    LAKE_UNUSED lake_result __ignore = LAKE_SUCCESS;

    game_prototype_fini(amw);
    if (amw->primary_device.impl)
        __ignore = amw->moon.interface->device_commit_deferred_destructors(amw->primary_device.impl);
    imgui_tools_fini(&amw->imgui);

    lake_darray_foreach_v(amw->swapchains, moon_swapchain, swapchain)
        moon_swapchain_unref(*swapchain);
    /* TODO implement deferred allocator */
    lake_darray_fini(&amw->swapchains.da);
    amw->primary_swapchain.v = nullptr;

    lake_darray_foreach_v(amw->devices, moon_device, device)
        moon_device_unref(amw->primary_device);
    /* TODO implement deferred allocator */
    lake_darray_fini(&amw->devices.da);
    amw->primary_device.v = nullptr;

    if (amw->moon.v)
        lake_dec_refcnt(&amw->moon.header->refcnt, amw->moon.v, amw->moon.header->zero_refcnt);
    if (amw->soma.v)
        lake_dec_refcnt(&amw->soma.header->refcnt, amw->soma.v, amw->soma.header->zero_refcnt);

    lake_darray_foreach_v(amw->windows, hadal_window, window)
        hadal_window_unref(*window);
    /* TODO implement deferred allocator */
    lake_darray_fini(&amw->windows.da);
    amw->primary_window.v = nullptr;

    if (amw->hadal.v)
        lake_dec_refcnt(&amw->hadal.header->refcnt, amw->hadal.v, amw->hadal.header->zero_refcnt);
}

#define engine_fini(AMW) \
    ({ \
        for (s32 i = 0; i < PIPELINE_WORK_COUNT; i++) { \
            /* TODO implement deferred allocator */ \
            lake_darray_fini(&pipeline_work[i].cmd_lists.da); \
            lake_darray_fini(&pipeline_work[i].swapchains.da); \
        } \
        engine_fini_(AMW); \
    })

static FN_LAKE_FRAMEWORK(a_moonlit_walk__main)
{
    struct a_moonlit_walk amw = {0};
    u64 timeline = 0, time_last = 0, time_now = 0;
    f64 const dt_freq_reciprocal = 1.0f/(f64)lake_rtc_frequency();
    f64 dt = 0.0;

    struct pipeline_work pipeline_work[PIPELINE_WORK_COUNT];
    lake_zeroa(pipeline_work);
    for (s32 i = 0; i < PIPELINE_WORK_COUNT; i++) {
        pipeline_work[i].amw = &amw;
        pipeline_work[i].last_work = &pipeline_work[(i-1 + PIPELINE_WORK_MASK) & PIPELINE_WORK_MASK];
        pipeline_work[i].next_work = &pipeline_work[(i+1) & PIPELINE_WORK_MASK];
        lake_darray_init_t(&pipeline_work[i].cmd_lists.da, moon_staged_command_list, 8);
        lake_darray_init_t(&pipeline_work[i].swapchains.da, moon_swapchain, 2);
    }
    lake_work_details stages[3];
    stages[GAMEPLAY_STAGE_INDEX].procedure = (PFN_lake_work)a_moonlit_walk__gameplay;
    stages[GAMEPLAY_STAGE_INDEX].name = "main/gameplay";
    stages[RENDERING_STAGE_INDEX].procedure = (PFN_lake_work)a_moonlit_walk__rendering;
    stages[RENDERING_STAGE_INDEX].name = "main/rendering";
    stages[GPUEXEC_STAGE_INDEX].procedure = (PFN_lake_work)a_moonlit_walk__gpuexec;
    stages[GPUEXEC_STAGE_INDEX].name = "main/gpuexec";
    amw.framework = framework;
    amw.frames_in_flight = 3;

    if (engine_init(&amw) != LAKE_SUCCESS) {
        engine_fini(&amw);
        return;
    }

    /* this additional loop controls engine state updates */
    do {
        /* TODO get any means to update and determine if state is valid */
        if (lake_atomic_read(&amw.stage_hint) == pipeline_stage_hint_try_recover)
            break;

        struct pipeline_work *gameplay = pipeline_work;
        struct pipeline_work *rendering = nullptr;
        struct pipeline_work *gpuexec = nullptr;

        lake_work_chain       gameplay_chain = nullptr;
        lake_work_chain       rendering_chain = nullptr;
        lake_work_chain       gpuexec_chain = nullptr;

        /* TODO setup and update the game and engine state */
        s32 close_counter = DEBUG_CLOSE_COUNTER;

        while (gameplay || rendering || gpuexec) {
            bool try_recover = false;

            if (lake_likely(gpuexec)) { /* timeline N-2 */
                lake_yield(gpuexec_chain);
                gpuexec_chain = nullptr;

                try_recover = lake_atomic_read(&amw.stage_hint) == pipeline_stage_hint_try_recover;
                stages[GPUEXEC_STAGE_INDEX].argument = try_recover ? nullptr : gpuexec;
                lake_submit_work(1, &stages[GPUEXEC_STAGE_INDEX], &gpuexec_chain);
            }

            if (lake_likely(rendering)) { /* timeline N-1 */
                lake_yield(rendering_chain);
                rendering_chain = nullptr;

                try_recover = lake_atomic_read(&amw.stage_hint) == pipeline_stage_hint_try_recover;
                stages[RENDERING_STAGE_INDEX].argument = try_recover ? nullptr : rendering;
                lake_submit_work(1, &stages[RENDERING_STAGE_INDEX], &rendering_chain);
            }

            if (lake_likely(gameplay)) { /* timeline N */
                lake_yield(gameplay_chain);
                gameplay_chain = nullptr;

                time_last = time_now;
                time_now = lake_rtc_counter();
                dt = ((f64)(time_now - time_last) * dt_freq_reciprocal);

                lake_frame_time_record(framework->timer_start, time_now, dt_freq_reciprocal);
                lake_frame_time_print(1000.f);

                lake_darray_clear(&gameplay->swapchains.da);
                lake_darray_clear(&gameplay->cmd_lists.da);
                gameplay->timeline = timeline;
                gameplay->dt = dt;

                try_recover = try_recover || lake_atomic_read(&amw.stage_hint) == pipeline_stage_hint_try_recover;
                stages[GAMEPLAY_STAGE_INDEX].argument = try_recover ? nullptr : gameplay;
                lake_submit_work(1, &stages[GAMEPLAY_STAGE_INDEX], &gameplay_chain);
            }
            /* FIXME debug, delete later */
            if (--close_counter <= 0) lake_atomic_write(&amw.stage_hint, pipeline_stage_hint_save_and_exit);
            if (try_recover) lake_atomic_write(&amw.stage_hint, pipeline_stage_hint_try_recover);

            gpuexec = rendering;
            rendering = gameplay;
            gameplay = lake_atomic_read(&amw.stage_hint) == pipeline_stage_hint_continue
                ? &pipeline_work[(timeline++) & (PIPELINE_WORK_COUNT - 1)] : nullptr;
        }
    } while (false);

    engine_fini(&amw);
    dt = lake_frame_time_median();
    lake_trace("Last recorded frame time: %.3f ms (%.0f FPS), on %u worker threads.", 1000.f * dt, 1.f/dt, framework->hints.worker_thread_count);
}

s32 lake_main(lake_framework *framework, s32 argc, const char **argv)
{
    (void)argv;
    framework->hints.worker_thread_count = (argc > 1 ? 1 : 0);
    framework->hints.fiber_stack_size = 128*1024; /* 128 KB bo vulkan daje dupy */
    lake_abort(lake_in_the_lungs(a_moonlit_walk__main, framework));
}
