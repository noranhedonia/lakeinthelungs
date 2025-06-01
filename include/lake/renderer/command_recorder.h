#pragma once

/** @file lake/renderer/command_recorder.h
 *  @brief TODO
 *
 *  TODO docs
 */
#include <lake/renderer/moon_adapter.h>
#include <lake/renderer/device.h>

#define PFN_MOON_COMMAND_NO_DISCARD(fn, ...) \
    typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_cmd_##fn)(moon_command_recorder cmd, __VA_ARGS__)
#define FN_MOON_COMMAND_NO_DISCARD(fn, backend, ...) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_cmd_##fn(moon_command_recorder cmd, __VA_ARGS__)

#define PFN_MOON_COMMAND(fn, ...) \
    typedef void (LAKECALL *PFN_moon_cmd_##fn)(moon_command_recorder cmd, __VA_ARGS__)
#define FN_MOON_COMMAND(fn, backend, ...) \
    void LAKECALL _moon_##backend##_cmd_##fn(moon_command_recorder cmd, __VA_ARGS__)

#define PFN_MOON_COMMAND_NO_ARGS(fn) \
    typedef void (LAKECALL *PFN_moon_cmd_##fn)(moon_command_recorder cmd)
#define FN_MOON_COMMAND_NO_ARGS(fn, backend) \
    void LAKECALL _moon_##backend##_cmd_##fn(moon_command_recorder cmd)

typedef struct moon_buffer_copy_region {
    u64                                 src_offset;
    u64                                 dst_offset;
    u64                                 size;
} moon_buffer_copy_region;

typedef struct moon_copy_buffer {
    moon_buffer_id                      src_buffer;
    moon_buffer_id                      dst_buffer;
    u32                                 region_count;
    moon_buffer_copy_region const      *regions;
} moon_copy_buffer;
static constexpr moon_copy_buffer MOON_COPY_BUFFER_INIT = {
    .region_count = 1,
};

typedef struct moon_buffer_and_texture_copy_region {
    u64                                 buffer_offset;
    u32                                 buffer_row_length;
    u32                                 buffer_texture_height;
    moon_texture_array_slice            texture_slice;
    lake_offset3d                       texture_offset;
    lake_extent3d                       texture_extent;
} moon_buffer_and_texture_copy_region;

typedef struct moon_copy_buffer_and_texture {
    moon_buffer_id                              buffer;
    moon_texture_id                             texture;
    moon_texture_layout                         texture_layout;
    u32                                         region_count;
    moon_buffer_and_texture_copy_region const  *regions;
} moon_copy_buffer_and_texture;
static constexpr moon_copy_buffer_and_texture MOON_COPY_BUFFER_TO_TEXTURE_INIT = {
    .texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};
static constexpr moon_copy_buffer_and_texture MOON_COPY_TEXTURE_TO_BUFFER_INIT = {
    .texture_layout = moon_texture_layout_transfer_src_optimal,
    .region_count = 1,
};

typedef struct moon_texture_copy_region {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offset;
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offset;
    lake_extent3d                       extent;
} moon_texture_copy_region;

typedef struct moon_copy_texture {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    u32                                 region_count;
    moon_texture_copy_region const     *regions;
} moon_copy_texture;
static constexpr moon_copy_texture MOON_COPY_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};

typedef struct moon_texture_blit_region {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offsets[2];
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offsets[2];
} moon_texture_blit_region;

typedef struct moon_blit_texture {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    moon_filter_mode                    filter;
    u32                                 region_count;
    moon_texture_blit_region const     *regions;
} moon_blit_texture;
static constexpr moon_blit_texture MOON_BLIT_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .filter = moon_filter_mode_linear,
    .region_count = 1,
};

typedef struct moon_texture_resolve_region {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offset;
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offset;
    lake_extent3d                       extent;
} moon_texture_resolve_region;

typedef struct moon_resolve_texture {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    u32                                 region_count;
    moon_texture_resolve_region const  *regions;
} moon_resolve_texture;
static constexpr moon_resolve_texture MOON_RESOLVE_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};

