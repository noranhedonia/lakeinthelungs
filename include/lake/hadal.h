#pragma once

/** @file lake/hadal.h
 *  @brief The display backend.
 *
 *  TODO docs
 */
#include <lake/bedrock.h>
#include <lake/data_structures/darray.h>
#include <lake/data_structures/strbuf.h>
#include <lake/input/keycodes.h>
#include <lake/input/scancodes.h>

/** The platform-specific display backend. */
LAKE_DECL_INTERFACE(hadal);
/** Represents a display output, maps directly to hardware. A backend without displays is headless. */
LAKE_DECL_HANDLE_INTERFACED(hadal_display, hadal_interface hadal);
/** Represents a system window with a surface we can draw to. Main context of interaction with the display. */
LAKE_DECL_HANDLE_INTERFACED(hadal_window, hadal_interface hadal);
/** Represents a keyboard device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_keyboard, hadal_interface hadal);
/** Represents a mouse or pointer device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_mouse, hadal_interface hadal);
/** Represents a haptic (force feedback) device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_haptic, hadal_interface hadal);
/** Represents a low-level joystick raw inputs. */
LAKE_DECL_HANDLE_INTERFACED(hadal_joystick, hadal_interface hadal);
/** Represents a gamepad, built on top of the joystick API to map console-style gamepads.
 *  The difference between a joystick and a gamepad is the gamepad tells "where" a button 
 *  or axis is on the device, by providing a configuration for the joystick device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_gamepad, hadal_joystick joystick);
/** Represents a touch device, especially important on mobile platforms. */
LAKE_DECL_HANDLE_INTERFACED(hadal_touch, hadal_interface hadal);
/** Represents a pen device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_pen, hadal_interface hadal);

typedef enum hadal_window_flag_bits : u32 {
    hadal_window_flag_none                  = 0u,
    hadal_window_flag_is_valid              = (1u << 0),
    hadal_window_flag_should_close          = (1u << 1),
    hadal_window_flag_visible               = (1u << 2),
    hadal_window_flag_fullscreen            = (1u << 3),
    hadal_window_flag_maximized             = (1u << 4),
    hadal_window_flag_minimized             = (1u << 5),
    hadal_window_flag_resizable             = (1u << 6),
    hadal_window_flag_no_decorations        = (1u << 7),
    hadal_window_flag_input_focus           = (1u << 8),
    hadal_window_flag_mouse_focus           = (1u << 9),
    hadal_window_flag_mouse_captured        = (1u << 11),
    hadal_window_flag_mouse_relative        = (1u << 12),
    hadal_window_flag_cursor_hovered        = (1u << 13),
    hadal_window_flag_modal                 = (1u << 14),
    hadal_window_flag_occluded              = (1u << 15),
    hadal_window_flag_always_on_top         = (1u << 16),
    hadal_window_flag_auto_minimize         = (1u << 17),
    hadal_window_flag_shell_activated       = (1u << 18),
    hadal_window_flag_mouse_grabbed         = (1u << 19),
    hadal_window_flag_keyboard_grabbed      = (1u << 20),
    hadal_window_flag_transparent           = (1u << 21),
    hadal_window_flag_vulkan                = (1u << 22),
    hadal_window_flag_swapchain_out_of_date = (1u << 23),
} hadal_window_flag_bits;

typedef struct hadal_window_assembly {
    s32                 width, height;
    s32                 min_width, min_height;
    s32                 max_width, max_height;
    u32                 flag_hints;
    hadal_display       fullscreen;
    lake_small_string   name;
} hadal_window_assembly;

typedef struct hadal_window_impl *(LAKECALL *PFN_hadal_window_assembly)(struct hadal_impl *hadal, hadal_window_assembly const *assembly);
#define FN_HADAL_WINDOW_ASSEMBLY(backend) \
    struct hadal_window_impl *LAKECALL _hadal_##backend##_window_assembly(struct hadal_impl *hadal, hadal_window_assembly const *assembly)

PFN_LAKE_WORK(PFN_hadal_window_zero_refcnt, struct hadal_window_impl *window);
#define FN_HADAL_WINDOW_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_hadal_##backend##_window_zero_refcnt, struct hadal_window_impl *window)

#ifdef MOON_VULKAN
/* to avoid including the Vulkan header */
struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
typedef void (*(*PFN_vkGetInstanceProcAddr)(struct VkInstance_T *, char const *))(void);

/** Allows the display backend to query procedures it needs to support Vulkan. */
typedef lake_result (LAKECALL *PFN_hadal_vulkan_connect_instance)(struct hadal_impl *hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr);
#define FN_HADAL_VULKAN_CONNECT_INSTANCE(backend) \
    lake_result LAKECALL _hadal_##backend##_vulkan_connect_instance(struct hadal_impl *hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr)

/** Returns whether a queue family in the physical device is capable of presenting to a display. */
typedef bool (LAKECALL *PFN_hadal_vulkan_presentation_support)(struct hadal_impl *hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family);
#define FN_HADAL_VULKAN_PRESENTATION_SUPPORT(backend) \
    bool LAKECALL _hadal_##backend##_vulkan_presentation_support(struct hadal_impl *hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family)

/** Create a surface that a Vulkan renderer may draw to. */
typedef lake_result (LAKECALL *PFN_hadal_vulkan_create_surface)(struct hadal_window_impl *window, struct VkAllocationCallbacks const *callbacks, struct VkSurfaceKHR_T **out_surface);
#define FN_HADAL_VULKAN_CREATE_SURFACE(backend) \
    lake_result LAKECALL _hadal_##backend##_vulkan_create_surface(struct hadal_window_impl *window, struct VkAllocationCallbacks const *callbacks, struct VkSurfaceKHR_T **out_surface)
#endif /* MOON_VULKAN */

/** Interface of the display backend. */
struct hadal_interface_impl {
    lake_interface_header                   header;
    lake_darray_t(hadal_display)            displays;
#ifdef MOON_VULKAN
    PFN_hadal_vulkan_connect_instance       vulkan_connect_instance;
    PFN_hadal_vulkan_presentation_support   vulkan_presentation_support;
    PFN_hadal_vulkan_create_surface         vulkan_create_surface;
#endif /* MOON_VULKAN */
    PFN_hadal_window_assembly               window_assembly;
    PFN_hadal_window_zero_refcnt            window_zero_refcnt;
};

LAKE_IMPL_HANDLE_INTERFACED(hadal_window, hadal_interface hadal,  atomic_u32 fb_width, fb_height; lake_strbuf title; )

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
