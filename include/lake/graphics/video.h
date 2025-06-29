#pragma once

/** @file lake/graphics/video.h
 *  @brief Serves as a layer between the display backend and user interaction.
 *
 *  TODO docs
 */
#include <lake/modules/hadal.h>
#include <lake/modules/hadean.h>
#include <lake/modules/moon.h>

#include <lake/data_structures/darray.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Information needed to setup a video layer. */
typedef struct lake_video_assembly {
    /** Interface of the display backend. */
    hadal_interface                 hadal;
    /** Interface of the XR backend, is optional. */
    hadean_interface                hadean;
    /** How many viewports can be at use, always assume a minimum of 1. */
    s32                             max_viewports;
} lake_video_assembly;

/** Collects handles related to a single window. */
typedef struct lake_video_viewport {
    /** The window this viewport relates to. */
    hadal_window                    window;
    /** Swapchain bound to the window surface. */
    moon_swapchain                  swapchain;
} lake_video_viewport;

/** Manages the display and XR backends. */
typedef struct lake_video {
    /** Details used to create this video layer. */
    struct lake_video_assembly      assembly;
    /** The native windows from the display backend. */
    struct hadal_window_impl      **windows;
    /** Swapchain obtained from the renderer, indices match windows. */
    struct moon_swapchain_impl    **swapchains;
    /** How many viewports are active. */
    atomic_s32                      active_viewport_count;
    /** Collects handles of the primary window and it's rendering context.
     *  If the window in this viewport closes, the video layer will too. */
    lake_video_viewport             primary_viewport;
} lake_video;

#ifdef __cplusplus
}
#endif /* __cplusplus */