typedef struct moon_clear_buffer {
    moon_buffer_id      dst_buffer;
    u64                 dst_offset;
    u64                 size;
    s32                 clear_value;
} moon_clear_buffer;
static constexpr moon_clear_buffer MOON_CLEAR_BUFFER_INIT = {0};

typedef struct moon_clear_texture {
    moon_texture_id     dst_texture;
    moon_texture_layout dst_texture_layout;
    moon_color_value    clear_value;
} moon_clear_texture;
static constexpr moon_clear_texture MOON_CLEAR_TEXTURE_INIT = {
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
};

typedef struct moon_build_acceleration_structures {
    u32                 tlas_assembly_count;
    u32                 blas_assembly_count;
    moon_tlas_assembly *tlas_assembly;
    moon_blas_assembly *blas_assembly;
} moon_build_acceleration_structures;
static constexpr moon_build_acceleration_structures MOON_BUILD_ACCELERATION_STRUCTURES_INIT = {0};

typedef struct moon_push_constants {
    void const         *data;
    u32                 offset;
    u32                 size;
} moon_push_constants;
static constexpr moon_push_constants MOON_PUSH_CONSTANTS_INIT = {0};

typedef struct moon_set_index_buffer {
    moon_buffer_id      buffer;
    u64                 offset;
    moon_index_format   index_format;
} moon_set_index_buffer;
static constexpr moon_set_index_buffer MOON_SET_INDEX_BUFFER_INIT = {
    .index_format = moon_index_format_u32,
};

typedef struct moon_barriers_and_transitions {
    u32                                 barrier_count;
    u32                                 transition_count;
    moon_memory_barrier const          *barriers;
    moon_texture_memory_barrier const  *transitions;
} moon_barriers_and_transitions;
static constexpr moon_barriers_and_transitions MOON_BARRIERS_AND_TRANSITIONS_INIT = {0};

typedef struct moon_render_attachment {
    moon_texture_view_id        texture_view;
    moon_texture_view_id        resolve_texture_view;
    moon_texture_layout         texture_layout;
    moon_texture_layout         resolve_texture_layout;
    moon_resolve_mode           resolve_mode;
    moon_load_op                load_op;
    moon_store_op               store_op;
    bool                        has_resolve;
    moon_color_value            clear_value;
} moon_render_attachment;
static constexpr moon_render_attachment MOON_RENDER_ATTACHMENT_INIT = {
    .texture_layout = moon_texture_layout_attachment_optimal,
    .resolve_texture_layout = moon_texture_layout_attachment_optimal,
    .resolve_mode = moon_resolve_mode_average,
    .load_op = moon_load_op_dont_care,
    .store_op = moon_store_op_store,
    .has_resolve = false,
};

typedef struct moon_begin_renderpass {
    moon_render_attachment      color_attachments[MOON_MAX_COLOR_ATTACHMENTS];
    u32                         color_attachment_count;
    bool                        has_depth_attachment;
    bool                        has_stencil_attachment;
    moon_render_attachment      depth_attachment;
    moon_render_attachment      stencil_attachment;
    lake_rect2d                 render_area;
} moon_begin_renderpass;
static constexpr moon_begin_renderpass MOON_BEGIN_RENDERPASS_INIT = {
    .color_attachments = {
        MOON_RENDER_ATTACHMENT_INIT,
        MOON_RENDER_ATTACHMENT_INIT,
        MOON_RENDER_ATTACHMENT_INIT,
        MOON_RENDER_ATTACHMENT_INIT,
        MOON_RENDER_ATTACHMENT_INIT,
        MOON_RENDER_ATTACHMENT_INIT,
        MOON_RENDER_ATTACHMENT_INIT,
        MOON_RENDER_ATTACHMENT_INIT,
    },
    .color_attachment_count = 1,
    .has_depth_attachment = false,
    .has_stencil_attachment = false,
    .depth_attachment = MOON_RENDER_ATTACHMENT_INIT,
    .stencil_attachment = MOON_RENDER_ATTACHMENT_INIT,
};

