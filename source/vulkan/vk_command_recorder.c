#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_COMMAND_RECORDER_ASSEMBLY(vulkan) 
{
    (void)device;
    (void)assembly;
    (void)out_command_recorder;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_COMMAND_RECORDER_DESTRUCTOR(vulkan)
{
    (void)command_recorder;
}

FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(vulkan)
{
    (void)command_recorder;
    (void)assembly;
    (void)out_staged_command_list;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_STAGED_COMMAND_LIST_DESTRUCTOR(vulkan)
{
    (void)staged_command_list;
}

FN_MOON_CMD_COPY_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_COPY_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_BLIT_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_RESOLVE_TEXTURE(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_CLEAR_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_CLEAR_TEXTURE(vulkan) 
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DESTROY_BUFFER_DEFERRED(vulkan)
{
    (void)cmd;
    (void)buffer;
}

FN_MOON_CMD_DESTROY_TEXTURE_DEFERRED(vulkan) 
{
    (void)cmd;
    (void)texture;
}

FN_MOON_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(vulkan)
{
    (void)cmd;
    (void)texture_view;
}

FN_MOON_CMD_DESTROY_SAMPLER_DEFERRED(vulkan)
{
    (void)cmd;
    (void)sampler;
}

FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_ROOT_CONSTANTS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_SET_COMPUTE_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
}

FN_MOON_CMD_SET_WORK_GRAPH_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
}

FN_MOON_CMD_SET_RASTER_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
}

FN_MOON_CMD_SET_RAY_TRACING_PIPELINE(vulkan)
{
    (void)cmd;
    (void)pipeline;
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

FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(vulkan)
{
    (void)cmd;
    (void)sample_count;
}

FN_MOON_CMD_SET_DEPTH_BIAS(vulkan)
{
    (void)cmd;
    (void)depth_bias;
}

FN_MOON_CMD_SET_INDEX_BUFFER(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_BEGIN_RENDERPASS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_END_RENDERPASS(vulkan)
{
    (void)cmd;
}

FN_MOON_CMD_WRITE_TIMESTAMPS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_RESOLVE_TIMESTAMPS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_BEGIN_LABEL(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_END_LABEL(vulkan)
{
    (void)cmd;
}

FN_MOON_CMD_DISPATCH(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DISPATCH_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DISPATCH_GRAPH(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DRAW(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DRAW_INDEXED(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DRAW_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DRAW_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DRAW_MESH_TASKS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_TRACE_RAYS(vulkan)
{
    (void)cmd;
    (void)params;
}

FN_MOON_CMD_TRACE_RAYS_INDIRECT(vulkan)
{
    (void)cmd;
    (void)params;
}
#endif /* MOON_VULKAN */
