#pragma once

/** @file lake/renderer/device.h
 *  @brief A device is used to allocate GPU resources and submit work.
 *
 *  TODO docs
 */
#include <lake/renderer/moon_adapter.h>
#include <lake/renderer/timeline_sync.h>

typedef enum moon_device_type : s16 {
    moon_device_type_other = 0,
    moon_device_type_integrated_gpu,
    moon_device_type_discrete_gpu,
    moon_device_type_virtual_gpu,
    moon_device_type_cpu,
} moon_device_type;

#define MOON_MAX_COMPUTE_QUEUE_COUNT            8
#define MOON_MAX_TRANSFER_QUEUE_COUNT           2
#define MOON_MAX_SPARSE_BINDING_QUEUE_COUNT     1
#define MOON_MAX_VIDEO_DECODE_QUEUE_COUNT       1
#define MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT       1

#define MOON_QUEUE_MAIN_BEGIN_INDEX             0
#define MOON_QUEUE_COMPUTE_BEGIN_INDEX          1
#define MOON_QUEUE_TRANSFER_BEGIN_INDEX         (MOON_QUEUE_COMPUTE_BEGIN_INDEX + MOON_MAX_COMPUTE_QUEUE_COUNT)
#define MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX   (MOON_QUEUE_TRANSFER_BEGIN_INDEX + MOON_MAX_TRANSFER_QUEUE_COUNT)
#define MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX     (MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX + MOON_MAX_SPARSE_BINDING_QUEUE_COUNT)
#define MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX     (MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX + MOON_MAX_VIDEO_DECODE_QUEUE_COUNT)
#define MOON_QUEUE_INDEX_COUNT                  (MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX + MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT)

/** Queue types used for scheduling different type of GPU work. */
typedef s8 moon_queue_mask;
typedef enum moon_queue_type : moon_queue_mask {
    moon_queue_type_main,
    moon_queue_type_compute,
    moon_queue_type_transfer,
    moon_queue_type_sparse_binding,
    moon_queue_type_video_decode,
    moon_queue_type_video_encode,
    moon_queue_type_count,
    moon_queue_type_none = -1,
} moon_queue_type;

typedef struct moon_queue {
    moon_queue_type type;
    s8              idx;
} moon_queue;

static constexpr moon_queue MOON_QUEUE_MAIN = {moon_queue_type_main, MOON_QUEUE_MAIN_BEGIN_INDEX};
static constexpr moon_queue MOON_QUEUE_COMPUTE_0 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 0};
static constexpr moon_queue MOON_QUEUE_COMPUTE_1 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 1};
static constexpr moon_queue MOON_QUEUE_COMPUTE_2 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 2};
static constexpr moon_queue MOON_QUEUE_COMPUTE_3 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 3};
static constexpr moon_queue MOON_QUEUE_COMPUTE_4 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 4};
static constexpr moon_queue MOON_QUEUE_COMPUTE_5 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 5};
static constexpr moon_queue MOON_QUEUE_COMPUTE_6 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 6};
static constexpr moon_queue MOON_QUEUE_COMPUTE_7 = {moon_queue_type_compute, MOON_QUEUE_COMPUTE_BEGIN_INDEX + 7};
static constexpr moon_queue MOON_QUEUE_TRANSFER_0 = {moon_queue_type_transfer, MOON_QUEUE_TRANSFER_BEGIN_INDEX + 0};
static constexpr moon_queue MOON_QUEUE_TRANSFER_1 = {moon_queue_type_transfer, MOON_QUEUE_TRANSFER_BEGIN_INDEX + 1};
static constexpr moon_queue MOON_QUEUE_SPARSE_BINDING = {moon_queue_type_sparse_binding, MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX};
static constexpr moon_queue MOON_QUEUE_VIDEO_DECODE = {moon_queue_type_video_decode, MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX};
static constexpr moon_queue MOON_QUEUE_VIDEO_ENCODE = {moon_queue_type_video_encode, MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX};

