#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_COMPUTE_PIPELINE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_COMPUTE_PIPELINE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(pipeline != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&pipeline->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Compute pipeline `%s` reference count is %d.", pipeline->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = pipeline->header.device;

    /* TODO create a pipeline zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroyPipeline(device->vk_device, pipeline->vk_pipeline, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(pipeline);
}

FN_MOON_WORK_GRAPH_PIPELINE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_WORK_GRAPH_PIPELINE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(pipeline != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&pipeline->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Work graph pipeline `%s` reference count is %d.", pipeline->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = pipeline->header.device;

    /* TODO create a pipeline zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroyPipeline(device->vk_device, pipeline->vk_pipeline, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(pipeline);
}

FN_MOON_RAY_TRACING_PIPELINE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_RAY_TRACING_PIPELINE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(pipeline != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&pipeline->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Ray tracing pipeline `%s` reference count is %d.", pipeline->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = pipeline->header.device;

    /* TODO create a pipeline zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroyPipeline(device->vk_device, pipeline->vk_pipeline, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(pipeline);
}

FN_MOON_RAY_TRACING_PIPELINE_CREATE_DEFAULT_SBT(vulkan)
{
    (void)pipeline;
    (void)out_sbt;
    (void)out_buffer;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_RAY_TRACING_PIPELINE_SHADER_GROUP_HANDLES(vulkan)
{
    moon_device device = pipeline->header.device;

    u32 const handle_count = pipeline->shader_group_count;
    u32 const handle_size = device->header.details->ray_tracing_pipeline_details.shader_group_handle_size;
    u32 const data_size = handle_count * handle_size;

    return vk_result_translate(
        device->vkGetRayTracingShaderGroupHandlesKHR(
            device->vk_device,
            pipeline->vk_pipeline,
            0,
            handle_count,
            data_size,
            out_blob));
}

FN_MOON_RASTER_PIPELINE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_RASTER_PIPELINE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(pipeline != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&pipeline->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Raster pipeline `%s` reference count is %d.", pipeline->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    moon_device device = pipeline->header.device;

    /* TODO create a pipeline zombie instead */
    // u64 const main_queue_cpu_timeline = lake_atomic_read(&device->submit_timeline);
    // ...
    device->vkDestroyPipeline(device->vk_device, pipeline->vk_pipeline, device->vk_allocator);

    lake_dec_refcnt(&device->header.refcnt, device, (PFN_lake_work)_moon_vulkan_device_zero_refcnt);
    __lake_free(pipeline);
}
#endif /* MOON_VULKAN */
