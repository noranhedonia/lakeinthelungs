#pragma once

/** @file lake/devel/imgui.h
 *  @brief ImGui custom backends and helper procedures.
 *
 *  Support for ImGui must be built in the engine. The docking branch is used.
 *
 *  The backends are implemented for both the renderer and the platform 
 *  layer using Moon and Hadal respectively. The ImGui shaders are written 
 *  in Slang and can be compiled at either runtime or compile time.
 *
 *  [Dear ImGui library]
 *  https://github.com/ocornut/imgui
 *
 *  [The C API wrapper for ImGui]
 *  https://github.com/cimgui/cimgui
 */
#ifdef LAKE_IMGUI
#include <lake/data_structures/darray.h>
#include <lake/moon.h>
#include <lake/hadal.h>
#include <cimgui.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct imgui_tools;

/** A sampled image used within ImGui, pairs the texture view and sampler. */
typedef struct imgui_image_context {
    moon_texture_view_id        texture_view;
    moon_sampler_id             sampler;
} imgui_image_context;

/** Can be passed into the assembly to change the ImGui style. */
PFN_LAKE_WORK(PFN_imgui_style, struct imgui_tools const *imgui);
#define FN_IMGUI_STYLE(fn) \
    FN_LAKE_WORK(fn, struct imgui_tools const *imgui)

/** The default style, will be used if no custom style was given. */
FN_IMGUI_STYLE(imgui_default_style_light);

/** Details needed to create an ImGui tooling context. */
typedef struct imgui_tools_assembly {
    /** The primary window. The docking branch may allow to create multiple windows as 
     *  child objects to this window. If this window closes, the ImGui context will too. */
    hadal_window            window;
    /** The selected rendering device. Only one device is allowed to perform work for ImGui.
     *  This may be the primary device, or a secondary device (e.g. the integrated gpu).
     *  If work is done on a secondary GPU, external setup must be done to transfer an 
     *  intermediate texture between the secondary GPU and primary GPU's swapchain images. */
    moon_device             device;
    /** The texture format used within swapchain images. */
    moon_format             format;
    /** Pointer to the library context, if left as nullptr will be provided at init. */
    ImGuiContext           *context;
    /** A custom style selector can be provided, otherwise default is used. */
    PFN_imgui_style         style; 
} imgui_tools_assembly;

/** Collects the internal and ImGui library data needed to access custom dev tools.
 *  XXX for now it is not thread safe. */
typedef struct imgui_tools {
    imgui_tools_assembly    assembly;
    moon_raster_pipeline    raster;
    moon_buffer_id          vertex_buffer;
    moon_buffer_id          index_buffer;
    moon_texture_id         font_sheet;
    moon_sampler_id         font_sampler;
    u64                     cpu_timeline;
    lake_darray_t(imgui_image_context) image_sampler_pairs;
} imgui_tools;

/** Create the imgui tools using provided assembly details. Will implement the 
 *  backend callbacks for both the platform layer and the renderer. Will not try 
 *  to allocate `dev_imgui` structure, must be provided by the application. */
LAKEAPI LAKE_NONNULL_ALL LAKE_NODISCARD 
lake_result LAKECALL imgui_tools_init(
    imgui_tools_assembly const *assembly, 
    imgui_tools                *out_imgui);

/** Destroy the imgui tools. Will zero out the `dev_imgui` structure. */
LAKEAPI FN_LAKE_WORK(imgui_tools_fini, imgui_tools *imgui);

/** Creates a texture used for direct ImGui calls. */
LAKE_NONNULL_ALL
LAKEAPI ImTextureID LAKECALL 
imgui_create_texture_id(
    imgui_tools               *imgui, 
    imgui_image_context const *context);

/** Populates details in a raster pipeline assembly for use by the internal renderer. */
LAKE_NONNULL_ALL

/** Updates the interface pass raster pipeline used when recording commands.
 *  The assembled pipeline must use the same device as given at initialization. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void imgui_set_raster_pipeline(
    imgui_tools            *imgui,
    moon_raster_pipeline    raster)
{
    lake_dbg_assert(raster.device->v == imgui->assembly.device.v, LAKE_ERROR_DEVICE_NOT_SUPPORTED, 
            "Given raster pipeline's device (%s) and ImGui tools device (%s) are different.",
            raster.device->header->assembly.name.str, imgui->assembly.device.header->assembly.name.str);
    moon_raster_pipeline old_raster = imgui->raster;
    moon_raster_pipeline_ref(raster);
    imgui->raster = raster;
    if (old_raster.v != nullptr)
        moon_raster_pipeline_unref(old_raster);
}

/** Translates ImGui draw commands into vertex and index buffers, 
 *  and into direct API calls to the rendering backend. */
LAKEAPI lake_result LAKECALL
imgui_record_commands(
    imgui_tools            *imgui, 
    moon_command_recorder   cmd, 
    moon_texture_id         target_texture,
    ImDrawData const       *draw_data, 
    u32                     size_x,
    u32                     size_y);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LAKE_IMGUI */