/* Is ABI compatible with Vulkan's VkPhysicalDeviceLimits. */
typedef struct moon_device_limits {
    u32 max_texture_dimension_1d;
    u32 max_texture_dimension_2d;
    u32 max_texture_dimension_3d;
    u32 max_texture_dimension_cube;
    u32 max_texture_array_layers;
    u32 max_texel_buffer_elements;
    u32 max_uniform_buffer_range;
    u32 max_storage_buffer_range;
    u32 max_push_constants_size;
    u32 max_memory_allocation_count;
    u32 max_sampler_allocation_count;
    u64 buffer_texture_granularity;
    u64 sparse_address_space_size;
    u32 max_bound_descriptor_sets;
    u32 max_per_stage_descriptor_samplers;
    u32 max_per_stage_descriptor_uniform_buffers;
    u32 max_per_stage_descriptor_storage_buffers;
    u32 max_per_stage_descriptor_sampled_textures;
    u32 max_per_stage_descriptor_storage_textures;
    u32 max_per_stage_descriptor_input_attachments;
    u32 max_per_stage_resources;
    u32 max_descriptor_set_samplers;
    u32 max_descriptor_set_uniformbuffers;
    u32 max_descriptor_set_uniform_buffers_dynamic;
    u32 max_descriptor_set_storage_buffers;
    u32 max_descriptor_set_storage_buffers_dynamic;
    u32 max_descriptor_set_sampled_textures;
    u32 max_descriptor_set_storage_textures;
    u32 max_descriptor_set_input_attachments;
    u32 max_vertex_input_attributes;
    u32 max_vertex_input_bindings;
    u32 max_vertex_input_attribute_offset;
    u32 max_vertex_input_binding_stride;
    u32 max_vertex_output_components;
    u32 max_tessellation_generation_level;
    u32 max_tessellation_patch_size;
    u32 max_tessellation_control_per_vertex_input_components;
    u32 max_tessellation_control_per_vertex_output_components;
    u32 max_tessellation_control_per_patch_output_components;
    u32 max_tessellation_control_total_output_components;
    u32 max_tessellation_evaluation_input_components;
    u32 max_tessellation_evaluation_output_components;
    u32 max_geometry_shader_invocations;
    u32 max_geometry_input_components;
    u32 max_geometry_output_components;
    u32 max_geometry_output_vertices;
    u32 max_geometry_total_output_components;
    u32 max_fragment_input_components;
    u32 max_fragment_output_attachments;
    u32 max_fragment_dual_src_attachments;
    u32 max_fragment_combined_output_resources;
    u32 max_compute_shared_memory_size;
    u32 max_compute_work_group_count[3];
    u32 max_compute_work_group_invocations;
    u32 max_compute_work_group_size[3];
    u32 sub_pixel_precision_bits;
    u32 sub_texel_precision_bits;
    u32 mipmap_precision_bits;
    u32 max_draw_indexed_index_value;
    u32 max_draw_indirect_count;
    f32 max_sampler_lod_bias;
    f32 max_sampler_anisotropy;
    u32 max_viewports;
    u32 max_viewport_dimensions[2];
    f32 viewport_bounds_range[2];
    u32 viewport_subpixel_bits;
    usize min_memory_map_alignment;
    u64 min_texel_buffer_offset_alignment;
    u64 min_uniform_buffer_offset_alignment;
    u64 min_storage_buffer_offset_alignment;
    s32 min_texel_offset;
    u32 max_texel_offset;
    s32 min_texel_gather_offset;
    u32 max_texel_gather_offset;
    f32 min_interpolation_offset;
    f32 max_interpolation_offset;
    u32 sub_pixel_interpolation_offset_bits;
    u32 max_framebuffer_width;
    u32 max_framebuffer_height;
    u32 max_framebuffer_layers;
    moon_sample_count framebuffer_color_sample_counts;
    moon_sample_count framebuffer_depth_sample_counts;
    moon_sample_count framebuffer_stencil_sample_counts;
    moon_sample_count framebuffer_no_attachments_sample_counts;
    u32 max_color_attachments;
    moon_sample_count sampled_texture_color_sample_counts;
    moon_sample_count sampled_texture_integer_sample_counts;
    moon_sample_count sampled_texture_depth_sample_counts;
    moon_sample_count sampled_texture_stencil_sample_counts;
    moon_sample_count storage_texture_sample_counts;
    u32 max_sample_mask_words;
    s32 timestamp_compute_and_graphics; /* bool */
    f32 timestamp_period;
    u32 max_clip_distances;
    u32 max_cull_distances;
    u32 max_combined_clip_and_cull_distances;
    u32 discrete_queue_priorities;
    f32 point_size_range[2];
    f32 line_width_range[2];
    f32 point_size_granularity;
    f32 line_width_granularity;
    s32 strict_lines; /* bool */
    s32 standard_sample_locations; /* bool */
    u64 optimal_buffer_copy_offset_alignment;
    u64 optimal_buffer_copy_row_pitch_alignment;
    u64 non_coherent_atom_size;
} moon_device_limits;

