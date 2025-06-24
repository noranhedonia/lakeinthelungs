#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(vulkan)
{
    struct moon_timeline_query_pool_impl timeline_query_pool = {
        .header = {
            .device.impl = device,
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
    if (timeline_query_pool.header.assembly.name.len && device->vkSetDebugUtilsObjectNameEXT) {
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
    lake_memcpy(*out_timeline_query_pool, &timeline_query_pool, sizeof(struct moon_timeline_query_pool_impl));
    return LAKE_SUCCESS;
}

FN_MOON_TIMELINE_QUERY_POOL_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(timeline_query_pool != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&timeline_query_pool->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Timeline query pool `%s` reference count is %d.", timeline_query_pool->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_device_impl *device = timeline_query_pool->header.device.impl;

    zombie_timeline_query_pool submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = { .vk_query_pool = timeline_query_pool->vk_query_pool }, 
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_query_pool_idx];
    lake_deque_unshift_v_locked(device->query_pool_zombies, zombie_timeline_query_pool, submit, lock);

    moon_device_unref(timeline_query_pool->header.device);
    __lake_free(timeline_query_pool);
}

FN_MOON_TIMELINE_QUERY_POOL_QUERY_RESULTS(vulkan)
{
    if (count == 0) {
        return LAKE_SUCCESS;
    } else if (!(start + count - 1 < timeline_query_pool->header.assembly.query_count)) {
        return LAKE_ERROR_OUT_OF_RANGE;
    }
    struct moon_device_impl *device = timeline_query_pool->header.device.impl;

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

struct vk_access_info {
    VkPipelineStageFlags2   stage_mask;
    VkAccessFlags2          access_mask;
    VkImageLayout           image_layout;
};

static const struct vk_access_info g_access_map[moon_access_max_enum] = {
    { /* moon_access_none */
        VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },

    /* READ */

    { /* moon_access_command_buffer_read */
        VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_EXT,
        VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_EXT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_indirect_buffer */
        VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,
        VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_index_buffer */
        VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
        VK_ACCESS_2_INDEX_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_vertex_buffer */
        VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
        VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_vertex_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_vertex_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_vertex_shader_read_other */
        VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_tesselation_control_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_tesselation_control_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_tesselation_control_shader_read_other */
        VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_tesselation_evaluation_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_tesselation_evaluation_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_tesselation_evaluation_shader_read_other */
        VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_geometry_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_geometry_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_geometry_shader_read_other */
        VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_task_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_task_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_task_shader_read_other */
        VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_mesh_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_mesh_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_mesh_shader_read_other */
        VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_fragment_density_map_read */
        VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT,
        VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT,
        VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT,
    },
    { /* moon_access_fragment_shading_rate_read */
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
        VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR,
        VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
    },
    { /* moon_access_fragment_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_fragment_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_fragment_shader_read_color_input_attachment */
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_fragment_shader_read_depth_stencil_input_attachment */
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_fragment_shader_read_other */
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_color_attachment_read */
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    },
    { /* moon_access_color_attachment_advanced_blending */
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    },
    { /* moon_access_depth_stencil_attachment_read */
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_compute_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_compute_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_compute_shader_read_other */
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_any_shader_read_uniform_buffer */
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_any_shader_read_uniform_buffer_or_vertex_buffer */
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_any_shader_read_sampled_texture_or_uniform_texel_buffer */
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_any_shader_read_other */
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_video_decode_read */
        VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR,
        VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR,
        VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR,
    },
    { /* moon_access_video_encode_read */
        VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR,
        VK_ACCESS_2_VIDEO_ENCODE_READ_BIT_KHR,
        VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR,
    },
    { /* moon_access_transfer_read */
        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    },
    { /* moon_access_host_read */
        VK_PIPELINE_STAGE_2_HOST_BIT,
        VK_ACCESS_2_HOST_READ_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_present */
        VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    },
    { /* moon_access_conditional_rendering */
        VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT,
        VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_ray_tracing_shader_acceleration_structure_read */
        VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR,
        VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_acceleration_structure_build_read */
        VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },

    /* WRITE */

    { /* moon_access_command_buffer_write */
        VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_EXT,
        VK_ACCESS_2_COMMAND_PREPROCESS_WRITE_BIT_EXT,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },
    { /* moon_access_vertex_shader_write */
        VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_tesselation_control_shader_write */
        VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_tesselation_evaluation_shader_write */
        VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_geometry_shader_write */
        VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_fragment_shader_write */
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_task_shader_write */
        VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_mesh_shader_write */
        VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_transfer_write */
        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    },
    { /* moon_access_host_preinitialized */
        VK_PIPELINE_STAGE_2_HOST_BIT,
        VK_ACCESS_2_HOST_WRITE_BIT,
        VK_IMAGE_LAYOUT_PREINITIALIZED,
    },
    { /* moon_access_host_write */
        VK_PIPELINE_STAGE_2_HOST_BIT,
        VK_ACCESS_2_HOST_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_clear_write */
        VK_PIPELINE_STAGE_2_CLEAR_BIT,
        VK_ACCESS_2_MEMORY_WRITE_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    },
    { /* moon_access_color_attachment_write */
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    },
    { /* moon_access_depth_stencil_attachment_write */
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    },
    { /* moon_access_depth_attachment_write_stencil_read_only */
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
    },
    { /* moon_access_stencil_attachment_write_depth_read_only */
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
        VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
    },
    { /* moon_access_compute_shader_write */
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_any_shader_write */
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
    { /* moon_access_video_decode_write */
        VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR,
        VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR,
        VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR,
    },
    { /* moon_access_video_encode_write */
        VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR,
        VK_ACCESS_2_VIDEO_ENCODE_WRITE_BIT_KHR,
        VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR,
    },
    { /* moon_access_acceleration_structure_build_write */
        VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
        VK_IMAGE_LAYOUT_UNDEFINED,
    },

    /* OTHER */

    { /* moon_access_color_attachment_read_write */
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    },
    { /* moon_access_general */
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT,
        VK_IMAGE_LAYOUT_GENERAL,
    },
};

void populate_vk_access_info(
    u32                         access_count,
    moon_access const          *accesses,
    VkPipelineStageFlags2      *out_stage_mask,
    VkAccessFlags2             *out_access_mask,
    VkImageLayout              *out_image_layout,
    bool                       *has_write_access)
{
    VkPipelineStageFlags2 stage_mask = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 access_mask = VK_ACCESS_2_NONE;
    VkImageLayout image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    bool write_access = false; 

    for (u32 i = 0; i < access_count; i++) {
        moon_access access = accesses[i];
        lake_san_assert(access < moon_access_max_enum, LAKE_ERROR_OUT_OF_RANGE, "not a valid moon access");
        lake_dbg_assert(access < moon_access_end_of_read_enum || access_count == 1, LAKE_INVALID_PARAMETERS, "the access, if it's a write, must appear on its own");

        struct vk_access_info const *access_info = &g_access_map[access];

        stage_mask = access_info->stage_mask;
        if (access > moon_access_end_of_read_enum)
            write_access = true;

        access_mask |= access_info->access_mask;

        VkImageLayout layout = access_info->image_layout;
        lake_dbg_assert(image_layout == VK_IMAGE_LAYOUT_UNDEFINED || image_layout == layout,
                LAKE_INVALID_PARAMETERS, "out image layout must be undefined or match to expected");
        image_layout = layout;
    }
    if (out_stage_mask) *out_stage_mask = stage_mask;
    if (out_access_mask) *out_access_mask = access_mask;
    if (out_image_layout) *out_image_layout = image_layout;
    if (has_write_access) *has_write_access = write_access;
}

void populate_vk_memory_barrier(
    moon_global_barrier const  *global_barrier, 
    VkMemoryBarrier2           *out_barrier)
{
    VkMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .pNext = nullptr,
    };
    
    for (u32 i = 0; i < 2; i++) {
        u32 access_count = (i == 0) ? global_barrier->src_access_count : global_barrier->dst_access_count;
        moon_access const *accesses = (i == 0) ? global_barrier->src_accesses : global_barrier->dst_accesses; 

        for (u32 j = 0; j < access_count; j++) {
            moon_access access = accesses[i];

            lake_san_assert(access < moon_access_max_enum, LAKE_ERROR_OUT_OF_RANGE, "not a valid moon access");
            lake_dbg_assert(access < moon_access_end_of_read_enum || global_barrier->src_access_count == 1, LAKE_INVALID_PARAMETERS, "the access, if it's a write, must appear on its own");

            struct vk_access_info const *info = &g_access_map[access];
            if (i == 0) barrier.srcStageMask |= info->stage_mask;
            else barrier.dstStageMask |= info->stage_mask;

            if (i == 0) {
                /** Add appropriate availability operations - for writes only. */
                if (access >= moon_access_end_of_read_enum)
                    barrier.srcAccessMask |= info->access_mask;
            } else { 
                /** Add visibility operations if necessary. If the src access mask is zero, 
                 *  this is a WAR hazard (or for some reason a "RAR"), so the dst access 
                 *  mask can be safely zeroed as these don't need visibility. */
                if (barrier.srcAccessMask != 0)
                    barrier.dstAccessMask |= info->access_mask;
            }
        }
    }
    if (barrier.srcStageMask == 0)
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    if (barrier.dstStageMask == 0)
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    *out_barrier = barrier;
}

void populate_vk_buffer_memory_barrier(
    struct moon_device_impl    *device,
    moon_buffer_barrier const  *buffer_barrier, 
    VkBufferMemoryBarrier2     *out_barrier)
{
    VkBufferMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcQueueFamilyIndex = device->physical_device->queue_families[buffer_barrier->src_queue_type].vk_index, 
        .dstQueueFamilyIndex = device->physical_device->queue_families[buffer_barrier->dst_queue_type].vk_index, 
        .buffer = acquire_buffer_slot(device, buffer_barrier->buffer)->vk_buffer,
        .offset = buffer_barrier->offset,
        .size = buffer_barrier->size,
    };
    
    for (u32 i = 0; i < 2; i++) {
        u32 access_count = (i == 0) ? buffer_barrier->src_access_count : buffer_barrier->dst_access_count;
        moon_access const *accesses = (i == 0) ? buffer_barrier->src_accesses : buffer_barrier->dst_accesses; 

        for (u32 j = 0; j < access_count; j++) {
            moon_access access = accesses[i];

            lake_san_assert(access < moon_access_max_enum, LAKE_ERROR_OUT_OF_RANGE, "not a valid moon access");
            lake_dbg_assert(access < moon_access_end_of_read_enum || buffer_barrier->src_access_count == 1, LAKE_INVALID_PARAMETERS, "the access, if it's a write, must appear on its own");

            struct vk_access_info const *info = &g_access_map[access];
            if (i == 0) barrier.srcStageMask |= info->stage_mask;
            else barrier.dstStageMask |= info->stage_mask;

            if (i == 0) {
                /** Add appropriate availability operations - for writes only. */
                if (access >= moon_access_end_of_read_enum)
                    barrier.srcAccessMask |= info->access_mask;
            } else { 
                /** Add visibility operations if necessary. If the src access mask is zero, 
                 *  this is a WAR hazard (or for some reason a "RAR"), so the dst access 
                 *  mask can be safely zeroed as these don't need visibility. */
                if (barrier.srcAccessMask != 0)
                    barrier.dstAccessMask |= info->access_mask;
            }
        }
    }
    if (barrier.srcStageMask == 0)
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    if (barrier.dstStageMask == 0)
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    *out_barrier = barrier;
}

void populate_vk_image_memory_barrier(
    struct moon_device_impl    *device,
    moon_texture_barrier const *texture_barrier, 
    VkImageMemoryBarrier2      *out_barrier)
{

    struct texture_impl_slot const *slot = acquire_texture_slot(device, texture_barrier->texture);
    VkImageMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcQueueFamilyIndex = device->physical_device->queue_families[texture_barrier->src_queue_type].vk_index, 
        .dstQueueFamilyIndex = device->physical_device->queue_families[texture_barrier->dst_queue_type].vk_index, 
        .image = slot->vk_image,
        .subresourceRange = make_subresource_range(&texture_barrier->texture_slice, slot->aspect_flags),
    };

    for (u32 i = 0; i < 2; i++) {
        u32 access_count = (i == 0) ? texture_barrier->src_access_count : texture_barrier->dst_access_count;
        moon_access const *accesses = (i == 0) ? texture_barrier->src_accesses : texture_barrier->dst_accesses; 

        for (u32 j = 0; j < access_count; j++) {
            moon_access access = accesses[j];

            lake_dbg_assert(access < moon_access_max_enum, LAKE_ERROR_OUT_OF_RANGE, "not a valid moon access: %u", access);
            lake_dbg_assert(access < moon_access_end_of_read_enum || texture_barrier->src_access_count == 1, LAKE_INVALID_PARAMETERS, "the access, if it's a write, must appear on its own");

            struct vk_access_info const *info = &g_access_map[access];
            if (i == 0) barrier.srcStageMask |= info->stage_mask;
            else barrier.dstStageMask |= info->stage_mask;

            VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
            switch ((i == 0) ? texture_barrier->src_layout : texture_barrier->dst_layout) {
                case moon_layout_general:
                    if (access == moon_access_present) {
                        layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    } else {
                        layout = VK_IMAGE_LAYOUT_GENERAL;
                    } break;
                case moon_layout_optimal:
                    layout = info->image_layout;
                    break;
                case moon_layout_shared_present:
                    layout = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;
                    break;
            }

            if (i == 0) {
                /** Add appropriate availability operations - for writes only. */
                if (access >= moon_access_end_of_read_enum)
                    barrier.srcAccessMask |= info->access_mask;

                if (texture_barrier->discard_contents == true) {
                    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                } else {
                    lake_dbg_assert(barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED || barrier.oldLayout == layout, 
                            LAKE_INVALID_PARAMETERS, "old texture layout doesn't match");
                    barrier.oldLayout = layout;
                }
            } else { 
                /** Add visibility operations if necessary. If the src access mask is zero, 
                 *  this is a WAR hazard (or for some reason a "RAR"), so the dst access 
                 *  mask can be safely zeroed as these don't need visibility. */
                if (barrier.srcAccessMask != 0)
                    barrier.dstAccessMask |= info->access_mask;

                lake_dbg_assert(barrier.newLayout == VK_IMAGE_LAYOUT_UNDEFINED || barrier.newLayout == layout, 
                            LAKE_INVALID_PARAMETERS, "new texture layouts doesn't match");
                barrier.newLayout = layout;
            }
        }
    }
    lake_dbg_assert(barrier.newLayout != barrier.oldLayout || barrier.srcQueueFamilyIndex != barrier.dstQueueFamilyIndex, 
            LAKE_INVALID_PARAMETERS, "there is no transition operation for the image barrier");

    if (barrier.srcStageMask == 0)
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    if (barrier.dstStageMask == 0)
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    *out_barrier = barrier;
}

FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(vulkan)
{
    struct moon_timeline_semaphore_impl sem = {
        .header = {
            .device.impl = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_timeline_semaphore_zero_refcnt,
        },
    };
    VkSemaphoreTypeCreateInfo vk_sem_timeline_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = sem.header.assembly.initial_value,
    };
    VkSemaphoreCreateInfo const vk_sem_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &vk_sem_timeline_info,
        .flags = 0,
    };
    VkResult vk_result = device->vkCreateSemaphore(device->vk_device, &vk_sem_create_info, device->vk_allocator, &sem.vk_semaphore);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

