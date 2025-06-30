#include "vk_moon.h"
#include <lake/modules/hadal.h>
#ifdef MOON_VULKAN

#include <stdio.h> /* snprintf */

static void partial_swapchain_cleanup(struct moon_swapchain_impl *swapchain)
{
    LAKE_UNUSED lake_result __ignore;
    lake_darray_foreach_v(swapchain->images, moon_texture_id, id)
        __ignore = _moon_vulkan_destroy_texture(swapchain->header.device.impl, *id);
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

    lake_darray_foreach_v(swapchain->acquire_semaphores, struct moon_binary_semaphore_impl *, impl) 
        if (impl != nullptr) moon_binary_semaphore_unref_v(*impl);

    lake_darray_foreach_v(swapchain->present_semaphores, struct moon_binary_semaphore_impl *, impl) 
        if (impl != nullptr) moon_binary_semaphore_unref_v(*impl);

    if (swapchain->gpu_timeline != nullptr)
        moon_timeline_semaphore_unref(lake_impl_v(moon_timeline_semaphore, swapchain->gpu_timeline));

    lake_darray_fini(&swapchain->acquire_semaphores.da);
    lake_darray_fini(&swapchain->present_semaphores.da);
    lake_darray_fini(&swapchain->supported_present_modes.da);
    lake_darray_fini(&swapchain->images.da);
    *swapchain = (struct moon_swapchain_impl){0};

    if (device != nullptr) moon_device_unref(lake_impl_v(moon_device, device));
}

static lake_result create_or_recreate_surface(struct moon_swapchain_impl *swapchain)
{
    struct moon_impl *moon = swapchain->header.device.header->moon.impl;
    hadal_window window = { .impl = swapchain->header.assembly.native_window };

    if (swapchain->vk_surface != nullptr)
        moon->vkDestroySurfaceKHR(moon->vk_instance, swapchain->vk_surface, &moon->vk_allocator);
    return window.hadal->interface->vulkan_create_surface(window.impl, &moon->vk_allocator, &swapchain->vk_surface);
}