typedef struct moon_device_ray_tracing_pipeline_details {
    u32 shader_group_handle_size;
    u32 shader_group_handle_capture_replay_size;
    u32 shader_group_handle_alignment;
    u32 shader_group_base_alignment;
    u32 max_shader_group_stride;
    u32 max_ray_recursion_depth;
    u32 max_ray_dispatch_invocation_count;
    u32 max_ray_hit_attribute_size;
    u32 invocation_reorder_hint;
} moon_device_ray_tracing_pipeline_details;

typedef struct moon_device_acceleration_structure_details {
    u64 max_geometry_count;
    u64 max_instance_count;
    u64 max_primitive_count;
    u32 max_per_stage_descriptor_acceleration_structures;
    u32 max_per_stage_descriptor_update_after_bind_acceleration_structures;
    u32 max_descriptor_set_acceleration_structures;
    u32 max_descriptor_set_update_after_bind_acceleration_structures;
    u32 min_acceleration_structure_scratch_offset_alignment;
    bool has_invocation_reorder;
} moon_device_acceleration_structure_details;

typedef struct moon_device_mesh_shader_details {
    u32 mesh_output_per_vertex_granularity;
    u32 mesh_output_per_primitive_granularity;
    u32 max_task_work_group_total_count;
    u32 max_task_work_group_count[3];
    u32 max_task_work_group_invocations;
    u32 max_task_work_group_size[3];
    u32 max_task_payload_size;
    u32 max_task_payload_and_shared_memory_size;
    u32 max_task_shared_memory_size;
    u32 max_mesh_work_group_total_count;
    u32 max_mesh_work_group_count[3];
    u32 max_mesh_work_group_invocations;
    u32 max_mesh_work_group_size[3];
    u32 max_mesh_payload_and_shared_memory_size;
    u32 max_mesh_payload_and_output_memory_size;
    u32 max_mesh_shared_memory_size;
    u32 max_mesh_output_memory_size;
    u32 max_mesh_output_components;
    u32 max_mesh_output_vertices;
    u32 max_mesh_output_primitives;
    u32 max_mesh_output_layers;
    u32 max_mesh_multiview_view_count;
    u32 max_preferred_task_work_group_invocations;
    u32 max_preferred_mesh_work_group_invocations;
    bool prefers_local_invocation_vertex_output;
    bool prefers_local_invocation_primitive_output;
    bool prefers_compact_vertex_output;
    bool prefers_compact_primitive_output;
} moon_device_mesh_shader_details;

typedef struct moon_device_work_graph_details {
    u32 work_graph_dispatch_address_alignment;
    u32 max_work_graph_depth;
    u32 max_work_graph_shader_output_nodes;
    u32 max_work_graph_shader_payload_size;
    u32 max_work_graph_shader_payload_count;
    u32 max_work_graph_workgroup_count[3];
    u32 max_work_graph_workgroups;
} moon_device_work_graph_details;

/** These features are required, a device is discarded if incompatibilities are unresolved. */
typedef u32 moon_missing_required_features;
typedef enum moon_missing_required_feature_bits : moon_missing_required_features {
    moon_missing_required_feature_none = 0u,
    moon_missing_required_feature_descriptor_indexing                   = (1u << 0), /* update after bind, runtime descriptor array */
    moon_missing_required_feature_buffer_device_address                 = (1u << 1), /* capture replay, multi device */
    moon_missing_required_feature_multi_draw_indirect                   = (1u << 2),
    moon_missing_required_feature_tessellation_shader                   = (1u << 3),
    moon_missing_required_feature_depth_clamp                           = (1u << 4),
    moon_missing_required_feature_sampler_anisotropy                    = (1u << 5),
    moon_missing_required_feature_framebuffer_local_dependencies        = (1u << 6), /* dynamic rendering, local read */
    moon_missing_required_feature_fragment_stores_and_atomics           = (1u << 7),
    moon_missing_required_feature_timeline_semaphore                    = (1u << 8),
    moon_missing_required_feature_texture_cube_array                    = (1u << 9),
    moon_missing_required_feature_shader_storage_texture                = (1u << 10), /* multisample, read/write without format */
    moon_missing_required_feature_shader_int64                          = (1u << 11),
    moon_missing_required_feature_fill_mode_wireframe                   = (1u << 12),
    moon_missing_required_feature_resolve_host_query_data               = (1u << 13),
    moon_missing_required_feature_subgroup_size_control                 = (1u << 14),
    moon_missing_required_feature_scalar_block_layout                   = (1u << 15),
    moon_missing_required_feature_independent_blend                     = (1u << 16),
    moon_missing_required_feature_variable_pointers                     = (1u << 17),
} moon_missing_required_feature_bits;

