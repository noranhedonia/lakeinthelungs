#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_SWAPCHAIN_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_swapchain;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_SWAPCHAIN_DESTRUCTOR(vulkan)
{
    (void)swapchain;
}

FN_MOON_SWAPCHAIN_WAIT_FOR_NEXT_FRAME(vulkan)
{
    (void)swapchain;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_SWAPCHAIN_ACQUIRE_NEXT_IMAGE(vulkan)
{
    (void)swapchain;
    (void)out_texture;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_SWAPCHAIN_CURRENT_ACQUIRE_SEMAPHORE(vulkan)
{
    (void)swapchain;
    return nullptr;
}

FN_MOON_SWAPCHAIN_CURRENT_PRESENT_SEMAPHORE(vulkan)
{
    (void)swapchain;
    return nullptr;
}

FN_MOON_SWAPCHAIN_CURRENT_CPU_TIMELINE_VALUE(vulkan)
{
    (void)swapchain;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_SWAPCHAIN_CURRENT_TIMELINE_PAIR(vulkan)
{
    (void)swapchain;
    (void)out_timeline;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_SWAPCHAIN_GPU_TIMELINE_SEMAPHORE(vulkan)
{
    (void)swapchain;
    return nullptr;
}

FN_MOON_SWAPCHAIN_SET_PRESENT_MODE(vulkan)
{
    (void)swapchain;
    (void)present_mode;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_SWAPCHAIN_RESIZE(vulkan)
{
    (void)swapchain;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}
#endif /* MOON_VULKAN */