typedef struct moon_write_timestamps {
    moon_timeline_query_pool    timeline_query_pool;
    u32                         query_index;
    moon_pipeline_stage_bits    stage;
} moon_write_timestamps;
static constexpr moon_write_timestamps MOON_WRITE_TIMESTAMPS_INIT = {0};

typedef struct moon_resolve_timestamps {
    moon_timeline_query_pool    timeline_query_pool;
    u32                         start_index;
    u32                         count;
} moon_resolve_timestamps;
static constexpr moon_resolve_timestamps MOON_RESOLVE_TIMESTAMPS_INIT = {0};

typedef struct moon_signal_event {
    moon_event                          event;
    usize                               memory_barrier_count;
    moon_memory_barrier const          *memory_barriers;
    usize                               texture_memory_barrier_count;
    moon_texture_memory_barrier const  *texture_memory_barriers;
} moon_signal_event;
typedef moon_signal_event moon_wait_event;

typedef struct moon_reset_event {
    moon_event                  event;
    moon_pipeline_stage_bits    stage;
} moon_reset_event;
static constexpr moon_reset_event MOON_RESET_EVENT_INIT = {
    .stage = moon_pipeline_stage_bottom_of_pipe,
};

typedef struct moon_begin_label {
    vec4                        label_color;
    char const                 *label_name;
} moon_begin_label;
static constexpr moon_begin_label MOON_BEGIN_LABEL_INIT = {0};

/** Compatible with `moon_draw_mesh_tasks`. */
typedef struct moon_dispatch {
    u32                         group_x, group_y, group_z;
} moon_dispatch;

typedef struct moon_dispatch_indirect {
    moon_buffer_id              indirect_buffer;
    u64                         offset;
} moon_dispatch_indirect;
static constexpr moon_dispatch_indirect MOON_DISPATCH_INDIRECT_INIT = {0};

typedef struct moon_dispatch_node {
    u32                         node_index;
    u32                         payload_count;
    moon_device_or_host_address payloads; /**< Either `moon_dispatch` or `moon_draw_mesh_tasks`. */
    u64                         payload_stride;
} moon_dispatch_node;

typedef struct moon_dispatch_graph {
    u32                         node_count;
    moon_device_or_host_address nodes;
    u64                         node_stride;
} moon_dispatch_graph;

typedef struct moon_dispatch_graph_indirect {
    moon_device_address         scratch;
    u64                         scratch_size;
    moon_dispatch_graph         graph;
} moon_dispatch_graph_indirect;
static constexpr moon_dispatch_graph_indirect MOON_DISPATCH_GRAPH_INIT = {0};
static constexpr moon_dispatch_graph_indirect MOON_DISPATCH_GRAPH_INDIRECT_INIT = {0};

typedef struct moon_cmd_dispatch_graph_indirect_count {
    moon_device_address         scratch;
    u64                         scratch_size;
    moon_device_address         graph;
} moon_dispatch_graph_indirect_count;
static constexpr moon_dispatch_graph_indirect_count MOON_DISPATCH_GRAPH_INDIRECT_COUNT_INIT = {0};

typedef struct moon_ray_trace {
    u32                         width, height, depth;
} moon_ray_trace;

typedef struct moon_trace_rays {
    u32                         raygen_shader_binding_table_offset;
    u32                         miss_shader_binding_table_offset;
    u32                         miss_shader_binding_table_stride;
    u32                         hit_shader_binding_table_offset;
    moon_shader_binding_table   shader_binding_table;
    moon_ray_trace              payload;
} moon_trace_rays;
static constexpr moon_trace_rays MOON_TRACE_RAYS_INIT = {0};

typedef struct moon_trace_rays_indirect {
    moon_device_address         indirect_buffer_address;
    moon_shader_binding_table   shader_binding_table;
} moon_trace_rays_indirect;
static constexpr moon_trace_rays_indirect MOON_TRACE_RAYS_INDIRECT_INIT = {0};