/** These features are always enabled if the device supports them. */
typedef u32 moon_implicit_features;
typedef enum moon_implicit_feature_bits : moon_implicit_features {
    moon_implicit_feature_none = 0u,
    moon_implicit_feature_mesh_shader                                   = (1u << 0),
    moon_implicit_feature_basic_ray_tracing                             = (1u << 1),
    moon_implicit_feature_ray_tracing_pipeline                          = (1u << 2),
    moon_implicit_feature_ray_tracing_invocation_reorder                = (1u << 3),
    moon_implicit_feature_ray_tracing_position_fetch                    = (1u << 4),
    moon_implicit_feature_conservative_rasterization                    = (1u << 5),
    moon_implicit_feature_work_graph                                    = (1u << 6),
    moon_implicit_feature_image_atomic64                                = (1u << 7),
    moon_implicit_feature_shader_atomic_float                           = (1u << 8),
    moon_implicit_feature_shader_atomic_int64                           = (1u << 9),
    moon_implicit_feature_shader_float16                                = (1u << 10),
    moon_implicit_feature_shader_int16                                  = (1u << 11),
    moon_implicit_feature_shader_int8                                   = (1u << 12),
    moon_implicit_feature_dynamic_state                                 = (1u << 13),
    moon_implicit_feature_sparse_binding                                = (1u << 14),
    moon_implicit_feature_swapchain                                     = (1u << 15),
} moon_implicit_feature_bits;

/** These features must be enabled explicitly by the user. */
typedef u32 moon_explicit_features;
typedef enum moon_explicit_feature_bits : moon_explicit_features {
    moon_explicit_feature_none = 0u,
    moon_explicit_feature_buffer_device_address_capture_replay          = (1u << 0),
    moon_explicit_feature_acceleration_structure_capture_replay         = (1u << 1),
    moon_explicit_feature_vulkan_memory_model                           = (1u << 2),
    moon_explicit_feature_robust_access                                 = (1u << 3),
    moon_explicit_feature_video_decode_queue                            = (1u << 4),
    moon_explicit_feature_video_encode_queue                            = (1u << 5),
    moon_explicit_feature_multiview_xr                                  = (1u << 6),
} moon_explicit_feature_bits;

typedef struct moon_device_details {
    u32                                         api_version;
    u32                                         driver_version;
    u16                                         vendor_id;
    u16                                         device_id;
    moon_device_type                            device_type;
    char                                        device_name[256];
    char                                        pipeline_cache_uuid[16];
    u16                                         heap_memory_type_count;
    moon_memory_heap_type                       heap_memory_types[MOON_MAX_MEMORY_HEAP_TYPES];
    u64                                         heap_sizes[MOON_MAX_MEMORY_HEAP_SIZES];
    u32                                         heap_count;
    u16                                         compute_queue_count;
    u16                                         transfer_queue_count;
    moon_device_work_graph_details              work_graph_details;
    moon_device_mesh_shader_details             mesh_shader_details;
    moon_device_acceleration_structure_details  acceleration_structure_details;
    moon_device_ray_tracing_pipeline_details    ray_tracing_pipeline_details;
    moon_device_limits                          limits;
    moon_missing_required_features              missing_required_features;
    moon_implicit_features                      implicit_features;
    moon_explicit_features                      explicit_features;
    u32                                         total_score;
} moon_device_details;