#ifndef LAKE_NDEBUG
    if (sem.header.assembly.name.len && device->vkSetDebugUtilsObjectNameEXT) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_SEMAPHORE,
            .objectHandle = (u64)(uptr)sem.vk_semaphore,
            .pObjectName = sem.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&sem.header.refcnt);
    *out_timeline_semaphore = __lake_malloc_t(struct moon_timeline_semaphore_impl);
    lake_memcpy(*out_timeline_semaphore, &sem, sizeof(struct moon_timeline_semaphore_impl));
    return LAKE_SUCCESS;
}

FN_MOON_TIMELINE_SEMAPHORE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(timeline_semaphore != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&timeline_semaphore->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Timeline semaphore `%s` reference count is %d.", timeline_semaphore->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_device_impl *device = timeline_semaphore->header.device.impl;

    zombie_timeline_semaphore submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = { .vk_semaphore = timeline_semaphore->vk_semaphore }, 
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_semaphore_idx];
    lake_deque_unshift_v_locked(device->semaphore_zombies, zombie_timeline_semaphore, submit, lock);

    moon_device_unref(timeline_semaphore->header.device);
    __lake_free(timeline_semaphore);
}

FN_MOON_TIMELINE_SEMAPHORE_READ_VALUE(vulkan)
{
    struct moon_device_impl *device = timeline_semaphore->header.device.impl;
    return vk_result_translate(
        device->vkGetSemaphoreCounterValue(
            device->vk_device, 
            timeline_semaphore->vk_semaphore, 
            out_value));
}

