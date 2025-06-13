#include "vk_moon.h"
#include <lake/hadal.h>
#ifdef MOON_VULKAN

static s32 default_format_selector(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_R8G8B8A8_SRGB: return moon_format_r8g8b8a8_srgb;
        case VK_FORMAT_R8G8B8A8_UNORM: return moon_format_r8g8b8a8_unorm;
        case VK_FORMAT_B8G8R8A8_SRGB: return moon_format_b8g8r8a8_srgb;
        case VK_FORMAT_B8G8R8A8_UNORM: return moon_format_b8g8r8a8_unorm;
        default: return 0;
    }
}

static void partial_swapchain_cleanup(struct moon_swapchain_impl *swapchain)
{
    for (s32 i = 0; i < swapchain->images.da.size; i++) {
        lake_result _ignore = _moon_vulkan_destroy_texture(swapchain->header.device.impl, swapchain->images.v[i]);
        (void)_ignore;
    }
    swapchain->images.da.size = 0;
    lake_darray_clear(&swapchain->images.da);
}

static void full_swapchain_cleanup(struct moon_swapchain_impl *swapchain)
{
    struct moon_device_impl *device = swapchain->header.device.impl;
    struct moon_impl *moon = device->header.moon.impl;

    partial_swapchain_cleanup(swapchain);
    if (swapchain->vk_swapchain != VK_NULL_HANDLE) {
        device->vkDeviceWaitIdle(device->vk_device);
        device->vkDestroySwapchainKHR(device->vk_device, swapchain->vk_swapchain, device->vk_allocator);
    }
    if (swapchain->vk_surface != VK_NULL_HANDLE)
        moon->vkDestroySurfaceKHR(moon->vk_instance, swapchain->vk_surface, &moon->vk_allocator);
    if (swapchain->header.assembly.native_window != nullptr)
        hadal_window_unref(lake_impl_v(hadal_window, swapchain->header.assembly.native_window));

    for (s32 i = 0; swapchain->acquire_semaphores.da.size; i++) {
        moon_binary_semaphore sem = { .impl = swapchain->acquire_semaphores.v[i] };
        if (sem.impl != nullptr) moon_binary_semaphore_unref(sem);
    }
    for (s32 i = 0; swapchain->present_semaphores.da.size; i++) {
        moon_binary_semaphore sem = { .impl = swapchain->present_semaphores.v[i] };
        if (sem.impl != nullptr) moon_binary_semaphore_unref(sem);
    }
    if (swapchain->gpu_timeline != nullptr)
        moon_timeline_semaphore_unref(lake_impl_v(moon_timeline_semaphore, swapchain->gpu_timeline));

    if (swapchain->acquire_semaphores.da.v != nullptr)
        __lake_free(swapchain->acquire_semaphores.da.v);
    if (swapchain->present_semaphores.da.v != nullptr)
        __lake_free(swapchain->present_semaphores.da.v);
    if (swapchain->supported_present_modes.da.v != nullptr)
        __lake_free(swapchain->supported_present_modes.da.v);
    if (swapchain->images.da.v != nullptr)
        __lake_free(swapchain->images.da.v);
    *swapchain = (struct moon_swapchain_impl){0};

    if (device != nullptr) 
        lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
}

static lake_result create_or_recreate_surface(struct moon_swapchain_impl *swapchain)
{
    struct moon_impl *moon = swapchain->header.device.header->moon.impl;
    hadal_window window = { .impl = swapchain->header.assembly.native_window };

    if (swapchain->vk_surface != nullptr)
        moon->vkDestroySurfaceKHR(moon->vk_instance, swapchain->vk_surface, &moon->vk_allocator);
    return window.hadal.interface->vulkan_create_surface(window.impl, &moon->vk_allocator, &swapchain->vk_surface);
}