typedef struct moon_device_assembly {
    /** Index into a list of device details returned from `PFN_moon_list_device_details`. */
    s32                             device_idx;
    /** Explicit features of set bits will be enabled, otherwise they are ignored. */
    moon_explicit_features          explicit_features;
    /** User's device limits. A device may be discarded if it doesn't meet these requirements. */
    u32                             max_allowed_textures;
    u32                             max_allowed_buffers;
    u32                             max_allowed_samplers;
    u32                             max_allowed_acceleration_structures;
    lake_small_string               name;
} moon_device_assembly;
static constexpr moon_device_assembly MOON_DEVICE_ASSEMBLY_INIT = {
    .device_idx = -1,
    .explicit_features = moon_explicit_feature_buffer_device_address_capture_replay,
    .max_allowed_textures = 10000,
    .max_allowed_buffers = 10000,
    .max_allowed_samplers = 400,
    .max_allowed_acceleration_structures = 10000,
};

/** Describes a submit. */
typedef struct moon_device_submit {
    moon_queue                      queue;
    moon_pipeline_stages            wait_stages;
    moon_staged_command_list const *staged_command_lists;
    usize                           staged_command_list_count;
    moon_binary_semaphore const    *wait_binary_semaphores;
    usize                           wait_binary_semaphore_count;
    moon_binary_semaphore const    *signal_binary_semaphores;
    usize                           signal_binary_semaphore_count;
    moon_timeline_pair const       *wait_timeline_semaphores;
    usize                           wait_timeline_semaphore_count;
    moon_timeline_pair const       *signal_timeline_semaphores;
    usize                           signal_timeline_semaphore_count;
} moon_device_submit;
static constexpr moon_device_submit MOON_DEVICE_SUBMIT_INIT = {0};

/** Describes a present. */
typedef struct moon_device_present {
    moon_binary_semaphore const    *wait_binary_semaphores;
    moon_swapchain const           *swapchains;
    u32                             wait_binary_semaphore_count;
    u16                             swapchain_count;
    moon_queue                      queue;
} moon_device_present;
static constexpr moon_device_present MOON_DEVICE_PRESENT_INIT = {0};

typedef struct moon_memory_heap_assembly {
    moon_memory_requirements        requirements;
    moon_memory_flags               flags;
} moon_memory_heap_assembly;
static constexpr moon_memory_heap_assembly MOON_MEMORY_HEAP_ASSEMBLY_INIT = {0};

typedef struct moon_memory_buffer_size_pair {
    moon_buffer_id                  buffer;
    u64                             size : 63;
    u64                             heap_allocated : 1;
} moon_memory_buffer_size_pair;

typedef struct moon_memory_texture_size_pair {
    moon_texture_id                 texture;
    u64                             size : 63;
    u64                             heap_allocated : 1;
} moon_memory_texture_size_pair;

typedef struct moon_memory_tlas_size_pair {
    moon_tlas_id                    tlas;
    u64                             size;
} moon_memory_tlas_size_pair;

typedef struct moon_memory_blas_size_pair {
    moon_blas_id                    blas;
    u64                             size;
} moon_memory_blas_size_pair;

typedef struct moon_memory_heap_size_pair {
    moon_memory_heap                heap;
    u64                             size;
} moon_memory_heap_size_pair;

typedef struct moon_memory_report {
    u64                             total_memory_use;
    u64                             total_buffer_memory_use;
    u64                             total_texture_memory_use;
    u64                             total_aliased_tlas_memory_use;
    u64                             total_aliased_blas_memory_use;
    u64                             total_device_memory_use;
    u32                             buffer_count;
    u32                             texture_count;
    u32                             tlas_count;
    u32                             blas_count;
    u32                             heap_count;
    moon_memory_buffer_size_pair   *buffer_list;
    moon_memory_texture_size_pair  *texture_list;
    moon_memory_tlas_size_pair     *tlas_list;
    moon_memory_blas_size_pair     *blas_list;
    moon_memory_heap_size_pair     *heap_list;
} moon_memory_report;

/** List properties of available physical devices. The `out_details` argument may be nullptr to query 
 *  the details count and allocate an array of pointers to grab the device details with a second call. */
typedef void (LAKECALL *PFN_moon_list_device_details)(moon_adapter moon, u32 *out_device_count, moon_device_details const **out_details);
#define FN_MOON_LIST_DEVICE_DETAILS(backend) \
    void LAKECALL _moon_##backend##_list_device_details(moon_adapter moon, u32 *out_device_count, moon_device_details const **out_details)