typedef struct moon_draw {
    u32                         vertex_count;
    u32                         instance_count;
    u32                         first_vertex;
    u32                         first_instance;
} moon_draw;

typedef struct moon_draw_indexed {
    u32                         index_count;
    u32                         instance_count;
    u32                         first_index;
    s32                         vertex_offset;
    u32                         first_instance;
} moon_draw_indexed;

typedef struct moon_draw_indirect {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    u32                         draw_count;
    u32                         draw_stride;
    bool                        is_indexed;
} moon_draw_indirect;
static constexpr moon_draw_indirect MOON_DRAW_INDIRECT_INIT = {0};

typedef struct moon_draw_indirect_count {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    moon_buffer_id              count_buffer;
    u64                         count_buffer_offset;
    u32                         max_draw_count;
    u32                         draw_stride;
    bool                        is_indexed;
} moon_draw_indirect_count;
static constexpr moon_draw_indirect_count MOON_DRAW_INDIRECT_COUNT_INIT = {0};

/** Compatible with `moon_dispatch`. */
typedef struct moon_draw_mesh_tasks {
    u32                         group_x, group_y, group_z;
} moon_draw_mesh_tasks;

typedef struct moon_draw_mesh_tasks_indirect {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    u32                         draw_count;
    u32                         draw_stride;
} moon_draw_mesh_tasks_indirect;
static constexpr moon_draw_mesh_tasks_indirect MOON_DRAW_MESH_TASKS_INDIRECT_INIT = {0};

typedef struct moon_draw_mesh_tasks_indirect_count {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    moon_buffer_id              count_buffer;
    u64                         count_buffer_offset;
    u32                         max_draw_count;
    u32                         draw_stride;
} moon_draw_mesh_tasks_indirect_count;
static constexpr moon_draw_mesh_tasks_indirect_count MOON_DRAW_MESH_TASKS_INDIRECT_COUNT_INIT = {0};

typedef struct moon_command_recorder_assembly {
    moon_queue_type             queue_type;
    lake_small_string           name;
} moon_command_recorder_assembly;

typedef struct moon_staged_command_list_assembly {
    lake_small_string           name;
} moon_staged_command_list_assembly;

/** Assemble a command recorder. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_command_recorder_assembly)(moon_device device, moon_command_recorder_assembly const *assembly, moon_command_recorder *out_cmd);
#define FN_MOON_COMMAND_RECORDER_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_command_recorder_assembly(moon_device device, moon_command_recorder_assembly const *assembly, moon_command_recorder *out_cmd)

/** Destroy a command recorder. */
PFN_LAKE_WORK(PFN_moon_command_recorder_zero_refcnt, moon_command_recorder cmd);
#define FN_MOON_COMMAND_RECORDER_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_command_recorder_zero_refcnt, moon_command_recorder cmd)

/** Assemble a staged command list from a command recorder. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_staged_command_list_assembly)(moon_command_recorder cmd, moon_staged_command_list_assembly const *assembly, moon_staged_command_list *out_cmd_list);
#define FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_staged_command_list_assembly(moon_command_recorder cmd, moon_staged_command_list_assembly const *assembly, moon_staged_command_list *out_cmd_list)

/** Destroy a staged command list. */
PFN_LAKE_WORK(PFN_moon_staged_command_list_zero_refcnt, moon_staged_command_list cmd_list);
#define FN_MOON_STAGED_COMMAND_LIST_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_staged_command_list_zero_refcnt, moon_staged_command_list cmd_list)

/** Copy data between buffer regions. */
PFN_MOON_COMMAND_NO_DISCARD(copy_buffer, moon_copy_buffer const *work);
#define FN_MOON_CMD_COPY_BUFFER(backend) \
    FN_MOON_COMMAND_NO_DISCARD(copy_buffer, backend, moon_copy_buffer const *work)