FN_MOON_TIMELINE_SEMAPHORE_WRITE_VALUE(vulkan)
{
    struct moon_device_impl *device = timeline_semaphore->header.device.impl;
    VkSemaphoreSignalInfo const vk_sem_signal_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO,
        .pNext = nullptr,
        .semaphore = timeline_semaphore->vk_semaphore,
        .value = value,
    };
    return vk_result_translate(device->vkSignalSemaphore(device->vk_device, &vk_sem_signal_info));
}

FN_MOON_TIMELINE_SEMAPHORE_WAIT_FOR_VALUE(vulkan)
{
    struct moon_device_impl *device = timeline_semaphore->header.device.impl;
    VkSemaphoreWaitInfo const vk_sem_wait_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext = nullptr,
        .flags = 0,
        .semaphoreCount = 1,
        .pSemaphores = &timeline_semaphore->vk_semaphore,
        .pValues = &value,
    };
    return vk_result_translate(device->vkWaitSemaphores(device->vk_device, &vk_sem_wait_info, timeout));
}

FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(vulkan)
{
    struct moon_binary_semaphore_impl sem = {
        .header = {
            .device.impl = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_binary_semaphore_zero_refcnt,
        },
    };
    VkSemaphoreCreateInfo const vk_sem_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    VkResult vk_result = device->vkCreateSemaphore(device->vk_device, &vk_sem_create_info, device->vk_allocator, &sem.vk_semaphore);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

#ifndef LAKE_NDEBUG
    if (sem.header.assembly.name.len && device->vkSetDebugUtilsObjectNameEXT) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_SEMAPHORE,
            .objectHandle = (u64)(uptr)sem.vk_semaphore,
            .pObjectName = sem.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&sem.header.refcnt);
    *out_binary_semaphore = __lake_malloc_t(struct moon_binary_semaphore_impl);
    lake_memcpy(*out_binary_semaphore, &sem, sizeof(struct moon_binary_semaphore_impl));
    return LAKE_SUCCESS;
}

