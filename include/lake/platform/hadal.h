#pragma once

/** @file lake/platform/hadal.h
 *  @brief The display interface.
 *
 *  TODO docs
 */
#include <lake/platform/input/keycodes.h>
#include <lake/platform/input/scancodes.h>
#include <lake/platform/window.h>
#include <lake/data_structures/strbuf.h>

/** Interface of the display backend. */
typedef struct hadal_interface_impl {
    lake_interface_header                   header;
    s16                                     keycodes[256];
    s16                                     scancodes[hadal_keycode_last + 1];
    s8                                      keynames[hadal_keycode_last + 1][5];
    s8                                      keys[hadal_keycode_last + 1];
#ifdef MOON_VULKAN
    PFN_hadal_vulkan_connect_instance       vulkan_connect_instance;
    PFN_hadal_vulkan_presentation_support   vulkan_presentation_support;
    PFN_hadal_vulkan_create_surface         vulkan_create_surface;
#endif /* MOON_VULKAN */
    PFN_hadal_window_assembly               window_assembly;
    PFN_hadal_window_zero_refcnt            window_zero_refcnt;
} hadal_interface_impl;

LAKE_DECL_HANDLE_IMPL(hadal, window, hadal_interface, hadal, hadal_display fullscreen; lake_strbuf title; )

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef HADAL_WIN32
/* TODO */
#endif /* HADAL_WIN32 */
#ifdef HADAL_COCOA
/* TODO */
#endif /* HADAL_COCOA */
#ifdef HADAL_UIKIT
/* TODO */
#endif /* HADAL_UIKIT */
#ifdef HADAL_ANDROID
/* TODO */
#endif /* HADAL_ANDROID */
#ifdef HADAL_HAIKU
/* TODO */
#endif /* HADAL_HAIKU */
#ifdef HADAL_HTML5
/* TODO */
#endif /* HADAL_HTML5 */
#ifdef HADAL_WAYLAND
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, wayland, lake_framework);
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
/* TODO */
#endif /* HADAL_XCB */
#ifdef HADAL_KMS
/* TODO */
#endif /* HADAL_KMS */

#ifdef __cplusplus
}
#endif /* __cplusplus */