#define MAX_SWAPCHAIN_IMAGES 8
static lake_result recreate_swapchain(struct moon_swapchain_impl *swapchain)
{
    struct moon_device_impl *device = swapchain->header.device.impl;
    hadal_window window = lake_impl_v(hadal_window, swapchain->header.assembly.native_window);
    VkSwapchainKHR old_swapchain = swapchain->vk_swapchain;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    bool use_vsync = swapchain->header.assembly.present_mode == moon_present_mode_fifo;

    /* TODO delete later !! */
    (void)create_or_recreate_surface(swapchain);
    (void)default_format_selector(0);

    u32 window_flags = lake_atomic_read(&window.header->flags);
    if (window_flags & hadal_window_flag_should_close)
        return LAKE_ERROR_SURFACE_LOST;

    VkResult vk_result = device->header.moon.impl->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            device->physical_device->vk_physical_device, swapchain->vk_surface, &surface_capabilities);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    swapchain->vk_surface_extent.width = surface_capabilities.currentExtent.width;
    swapchain->vk_surface_extent.height = surface_capabilities.currentExtent.height;

    lake_result result = _moon_vulkan_device_wait_idle(device);
    if (result != LAKE_SUCCESS)
        return result;

    partial_swapchain_cleanup(swapchain);
    moon_texture_usage const usage = swapchain->header.assembly.image_usage | moon_texture_usage_color_attachment;

    u32 min_image_count = lake_max(3, surface_capabilities.minImageCount);
    VkSwapchainCreateInfoKHR const vk_swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = swapchain->vk_surface,
        .minImageCount = min_image_count,
        .imageFormat = swapchain->vk_surface_format.format,
        .imageColorSpace = swapchain->vk_surface_format.colorSpace,
        .imageExtent = swapchain->vk_surface_extent,
        .imageArrayLayers = 1,
        .imageUsage = usage,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &get_device_queue_impl(device, MOON_QUEUE_MAIN)->vk_queue_family_idx,
        .preTransform = (VkSurfaceTransformFlagsKHR)swapchain->header.assembly.present_transform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = use_vsync ? VK_PRESENT_MODE_FIFO_KHR : swapchain->no_vsync_present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = old_swapchain,
    };
    vk_result = device->vkCreateSwapchainKHR(device->vk_device, &vk_swapchain_create_info, device->vk_allocator, &swapchain->vk_swapchain);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);
u32 image_count = 0;
    vk_result = device->vkGetSwapchainImagesKHR(
            device->vk_device,
            swapchain->vk_swapchain,
            &image_count, 
            nullptr);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    VkImage images[MAX_SWAPCHAIN_IMAGES];
    vk_result = device->vkGetSwapchainImagesKHR(
            device->vk_device,
            swapchain->vk_swapchain,
            &image_count, 
            images);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    /* TODO darray procedures */
    swapchain->images.da.alloc = swapchain->images.da.size = image_count;
    swapchain->images.da.v = __lake_malloc_n(moon_texture_id, image_count);

    while (swapchain->images.da.size < swapchain->images.da.alloc) {
        s32 idx = swapchain->images.da.size;
        moon_texture_assembly const texture_assembly = {
            .format = (moon_format)swapchain->vk_surface_format.format,
            .extent = {
                .width = swapchain->vk_surface_extent.width,
                .height = swapchain->vk_surface_extent.height,
                .depth = 0,
            },
            .usage = usage,
        };
        result = create_texture_from_swapchain_image(device, 
                images[idx], swapchain->vk_surface_format.format, 
                idx, usage, &texture_assembly, &swapchain->images.v[idx]);
        if (result != LAKE_SUCCESS)
            return result;
        swapchain->images.da.size++;
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_SWAPCHAIN_KHR,
            .objectHandle = (u64)(uptr)swapchain->vk_swapchain,
            .pObjectName = swapchain->header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    if (old_swapchain != VK_NULL_HANDLE)
        device->vkDestroySwapchainKHR(device->vk_device, old_swapchain, device->vk_allocator);
    return LAKE_SUCCESS;
}

FN_MOON_SWAPCHAIN_ASSEMBLY(vulkan)
{
    lake_result result = LAKE_SUCCESS;
    if ((device->header.details->implicit_features & moon_implicit_feature_swapchain) == moon_implicit_feature_none) {
        lake_error("Device `%s: %s` does not support the swapchain.",
            device->header.assembly.name.str, device->header.details->device_name);
        return LAKE_ERROR_FEATURE_NOT_PRESENT;
    }
    struct moon_swapchain_impl swapchain = {
        .header = {
            .device.impl = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_swapchain_zero_refcnt,
        },
    };

    /* TODO */

    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&swapchain.header.refcnt);
    *out_swapchain = __lake_malloc_t(struct moon_swapchain_impl);
    lake_memcpy(*out_swapchain, &swapchain, sizeof(struct moon_swapchain_impl));
    return result;
}