FN_MOON_BINARY_SEMAPHORE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(binary_semaphore != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&binary_semaphore->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Binary semaphore `%s` reference count is %d.", binary_semaphore->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_device_impl *device = binary_semaphore->header.device.impl;

    zombie_timeline_semaphore submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = { .vk_semaphore = binary_semaphore->vk_semaphore }, 
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_semaphore_idx];
    lake_deque_unshift_v_locked(device->semaphore_zombies, zombie_timeline_semaphore, submit, lock);

    moon_device_unref(binary_semaphore->header.device);
    __lake_free(binary_semaphore);
}

FN_MOON_EVENT_ASSEMBLY(vulkan)
{
    struct moon_event_impl event = {
        .header = {
            .device.impl = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_event_zero_refcnt,
        },
    };
    VkEventCreateInfo const vk_event_create_info = {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_EVENT_CREATE_DEVICE_ONLY_BIT,
    };
    VkResult vk_result = device->vkCreateEvent(device->vk_device, &vk_event_create_info, device->vk_allocator, &event.vk_event);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

#ifndef LAKE_NDEBUG
    if (event.header.assembly.name.len && device->vkSetDebugUtilsObjectNameEXT) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_EVENT,
            .objectHandle = (u64)(uptr)event.vk_event,
            .pObjectName = event.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&event.header.refcnt);
    *out_event = __lake_malloc_t(struct moon_event_impl);
    lake_memcpy(*out_event, &event, sizeof(struct moon_event_impl));
    return LAKE_SUCCESS;
}

FN_MOON_EVENT_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(event != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&event->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Event `%s` reference count is %d.", event->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_device_impl *device = event->header.device.impl;

    zombie_timeline_event submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = { .vk_event = event->vk_event }, 
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_event_idx];
    lake_deque_unshift_v_locked(device->event_zombies, zombie_timeline_event, submit, lock);

    moon_device_unref(event->header.device);
    __lake_free(event);
}
#endif /* MOON_VULKAN */
