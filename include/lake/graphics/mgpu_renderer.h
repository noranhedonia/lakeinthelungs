#pragma once

/** @file lake/graphics/mgpu_renderer.h
 *  @brief Responsible for scheduling graphics work and managing render data between GPUs.
 *
 *  TODO docs
 */
#include <lake/modules/hadal.h>
#include <lake/modules/hadean.h>
#include <lake/modules/moon.h>

#ifdef LAKE_HAS_RENDERDOC
#include <renderdoc_app.h>
#endif /* LAKE_HAS_RENDERDOC */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Controls how rendering work is done. */
typedef enum lake_mgpu_strategy : s8 {
    /** Allow the initialization process to figure out what strategy is optimal. */
    lake_mgpu_strategy_auto_optimal = 0,
    /** Rendering is done on a single primary device, no mGPU-related scheduling or transfer overhead.
     *  It's the default strategy if any other strategies were deemed unoptimal. Most systems that 
     *  only have a single GPU in their system will be using this strategy. */
    lake_mgpu_strategy_primary_optimal,
    /** Rendering on the discrete GPU is supported by the less-powerful integrated GPU.
     *  The rendering work is distributed such that the integrated GPU nor work transfers 
     *  must not be a bottleneck. Only secondary-priority work is distributed to the iGPU. */
    lake_mgpu_strategy_pair_discrete_integrated,
    /** Multi-device rendering, explicit primary device and multiple secondary devices. 
     *  Sort first redistributes primitives early in the graphics pipeline, during geometry 
     *  processing. This is achieved by dividing the screenspace such that each GPU renders 
     *  it's own region, and the final image is composed from the contributing regions. */
    lake_mgpu_strategy_sort_first,
    /** Multi-device rendering, explicit primary device and multiple secondary devices.
     *  Sort last is defined as deferring primitive redistribution until the end of the graphics 
     *  pipeline. It divides primitives such that each GPU renders it's own portion of mesh data 
     *  into the framebuffer. Then these are composed into the final image. */
    lake_mgpu_strategy_sort_last,
} lake_mgpu_strategy;

/** Information needed to setup a renderer. */
typedef struct lake_mgpu_renderer_assembly {
    /** Interface of the display backend. */
    hadal_interface                 hadal;
    /** Interface of the XR backend, is optional. */
    hadean_interface                hadean;
    /** Interface of the rendering backend. */
    moon_interface                  moon;
    /** Requested mGPU strategy. */
    lake_mgpu_strategy              mgpu_strategy;
    /** Index into PFN_moon_list_device_details to use as the primary device. */
    s32                             primary_device_idx;
    /** Maximum devices to be used in an mGPU setting. */
    s32                             max_devices;
    /** How many viewports can be at use, always assume a minimum of 1. */
    s32                             max_viewports;
    /** Creates virtual devices from a single GPU, useful for testing mGPU work on single GPU systems.
     *  This setting is always ignored on non-debug builds, or if debug instruments are disabled. */
    s32                             dbg_virtual_devices;
    /** Must be no less than the count of stages that access the renderer to run in parallel. */
    u32                             frames_in_flight;
} lake_mgpu_renderer_assembly;

/** Collects handles related to a single window. */
typedef struct lake_render_viewport {
    /** The window this viewport relates to. */
    hadal_window                    window;
    /** Swapchain bound to the window surface. */
    moon_swapchain                  swapchain;
} lake_video_viewport;

/** Encapsulates rendering work for parallel and mGPU setups. */
typedef struct lake_mgpu_renderer {
    /** Details used to create this renderer. */
    lake_mgpu_renderer_assembly     assembly;
    /** The native windows from the display backend. */
    struct hadal_window_impl      **windows;
    /** Swapchain obtained from the renderer, indices match windows. */
    struct moon_swapchain_impl    **swapchains;
    /** A list of all created devices, device at index 0 is the primary. */
    struct moon_device_impl       **devices;
    /** Devices in our mGPU strategies are explicitly managed, where one device is considered the 
     *  primary device, and others are considered secondary. The primary has more responsibility. */
    moon_device                     primary_device;
    /** How many devices are available to us. */
    s32                             device_count;
    /** How many viewports are active. */
    atomic_s32                      active_viewport_count;
    /** Collects handles of the primary window and it's rendering context.
     *  If the window in this viewport closes, the renderer will too. */
    lake_video_viewport             primary_viewport;
} lake_mgpu_renderer;

#ifdef __cplusplus
}
#endif /* __cplusplus */
