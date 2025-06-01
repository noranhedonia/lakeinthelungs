#include "vk_moon.h"
#include <lake/platform/hadal.h>
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

static void partial_swapchain_cleanup(moon_swapchain swapchain)
{
    (void)swapchain;
    /* TODO destroy images, clear the images darray */
}

static void full_swapchain_cleanup(moon_swapchain swapchain)
{
    moon_device device = swapchain->header.device;
    moon_adapter moon = device->header.moon.adapter;

    partial_swapchain_cleanup(swapchain);
    if (swapchain->vk_swapchain != VK_NULL_HANDLE) {
        device->vkDeviceWaitIdle(device->vk_device);
        device->vkDestroySwapchainKHR(device->vk_device, swapchain->vk_swapchain, device->vk_allocator);
    }
    if (swapchain->vk_surface != VK_NULL_HANDLE)
        moon->vkDestroySurfaceKHR(moon->vk_instance, swapchain->vk_surface, &moon->vk_allocator);
    if (swapchain->header.device != nullptr)
        lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    if (swapchain->header.assembly.native_window != nullptr)
        hadal_window_v_dec_refcnt((hadal_window_v){ .impl = swapchain->header.assembly.native_window });

    s32 const sem_count = swapchain->acquire_semaphores.da.size + swapchain->present_semaphores.da.size + 1;

    lake_work_details *work = __lake_malloc_n(lake_work_details, sem_count);
    s32 o = 0;
    for (s32 i = 0; swapchain->acquire_semaphores.da.size; i++) {
        moon_binary_semaphore_v sem = { .impl = swapchain->acquire_semaphores.v[i] };
        if (sem.impl != nullptr) 
            moon_binary_semaphore_v_dec_refcnt_deferred(sem, &o, work);
    }
    for (s32 i = 0; swapchain->present_semaphores.da.size; i++) {
        moon_binary_semaphore_v sem = { .impl = swapchain->present_semaphores.v[i] };
        if (sem.impl != nullptr) 
            moon_binary_semaphore_v_dec_refcnt_deferred(sem, &o, work);
    }
    moon_timeline_semaphore_v_dec_refcnt_deferred((moon_timeline_semaphore_v){ .impl = swapchain->gpu_timeline }, &o, work);

    if (o > 0) lake_submit_work_and_yield(o, work);
    /* TODO free darrays memory */

    *swapchain = (struct moon_swapchain_impl){0};
}

static lake_result create_surface(moon_swapchain swapchain)
{
    moon_adapter moon = swapchain->header.device->header.moon.adapter;
    hadal_window_v window = { .impl = swapchain->header.assembly.native_window };

    return window.hadal->interface->vulkan_create_surface(window.impl, &moon->vk_allocator, &swapchain->vk_surface);
}

static lake_result recreate_surface(moon_swapchain swapchain)
{
    moon_adapter moon = swapchain->header.device->header.moon.adapter;

    if (swapchain->vk_surface != nullptr)
        moon->vkDestroySurfaceKHR(moon->vk_instance, swapchain->vk_surface, &moon->vk_allocator);
    return create_surface(swapchain); 
}

static lake_result recreate_swapchain(moon_swapchain swapchain)
{
    /* TODO */
}

FN_MOON_SWAPCHAIN_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_swapchain;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_SWAPCHAIN_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(swapchain != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&swapchain->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Swapchain `%s` reference count is %d.", swapchain->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    if (swapchain->header.device != nullptr)
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
    moon_device device = swapchain->header.device;
    lake_result result = _moon_vulkan_swapchain_wait_for_next_frame(swapchain);
    if (result != LAKE_SUCCESS)
        return result;

    swapchain->acquire_semaphore_idx = (swapchain->cpu_timeline + 1) % swapchain->header.assembly.max_allowed_frames_in_flight;
    moon_binary_semaphore acquire_sem = swapchain->acquire_semaphores.v[swapchain->acquire_semaphore_idx];
    result = vk_result_translate(
        device->vkAcquireNextImageKHR(
            device->vk_device,
            swapchain->vk_swapchain,
            UINT64_MAX,
            acquire_sem->vk_semaphore,
            nullptr, &swapchain->current_image_idx));

    /* only bump the cpu timeline, when the acquire succeeds */
    swapchain->cpu_timeline++;
    *out_texture = swapchain->images.v[swapchain->current_image_idx];
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
    *out_timeline = (moon_timeline_pair){ .timeline_semaphore = swapchain->gpu_timeline, swapchain->cpu_timeline };
    return LAKE_SUCCESS;
}

FN_MOON_SWAPCHAIN_SET_PRESENT_MODE(vulkan)
{
    swapchain->header.assembly.present_mode = present_mode;
    lake_result result = recreate_swapchain(swapchain);
    if (result != LAKE_SUCCESS) {
        VERIFY_VK_ERROR(swapchain->header.device->vkDeviceWaitIdle(swapchain->header.device->vk_device));
        full_swapchain_cleanup(swapchain);
    }
    return result;
}

FN_MOON_SWAPCHAIN_RESIZE(vulkan)
{
    lake_result result = recreate_swapchain(swapchain);
    if (result != LAKE_SUCCESS) {
        VERIFY_VK_ERROR(swapchain->header.device->vkDeviceWaitIdle(swapchain->header.device->vk_device));
        full_swapchain_cleanup(swapchain);
    }
    return result;
}
#endif /* MOON_VULKAN */
