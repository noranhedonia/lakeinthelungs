#pragma once

/** @file lake/moon.h
 *  @brief Rendering interface.
 *
 *  TODO docs
 */
#include <lake/renderer/render_resources.h>
#include <lake/renderer/timeline_sync.h>
#include <lake/renderer/pipelines.h>
#include <lake/renderer/device.h>
#include <lake/renderer/swapchain.h>
#include <lake/renderer/command_recorder.h>

/** Interface of the rendering backend. */
struct moon_interface_impl {
    lake_interface_header                               header;
    PFN_moon_connect_to_display                         connect_to_display;
    PFN_moon_list_device_details                        list_device_details;
    
    PFN_moon_device_assembly                            device_assembly;
    PFN_moon_device_zero_refcnt                         device_zero_refcnt;
    PFN_moon_device_queue_count                         device_queue_count;
    PFN_moon_device_queue_wait_idle                     device_queue_wait_idle;
    PFN_moon_device_wait_idle                           device_wait_idle;
    PFN_moon_device_submit_commands                     device_submit_commands;
    PFN_moon_device_present_frames                      device_present_frames;
    PFN_moon_device_commit_deferred_destructors         device_commit_deferred_destructors;
    PFN_moon_device_buffer_memory_requirements          device_buffer_memory_requirements;
    PFN_moon_device_texture_memory_requirements         device_texture_memory_requirements;
    PFN_moon_device_tlas_build_sizes                    device_tlas_build_sizes;
    PFN_moon_device_blas_build_sizes                    device_blas_build_sizes;
    PFN_moon_device_memory_report                       device_memory_report;

    PFN_moon_memory_heap_assembly                       memory_heap_assembly;
    PFN_moon_memory_heap_zero_refcnt                    memory_heap_zero_refcnt;

    PFN_moon_create_buffer                              create_buffer;
    PFN_moon_create_buffer_from_memory_heap             create_buffer_from_memory_heap;
    PFN_moon_create_texture                             create_texture;
    PFN_moon_create_texture_from_memory_heap            create_texture_from_memory_heap;
    PFN_moon_create_texture_view                        create_texture_view;
    PFN_moon_create_sampler                             create_sampler;
    PFN_moon_create_tlas                                create_tlas;
    PFN_moon_create_tlas_from_buffer                    create_tlas_from_buffer;
    PFN_moon_create_blas                                create_blas;
    PFN_moon_create_blas_from_buffer                    create_blas_from_buffer;

    PFN_moon_is_buffer_valid                            is_buffer_valid;
    PFN_moon_is_texture_valid                           is_texture_valid;
    PFN_moon_is_texture_view_valid                      is_texture_view_valid;
    PFN_moon_is_sampler_valid                           is_sampler_valid;
    PFN_moon_is_tlas_valid                              is_tlas_valid;
    PFN_moon_is_blas_valid                              is_blas_valid;

    PFN_moon_buffer_host_address                        buffer_host_address;
    PFN_moon_buffer_device_address                      buffer_device_address;
    PFN_moon_tlas_device_address                        tlas_device_address;
    PFN_moon_blas_device_address                        blas_device_address;

    PFN_moon_read_buffer_assembly                       read_buffer_assembly;
    PFN_moon_read_texture_assembly                      read_texture_assembly;
    PFN_moon_read_texture_view_assembly                 read_texture_view_assembly;
    PFN_moon_read_sampler_assembly                      read_sampler_assembly;
    PFN_moon_read_tlas_assembly                         read_tlas_assembly;
    PFN_moon_read_blas_assembly                         read_blas_assembly;

    PFN_moon_destroy_buffer                             destroy_buffer;
    PFN_moon_destroy_texture                            destroy_texture;
    PFN_moon_destroy_texture_view                       destroy_texture_view;
    PFN_moon_destroy_sampler                            destroy_sampler;
    PFN_moon_destroy_tlas                               destroy_tlas;
    PFN_moon_destroy_blas                               destroy_blas;

    PFN_moon_timeline_query_pool_assembly               timeline_query_pool_assembly;
    PFN_moon_timeline_query_pool_zero_refcnt            timeline_query_pool_zero_refcnt;
    PFN_moon_timeline_query_pool_query_results          timeline_query_pool_query_results;

    PFN_moon_timeline_semaphore_assembly                timeline_semaphore_assembly;
    PFN_moon_timeline_semaphore_zero_refcnt             timeline_semaphore_zero_refcnt;
    PFN_moon_timeline_semaphore_read_value              timeline_semaphore_read_value;
    PFN_moon_timeline_semaphore_write_value             timeline_semaphore_write_value;
    PFN_moon_timeline_semaphore_wait_for_value          timeline_semaphore_wait_for_value;

