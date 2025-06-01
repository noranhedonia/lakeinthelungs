#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(vulkan)
{
    struct moon_timeline_query_pool_impl timeline_query_pool = {
        .header = {
            .device = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_timeline_query_pool_zero_refcnt,
        },
    };
    return LAKE_ERROR_FEATURE_NOT_PRESENT;

    VkQueryPoolCreateInfo const vk_query_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queryType = VK_QUERY_TYPE_TIMESTAMP,
        .queryCount = timeline_query_pool.header.assembly.query_count,
        .pipelineStatistics = 0,
    };
    VkResult vk_result = device->vkCreateQueryPool(device->vk_device, &vk_query_pool_create_info, device->vk_allocator, &timeline_query_pool.vk_query_pool);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    device->vkResetQueryPool(device->vk_device, timeline_query_pool.vk_query_pool, 0, timeline_query_pool.header.assembly.query_count);
#ifndef LAKE_NDEBUG
    if (timeline_query_pool.header.assembly.name.len && (device->header.moon.header->framework->hints.use_debug_instruments)) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_QUERY_POOL,
            .objectHandle = (u64)(uptr)timeline_query_pool.vk_query_pool,
            .pObjectName = timeline_query_pool.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&timeline_query_pool.header.refcnt);
    *out_timeline_query_pool = __lake_malloc_t(struct moon_timeline_query_pool_impl);
    **out_timeline_query_pool = timeline_query_pool;
    return LAKE_SUCCESS;
}

FN_MOON_TIMELINE_QUERY_POOL_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(timeline_query_pool != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&timeline_query_pool->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Timeline query pool `%s` reference count is %d.", timeline_query_pool->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = timeline_query_pool->header.device;

    /* TODO create a timeline query pool zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroyQueryPool(device->vk_device, timeline_query_pool->vk_query_pool, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(timeline_query_pool);
}

FN_MOON_TIMELINE_QUERY_POOL_QUERY_RESULTS(vulkan)
{
    if (count == 0) {
        return LAKE_SUCCESS;
    } else if (!(start + count - 1 < timeline_query_pool->header.assembly.query_count)) {
        return LAKE_ERROR_OUT_OF_RANGE;
    }
    moon_device device = timeline_query_pool->header.device;

    return vk_result_translate(
        device->vkGetQueryPoolResults(
            device->vk_device, 
            timeline_query_pool->vk_query_pool, 
            start, 
            count, 
            count * 2ul * sizeof(u64), 
            out_results, 
            2ul * sizeof(u64),
            VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT));
}
#endif /* MOON_VULKAN */