/** Copy data between source buffer and destination texture regions. */
PFN_MOON_COMMAND_NO_DISCARD(copy_buffer_to_texture, moon_copy_buffer_and_texture const *work);
#define FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(copy_buffer_to_texture, backend, moon_copy_buffer_and_texture const *work)

/** Copy data between source buffer and destination texture regions. */
PFN_MOON_COMMAND_NO_DISCARD(copy_texture_to_buffer, moon_copy_buffer_and_texture const *work);
#define FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(backend) \
    FN_MOON_COMMAND_NO_DISCARD(copy_texture_to_buffer, backend, moon_copy_buffer_and_texture const *work)

/** Copy regions between textures. */
PFN_MOON_COMMAND_NO_DISCARD(copy_texture, moon_copy_texture const *work);
#define FN_MOON_CMD_COPY_TEXTURE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(copy_texture, backend, moon_copy_texture const *work)

/** Copy regions of source texture into a destination texture, potentially performing 
 *  format conversion, arbitrary scaling, and filtering. */
PFN_MOON_COMMAND_NO_DISCARD(blit_texture, moon_blit_texture const *work);
#define FN_MOON_CMD_BLIT_TEXTURE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(blit_texture, backend, moon_blit_texture const *work)

/** Resolve regions of a multisample color image to a non-multisample color image. */
PFN_MOON_COMMAND_NO_DISCARD(resolve_texture, moon_resolve_texture const *work);
#define FN_MOON_CMD_RESOLVE_TEXTURE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(resolve_texture, backend, moon_resolve_texture const *work)

/** Fill a region of a buffer with a fixed value, used to zero out a buffer. */
PFN_MOON_COMMAND_NO_DISCARD(clear_buffer, moon_clear_buffer const *work);
#define FN_MOON_CMD_CLEAR_BUFFER(backend) \
    FN_MOON_COMMAND_NO_DISCARD(clear_buffer, backend, moon_clear_buffer const *work)

/** Fill a region of a texture with a fixed value, used to zero out a texture. */
PFN_MOON_COMMAND_NO_DISCARD(clear_texture, moon_clear_texture const *work);
#define FN_MOON_CMD_CLEAR_TEXTURE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(clear_texture, backend, moon_clear_texture const *work)

/** Build or update bottom-level and top-level acceleration structures. */
PFN_MOON_COMMAND_NO_DISCARD(build_acceleration_structures, moon_build_acceleration_structures const *work);
#define FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(backend) \
    FN_MOON_COMMAND_NO_DISCARD(build_acceleration_structures, backend, moon_build_acceleration_structures const *work)

/** Destroys a buffer after the GPU is finished executing the command list. */
PFN_MOON_COMMAND_NO_DISCARD(destroy_buffer_deferred, moon_buffer_id buffer);
#define FN_MOON_CMD_DESTROY_BUFFER_DEFERRED(backend) \
    FN_MOON_COMMAND_NO_DISCARD(destroy_buffer_deferred, backend, moon_buffer_id buffer)

/** Destroys a texture after the GPU is finished executing the command list. */
PFN_MOON_COMMAND_NO_DISCARD(destroy_texture_deferred, moon_texture_id texture);
#define FN_MOON_CMD_DESTROY_TEXTURE_DEFERRED(backend) \
    FN_MOON_COMMAND_NO_DISCARD(destroy_texture_deferred, backend, moon_texture_id texture)

/** Destroys a texture view after the GPU is finished executing the command list. */
PFN_MOON_COMMAND_NO_DISCARD(destroy_texture_view_deferred, moon_texture_view_id texture_view);
#define FN_MOON_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(backend) \
    FN_MOON_COMMAND_NO_DISCARD(destroy_texture_view_deferred, backend, moon_texture_view_id texture_view)