    PFN_moon_binary_semaphore_assembly                  binary_semaphore_assembly;
    PFN_moon_binary_semaphore_zero_refcnt               binary_semaphore_zero_refcnt;

    PFN_moon_event_assembly                             event_assembly;
    PFN_moon_event_zero_refcnt                          event_zero_refcnt;

    PFN_moon_compute_pipeline_assembly                  compute_pipeline_assembly;
    PFN_moon_compute_pipeline_zero_refcnt               compute_pipeline_zero_refcnt;

    PFN_moon_work_graph_pipeline_assembly               work_graph_pipeline_assembly;
    PFN_moon_work_graph_pipeline_zero_refcnt            work_graph_pipeline_zero_refcnt;
    PFN_moon_work_graph_pipeline_node_index             work_graph_pipeline_node_index;
    PFN_moon_work_graph_pipeline_scratch_size           work_graph_pipeline_scratch_size;

    PFN_moon_ray_tracing_pipeline_assembly              ray_tracing_pipeline_assembly;
    PFN_moon_ray_tracing_pipeline_zero_refcnt           ray_tracing_pipeline_zero_refcnt;
    PFN_moon_ray_tracing_pipeline_create_default_sbt    ray_tracing_pipeline_create_default_sbt;
    PFN_moon_ray_tracing_pipeline_shader_group_handles  ray_tracing_pipeline_shader_group_handles;

    PFN_moon_raster_pipeline_assembly                   raster_pipeline_assembly;
    PFN_moon_raster_pipeline_zero_refcnt                raster_pipeline_zero_refcnt;

    PFN_moon_swapchain_assembly                         swapchain_assembly;
    PFN_moon_swapchain_zero_refcnt                      swapchain_zero_refcnt;
    PFN_moon_swapchain_wait_for_next_frame              swapchain_wait_for_next_frame;
    PFN_moon_swapchain_acquire_next_image               swapchain_acquire_next_image;
    PFN_moon_swapchain_current_acquire_semaphore        swapchain_current_acquire_semaphore;
    PFN_moon_swapchain_current_present_semaphore        swapchain_current_present_semaphore;
    PFN_moon_swapchain_current_cpu_timeline_value       swapchain_current_cpu_timeline_value;
    PFN_moon_swapchain_current_timeline_pair            swapchain_current_timeline_pair;
    PFN_moon_swapchain_gpu_timeline_semaphore           swapchain_gpu_timeline_semaphore;
    PFN_moon_swapchain_set_present_mode                 swapchain_set_present_mode;
    PFN_moon_swapchain_resize                           swapchain_resize;

    PFN_moon_command_recorder_assembly                  command_recorder_assembly;
    PFN_moon_command_recorder_zero_refcnt               command_recorder_zero_refcnt;

    PFN_moon_staged_command_list_assembly               staged_command_list_assembly;
    PFN_moon_staged_command_list_zero_refcnt            staged_command_list_zero_refcnt;