/** Assemble a rendering device from given details. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_assembly)(moon_adapter moon, moon_device_assembly const *assembly, moon_device *out_device);
#define FN_MOON_DEVICE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_assembly(moon_adapter moon, moon_device_assembly const *assembly, moon_device *out_device)

/** Destroy a rendering device. */
PFN_LAKE_WORK(PFN_moon_device_zero_refcnt, moon_device device);
#define FN_MOON_DEVICE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_device_zero_refcnt, moon_device device)

/** Retrieve the number of queues available for a given queue type. Writes 0 if the command queue is unavailable. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_queue_count)(moon_device device, moon_queue_type queue_type, u32 *out_queue_count);
#define FN_MOON_DEVICE_QUEUE_COUNT(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_queue_count(moon_device device, moon_queue_type queue_type, u32 *out_queue_count)

/** Wait until GPU work on a given command queue is done. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_queue_wait_idle)(moon_device device, moon_queue queue);
#define FN_MOON_DEVICE_QUEUE_WAIT_IDLE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_queue_wait_idle(moon_device device, moon_queue queue)

/** Wait until all GPU work is done. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_wait_idle)(moon_device device);
#define FN_MOON_DEVICE_WAIT_IDLE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_wait_idle(moon_device device)

/** Submit work into a command queue. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_submit_commands)(moon_device device, moon_device_submit const *submit);
#define FN_MOON_DEVICE_SUBMIT_COMMANDS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_submit_commands(moon_device device, moon_device_submit const *submit)

/** Present swapchain images. Used by a primary device that supports presentation to a window surface. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_present_frames)(moon_device device, moon_device_present const *present);
#define FN_MOON_DEVICE_PRESENT_FRAMES(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_present_frames(moon_device device, moon_device_present const *present)

/** Destroys all resources that were zombified and are ready to be destroyed. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_commit_deferred_destructors)(moon_device device);
#define FN_MOON_DEVICE_COMMIT_DEFERRED_DESTRUCTORS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_commit_deferred_destructors(moon_device device)

/** Get GPU memory requirements for a buffer. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_buffer_memory_requirements)(moon_device device, moon_buffer_assembly const *assembly, moon_memory_requirements *out_requirements);
#define FN_MOON_DEVICE_BUFFER_MEMORY_REQUIREMENTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_buffer_memory_requirements(moon_device device, moon_buffer_assembly const *assembly, moon_memory_requirements *out_requirements)

/** Get GPU memory requirements for a texture. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_texture_memory_requirements)(moon_device device, moon_texture_assembly const *assembly, moon_memory_requirements *out_requirements);
#define FN_MOON_DEVICE_TEXTURE_MEMORY_REQUIREMENTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_texture_memory_requirements(moon_device device, moon_texture_assembly const *assembly, moon_memory_requirements *out_requirements)

/** Query device acceleration structure build sizes for a top-level. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_tlas_build_sizes)(moon_device device, moon_tlas_assembly const *assembly, moon_build_acceleration_structure_sizes *out_sizes);
#define FN_MOON_DEVICE_TLAS_BUILD_SIZES(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_tlas_build_sizes(moon_device device, moon_tlas_assembly const *assembly, moon_build_acceleration_structure_sizes *out_sizes)

/** Query device acceleration structure build sizes for a bottom-level. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_blas_build_sizes)(moon_device device, moon_blas_assembly const *assembly, moon_build_acceleration_structure_sizes *out_sizes);
#define FN_MOON_DEVICE_BLAS_BUILD_SIZES(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_blas_build_sizes(moon_device device, moon_blas_assembly const *assembly, moon_build_acceleration_structure_sizes *out_sizes)

/** Writes statistics of all device heap memory in use. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_memory_report)(moon_device device, moon_memory_report *out_report);
#define FN_MOON_DEVICE_MEMORY_REPORT(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_memory_report(moon_device device, moon_memory_report *out_report)

/** Assemble and allocate heap memory on a device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_memory_heap_assembly)(moon_device device, moon_memory_heap_assembly const *assembly, moon_memory_heap *out_heap);
#define FN_MOON_MEMORY_HEAP_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_memory_heap_assembly(moon_device device, moon_memory_heap_assembly const *assembly, moon_memory_heap *out_heap)

/** Destroy and deallocate heap memory on a device. */
PFN_LAKE_WORK(PFN_moon_memory_heap_zero_refcnt, moon_memory_heap heap);
#define FN_MOON_MEMORY_HEAP_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_memory_heap_zero_refcnt, moon_memory_heap heap)

