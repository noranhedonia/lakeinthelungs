#pragma once

/** @file lake/hadal.h
 *  @brief Display backend and XR runtime.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>

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
    LAKE_NODISCARD bool LAKECALL _hadal_##backend##_vulkan_connect_instance(hadal_adapter hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr)

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
#ifdef MOON_VULKAN
    PFN_hadal_vulkan_connect_instance       vulkan_connect_instance;
    PFN_hadal_vulkan_presentation_support   vulkan_presentation_support;
    PFN_hadal_vulkan_create_surface         vulkan_create_surface;
#endif /* MOON_VULKAN */
} hadal_interface_impl;

/** A view into the display interface implementation. */
typedef union hadal_interface {
    lake_interface_header                  *header;
    struct hadal_adapter_impl              *adapter;
    struct hadal_interface_impl            *interface;
    void                                   *data;
} hadal_interface;

/** Details needed to assemble a display backend. */
typedef struct hadal_interface_assembly {
    hadal_interface                        *out_impl; 
} hadal_interface_assembly;

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
