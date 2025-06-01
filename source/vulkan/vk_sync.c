#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(vulkan)
{
    struct moon_timeline_semaphore_impl sem = {
        .header = {
            .device = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_timeline_semaphore_zero_refcnt,
        },
    };
    VkSemaphoreTypeCreateInfo vk_sem_timeline_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = sem.header.assembly.initial_value,
    };
    VkSemaphoreCreateInfo const vk_sem_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &vk_sem_timeline_info,
        .flags = 0,
    };
    VkResult vk_result = device->vkCreateSemaphore(device->vk_device, &vk_sem_create_info, device->vk_allocator, &sem.vk_semaphore);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

#ifndef LAKE_NDEBUG
    if (sem.header.assembly.name.len && (device->header.moon.header->framework->hints.use_debug_instruments)) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_SEMAPHORE,
            .objectHandle = (u64)(uptr)sem.vk_semaphore,
            .pObjectName = sem.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&sem.header.refcnt);
    *out_timeline_semaphore = __lake_malloc_t(struct moon_timeline_semaphore_impl);
    **out_timeline_semaphore = sem;
    return LAKE_SUCCESS;
}

FN_MOON_TIMELINE_SEMAPHORE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(timeline_semaphore != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&timeline_semaphore->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Timeline semaphore `%s` reference count is %d.", timeline_semaphore->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = timeline_semaphore->header.device;

    /* TODO create a semaphore zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroySemaphore(device->vk_device, timeline_semaphore->vk_semaphore, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(timeline_semaphore);
}

FN_MOON_TIMELINE_SEMAPHORE_READ_VALUE(vulkan)
{
    moon_device device = timeline_semaphore->header.device;
    return vk_result_translate(
        device->vkGetSemaphoreCounterValue(
            device->vk_device, 
            timeline_semaphore->vk_semaphore, 
            out_value));
}

FN_MOON_TIMELINE_SEMAPHORE_WRITE_VALUE(vulkan)
{
    moon_device device = timeline_semaphore->header.device;
    VkSemaphoreSignalInfo const vk_sem_signal_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO,
        .pNext = nullptr,
        .semaphore = timeline_semaphore->vk_semaphore,
        .value = value,
    };
    return vk_result_translate(device->vkSignalSemaphore(device->vk_device, &vk_sem_signal_info));
}

FN_MOON_TIMELINE_SEMAPHORE_WAIT_FOR_VALUE(vulkan)
{
    moon_device device = timeline_semaphore->header.device;
    VkSemaphoreWaitInfo const vk_sem_wait_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext = nullptr,
        .flags = 0,
        .semaphoreCount = 1,
        .pSemaphores = &timeline_semaphore->vk_semaphore,
        .pValues = &value,
    };
    return vk_result_translate(device->vkWaitSemaphores(device->vk_device, &vk_sem_wait_info, timeout));
}

FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(vulkan)
{
    struct moon_binary_semaphore_impl sem = {
        .header = {
            .device = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_binary_semaphore_zero_refcnt,
        },
    };
    VkSemaphoreCreateInfo const vk_sem_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    VkResult vk_result = device->vkCreateSemaphore(device->vk_device, &vk_sem_create_info, device->vk_allocator, &sem.vk_semaphore);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

#ifndef LAKE_NDEBUG
    if (sem.header.assembly.name.len && (device->header.moon.header->framework->hints.use_debug_instruments)) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_SEMAPHORE,
            .objectHandle = (u64)(uptr)sem.vk_semaphore,
            .pObjectName = sem.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&sem.header.refcnt);
    *out_binary_semaphore = __lake_malloc_t(struct moon_binary_semaphore_impl);
    **out_binary_semaphore = sem;
    return LAKE_SUCCESS;
}

FN_MOON_BINARY_SEMAPHORE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(binary_semaphore != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&binary_semaphore->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Binary semaphore `%s` reference count is %d.", binary_semaphore->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = binary_semaphore->header.device;

    /* TODO create a semaphore zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroySemaphore(device->vk_device, binary_semaphore->vk_semaphore, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(binary_semaphore);
}

FN_MOON_EVENT_ASSEMBLY(vulkan)
{
    struct moon_event_impl event = {
        .header = {
            .device = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_event_zero_refcnt,
        },
    };
    VkEventCreateInfo const vk_event_create_info = {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_EVENT_CREATE_DEVICE_ONLY_BIT,
    };
    VkResult vk_result = device->vkCreateEvent(device->vk_device, &vk_event_create_info, device->vk_allocator, &event.vk_event);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

#ifndef LAKE_NDEBUG
    if (event.header.assembly.name.len && (device->header.moon.header->framework->hints.use_debug_instruments)) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_EVENT,
            .objectHandle = (u64)(uptr)event.vk_event,
            .pObjectName = event.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&event.header.refcnt);
    *out_event = __lake_malloc_t(struct moon_event_impl);
    **out_event = event;
    return LAKE_SUCCESS;
}

FN_MOON_EVENT_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(event != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&event->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Event `%s` reference count is %d.", event->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = event->header.device;

    /* TODO create an event zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroyEvent(device->vk_device, event->vk_event, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(event);
}
#endif /* MOON_VULKAN */