/** Destroys a sampler after the GPU is finished executing the command list. */
PFN_MOON_COMMAND_NO_DISCARD(destroy_sampler_deferred, moon_sampler_id sampler);
#define FN_MOON_CMD_DESTROY_SAMPLER_DEFERRED(backend) \
    FN_MOON_COMMAND_NO_DISCARD(destroy_sampler_deferred, backend, moon_sampler_id sampler)

/** Push a range of constant data to shader stages. */
PFN_MOON_COMMAND_NO_DISCARD(push_constants, moon_push_constants const *work);
#define FN_MOON_CMD_PUSH_CONSTANTS(backend) \
    FN_MOON_COMMAND_NO_DISCARD(push_constants, backend, moon_push_constants const *work)

/** Bind compute pipeline for dispatch commands. */
PFN_MOON_COMMAND_NO_DISCARD(set_compute_pipeline, moon_compute_pipeline pipeline);
#define FN_MOON_CMD_SET_COMPUTE_PIPELINE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(set_compute_pipeline, backend, moon_compute_pipeline pipeline)

/** Bind ray tracing pipeline for trace commands. */
PFN_MOON_COMMAND_NO_DISCARD(set_ray_tracing_pipeline, moon_ray_tracing_pipeline pipeline);
#define FN_MOON_CMD_SET_RAY_TRACING_PIPELINE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(set_ray_tracing_pipeline, backend, moon_ray_tracing_pipeline pipeline)

/** Bind work graph pipeline for dispatch graph commands. */
PFN_MOON_COMMAND_NO_DISCARD(set_work_graph_pipeline, moon_work_graph_pipeline pipeline);
#define FN_MOON_CMD_SET_WORK_GRAPH_PIPELINE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(set_work_graph_pipeline, backend, moon_work_graph_pipeline pipeline)

/** Bind raster pipeline for draw commands. */
PFN_MOON_COMMAND_NO_DISCARD(set_raster_pipeline, moon_raster_pipeline pipeline);
#define FN_MOON_CMD_SET_RASTER_PIPELINE(backend) \
    FN_MOON_COMMAND_NO_DISCARD(set_raster_pipeline, backend, moon_raster_pipeline pipeline)

/** Configure viewport transformation dimensions. */
PFN_MOON_COMMAND(set_viewport, lake_viewport const *viewport);
#define FN_MOON_CMD_SET_VIEWPORT(backend) \
    FN_MOON_COMMAND(set_viewport, backend, lake_viewport const *viewport)

/** Configure scissor rectangle for fragment clipping. */
PFN_MOON_COMMAND(set_scissor, lake_rect2d const *scissor);
#define FN_MOON_CMD_SET_SCISSOR(backend) \
    FN_MOON_COMMAND(set_scissor, backend, lake_rect2d const *scissor)

/** Configure depth bias constants for depth calculations. */
PFN_MOON_COMMAND(set_depth_bias, moon_depth_bias const *work);
#define FN_MOON_CMD_SET_DEPTH_BIAS(backend) \
    FN_MOON_COMMAND(set_depth_bias, backend, moon_depth_bias const *work)

/** Configure index buffer for indexed draw calls. */
PFN_MOON_COMMAND_NO_DISCARD(set_index_buffer, moon_set_index_buffer const *work);
#define FN_MOON_CMD_SET_INDEX_BUFFER(backend) \
    FN_MOON_COMMAND_NO_DISCARD(set_index_buffer, backend, moon_set_index_buffer const *work)

/** Configure number of rasterization samples for raster pipelines. */
PFN_MOON_COMMAND_NO_DISCARD(set_rasterization_samples, moon_sample_count sample_count);
#define FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(backend) \
    FN_MOON_COMMAND_NO_DISCARD(set_rasterization_samples, backend, moon_sample_count sample_count)

/** Insert pipeline memory barriers and texture barriers for layout transitions. Successive barriers_and_transitions 
 *  calls are combined, as soon as any other command is recorder, the barrier batch is flushed. */
PFN_MOON_COMMAND_NO_DISCARD(barriers_and_transitions, moon_barriers_and_transitions const *work);
#define FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(backend) \
    FN_MOON_COMMAND_NO_DISCARD(barriers_and_transitions, backend, moon_barriers_and_transitions const *work)

