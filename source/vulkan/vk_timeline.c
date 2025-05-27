#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_timeline_query_pool;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_TIMELINE_QUERY_POOL_DESTRUCTOR(vulkan)
{
    (void)timeline_query_pool;
}

FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_timeline_semaphore;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_TIMELINE_SEMAPHORE_DESTRUCTOR(vulkan)
{
    (void)timeline_semaphore;
}

FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_binary_semaphore;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_BINARY_SEMAPHORE_DESTRUCTOR(vulkan)
{
    (void)binary_semaphore;
}
#endif /* MOON_VULKAN */