/** Create a buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_buffer)(moon_device device, moon_buffer_assembly const *assembly, moon_buffer_id *out_buffer);
#define FN_MOON_CREATE_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_buffer(moon_device device, moon_buffer_assembly const *assembly, moon_buffer_id *out_buffer)

/** Create a buffer from memory on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_buffer_from_memory_heap)(moon_device device, moon_buffer_assembly_from_memory_heap const *assembly, moon_buffer_id *out_buffer);
#define FN_MOON_CREATE_BUFFER_FROM_MEMORY_HEAP(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_buffer_from_memory_heap(moon_device device, moon_buffer_assembly_from_memory_heap const *assembly, moon_buffer_id *out_buffer)

/** Create a texture on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture)(moon_device device, moon_texture_assembly const *assembly, moon_texture_id *out_texture);
#define FN_MOON_CREATE_TEXTURE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture(moon_device device, moon_texture_assembly const *assembly, moon_texture_id *out_texture)

/** Create a texture from memory on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture_from_memory_heap)(moon_device device, moon_texture_assembly_from_memory_heap const *assembly, moon_texture_id *out_texture);
#define FN_MOON_CREATE_TEXTURE_FROM_MEMORY_HEAP(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture_from_memory_heap(moon_device device, moon_texture_assembly_from_memory_heap const *assembly, moon_texture_id *out_texture)

/** Create a texture view on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture_view)(moon_device device, moon_texture_view_assembly const *assembly, moon_texture_view_id *out_texture_view);
#define FN_MOON_CREATE_TEXTURE_VIEW(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture_view(moon_device device, moon_texture_view_assembly const *assembly, moon_texture_view_id *out_texture_view)

/** Create a sampler on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_sampler)(moon_device device, moon_sampler_assembly const *assembly, moon_sampler_id *out_sampler);
#define FN_MOON_CREATE_SAMPLER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_sampler(moon_device device, moon_sampler_assembly const *assembly, moon_sampler_id *out_sampler)

/** Create a top-level on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_tlas)(moon_device device, moon_tlas_assembly const *assembly, moon_tlas_id *out_tlas);
#define FN_MOON_CREATE_TLAS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_tlas(moon_device device, moon_tlas_assembly const *assembly, moon_tlas_id *out_tlas)

/** Create a top-level from buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_tlas_from_buffer)(moon_device device, moon_tlas_assembly_from_buffer const *assembly, moon_tlas_id *out_tlas);
#define FN_MOON_CREATE_TLAS_FROM_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_tlas_from_buffer(moon_device device, moon_tlas_assembly_from_buffer const *assembly, moon_tlas_id *out_tlas)

/** Create a bottom-level on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_blas)(moon_device device, moon_blas_assembly const *assembly, moon_blas_id *out_blas);
#define FN_MOON_CREATE_BLAS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_blas(moon_device device, moon_blas_assembly const *assembly, moon_blas_id *out_blas)

/** Create a bottom-level from buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_blas_from_buffer)(moon_device device, moon_blas_assembly_from_buffer const *assembly, moon_blas_id *out_blas);
#define FN_MOON_CREATE_BLAS_FROM_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_blas_from_buffer(moon_device device, moon_blas_assembly_from_buffer const *assembly, moon_blas_id *out_blas)

/** The buffer handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_buffer_valid)(moon_device device, moon_buffer_id buffer);
#define FN_MOON_IS_BUFFER_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_buffer_valid(moon_device device, moon_buffer_id buffer)

/** The texture handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_texture_valid)(moon_device device, moon_texture_id texture);
#define FN_MOON_IS_TEXTURE_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_texture_valid(moon_device device, moon_texture_id texture)

/** The texture view handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_texture_view_valid)(moon_device device, moon_texture_view_id texture_view);
#define FN_MOON_IS_TEXTURE_VIEW_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_texture_view_valid(moon_device device, moon_texture_view_id texture_view)

/** The sampler handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_sampler_valid)(moon_device device, moon_sampler_id sampler);
#define FN_MOON_IS_SAMPLER_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_sampler_valid(moon_device device, moon_sampler_id sampler)

/** The tlas-level handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_tlas_valid)(moon_device device, moon_tlas_id tlas);
#define FN_MOON_IS_TLAS_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_tlas_valid(moon_device device, moon_tlas_id tlas)

/** The bottom-level handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_blas_valid)(moon_device device, moon_blas_id blas);
#define FN_MOON_IS_BLAS_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_blas_valid(moon_device device, moon_blas_id blas)

/** Returns the host address of a mapped buffer handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_buffer_host_address)(moon_device device, moon_buffer_id buffer, void **out_host_address);
#define FN_MOON_BUFFER_HOST_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_buffer_host_address(moon_device device, moon_buffer_id buffer, void **out_host_address)

/** Returns the device address of a buffer handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_buffer_device_address)(moon_device device, moon_buffer_id buffer, moon_device_address *out_device_address);
#define FN_MOON_BUFFER_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_buffer_device_address(moon_device device, moon_buffer_id buffer, moon_device_address *out_device_address)

/** Returns the device address of an top-level acceleration structure handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_tlas_device_address)(moon_device device, moon_tlas_id tlas, moon_device_address *out_device_address);
#define FN_MOON_TLAS_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_tlas_device_address(moon_device device, moon_tlas_id tlas, moon_device_address *out_device_address)

/** Returns the device address of an bottom-level acceleration structure handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_blas_device_address)(moon_device device, moon_blas_id blas, moon_device_address *out_device_address);
#define FN_MOON_BLAS_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_blas_device_address(moon_device device, moon_blas_id blas, moon_device_address *out_device_address)

/** The buffer will be zombified and destroyed after `PFN_moon_device_commit_deferred_destructors` is called. */
typedef void (LAKECALL *PFN_moon_destroy_buffer)(moon_device device, moon_buffer_id buffer);
#define FN_MOON_DESTROY_BUFFER(backend) \
    void LAKECALL _moon_##backend##_destroy_buffer(moon_device device, moon_buffer_id buffer)