static lake_result recreate_swapchain(struct moon_swapchain_impl *swapchain)
{
    struct moon_device_impl *device = swapchain->header.device.impl;
    hadal_window window = lake_impl_v(hadal_window, swapchain->header.assembly.native_window);
    VkSwapchainKHR old_swapchain = swapchain->vk_swapchain;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    u32 image_count = 0;
    bool use_vsync = swapchain->header.assembly.present_mode == moon_present_mode_fifo;


    u32 window_flags = lake_atomic_and_explicit(&window.header->flags, ~(hadal_window_flag_swapchain_out_of_date), lake_memory_model_release);
    hadal_window_ref(window);

    if (window_flags & hadal_window_flag_should_close)
        return LAKE_ERROR_SURFACE_LOST;

    VkResult vk_result = device->header.moon.impl->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            device->physical_device->vk_physical_device, swapchain->vk_surface, &surface_capabilities);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    swapchain->vk_surface_extent.width = surface_capabilities.currentExtent.width != UINT32_MAX
        ? surface_capabilities.currentExtent.width : (u32)window.header->fb_width;
    swapchain->vk_surface_extent.height = surface_capabilities.currentExtent.height != UINT32_MAX
        ? surface_capabilities.currentExtent.height : (u32)window.header->fb_height;

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

    vk_result = device->vkGetSwapchainImagesKHR(
            device->vk_device,
            swapchain->vk_swapchain,
            &image_count, 
            nullptr);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    VkImage *images = lake_drift_n(VkImage, image_count);
    vk_result = device->vkGetSwapchainImagesKHR(
            device->vk_device,
            swapchain->vk_swapchain,
            &image_count, 
            images);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    lake_darray_resize_t(&swapchain->images.da, moon_texture_id, image_count);
    for (u32 i = 0; i < image_count; i++) {
        moon_texture_assembly texture_assembly = {
            .format = (moon_format)swapchain->vk_surface_format.format,
            .extent = {
                .width = swapchain->vk_surface_extent.width,
                .height = swapchain->vk_surface_extent.height,
                .depth = 0,
            },
            .usage = usage,
        };
        texture_assembly.name.len = snprintf(texture_assembly.name.str + texture_assembly.name.len, 
            LAKE_SMALL_STRING_CAPACITY - texture_assembly.name.len, "%s sc img %u", swapchain->header.assembly.name.str, i);
        moon_texture_id id;
        result = create_texture_from_swapchain_image(device, 
                images[i], swapchain->vk_surface_format.format, 
                i, usage, &texture_assembly, &id);
        if (result != LAKE_SUCCESS)
            return result;
        lake_darray_append_t(&swapchain->images.da, moon_texture_id, &id);
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
    VkResult vk_result = VK_SUCCESS;
    struct moon_impl *moon = device->header.moon.impl;
    VkPhysicalDevice vk_physical_device = device->physical_device->vk_physical_device;

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
    lake_defer_begin;
    lake_defer({ if (result != LAKE_SUCCESS) full_swapchain_cleanup(&swapchain); })

    /* create the surface */
    result = create_or_recreate_surface(&swapchain);
    if (result != LAKE_SUCCESS) 
        { lake_defer_return result; }

    /* save supported present modes */
    u32 present_mode_count = 0;
    vk_result = moon->vkGetPhysicalDeviceSurfacePresentModesKHR(
            vk_physical_device, 
            swapchain.vk_surface, 
            &present_mode_count, 
            nullptr);
    if (vk_result != VK_SUCCESS) 
        { lake_defer_return vk_result_translate(vk_result); }

    lake_darray_resize_t(&swapchain.supported_present_modes.da, VkPresentModeKHR, present_mode_count);
    vk_result = moon->vkGetPhysicalDeviceSurfacePresentModesKHR(
            vk_physical_device, 
            swapchain.vk_surface, 
            &present_mode_count, 
            lake_darray_first_v(swapchain.supported_present_modes));
    if (vk_result != VK_SUCCESS) 
        { lake_defer_return vk_result_translate(vk_result); }

    /* select format */
    u32 format_count = 0;
    vk_result = moon->vkGetPhysicalDeviceSurfaceFormatsKHR(
            vk_physical_device,
            swapchain.vk_surface,
            &format_count,
            nullptr);
    if (vk_result != VK_SUCCESS) 
        { lake_defer_return vk_result_translate(vk_result); }
    else if (format_count == 0)
        { lake_defer_return LAKE_ERROR_FORMAT_NOT_SUPPORTED; }

    lake_darray_t(VkSurfaceFormatKHR) surface_formats;
    lake_darray_init_t(&surface_formats.da, VkSurfaceFormatKHR, format_count);
    vk_result = moon->vkGetPhysicalDeviceSurfaceFormatsKHR(
            vk_physical_device,
            swapchain.vk_surface,
            &format_count,
            lake_darray_first_v(surface_formats));
    if (vk_result != VK_SUCCESS) 
        { lake_defer_return vk_result_translate(vk_result); }

    if (swapchain.header.assembly.surface_format_selector == nullptr)
        swapchain.header.assembly.surface_format_selector = moon_default_surface_format_selector;
    s32 best_format = swapchain.header.assembly.surface_format_selector(format_count, (moon_format const *)surface_formats.v);
    if (best_format < 0) { lake_defer_return LAKE_ERROR_FORMAT_NOT_SUPPORTED; }

    swapchain.vk_surface_format = *lake_darray_elem_v(surface_formats, lake_min(lake_darray_size(&surface_formats.da), best_format));

    /* most of the swapchain creation work comes from here :D */
    result = recreate_swapchain(&swapchain);
    if (result != LAKE_SUCCESS)
        { lake_defer_return result; }

    /* an acquire semaphore for each frame in flight */
    s32 const frames_in_flight = moon->interface.header.bedrock->hints.frames_in_flight;
    for (s32 i = 0; i < frames_in_flight + 1; i++) {
        struct moon_binary_semaphore_impl *sem;
        moon_binary_semaphore_assembly const sem_assembly = {0};
        result = _moon_vulkan_binary_semaphore_assembly(device, &sem_assembly, &sem);
        if (result != LAKE_SUCCESS)
            { lake_defer_return result; }
        lake_darray_append_t(&swapchain.acquire_semaphores.da, struct moon_binary_semaphore_impl *, &sem);
    }

    /* a present semaphore for each swapchain image */
    for (s32 i = 0; i < lake_darray_size(&swapchain.images.da); i++) {
        struct moon_binary_semaphore_impl *sem;
        moon_binary_semaphore_assembly const sem_assembly = {0};
        result = _moon_vulkan_binary_semaphore_assembly(device, &sem_assembly, &sem);
        if (result != LAKE_SUCCESS)
            { lake_defer_return result; }
        lake_darray_append_t(&swapchain.present_semaphores.da, struct moon_binary_semaphore_impl *, &sem);
    }

    moon_timeline_semaphore_assembly tsem_assembly = {
        .initial_value = 0,
        .name = assembly->name,
    };
    tsem_assembly.name.str[tsem_assembly.name.len++] = ' ';
    tsem_assembly.name.str[tsem_assembly.name.len++] = 't';
    tsem_assembly.name.str[tsem_assembly.name.len++] = 's';
    result = _moon_vulkan_timeline_semaphore_assembly(device, &tsem_assembly, &swapchain.gpu_timeline);
    if (result != LAKE_SUCCESS)
        { lake_defer_return result; }

    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&swapchain.header.refcnt);
    *out_swapchain = __lake_malloc_t(struct moon_swapchain_impl);
    lake_memcpy(*out_swapchain, &swapchain, sizeof(struct moon_swapchain_impl));
    lake_defer_return result;
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
