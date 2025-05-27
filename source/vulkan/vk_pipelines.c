#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_COMPUTE_PIPELINES_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_COMPUTE_PIPELINE_DESTRUCTOR(vulkan)
{
    (void)compute_pipeline;
}

FN_MOON_WORK_GRAPH_PIPELINES_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_WORK_GRAPH_PIPELINE_DESTRUCTOR(vulkan)
{
    (void)work_graph_pipeline;
}

FN_MOON_RASTER_PIPELINES_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_RASTER_PIPELINE_DESTRUCTOR(vulkan)
{
    (void)raster_pipeline;
}

FN_MOON_RAY_TRACING_PIPELINES_ASSEMBLY(vulkan)
{
    (void)device;
    (void)count;
    (void)assembly_array;
    (void)out_pipelines;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_RAY_TRACING_PIPELINE_DESTRUCTOR(vulkan)
{
    (void)ray_tracing_pipeline;
}
#endif /* MOON_VULKAN */
