#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_timeline_query_pool;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_TIMELINE_QUERY_POOL_ZERO_REFCNT(vulkan)
{
    (void)timeline_query_pool;
}

FN_MOON_TIMELINE_QUERY_POOL_QUERY_RESULTS(vulkan)
{
    (void)timeline_query_pool;
    (void)start;
    (void)count;
    (void)out_results;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}
#endif /* MOON_VULKAN */