    PFN_moon_cmd_copy_buffer                            cmd_copy_buffer;
    PFN_moon_cmd_copy_buffer_to_texture                 cmd_copy_buffer_to_texture;
    PFN_moon_cmd_copy_texture_to_buffer                 cmd_copy_texture_to_buffer;
    PFN_moon_cmd_copy_texture                           cmd_copy_texture;
    PFN_moon_cmd_blit_texture                           cmd_blit_texture;
    PFN_moon_cmd_resolve_texture                        cmd_resolve_texture;
    PFN_moon_cmd_clear_buffer                           cmd_clear_buffer;
    PFN_moon_cmd_clear_texture                          cmd_clear_texture;
    PFN_moon_cmd_build_acceleration_structures          cmd_build_acceleration_structures;
    PFN_moon_cmd_destroy_buffer_deferred                cmd_destroy_buffer_deferred;
    PFN_moon_cmd_destroy_texture_deferred               cmd_destroy_texture_deferred;
    PFN_moon_cmd_destroy_texture_view_deferred          cmd_destroy_texture_view_deferred;
    PFN_moon_cmd_destroy_sampler_deferred               cmd_destroy_sampler_deferred;
    PFN_moon_cmd_push_constants                         cmd_push_constants;
    PFN_moon_cmd_set_compute_pipeline                   cmd_set_compute_pipeline;
    PFN_moon_cmd_set_work_graph_pipeline                cmd_set_work_graph_pipeline;
    PFN_moon_cmd_set_ray_tracing_pipeline               cmd_set_ray_tracing_pipeline;
    PFN_moon_cmd_set_raster_pipeline                    cmd_set_raster_pipeline;
    PFN_moon_cmd_set_viewport                           cmd_set_viewport;
    PFN_moon_cmd_set_scissor                            cmd_set_scissor;
    PFN_moon_cmd_set_depth_bias                         cmd_set_depth_bias;
    PFN_moon_cmd_set_index_buffer                       cmd_set_index_buffer;
    PFN_moon_cmd_set_rasterization_samples              cmd_set_rasterization_samples;
    PFN_moon_cmd_begin_renderpass                       cmd_begin_renderpass;
    PFN_moon_cmd_end_renderpass                         cmd_end_renderpass;
    PFN_moon_cmd_write_timestamps                       cmd_write_timestamps;
    PFN_moon_cmd_resolve_timestamps                     cmd_resolve_timestamps;
    PFN_moon_cmd_pipeline_barrier                       cmd_pipeline_barrier;
    PFN_moon_cmd_signal_event                           cmd_signal_event;
    PFN_moon_cmd_wait_events                            cmd_wait_events;
    PFN_moon_cmd_reset_event                            cmd_reset_event;
    PFN_moon_cmd_begin_label                            cmd_begin_label;
    PFN_moon_cmd_end_label                              cmd_end_label;
    PFN_moon_cmd_dispatch                               cmd_dispatch;
    PFN_moon_cmd_dispatch_indirect                      cmd_dispatch_indirect;
    PFN_moon_cmd_dispatch_graph                         cmd_dispatch_graph;
    PFN_moon_cmd_dispatch_graph_indirect                cmd_dispatch_graph_indirect;
    PFN_moon_cmd_dispatch_graph_indirect_count          cmd_dispatch_graph_indirect_count;
    PFN_moon_cmd_dispatch_graph_scratch_memory          cmd_dispatch_graph_scratch_memory;
    PFN_moon_cmd_trace_rays                             cmd_trace_rays;
    PFN_moon_cmd_trace_rays_indirect                    cmd_trace_rays_indirect;
    PFN_moon_cmd_draw                                   cmd_draw;
    PFN_moon_cmd_draw_indexed                           cmd_draw_indexed;
    PFN_moon_cmd_draw_indirect                          cmd_draw_indirect;
    PFN_moon_cmd_draw_indirect_count                    cmd_draw_indirect_count;
    PFN_moon_cmd_draw_mesh_tasks                        cmd_draw_mesh_tasks;
    PFN_moon_cmd_draw_mesh_tasks_indirect               cmd_draw_mesh_tasks_indirect;
    PFN_moon_cmd_draw_mesh_tasks_indirect_count         cmd_draw_mesh_tasks_indirect_count;
};

LAKE_IMPL_HANDLE_INTERFACED(moon_device,                moon_interface moon,        moon_device_details const *details; );
LAKE_IMPL_HANDLE_INTERFACED(moon_memory_heap,           moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_timeline_query_pool,   moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_timeline_semaphore,    moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_binary_semaphore,      moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_event,                 moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_compute_pipeline,      moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_work_graph_pipeline,   moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_ray_tracing_pipeline,  moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_raster_pipeline,       moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_swapchain,             moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_command_recorder,      moon_device device,         LAKE_MAGIC_NOTHING());
LAKE_IMPL_HANDLE_INTERFACED(moon_staged_command_list,   moon_command_recorder cmd,  LAKE_MAGIC_NOTHING());

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MOON_D3D12
LAKEAPI FN_LAKE_INTERFACE_IMPL(moon, d3d12, lake_framework);
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
LAKEAPI FN_LAKE_INTERFACE_IMPL(moon, metal, lake_framework);
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
LAKEAPI FN_LAKE_INTERFACE_IMPL(moon, webgpu, lake_framework);
#endif /* MOON_WEBGPU */
#ifdef MOON_VULKAN
LAKEAPI FN_LAKE_INTERFACE_IMPL(moon, vulkan, lake_framework);
#endif /* MOON_VULKAN */

LAKEAPI FN_LAKE_INTERFACE_IMPL(moon, mock, lake_framework);

/** Returns a score for the device, by checking it's details. */
LAKEAPI u32 LAKECALL 
moon_calculate_score_from_device_details(
    moon_device_details const *details);

/** Obtain a string for the queue type. */
LAKE_CONST_FN
LAKEAPI char const *LAKECALL 
moon_queue_type_to_string(
    moon_queue_type type);

/** Used as a suplementary for the swapchain surface selector, if no custom one was given. */
LAKEAPI FN_MOON_SURFACE_FORMAT_SELECTOR(moon_default_surface_format_selector);

#ifdef __cplusplus
}
#endif /* __cplusplus */