/** The texture will be zombified and destroyed after `PFN_moon_device_commit_deferred_destructors` is called. */
typedef void (LAKECALL *PFN_moon_destroy_texture)(moon_device device, moon_texture_id texture);
#define FN_MOON_DESTROY_TEXTURE(backend) \
    void LAKECALL _moon_##backend##_destroy_texture(moon_device device, moon_texture_id texture)

/** The texture view will be zombified and destroyed after `PFN_moon_device_commit_deferred_destructors` is called. */
typedef void (LAKECALL *PFN_moon_destroy_texture_view)(moon_device device, moon_texture_view_id texture_view);
#define FN_MOON_DESTROY_TEXTURE_VIEW(backend) \
    void LAKECALL _moon_##backend##_destroy_texture_view(moon_device device, moon_texture_view_id texture_view)

/** The sampler will be zombified and destroyed after `PFN_moon_device_commit_deferred_destructors` is called. */
typedef void (LAKECALL *PFN_moon_destroy_sampler)(moon_device device, moon_sampler_id sampler);
#define FN_MOON_DESTROY_SAMPLER(backend) \
    void LAKECALL _moon_##backend##_destroy_sampler(moon_device device, moon_sampler_id sampler)

/** The top-level will be zombified and destroyed after `PFN_moon_device_commit_deferred_destructors` is called. */
typedef void (LAKECALL *PFN_moon_destroy_tlas)(moon_device device, moon_tlas_id tlas);
#define FN_MOON_DESTROY_TLAS(backend) \
    void LAKECALL _moon_##backend##_destroy_tlas(moon_device device, moon_tlas_id tlas)

/** The bottom-level will be zombified and destroyed after `PFN_moon_device_commit_deferred_destructors` is called. */
typedef void (LAKECALL *PFN_moon_destroy_blas)(moon_device device, moon_blas_id blas);
#define FN_MOON_DESTROY_BLAS(backend) \
    void LAKECALL _moon_##backend##_destroy_blas(moon_device device, moon_blas_id blas)

/** Header for `struct moon_device_impl`. */
typedef struct moon_device_header {
    moon_interface              moon;
    atomic_u32                  flags;
    lake_refcnt                 refcnt;
    moon_device_details const  *details;
    moon_device_assembly        assembly;
} moon_device_header;

/** Header for `struct moon_memory_heap_impl`. */
typedef struct moon_memory_heap_header {
    moon_device                 device;
    atomic_u32                  flags;
    lake_refcnt                 refcnt;
    moon_memory_heap_assembly   assembly;
} moon_memory_heap_header;
