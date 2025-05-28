#include "wl_hadal.h"

#if defined(HADAL_WAYLAND) && defined(MOON_VULKAN) 
FN_HADAL_VULKAN_CONNECT_INSTANCE(wayland)
{
    lake_dbg_assert(hadal && vk_instance && vkGetInstanceProcAddr, LAKE_INVALID_PARAMETERS, nullptr);

    hadal->vulkan.vk_instance = vk_instance;
    hadal->vulkan.vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)
        (void *)vkGetInstanceProcAddr(vk_instance, "vkCreateWaylandSurfaceKHR");
    hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)
        (void *)vkGetInstanceProcAddr(vk_instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    if (!hadal->vulkan.vkCreateWaylandSurfaceKHR || !hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR)
    {
        hadal->vulkan.vk_instance = nullptr;
        hadal->vulkan.vkCreateWaylandSurfaceKHR = nullptr;
        hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
        return LAKE_ERROR_EXTENSION_NOT_PRESENT;
    }
    return LAKE_SUCCESS;
}

FN_HADAL_VULKAN_PRESENTATION_SUPPORT(wayland)
{
    lake_dbg_assert(hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR != nullptr, LAKE_ERROR_FEATURE_NOT_PRESENT, nullptr);
    return (bool)hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR(vk_physical_device, queue_family, hadal->wl_display);
}

FN_HADAL_VULKAN_CREATE_SURFACE(wayland)
{
    hadal_adapter hadal = window->header.hadal.adapter;
    lake_dbg_assert(hadal->vulkan.vkCreateWaylandSurfaceKHR != nullptr, LAKE_ERROR_FEATURE_NOT_PRESENT, nullptr);

    struct VkWaylandSurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .display = hadal->wl_display,
        .surface = window->surface,
    };
    s32 result = hadal->vulkan.vkCreateWaylandSurfaceKHR(hadal->vulkan.vk_instance, &surface_info, callbacks, out_vk_surface);

    if (result == -1)
        return LAKE_ERROR_OUT_OF_HOST_MEMORY;
    else if (result == -2)
        return LAKE_ERROR_OUT_OF_DEVICE_MEMORY;
    return LAKE_SUCCESS;
}
#endif /* HADAL_WAYLAND && MOON_VULKAN */
