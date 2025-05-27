#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_DEVICE_ASSEMBLY(vulkan)
{
    (void)moon;
    (void)assembly;
    (void)out_device;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_DESTRUCTOR(vulkan)
{
    (void)device;
}

FN_MOON_DEVICE_QUEUE_COUNT(vulkan)
{
    (void)device;
    (void)queue_type;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_QUEUE_WAIT_IDLE(vulkan)
{
    (void)device;
    (void)queue;
}

FN_MOON_DEVICE_WAIT_IDLE(vulkan)
{
    (void)device;
}

FN_MOON_DEVICE_SUBMIT_COMMANDS(vulkan)
{
    (void)device;
    (void)submit;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_PRESENT_FRAMES(vulkan)
{
    (void)device;
    (void)present;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_DESTROY_DEFERRED(vulkan)
{
    (void)device;
}

FN_MOON_DEVICE_HEAP_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_device_heap;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_HEAP_DESTRUCTOR(vulkan)
{
    (void)device_heap;
}

FN_MOON_DEVICE_BUFFER_MEMORY_REQUIREMENTS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_requirements;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_TEXTURE_MEMORY_REQUIREMENTS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_requirements;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_MEMORY_REPORT(vulkan)
{
    (void)device;
    (void)out_report;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}
#endif /* MOON_VULKAN */