/** Start a renderpass with specified render targets for draw commands. */
PFN_MOON_COMMAND_NO_DISCARD(begin_renderpass, moon_begin_renderpass const *work);
#define FN_MOON_CMD_BEGIN_RENDERPASS(backend) \
    FN_MOON_COMMAND_NO_DISCARD(begin_renderpass, backend, moon_begin_renderpass const *work)

/** End the current renderpass. */
PFN_MOON_COMMAND_NO_ARGS(end_renderpass);
#define FN_MOON_CMD_END_RENDERPASS(backend) \
    FN_MOON_COMMAND_NO_ARGS(end_renderpass, backend)

/** Record timestamp values into a query pool. */
PFN_MOON_COMMAND(write_timestamps, moon_write_timestamps const *work);
#define FN_MOON_CMD_WRITE_TIMESTAMPS(backend) \
    FN_MOON_COMMAND(write_timestamps, backend, moon_write_timestamps const *work)

/** Reset timestamp values in the query pool. */
PFN_MOON_COMMAND(resolve_timestamps, moon_resolve_timestamps const *work);
#define FN_MOON_CMD_RESOLVE_TIMESTAMPS(backend) \
    FN_MOON_COMMAND(resolve_timestamps, backend, moon_resolve_timestamps const *work)

/** Signal an event. */
PFN_MOON_COMMAND(signal_event, moon_signal_event const *work);
#define FN_MOON_CMD_SIGNAL_EVENT(backend) \
    FN_MOON_COMMAND(signal_event, backend, moon_signal_event const *work)

/** Wait on a list of events. */
PFN_MOON_COMMAND(wait_on_events, u32 work_count, moon_wait_event const *work);
#define FN_MOON_CMD_WAIT_ON_EVENTS(backend) \
    FN_MOON_COMMAND(wait_on_events, backend, u32 work_count, moon_wait_event const *work)

/** Reset an event. */
PFN_MOON_COMMAND(reset_event, moon_reset_event const *work);
#define FN_MOON_CMD_RESET_EVENT(backend) \
    FN_MOON_COMMAND(reset_event, backend, moon_reset_event const *work)

/** Begin a debug label. */
PFN_MOON_COMMAND(begin_label, moon_begin_label const *work);
#define FN_MOON_CMD_BEGIN_LABEL(backend) \
    FN_MOON_COMMAND(begin_label, backend, moon_begin_label const *work)

/** End a debug label. */
PFN_MOON_COMMAND_NO_ARGS(end_label);
#define FN_MOON_CMD_END_LABEL(backend) \
    FN_MOON_COMMAND_NO_ARGS(end_label, backend)

/** Execute compute workgroups with direct parameters. */
PFN_MOON_COMMAND_NO_DISCARD(dispatch, moon_dispatch const *work);
#define FN_MOON_CMD_DISPATCH(backend) \
    FN_MOON_COMMAND_NO_DISCARD(dispatch, backend, moon_dispatch const *work)

/** Execute compute workgroups with indirect parameters buffer. */
PFN_MOON_COMMAND_NO_DISCARD(dispatch_indirect, moon_dispatch_indirect const *work);
#define FN_MOON_CMD_DISPATCH_INDIRECT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(dispatch_indirect, backend, moon_dispatch_indirect const *work)

/** Execute dispatch nodes with direct parameters. */
PFN_MOON_COMMAND_NO_DISCARD(dispatch_graph, moon_dispatch_graph_indirect const *work);
#define FN_MOON_CMD_DISPATCH_GRAPH(backend) \
    FN_MOON_COMMAND_NO_DISCARD(dispatch_graph, backend, moon_dispatch_graph_indirect const *work)

/** Execute dispatch nodes and payload parameters read on the device. */
PFN_MOON_COMMAND_NO_DISCARD(dispatch_graph_indirect, moon_dispatch_graph_indirect const *work);
#define FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(dispatch_graph_indirect, backend, moon_dispatch_graph_indirect const *work)

