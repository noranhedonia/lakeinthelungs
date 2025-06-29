#pragma once

/** @file lake/graphics/renderer.h
 *  @brief Responsible for scheduling graphics work and managing render data.
 *
 *  TODO docs
 */
#include <lake/modules/moon.h>

#ifdef LAKE_HAS_RENDERDOC
#include <renderdoc_app.h>
#endif /* LAKE_HAS_RENDERDOC */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Controls how rendering work is done. */
typedef enum lake_render_mgpu_strategy : s8 {
    /** Allow the initialization process to figure out what strategy is optimal. */
    lake_render_mgpu_strategy_auto_optimal = 0,
    /** Rendering is done on a single primary device, no mGPU-related scheduling or transfer overhead.
     *  It's the default strategy if any other strategies were deemed unoptimal. Most systems that 
     *  only have a single GPU in their system will be using this strategy. */
    lake_render_mgpu_strategy_primary_optimal,
    /** Rendering on the discrete GPU is supported by the less-powerful integrated GPU.
     *  The rendering work is distributed such that the integrated GPU nor work transfers 
     *  must not be a bottleneck. Only secondary-priority work is distributed to the iGPU. */
    lake_render_mgpu_strategy_pair_discrete_integrated,
    /** Multi-device rendering, explicit primary device and multiple secondary devices. 
     *  Sort first redistributes primitives early in the graphics pipeline, during geometry 
     *  processing. This is achieved by dividing the screenspace such that each GPU renders 
     *  it's own region, and the final image is composed from the contributing regions. */
    lake_render_mgpu_strategy_sort_first,
    /** Multi-device rendering, explicit primary device and multiple secondary devices.
     *  Sort last is defined as deferring primitive redistribution until the end of the graphics 
     *  pipeline. It divides primitives such that each GPU renders it's own portion of mesh data 
     *  into the framebuffer. Then these are composed into the final image. */
    lake_render_mgpu_strategy_sort_last,
} lake_render_mgpu_strategy;

/** Information needed to setup a renderer. */
typedef struct lake_renderer_assembly {
    /** Interface of the rendering backend. */
    moon_interface                  moon;
    /** Requested mGPU strategy. */
    lake_render_mgpu_strategy       mgpu_strategy;
    /** Index into PFN_moon_list_device_details to use as the primary device. */
    s32                             primary_device_idx;
    /** Maximum devices to be used in an mGPU setting. */
    s32                             max_devices;
    /** Creates virtual devices from a single GPU, useful for testing mGPU work on single GPU systems.
     *  This setting is always ignored on non-debug builds, or if debug instruments are disabled. */
    s32                             dbg_virtual_devices;
    /** Must be no less than the count of stages that access the renderer to run in parallel. */
    u32                             frames_in_flight;
} lake_renderer_assembly;

/** Encapsulates rendering work for parallel and mGPU setups. */
typedef struct lake_renderer {
    /** Details used to create this renderer. */
    lake_renderer_assembly          assembly;
    /** A list of all created devices, device at index 0 is the primary. */
    struct moon_device_impl       **devices;
    /** How many devices are available to us. */
    s32                             device_count;
    /** Devices in our mGPU strategies are explicitly managed, where one device is considered the 
     *  primary device, and others are considered secondary. The primary has more responsibility. */
    moon_device                     primary_device;
    /** An array of render submits, allocated from (device_count * assembly->max_frames_in_flight).
     *  They should be redistributed between pipeline works, with a per-device submit list per frame.
     *
     *  Access this way: submits[device_idx + (timeline % max_frames_in_flight) * device_count]. */
    struct lake_render_submit      *submits;
} lake_renderer;

/** Per-device GPU submition, acquired from the results of rendering. 
 *  Every per-command queue submit may refer to a specific render graph frequency. */
typedef struct lake_render_submit {
    union {
        struct {
            moon_device_submit      main; /**< One per frame, MOON_QUEUE_MAIN */
            moon_device_submit      compute[MOON_MAX_COMPUTE_QUEUE_COUNT];
            moon_device_submit      transfer[MOON_MAX_TRANSFER_QUEUE_COUNT];
            moon_device_submit      sparse[MOON_MAX_SPARSE_BINDING_QUEUE_COUNT];
            moon_device_submit      decode[MOON_MAX_VIDEO_DECODE_QUEUE_COUNT];
            moon_device_submit      encode[MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT];
        };
        moon_device_submit          queue[MOON_QUEUE_INDEX_COUNT]; 
    } command;
    /** Bitmask constructed from (1u << MOON_QUEUE_INDEX) bits to access submitions
     *  defined above, for specific command queues. A quick popcnt calculates total 
     *  submits and distributes this work between independent jobs during GPUexec. */
    u32                             bitmask;
} lake_render_submit;

#ifdef __cplusplus
}
#endif /* __cplusplus */
