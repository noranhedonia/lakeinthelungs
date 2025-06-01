#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_COMMAND_RECORDER_ASSEMBLY(vulkan) 
{
    (void)device;
    (void)assembly;
    (void)out_cmd;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_COMMAND_RECORDER_ZERO_REFCNT(vulkan)
{
    (void)cmd;
}

FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(vulkan)
{
    (void)cmd;
    (void)assembly;
    (void)out_cmd_list;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_STAGED_COMMAND_LIST_ZERO_REFCNT(vulkan)
{
    (void)cmd_list;
}

FN_MOON_CMD_COPY_BUFFER(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_COPY_TEXTURE(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_BLIT_TEXTURE(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_RESOLVE_TEXTURE(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_CLEAR_BUFFER(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_CLEAR_TEXTURE(vulkan) 
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DESTROY_BUFFER_DEFERRED(vulkan)
{
    (void)cmd;
    (void)buffer;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DESTROY_TEXTURE_DEFERRED(vulkan) 
{
    (void)cmd;
    (void)texture;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(vulkan)
{
    (void)cmd;
    (void)texture_view;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DESTROY_SAMPLER_DEFERRED(vulkan)
{
    (void)cmd;
    (void)sampler;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_PUSH_CONSTANTS(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_SET_COMPUTE_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_SET_WORK_GRAPH_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_SET_RAY_TRACING_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_SET_RASTER_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_SET_VIEWPORT(vulkan)
{
    (void)cmd;
    (void)viewport;
}

FN_MOON_CMD_SET_SCISSOR(vulkan)
{
    (void)cmd;
    (void)scissor;
}

FN_MOON_CMD_SET_DEPTH_BIAS(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_SET_INDEX_BUFFER(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(vulkan)
{
    (void)cmd;
    (void)sample_count;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_BEGIN_RENDERPASS(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_END_RENDERPASS(vulkan)
{
    (void)cmd;
}

FN_MOON_CMD_WRITE_TIMESTAMPS(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_RESOLVE_TIMESTAMPS(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_SIGNAL_EVENT(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_WAIT_ON_EVENTS(vulkan)
{
    (void)cmd;
    (void)work_count;
    (void)work;
}

FN_MOON_CMD_RESET_EVENT(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_BEGIN_LABEL(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_END_LABEL(vulkan)
{
    (void)cmd;
}

FN_MOON_CMD_DISPATCH(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DISPATCH_INDIRECT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DISPATCH_GRAPH(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_TRACE_RAYS(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_TRACE_RAYS_INDIRECT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DRAW(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_DRAW_INDEXED(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_DRAW_INDIRECT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DRAW_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DRAW_MESH_TASKS(vulkan)
{
    (void)cmd;
    (void)work;
}

FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}
#endif /* MOON_VULKAN */