FN_MOON_SWAPCHAIN_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(swapchain != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&swapchain->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Swapchain `%s` reference count is %d.", swapchain->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    if (swapchain->header.device.impl != nullptr)
        full_swapchain_cleanup(swapchain);
    __lake_free(swapchain);
}

FN_MOON_SWAPCHAIN_WAIT_FOR_NEXT_FRAME(vulkan)
{
    u64 timeline_value = (u64)lake_max(0, (s64)swapchain->cpu_timeline - (s64)swapchain->header.assembly.max_allowed_frames_in_flight);
    return _moon_vulkan_timeline_semaphore_wait_for_value(swapchain->gpu_timeline, timeline_value, 0);
}

FN_MOON_SWAPCHAIN_ACQUIRE_NEXT_IMAGE(vulkan)
{
    struct moon_device_impl *device = swapchain->header.device.impl;
    lake_result result = _moon_vulkan_swapchain_wait_for_next_frame(swapchain);
    if (result != LAKE_SUCCESS)
        return result;

    swapchain->acquire_semaphore_idx = (swapchain->cpu_timeline + 1) % swapchain->header.assembly.max_allowed_frames_in_flight;
    struct moon_binary_semaphore_impl *acquire_sem = swapchain->acquire_semaphores.v[swapchain->acquire_semaphore_idx];
    result = vk_result_translate(
        device->vkAcquireNextImageKHR(
            device->vk_device,
            swapchain->vk_swapchain,
            UINT64_MAX,
            acquire_sem->vk_semaphore,
            nullptr, &swapchain->current_image_idx));

    /* only bump the cpu timeline, when the acquire succeeds */
    if (result == LAKE_SUCCESS) {
        swapchain->cpu_timeline++;
        *out_texture = swapchain->images.v[swapchain->current_image_idx];
    }
    return result;
}

FN_MOON_SWAPCHAIN_CURRENT_ACQUIRE_SEMAPHORE(vulkan)
{
    return swapchain->acquire_semaphores.v[swapchain->acquire_semaphore_idx];
}

FN_MOON_SWAPCHAIN_CURRENT_PRESENT_SEMAPHORE(vulkan)
{
    return swapchain->present_semaphores.v[swapchain->current_image_idx];
}

FN_MOON_SWAPCHAIN_CURRENT_CPU_TIMELINE_VALUE(vulkan)
{
    return swapchain->cpu_timeline;
}

FN_MOON_SWAPCHAIN_GPU_TIMELINE_SEMAPHORE(vulkan)
{
    return swapchain->gpu_timeline;
}

FN_MOON_SWAPCHAIN_CURRENT_TIMELINE_PAIR(vulkan)
{
    *out_timeline = (moon_timeline_pair){ 
        .timeline_semaphore = lake_impl_v(moon_timeline_semaphore, swapchain->gpu_timeline), 
        .value = swapchain->cpu_timeline 
    };
    return LAKE_SUCCESS;
}

FN_MOON_SWAPCHAIN_SET_PRESENT_MODE(vulkan)
{
    struct moon_device_impl *device = swapchain->header.device.impl;
    swapchain->header.assembly.present_mode = present_mode;

    lake_result result = recreate_swapchain(swapchain);
    if (result != LAKE_SUCCESS) {
        VERIFY_VK_ERROR(device->vkDeviceWaitIdle(device->vk_device));
        full_swapchain_cleanup(swapchain);
    }
    return result;
}

FN_MOON_SWAPCHAIN_RESIZE(vulkan)
{
    struct moon_device_impl *device = swapchain->header.device.impl;

    lake_result result = recreate_swapchain(swapchain);
    if (result != LAKE_SUCCESS) {
        VERIFY_VK_ERROR(device->vkDeviceWaitIdle(device->vk_device));
        full_swapchain_cleanup(swapchain);
    }
    return result;
}
#endif /* MOON_VULKAN */
