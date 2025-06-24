#define LAKE_IN_THE_LUNGS_MAIN
#include <lake/inthelungs.h>

#define MAX_FRAMES_IN_FLIGHT 3

/* The test scene contains a floor plane, and a cube placed on top of it at the center. */
static constexpr vec3 g_scene_vtx_data[] = {
    /* floor plane */
    {-100.0f, 0,  100.0f},
    { 100.0f, 0,  100.0f},
    { 100.0f, 0, -100.0f},
    {-100.0f, 0, -100.0f},
    /* cube face (+y) */
    {-1.0f, 2.0,  1.0f},
    { 1.0f, 2.0,  1.0f},
    { 1.0f, 2.0, -1.0f},
    {-1.0f, 2.0, -1.0f},
    /* cube face (+z) */
    {-1.0f, 0.0, 1.0f},
    { 1.0f, 0.0, 1.0f},
    { 1.0f, 2.0, 1.0f},
    {-1.0f, 2.0, 1.0f},
    /* cube face (-z) */
    {-1.0f, 0.0, -1.0f},
    {-1.0f, 2.0, -1.0f},
    { 1.0f, 2.0, -1.0f},
    { 1.0f, 0.0, -1.0f},
    /* cube face (-x) */
    {-1.0f, 0.0, -1.0f},
    {-1.0f, 0.0,  1.0f},
    {-1.0f, 2.0,  1.0f},
    {-1.0f, 2.0, -1.0f},
    /* cube face (+x) */
    {1.0f, 2.0, -1.0f},
    {1.0f, 2.0,  1.0f},
    {1.0f, 0.0,  1.0f},
    {1.0f, 0.0, -1.0f},
};
static constexpr int g_scene_idx_data[] = {
     0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11, 12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
};

