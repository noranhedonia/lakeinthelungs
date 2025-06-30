#include "../test_framework.h"

struct frame {
    moon_interface  moon;
    moon_device     device;
};

s32 MoonImpl_todo(void *) 
{
    return TEST_RESULT_SKIPPED;
}

static bool interface_impl_is_valid(struct moon_interface_impl *impl) 
{
    /* if the backend is not supported, just skip */
    if (impl == nullptr) return false;

    /* if any procedure is missing we would like to log this */
    bool is_valid = true;
    lake_drift_push();
    lake_strbuf buf = { .v = lake_drift(4096, 1), .len = 0, .alloc = 4096 };
    char const *whitespace = "\n            ";

#define VALIDATE_PROC(FN) \
    if (impl->FN == nullptr) { is_valid = false; \
        lake_strbuf_appendstrn(&buf, whitespace, 13); \
        lake_strbuf_appendstrn(&buf, "PFN_moon_" #FN, lake_lengthof("PFN_moon_" #FN)); \
    }
    VALIDATE_PROC(connect_to_hadal)
    VALIDATE_PROC(list_device_details)
    VALIDATE_PROC(device_assembly)
    VALIDATE_PROC(device_zero_refcnt)
    VALIDATE_PROC(device_queue_count)
    VALIDATE_PROC(device_queue_wait_idle)
    VALIDATE_PROC(device_wait_idle)
    VALIDATE_PROC(device_submit_commands)
    VALIDATE_PROC(device_present_frames)
    VALIDATE_PROC(device_commit_deferred_destructors)
    VALIDATE_PROC(device_buffer_memory_requirements)
    VALIDATE_PROC(device_texture_memory_requirements)
    VALIDATE_PROC(device_tlas_build_sizes)
    VALIDATE_PROC(device_blas_build_sizes)
    VALIDATE_PROC(device_memory_report)
    VALIDATE_PROC(memory_heap_assembly)
    VALIDATE_PROC(memory_heap_zero_refcnt)
    VALIDATE_PROC(create_buffer)
    VALIDATE_PROC(create_buffer_from_memory_heap)
    VALIDATE_PROC(create_texture)
    VALIDATE_PROC(create_texture_from_memory_heap)
    VALIDATE_PROC(create_texture_view)
    VALIDATE_PROC(create_sampler)
    VALIDATE_PROC(create_tlas)
    VALIDATE_PROC(create_tlas_from_buffer)
    VALIDATE_PROC(create_blas)
    VALIDATE_PROC(create_blas_from_buffer)
    VALIDATE_PROC(is_buffer_valid)
    VALIDATE_PROC(is_texture_valid)
    VALIDATE_PROC(is_texture_view_valid)
    VALIDATE_PROC(is_sampler_valid)
    VALIDATE_PROC(is_tlas_valid)
    VALIDATE_PROC(is_blas_valid)
    VALIDATE_PROC(buffer_host_address)
    VALIDATE_PROC(buffer_device_address)
    VALIDATE_PROC(tlas_device_address)
    VALIDATE_PROC(blas_device_address)
    VALIDATE_PROC(read_buffer_assembly)
    VALIDATE_PROC(read_texture_assembly)
    VALIDATE_PROC(read_texture_view_assembly)
    VALIDATE_PROC(read_sampler_assembly)
    VALIDATE_PROC(read_tlas_assembly)
    VALIDATE_PROC(read_blas_assembly)
    VALIDATE_PROC(destroy_buffer)
    VALIDATE_PROC(destroy_texture)
    VALIDATE_PROC(destroy_texture_view)
    VALIDATE_PROC(destroy_sampler)
    VALIDATE_PROC(destroy_tlas)
    VALIDATE_PROC(destroy_blas)
    VALIDATE_PROC(timeline_query_pool_assembly)
    VALIDATE_PROC(timeline_query_pool_zero_refcnt)
    VALIDATE_PROC(timeline_query_pool_query_results)
    VALIDATE_PROC(timeline_semaphore_assembly)
    VALIDATE_PROC(timeline_semaphore_zero_refcnt)
    VALIDATE_PROC(timeline_semaphore_read_value)
    VALIDATE_PROC(timeline_semaphore_write_value)
    VALIDATE_PROC(timeline_semaphore_wait_for_value)
    VALIDATE_PROC(binary_semaphore_assembly)
    VALIDATE_PROC(binary_semaphore_zero_refcnt)
    VALIDATE_PROC(event_assembly)
    VALIDATE_PROC(event_zero_refcnt)
    VALIDATE_PROC(compute_pipeline_assembly)
    VALIDATE_PROC(compute_pipeline_zero_refcnt)
    VALIDATE_PROC(work_graph_pipeline_assembly)
    VALIDATE_PROC(work_graph_pipeline_zero_refcnt)
    VALIDATE_PROC(work_graph_pipeline_node_index)
    VALIDATE_PROC(work_graph_pipeline_scratch_size)
    VALIDATE_PROC(ray_tracing_pipeline_assembly)
    VALIDATE_PROC(ray_tracing_pipeline_zero_refcnt)
    VALIDATE_PROC(ray_tracing_pipeline_create_default_sbt)
    VALIDATE_PROC(ray_tracing_pipeline_shader_group_handles)
    VALIDATE_PROC(raster_pipeline_assembly)
    VALIDATE_PROC(raster_pipeline_zero_refcnt)
    VALIDATE_PROC(swapchain_assembly)
    VALIDATE_PROC(swapchain_zero_refcnt)
    VALIDATE_PROC(swapchain_wait_for_next_frame)
    VALIDATE_PROC(swapchain_acquire_next_image)
    VALIDATE_PROC(swapchain_current_acquire_semaphore)
    VALIDATE_PROC(swapchain_current_present_semaphore)
    VALIDATE_PROC(swapchain_current_cpu_timeline_value)
    VALIDATE_PROC(swapchain_current_timeline_pair)
    VALIDATE_PROC(swapchain_gpu_timeline_semaphore)
    VALIDATE_PROC(swapchain_set_present_mode)
    VALIDATE_PROC(swapchain_resize)
    VALIDATE_PROC(command_recorder_assembly)
    VALIDATE_PROC(command_recorder_zero_refcnt)
    VALIDATE_PROC(staged_command_list_assembly)
    VALIDATE_PROC(staged_command_list_zero_refcnt)
    VALIDATE_PROC(cmd_copy_buffer)
    VALIDATE_PROC(cmd_copy_buffer_to_texture)
    VALIDATE_PROC(cmd_copy_texture_to_buffer)
    VALIDATE_PROC(cmd_copy_texture)
    VALIDATE_PROC(cmd_blit_texture)
    VALIDATE_PROC(cmd_resolve_texture)
    VALIDATE_PROC(cmd_clear_buffer)
    VALIDATE_PROC(cmd_clear_texture)
    VALIDATE_PROC(cmd_build_acceleration_structures)
    VALIDATE_PROC(cmd_destroy_buffer_deferred)
    VALIDATE_PROC(cmd_destroy_texture_deferred)
    VALIDATE_PROC(cmd_destroy_texture_view_deferred)
    VALIDATE_PROC(cmd_destroy_sampler_deferred)
    VALIDATE_PROC(cmd_push_constants)
    VALIDATE_PROC(cmd_set_compute_pipeline)
    VALIDATE_PROC(cmd_set_work_graph_pipeline)
    VALIDATE_PROC(cmd_set_ray_tracing_pipeline)
    VALIDATE_PROC(cmd_set_raster_pipeline)
    VALIDATE_PROC(cmd_set_viewport)
    VALIDATE_PROC(cmd_set_scissor)
    VALIDATE_PROC(cmd_set_depth_bias)
    VALIDATE_PROC(cmd_set_index_buffer)
    VALIDATE_PROC(cmd_set_rasterization_samples)
    VALIDATE_PROC(cmd_begin_renderpass)
    VALIDATE_PROC(cmd_end_renderpass)
    VALIDATE_PROC(cmd_write_timestamps)
    VALIDATE_PROC(cmd_resolve_timestamps)
    VALIDATE_PROC(cmd_barriers_and_transitions)
    VALIDATE_PROC(cmd_signal_event)
    VALIDATE_PROC(cmd_wait_events)
    VALIDATE_PROC(cmd_reset_event)
    VALIDATE_PROC(cmd_begin_label)
    VALIDATE_PROC(cmd_end_label)
    VALIDATE_PROC(cmd_dispatch)
    VALIDATE_PROC(cmd_dispatch_indirect)
    VALIDATE_PROC(cmd_dispatch_graph)
    VALIDATE_PROC(cmd_dispatch_graph_indirect)
    VALIDATE_PROC(cmd_dispatch_graph_indirect_count)
    VALIDATE_PROC(cmd_dispatch_graph_scratch_memory)
    VALIDATE_PROC(cmd_trace_rays)
    VALIDATE_PROC(cmd_trace_rays_indirect)
    VALIDATE_PROC(cmd_draw)
    VALIDATE_PROC(cmd_draw_indexed)
    VALIDATE_PROC(cmd_draw_indirect)
    VALIDATE_PROC(cmd_draw_indirect_count)
    VALIDATE_PROC(cmd_draw_mesh_tasks)
    VALIDATE_PROC(cmd_draw_mesh_tasks_indirect)
    VALIDATE_PROC(cmd_draw_mesh_tasks_indirect_count)
#undef VALIDATE_PROC
    if (!is_valid) {
        test_log_context();
        test_log("Interface `%s` is not complete, procedures missing: %s", impl->header.name.str, buf.v);
    }
    lake_drift_pop();
    return is_valid;
}

#define IMPL_MOON_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(MoonImpl_##VAR) \
    { \
        struct frame *work = lake_drift_t(struct frame); \
        work->moon.impl = moon_interface_impl_##VAR(bedrock); \
        if (!interface_impl_is_valid(work->moon.interface)) { \
            *out = (struct test_suite_details){0}; \
            return; \
        } \
        moon_device_assembly const device_assembly = { \
            .device_idx = 0, \
            .explicit_features = moon_explicit_feature_buffer_device_address_capture_replay, \
            .max_allowed_buffers = 10000, \
            .max_allowed_textures = 10000, \
            .max_allowed_samplers = 400, \
            .max_allowed_acceleration_structures = 10000, \
            .name = lake_small_string_cstr("primary"), \
        }; \
        if (work->moon.interface->device_assembly && \
            work->moon.interface->device_assembly(work->moon.impl, &device_assembly, &work->device.impl) == LAKE_SUCCESS) \
        { \
            *out = (struct test_suite_details){ \
                .count = lake_arraysize(g_tests), \
                .tests = g_tests, \
                .userdata = work, \
            }; \
            return; \
        } \
        moon_interface_unref(work->moon); \
        *out = (struct test_suite_details){0}; \
    } \
    FN_TEST_SUITE_FINI(MoonImpl_##VAR) \
    { \
        struct frame *work = (struct frame *)userdata; \
        moon_device_unref(work->device); \
        moon_interface_unref(work->moon); \
    }

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(MoonImpl, todo),
};

#ifdef MOON_D3D12
IMPL_MOON_TEST_SUITE(d3d12)
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
IMPL_MOON_TEST_SUITE(metal)
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
IMPL_MOON_TEST_SUITE(webgpu)
#endif /* MOON_WEBGPU */
#ifdef MOON_VULKAN
IMPL_MOON_TEST_SUITE(vulkan)
#endif /* MOON_VULKAN */
IMPL_MOON_TEST_SUITE(mock)
