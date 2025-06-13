#pragma once

#include <lake/inthelungs.h>

static constexpr s32 RENDER_TARGETS_COUNT = 5;
static constexpr s32 RASTER_PIPELINES_COUNT = 2;
static constexpr s32 COMPUTE_PIPELINES_COUNT = 1;
static constexpr s32 RT_PIPELINES_COUNT = 1;


enum pipeline_stage_hint : s32 {
    pipeline_stage_hint_continue = 0,
    pipeline_stage_hint_save_and_exit,
    pipeline_stage_hint_try_recover,
};

/** After I figure out some use patterns for my interfaces I may move the details into a public header file. */
struct a_moonlit_walk {
    lake_framework const           *framework;
    imgui_tools                     imgui;

    soma_interface                  soma;

    /** */
    hadal_interface                 hadal;
    /** Main window for the game application. */
    hadal_window                    primary_window;
    /** Holds the primary window and it's children. */
    lake_darray_t(hadal_window)     windows;

    /** Interface for the rendering backend. */
    moon_interface                  moon;
    /** Points to first device in the `devices` darray. */
    moon_device                     primary_device;
    /** Rendering devices available to us. TODO different strategies for mGPU rendering. */
    lake_darray_t(moon_device)      devices;

    /** Points to first swapchain in the `swapchains` darray. */
    moon_swapchain                  primary_swapchain;
    /** Swapchains used in the application. One swapchain per window.
     *  A swapchain index will match the current window index in `windows`. */
    lake_darray_t(moon_swapchain)   swapchains;

    /** Limits how many frames the CPU can get ahead of the GPU (usually 2 to 4). */
    u32                             frames_in_flight;
    /** Controls the gameloop. */
    LAKE_ATOMIC(enum pipeline_stage_hint) stage_hint;
    /* PROTOTYPE:
     * - Only primary device & swapchain (no mGPU, no imgui docking) */
    union {
        moon_texture_id                 v[RENDER_TARGETS_COUNT];
        struct {
            moon_texture_id             normal_map;
            moon_texture_id             albedo_buffer;
            moon_texture_id             material_buffer;
            moon_texture_id             depth_buffer;
            moon_texture_id             velocity_buffer;
        };
    /* one copy per frame in flight */
    } *render_targets;
    union {
        moon_compute_pipeline           v[COMPUTE_PIPELINES_COUNT];
        struct {
            moon_compute_pipeline       lighting_pass;
        };
    } compute_pipelines;
    union {
        moon_raster_pipeline            v[RASTER_PIPELINES_COUNT];
        struct {
            moon_raster_pipeline        geometry_pass;
            moon_raster_pipeline        interface_pass;
        };
    } raster_pipelines;
    union {
        moon_ray_tracing_pipeline       v[RT_PIPELINES_COUNT];
        struct {
            moon_ray_tracing_pipeline   path_trace_pass;
        };
    } rt_pipelines;

    moon_buffer_id                      scene_vtx_buffer;
    moon_buffer_id                      scene_idx_buffer;
    moon_tlas_id                        scene_tlas;
    moon_blas_id                        scene_blas;
};

struct pipeline_work {
    u64                             timeline;
    f64                             dt;

    /** Lifetime: rendering(write) -> gpuexec(read) */
    lake_darray_t(moon_swapchain)           swapchains;
    /** Lifetime: rendering(write) -> gpuexec(read) */
    lake_darray_t(moon_staged_command_list) cmd_lists;

    struct a_moonlit_walk          *amw;
    struct pipeline_work const     *last_work;
    struct pipeline_work           *next_work;
};

extern FN_LAKE_WORK(a_moonlit_walk__gameplay, struct pipeline_work *work);
extern FN_LAKE_WORK(a_moonlit_walk__rendering, struct pipeline_work *work);
extern FN_LAKE_WORK(a_moonlit_walk__gpuexec, struct pipeline_work *work);

extern lake_result game_prototype_init(struct a_moonlit_walk *amw);
extern void game_prototype_fini(struct a_moonlit_walk *amw);