/** Execute dispatch nodes with all parameters read on the device. */
PFN_MOON_COMMAND_NO_DISCARD(dispatch_graph_indirect_count, moon_dispatch_graph_indirect_count const *work);
#define FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(dispatch_graph_indirect_count, backend, moon_dispatch_graph_indirect_count const *work)

/** Execute ray tracing workload with direct parameters. */
PFN_MOON_COMMAND_NO_DISCARD(trace_rays, moon_trace_rays const *work);
#define FN_MOON_CMD_TRACE_RAYS(backend) \
    FN_MOON_COMMAND_NO_DISCARD(trace_rays, backend, moon_trace_rays const *work)

/** Execute ray tracing workload using indirect parameters buffer. */
PFN_MOON_COMMAND_NO_DISCARD(trace_rays_indirect, moon_trace_rays_indirect const *work);
#define FN_MOON_CMD_TRACE_RAYS_INDIRECT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(trace_rays_indirect, backend, moon_trace_rays_indirect const *work)

/** Draw non-indexed primitives with vertex count. */
PFN_MOON_COMMAND(draw, moon_draw const *work);
#define FN_MOON_CMD_DRAW(backend) \
    FN_MOON_COMMAND(draw, backend, moon_draw const *work)

/** Draw indexed primitives using bound index buffer. */
PFN_MOON_COMMAND(draw_indexed, moon_draw_indexed const *work);
#define FN_MOON_CMD_DRAW_INDEXED(backend) \
    FN_MOON_COMMAND(draw_indexed, backend, moon_draw_indexed const *work)

/** Draw primitives using indirect parameters buffer. */
PFN_MOON_COMMAND_NO_DISCARD(draw_indirect, moon_draw_indirect const *work);
#define FN_MOON_CMD_DRAW_INDIRECT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(draw_indirect, backend, moon_draw_indirect const *work)

/** Draw primitives using indirect parameters buffer with device count argument. */
PFN_MOON_COMMAND_NO_DISCARD(draw_indirect_count, moon_draw_indirect_count const *work);
#define FN_MOON_CMD_DRAW_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(draw_indirect_count, backend, moon_draw_indirect_count const *work)

/** Execute mesh shader workgroups with direct parameters. */
PFN_MOON_COMMAND(draw_mesh_tasks, moon_draw_mesh_tasks const *work);
#define FN_MOON_CMD_DRAW_MESH_TASKS(backend) \
    FN_MOON_COMMAND(draw_mesh_tasks, backend, moon_draw_mesh_tasks const *work)

/** Execute mesh shader workgroups using indirect parameters buffer. */
PFN_MOON_COMMAND_NO_DISCARD(draw_mesh_tasks_indirect, moon_draw_mesh_tasks_indirect const *work);
#define FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(draw_mesh_tasks_indirect, backend, moon_draw_mesh_tasks_indirect const *work)

/** Execute mesh shader workgroups using indirect parameters buffer with deferred count argument. */
PFN_MOON_COMMAND_NO_DISCARD(draw_mesh_tasks_indirect_count, moon_draw_mesh_tasks_indirect_count const *work);
#define FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND_NO_DISCARD(draw_mesh_tasks_indirect_count, backend, moon_draw_mesh_tasks_indirect_count const *work)

/** Header for `struct moon_command_recorder_impl`. */
typedef struct moon_command_recorder_header {
    moon_device                     device;
    atomic_u32                      flags;
    lake_refcnt                     refcnt;
    moon_command_recorder_assembly  assembly;
} moon_command_recorder_header;

/** Header for `struct moon_staged_command_list_impl`. */
typedef struct moon_staged_command_list_header {
    moon_command_recorder           cmd;
    atomic_u32                      flags;
    lake_refcnt                     refcnt;
    moon_command_recorder_assembly  assembly;
} moon_staged_command_list_header;