struct primitive {
    vec4 data;
    vec4 color;
};
static constexpr struct primitive g_scene_primitive_data[] = {
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.75f, 0.8f,  0.85f, 1.0f}},
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.75f, 0.8f,  0.85f, 1.0f}},
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f,  1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f,  1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f, -1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f, -1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{-1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{-1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
};

enum : s32 {
    render_pass_raster_gbuffer = 0,
    render_pass_raster_imgui,
    render_pass_raster_count,
};

enum : s32 {
    render_pass_ray_tracing_path_tracer = 0,
    render_pass_ray_tracing_count,
};

enum : s32 {
    render_target_normal = 0,   /* octahedral map (rg16f) */
    render_target_albedo,       /* color of non-transparent geometry (rgb8u) */
    render_target_orm,          /* ao, roughness, metalic, emissive (rgba8u) */
    render_target_velocity,     /* velocity of non-static geometry (rg16f) */
    render_target_depth,        /* depth buffer of non-transparent geometry (d32f) */
    render_target_count,
};

struct game_prototype {
    /** Hints and app metadata acquired from main. */
    lake_framework const       *framework;
    /** Editor tools using imgui. */
    imgui_tools                 imgui;
    /** Interface for the audio backend. */
    soma_interface              soma;

    /** Interface for the display backend. */
    hadal_interface             hadal;
    /** Main window for the game application. */
    hadal_window                window;

    /** Interface for the rendering backend. */
    moon_interface              moon;
    /** Main rendering device. */
    moon_device                 device;
    /** Swapchain connected to the main window. */
    moon_swapchain              swapchain;

    /** One set of geometry buffers per frame in flight, with following targets in order:
     *  - normal octahedral map (rg16f)
     *  - albedo (rgb8u)
     *  - ao + roughness + metalic (rgb8u)
     *  - velocity (rg16f)
     *  - depth (d32f)
     *  Offsets to this data is calculated from data format and window dimensions. */
    moon_texture_id             render_targets[render_target_count][MAX_FRAMES_IN_FLIGHT];
    /** Used to sample material data when rasterizing geometry in the gbuffer pass. */
    moon_sampler_id             gbuffer_sampler;

    /** Pipelines implemented by shaders. */
    moon_raster_pipeline        raster_pipelines[render_pass_raster_count];
    moon_ray_tracing_pipeline   ray_tracing_pipelines[render_pass_ray_tracing_count];

    /** Scene draw and geometry GPU data. */
    moon_buffer_id              scene_buffer;
    u64                         scene_buffer_vtx_offset;
    u64                         scene_buffer_idx_offset;
    u64                         scene_buffer_tlas_offset;
    u64                         scene_buffer_blas_offset;
    moon_tlas_id                scene_tlas;
    moon_blas_id                scene_blas;
};

static void fini_game_prototype(struct game_prototype *game)
{
    LAKE_UNUSED lake_result __ignore = LAKE_SUCCESS;
    u32 const frames_in_flight = game->framework->hints.frames_in_flight;

    lake_dbg_3("Main trace fini");
    imgui_tools_fini(&game->imgui);

    if (game->device.v) {
        for (u32 i = 0; i < render_target_count; i++)
            for (u32 j = 0; j < frames_in_flight; j++)
                __ignore = game->moon.interface->destroy_texture(game->device.impl, game->render_targets[i][j]);

        __ignore = game->moon.interface->destroy_tlas(game->device.impl, game->scene_tlas);
        __ignore = game->moon.interface->destroy_blas(game->device.impl, game->scene_blas);
        __ignore = game->moon.interface->destroy_buffer(game->device.impl, game->scene_buffer);
        __ignore = game->moon.interface->device_commit_deferred_destructors(game->device.impl);

        for (u32 i = 0; i < render_pass_raster_count; i++)
            if (game->raster_pipelines[i].v)
                moon_raster_pipeline_unref(game->raster_pipelines[i]);
        for (u32 i = 0; i < render_pass_ray_tracing_count; i++)
            if (game->ray_tracing_pipelines[i].v)
                moon_ray_tracing_pipeline_unref(game->ray_tracing_pipelines[i]);

        if (game->swapchain.v) 
            moon_swapchain_unref(game->swapchain);
        moon_device_unref(game->device);
    }
    if (game->window.v) 
        hadal_window_unref(game->window);

    if (game->moon.v) 
        moon_interface_unref(game->moon);
    if (game->hadal.v) 
        hadal_interface_unref(game->hadal);
    if (game->soma.v) 
        soma_interface_unref(game->soma);
    lake_zerop(game);
}

static FN_LAKE_WORK(init_audio, struct game_prototype *game)
{
    game->soma.impl = soma_interface_impl_pipewire(game->framework);
    if (game->soma.impl == nullptr)
        game->soma.impl = soma_interface_impl_dummy(game->framework);
}

static FN_LAKE_WORK(init_display, struct game_prototype *game)
{
    game->hadal.impl = hadal_interface_impl_wayland(game->framework);
    if (game->hadal.impl == nullptr)
        game->hadal.impl = hadal_interface_impl_headless(game->framework);
    if (game->hadal.impl == nullptr)
        return;

    hadal_window_assembly const window_assembly = {
        .width = 1200,
        .height = 800,
        .flag_hints = hadal_window_flag_visible,
        .fullscreen.impl = nullptr,
        .name = { "primary", lake_lengthof("primary") },
    };
    game->window.impl = game->hadal.interface->window_assembly(game->hadal.impl, &window_assembly);
    if (game->window.impl == nullptr) 
        hadal_interface_unref(game->hadal);
}

static FN_LAKE_WORK(init_renderer, struct game_prototype *game)
{
    game->moon.impl = moon_interface_impl_vulkan(game->framework);
    if (game->moon.impl == nullptr)
        game->moon.impl = moon_interface_impl_mock(game->framework);
    if (game->moon.impl == nullptr)
        return;

    u32 device_count = 0;
    game->moon.interface->list_device_details(game->moon.impl, &device_count, nullptr);
    if (device_count == 0) {
        lake_fatal("No physical rendering devices are available to us."
                "Consider installing a software driver for your graphics API in use.");
        return;
    }
    moon_device_details const **device_details = lake_drift_n(moon_device_details const *, device_count);
    game->moon.interface->list_device_details(game->moon.impl, &device_count, device_details);

    moon_device_assembly device_assembly = MOON_DEVICE_ASSEMBLY_INIT;
    device_assembly.name = (lake_small_string){ "primary", lake_lengthof("primary") };
    device_assembly.device_idx = 0; /* pick the first device as is */

    lake_result result = game->moon.interface->device_assembly(game->moon.impl, &device_assembly, &game->device.impl);
    if (result != LAKE_SUCCESS)
        moon_interface_unref(game->moon);
}

static s32 init_game_prototype(struct game_prototype *game)
{
    lake_defer_begin();
    lake_result result = LAKE_SUCCESS;
    bool const enable_imgui = game->framework->hints.enable_debug_instruments;
    lake_work_details const work[3] = {
        { /* renderer */
            .procedure = (PFN_lake_work)init_renderer,
            .argument = (void *)game,
            .name = "main::init_renderer",
        },
        { /* display */
            .procedure = (PFN_lake_work)init_display,
            .argument = (void *)game,
            .name = "main::init_display",
        },
        { /* audio */
            .procedure = (PFN_lake_work)init_audio,
            .argument = (void *)game,
            .name = "main::init_audio",
        },
    };
    lake_drift_push();
    lake_defer({ 
        if (result != LAKE_SUCCESS) {
            lake_error("Initialization failed with status: %d.", result);
            fini_game_prototype(game);
        }
        lake_drift_pop(); 
    });
    lake_submit_work_and_yield(3, work);

    if (!game->hadal.v || !game->soma.v || !game->moon.v) {
        result = LAKE_ERROR_INITIALIZATION_FAILED;
        lake_defer_return result;
    }
    result = game->moon.interface->connect_to_display(game->moon.impl, game->hadal.impl);
    lake_defer_return_if_status(result);

    moon_swapchain_assembly const swapchain_assembly = {
        .native_window = game->window.impl,
        .queue_type = moon_queue_type_main,
        .max_allowed_frames_in_flight = 4,
        .image_usage = moon_texture_usage_color_attachment | moon_texture_usage_transfer_src | moon_texture_usage_transfer_dst,
        .present_mode = moon_present_mode_mailbox,
        .present_transform = moon_present_transform_identity,
        .surface_format_selector = moon_default_surface_format_selector,
        .name = { .str = "primary", .len = lake_lengthof("primary") },
    };
    result = game->moon.interface->swapchain_assembly(game->device.impl, &swapchain_assembly, &game->swapchain.impl);
    lake_defer_return_if_status(result);

    if (enable_imgui) {
        moon_format const image_format = moon_format_r8g8b8a8_srgb; // game->swapchain.header->assembly.image_format;
        imgui_tools_assembly const tools_assembly = {
            .device = game->device,
            .window = game->window,
            .format = image_format,
            .style = imgui_default_style_light,
        };
        result = imgui_tools_init(&tools_assembly, &game->imgui);
    }
    lake_defer_return result;
}

struct framedata {
    u64                         timeline;
    f64                         dt;
    moon_staged_command_list   *cmd_lists;
    s32                         cmd_list_count;
};

static lake_result LAKECALL state_updates(struct game_prototype *game, struct framedata *frame)
{
    lake_result result = LAKE_SUCCESS;

    (void)g_scene_vtx_data;
    (void)g_scene_idx_data;
    (void)g_scene_primitive_data;
    (void)frame;

    u32 const window_flags = lake_atomic_and_explicit(&game->window.header->flags, ~(hadal_window_flag_swapchain_out_of_date), lake_memory_model_release);

    bool const update_swapchain = game->swapchain.v && (window_flags & hadal_window_flag_swapchain_out_of_date);
    bool const any_updates =
        update_swapchain; 

    if (!any_updates) return result;

    if (update_swapchain) {
        result = game->moon.interface->swapchain_resize(game->swapchain.impl);
        if (result != LAKE_SUCCESS) return result;
    }
    return result;
}

static lake_result LAKECALL rendering(struct game_prototype *game, struct framedata *frame)
{
    lake_defer_begin();
    lake_result result = LAKE_SUCCESS;

    moon_command_recorder_assembly const cmd_assembly = {
        .queue_type = moon_queue_type_main,
        .name = lake_small_string_cstr("rendering"),
    };
    moon_command_recorder cmd = {0};
    if (!game->swapchain.v) {
        lake_defer_return LAKE_SUCCESS;
    }
    result = game->moon.interface->command_recorder_assembly(game->device.impl, &cmd_assembly, &cmd.impl);
    lake_defer_return_if_status(result);

    lake_defer({ moon_command_recorder_unref(cmd); });

    moon_texture_id image;
    result = game->moon.interface->swapchain_acquire_next_image(game->swapchain.impl, &image);
    if (moon_id_is_empty(image)) {
        lake_defer_return result;
    }
    moon_texture_view_id image_view = moon_id_t(moon_texture_view_id, image);
    moon_texture_view_assembly image_view_assembly;
    result = game->moon.interface->read_texture_view_assembly(game->device.impl, image_view, &image_view_assembly);
    lake_dbg_assert(result == LAKE_SUCCESS, result, "read_texture_view_assembly");

    moon_access src_access = 0;
    moon_access dst_access = 0;

    moon_texture_barrier const transition = {
        .dst_accesses = &dst_access,
        .dst_access_count = 1,
        .dst_layout = moon_layout_optimal,
        .src_accesses = &src_access,
        .src_access_count = 1,
        .src_layout = moon_layout_optimal,
        .texture_slice = image_view_assembly.slice,
        .texture = image,
    };
    moon_pipeline_barrier const barriers_and_transitions = {
        .texture_barrier_count = 1,
        .texture_bariers = &transition,
    };

    src_access = moon_access_none;
    dst_access = moon_access_clear_write;
    result = game->moon.interface->cmd_pipeline_barrier(cmd.impl, &barriers_and_transitions);
    lake_dbg_assert(result == LAKE_SUCCESS, result, "cmd_pipeline_barrier, 1st");

    moon_clear_texture const clear_image = {
        .dst_texture = image,
        .dst_slice = image_view_assembly.slice,
        .dst_layout = moon_layout_optimal,
        .dst_access = moon_access_clear_write,
        .clear_value = { .color = { .vec = { 1.f, 0.f, 1.f, 1.f }}},
    };
    result = game->moon.interface->cmd_clear_texture(cmd.impl, &clear_image);
    lake_dbg_assert(result == LAKE_SUCCESS, result, "cmd_clear_texture");

    src_access = moon_access_clear_write;
    dst_access = moon_access_present;
    result = game->moon.interface->cmd_pipeline_barrier(cmd.impl, &barriers_and_transitions);
    lake_dbg_assert(result == LAKE_SUCCESS, result, "cmd_pipeline_barrier, 2nd");

    frame->cmd_list_count = 1;
    frame->cmd_lists = lake_drift_n(moon_staged_command_list, frame->cmd_list_count);

    moon_staged_command_list_assembly const staged_cmd_list_assembly = {
        .name = cmd_assembly.name,
    };
    result = game->moon.interface->staged_command_list_assembly(cmd.impl, &staged_cmd_list_assembly, &frame->cmd_lists[0].impl);
    lake_defer_return result;
}

static lake_result LAKECALL gpuexec(struct game_prototype *game, struct framedata *frame)
{
    lake_defer_begin();
    lake_result result = LAKE_SUCCESS;

    lake_defer({
        result = game->moon.interface->device_commit_deferred_destructors(game->device.impl);
        for (s32 i = 0; i < frame->cmd_list_count; i++)
            if (frame->cmd_lists[i].v) moon_staged_command_list_unref(frame->cmd_lists[i]);
    });
    if (frame->cmd_list_count <= 0) {
        lake_defer_return result;
    }

    moon_binary_semaphore acquire_sem = { .impl = game->moon.interface->swapchain_current_acquire_semaphore(game->swapchain.impl) };
    moon_binary_semaphore present_sem = { .impl = game->moon.interface->swapchain_current_present_semaphore(game->swapchain.impl) };
    moon_timeline_pair timeline_pair;
    result = game->moon.interface->swapchain_current_timeline_pair(game->swapchain.impl, &timeline_pair);

    moon_device_submit const submit = {
        .queue = MOON_QUEUE_MAIN,   
        .staged_command_list_count = frame->cmd_list_count,
        .staged_command_lists = (struct moon_staged_command_list_impl const **)&frame->cmd_lists[0].impl,
        .wait_binary_semaphore_count = 1,
        .wait_binary_semaphores = (struct moon_binary_semaphore_impl const **)&acquire_sem.impl,
        .wait_timeline_semaphore_count = 0,
        .wait_timeline_semaphores = nullptr,
        .signal_binary_semaphore_count = 1,
        .signal_binary_semaphores = (struct moon_binary_semaphore_impl const **)&present_sem.impl,
        .signal_timeline_semaphore_count = 1,
        .signal_timeline_semaphores = (moon_timeline_pair const *)&timeline_pair,
        .wait_stages = moon_access_none,
    };
    result = game->moon.interface->device_submit_commands(game->device.impl, &submit);
    lake_defer_return_if_status(result);

    moon_device_present const present = {
        .queue = MOON_QUEUE_MAIN,
        .swapchain_count = 1,
        .swapchains = (struct moon_swapchain_impl const **)&game->swapchain.impl,
        .wait_binary_semaphore_count = 1,
        .wait_binary_semaphores = (struct moon_binary_semaphore_impl const **)&acquire_sem.impl,
    };
    result = game->moon.interface->device_present_frames(game->device.impl, &present);
    lake_defer_return result;
}

typedef lake_result (LAKECALL *PFN_gameloop_step)(struct game_prototype *game, struct framedata *frame);

static void application(lake_framework const *framework)
{
    struct game_prototype game = { .framework = framework }; 
    f64 const dt_freq_reciprocal = 1.0f/(f64)lake_rtc_frequency();
    u64 time_last = 0, time_now = 0;
    bool should_close = false;

    PFN_gameloop_step const gameloop_steps[] = {
        state_updates,
        rendering,
        gpuexec,
    };
    (void)rendering;
    (void)gpuexec;
    constexpr u32 gameloop_step_count = lake_arraysize(gameloop_steps);

    lake_result result = init_game_prototype(&game);
    if (result != LAKE_SUCCESS) return;

    s32 close_counter = 1024;
    struct framedata frame = {0};

    while (!should_close) {
        time_last = time_now;
        time_now = lake_rtc_counter();
        frame.dt = ((f64)(time_now - time_last) * dt_freq_reciprocal);

        lake_frame_time_record(framework->timer_start, time_now, dt_freq_reciprocal);
        lake_frame_time_print(1000.f);

        lake_drift_push();
        for (u32 i = 0; i < gameloop_step_count; i++) {
            result = gameloop_steps[i](&game, &frame);
            if (result != LAKE_SUCCESS) {
                should_close = true;
                break;
            }
        }
        lake_drift_pop();
        should_close = should_close || (lake_atomic_read(&game.window.header->flags) & hadal_window_flag_should_close);

        frame.cmd_list_count = 0;
        frame.cmd_lists = nullptr;

        --close_counter;
        if (close_counter <= 0) should_close = true;

        frame.timeline++;
    }
    fini_game_prototype(&game);

    frame.dt = lake_frame_time_median();
    lake_trace("Last recorded frame time: %.3f ms (%.0f FPS), total run time was %.f s.", 1000.f * frame.dt, 1.f/frame.dt, 
            ((f64)(lake_rtc_counter() - framework->timer_start) * dt_freq_reciprocal));
}

s32 lake_main(lake_framework *framework, s32 argc, char const **argv)
{
    (void)argv;
    framework->hints.worker_thread_count = (argc > 1 ? 1 : 0);
    framework->hints.fiber_stack_size = 128*1024;
    framework->hints.frames_in_flight = MAX_FRAMES_IN_FLIGHT;
    // lake_log_enable_context(true);
    lake_abort(lake_in_the_lungs(application, framework));
}
