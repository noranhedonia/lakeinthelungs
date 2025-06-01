#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_timeline_semaphore;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_TIMELINE_SEMAPHORE_ZERO_REFCNT(vulkan)
{
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
    (void)timeline_semaphore;
    (void)out_value;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_TIMELINE_SEMAPHORE_WRITE_VALUE(vulkan)
{
    (void)timeline_semaphore;
    (void)value;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_TIMELINE_SEMAPHORE_WAIT_FOR_VALUE(vulkan)
{
    (void)timeline_semaphore;
    (void)value;
    (void)timeout;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_binary_semaphore;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_BINARY_SEMAPHORE_ZERO_REFCNT(vulkan)
{
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
    (void)device;
    (void)assembly;
    (void)out_event;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_EVENT_ZERO_REFCNT(vulkan)
{
    (void)event;
}
#endif /* MOON_VULKAN */
