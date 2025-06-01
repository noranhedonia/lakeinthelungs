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
    (void)pipeline;
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
    (void)pipeline;
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
    (void)pipeline;
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
    (void)pipeline;
    (void)out_blob;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
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
    (void)pipeline;
}
#endif /* MOON_VULKAN */
