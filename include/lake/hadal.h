#pragma once

/** @file lake/hadal.h
 *  @brief Display backend and XR runtime.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>
#include <lake/input/keycodes.h>
#include <lake/input/scancodes.h>

/** The platform-specific display backend. */
LAKE_DECL_HANDLE(hadal_adapter);
/** Represents a display output, maps directly to hardware. A backend without displays is headless. */
LAKE_DECL_HANDLE(hadal_display);
/** Represents a system window with a surface we can draw to. Main context of interaction with the display. */
LAKE_DECL_HANDLE(hadal_window);
/** Represents a keyboard device. */
LAKE_DECL_HANDLE(hadal_keyboard);
/** Represents a mouse or pointer device. */
LAKE_DECL_HANDLE(hadal_mouse);
/** Represents a haptic (force feedback) device. */
LAKE_DECL_HANDLE(hadal_haptic);
/** Represents a low-level joystick raw inputs. */
LAKE_DECL_HANDLE(hadal_joystick);
/** Represents a gamepad, built on top of the joystick API to map console-style gamepads.
 *  The difference between a joystick and a gamepad is the gamepad tells "where" a button 
 *  or axis is on the device, by providing a configuration for the joystick device. */
LAKE_DECL_HANDLE(hadal_gamepad);
/** Represents a touch device, especially important on mobile platforms. */
LAKE_DECL_HANDLE(hadal_touch);
/** Represents a pen device. */
LAKE_DECL_HANDLE(hadal_pen);

typedef enum hadal_window_flag_bits : u32 {
    hadal_window_flag_none              = 0u,
    hadal_window_flag_is_valid          = (1u << 0),
    hadal_window_flag_should_close      = (1u << 1),
    hadal_window_flag_visible           = (1u << 2),
    hadal_window_flag_fullscreen        = (1u << 3),
    hadal_window_flag_maximized         = (1u << 4),
    hadal_window_flag_minimized         = (1u << 5),
    hadal_window_flag_resizable         = (1u << 6),
    hadal_window_flag_no_decorations    = (1u << 7),
    hadal_window_flag_input_focus       = (1u << 8),
    hadal_window_flag_mouse_focus       = (1u << 9),
    hadal_window_flag_mouse_captured    = (1u << 11),
    hadal_window_flag_mouse_relative    = (1u << 12),
    hadal_window_flag_cursor_hovered    = (1u << 13),
    hadal_window_flag_modal             = (1u << 14),
    hadal_window_flag_occluded          = (1u << 15),
    hadal_window_flag_always_on_top     = (1u << 16),
    hadal_window_flag_auto_minimize     = (1u << 17),
    hadal_window_flag_shell_activated   = (1u << 18),
    hadal_window_flag_mouse_grabbed     = (1u << 19),
    hadal_window_flag_keyboard_grabbed  = (1u << 20),
    hadal_window_flag_transparent       = (1u << 21),
    hadal_window_flag_vulkan            = (1u << 22),
} hadal_window_flag_bits;

typedef struct hadal_window_assembly {
    s32                 width, height;
    s32                 min_width, min_height;
    s32                 max_width, max_height;
    u32                 flag_hints;
    lake_small_string   name;
} hadal_window_assembly;

typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_hadal_window_assembly)(hadal_adapter hadal, hadal_window_assembly const *assembly, hadal_window *out_window);
#define FN_HADAL_WINDOW_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _hadal_##backend##_window_assembly(hadal_adapter hadal, hadal_window_assembly const *assembly, hadal_window *out_window)

PFN_LAKE_HANDLE_DESTRUCTOR(hadal, window);
#define FN_HADAL_WINDOW_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, hadal, window)

#ifdef MOON_VULKAN
/* to avoid including the Vulkan header */
struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
typedef void (*(*PFN_vkGetInstanceProcAddr)(struct VkInstance_T *, char const *))(void);

/** Allows the display backend to query procedures it needs to support Vulkan. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_hadal_vulkan_connect_instance)(hadal_adapter hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr);
#define FN_HADAL_VULKAN_CONNECT_INSTANCE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _hadal_##backend##_vulkan_connect_instance(hadal_adapter hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr)

/** Returns whether a queue family in the physical device is capable of presenting to a display. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_hadal_vulkan_presentation_support)(hadal_adapter hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family);
#define FN_HADAL_VULKAN_PRESENTATION_SUPPORT(backend) \
    LAKE_NODISCARD bool LAKECALL _hadal_##backend##_vulkan_presentation_support(hadal_adapter hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family)

/** Create a surface that a Vulkan renderer may draw to. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_hadal_vulkan_create_surface)(hadal_window window, struct VkAllocationCallbacks const *callbacks, struct VkSurfaceKHR_T **out_vk_surface);
#define FN_HADAL_VULKAN_CREATE_SURFACE(ENCORE) \
    LAKE_NODISCARD lake_result LAKECALL _hadal_##ENCORE##_vulkan_create_surface(hadal_window window, struct VkAllocationCallbacks const *callbacks, struct VkSurfaceKHR_T **out_vk_surface)
#endif /* MOON_VULKAN */

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
    PFN_hadal_window_destructor             window_destructor;
} hadal_interface_impl;

/** A view into the display interface implementation. */
typedef union hadal_interface {
    lake_interface_header                  *header;
    struct hadal_adapter_impl              *adapter;
    struct hadal_interface_impl            *interface;
    void                                   *v;
} hadal_interface;

/** Details needed to assemble a display backend. */
typedef struct hadal_interface_assembly {
    lake_framework const                   *framework;
    hadal_interface                        *out_impl; 
} hadal_interface_assembly;

/** Header for `struct hadal_window_impl`. */
typedef struct hadal_window_header {
    hadal_interface         hadal;
    atomic_u32              flags;
    lake_refcnt             refcnt;
    hadal_window_assembly   assembly;
    char const             *title;
    hadal_display           fullscreen;
} hadal_window_header;

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
LAKEAPI FN_LAKE_WORK(hadal_interface_assembly_wayland, hadal_interface_assembly const *assembly);
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
