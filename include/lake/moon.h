#pragma once

/** @file lake/moon.h
 *  @brief Rendering backend.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>

#define MOON_MAX_COLOR_ATTACHMENTS 8
#define MOON_MAX_PUSH_CONSTANT_WORD_SIZE (32u)
#define MOON_MAX_PUSH_CONSTANT_BYTE_SIZE (MOON_MAX_PUSH_CONSTANT_WORD_SIZE * 4)
#define MOON_PIPELINE_LAYOUT_COUNT       (MOON_MAX_PUSH_CONSTANT_WORD_SIZE + 1)
#define MOON_SHADER_UNUSED UINT32_MAX

/** Implements the rendering interface via a graphics API. */
LAKE_DECL_HANDLE(moon_adapter);
/** A rendering device is used to create GPU resources and execute rendering commands. */
LAKE_DECL_HANDLE(moon_device);
/** Represents a block of memory on a physical device (most often VRAM or mapped host memory).
 *  It can be passed into procedures used to create GPU resources to reuse memory blocks. */
LAKE_DECL_HANDLE(moon_device_heap);
/** Query pools are used to gather GPU execution statistics, like timestamps, occlusion queries 
 *  or pipelines stats - counting how many vertices, primitives, or fragments were processed, etc. 
 *  They are important for profiling, visibility queries and conditional rendering. */
LAKE_DECL_HANDLE(moon_timeline_query_pool);
/** Represents an atomic counter on the GPU used for synchronization on an ever incrementing timeline. */
LAKE_DECL_HANDLE(moon_timeline_semaphore);
/** Represents a binary synchronization primitive where signals and waits must always happen on 1:1 pairs. */
LAKE_DECL_HANDLE(moon_binary_semaphore);
/** Represents pipelines that dispatch compute shaders. */
LAKE_DECL_HANDLE(moon_compute_pipeline);
/** Work graphs enable shaders to create and schedule GPU work. Experimental hardware feature. */
LAKE_DECL_HANDLE(moon_work_graph_pipeline);
/** Represents a traditional shader pipeline, may include mesh shaders on hardware support. */
LAKE_DECL_HANDLE(moon_raster_pipeline);
/** A shader pipeline that uses hardware-accelerated ray tracing cores,
 *  enables the use of dedicated ray tracing shaders. */
LAKE_DECL_HANDLE(moon_ray_tracing_pipeline);
/** The swapchain is a way to present rendered images into a window surface, that is owned by the display backend. */
LAKE_DECL_HANDLE(moon_swapchain);
/** A context to record GPU work, later compiled into staged command lists. */
LAKE_DECL_HANDLE(moon_command_recorder);
/** An executable command buffer that can be submited to a device queue. */
LAKE_DECL_HANDLE(moon_staged_command_list);

typedef struct { u64 handle; } moon_buffer_id;
typedef struct { u64 handle; } moon_texture_id;
typedef struct { u64 handle; } moon_texture_view_id;
typedef struct { u64 handle; } moon_sampler_id;
typedef struct { u64 handle; } moon_tlas_id;
typedef struct { u64 handle; } moon_blas_id;

#define MOON_HANDLE_INDEX_MASK      ((1ull << 20) - 1)
#define MOON_HANDLE_VERSION_SHIFT   20

#define moon_handle_make(T, IDX, VER) \
    ((T){ .handle = ((u64)(VER) << MOON_HANDLE_VERSION_SHIFT) | ((IDX) & MOON_HANDLE_INDEX_MASK) })
#define moon_handle_get_index(ID) \
    ((u32)((ID).handle & MOON_HANDLE_INDEX_MASK))
#define moon_handle_get_version(ID) \
    ((u64)(((ID).handle) >> MOON_HANDLE_VERSION_SHIFT))
#define moon_handle_set_index(ID, IDX) \
    ((ID).handle = ((ID).handle & ~MOON_HANDLE_INDEX_MASK) | ((IDX) & MOON_HANDLE_INDEX_MASK))
#define moon_handle_set_version(ID, VER) \
    ((ID).handle = ((ID).handle & MOON_HANDLE_INDEX_MASK) | ((u64)(VER) << MOON_HANDLE_VERSION_SHIFT))

/** Represents a raw pointer in device memory. */
typedef u64 moon_device_address;

/** Represents a raw pointer in either device or host memory. */
typedef union moon_device_or_host_address {
    moon_device_address         device_address;
    void const                 *host_address;
} moon_device_or_host_address;

typedef u32 moon_pipeline_stages;
typedef enum moon_pipeline_stage_bits : moon_pipeline_stages {
    moon_pipeline_stage_none                                = 0u,
    moon_pipeline_stage_top_of_pipe                         = (1u << 0),
    moon_pipeline_stage_draw_indirect                       = (1u << 1),
    moon_pipeline_stage_vertex_shader                       = (1u << 2),
    moon_pipeline_stage_tesselation_control_shader          = (1u << 3),
    moon_pipeline_stage_tesselation_evaluation_shader       = (1u << 4),
    moon_pipeline_stage_geometry_shader                     = (1u << 5),
    moon_pipeline_stage_fragment_shader                     = (1u << 6),
    moon_pipeline_stage_early_fragment_tests                = (1u << 7),
    moon_pipeline_stage_late_fragment_tests                 = (1u << 8),
    moon_pipeline_stage_color_attachment_output             = (1u << 9),
    moon_pipeline_stage_compute_shader                      = (1u << 10),
    moon_pipeline_stage_transfer                            = (1u << 11),
    moon_pipeline_stage_bottom_of_pipe                      = (1u << 12),
    moon_pipeline_stage_host                                = (1u << 13),
    moon_pipeline_stage_all_graphics                        = (1u << 14),
    moon_pipeline_stage_all_commands                        = (1u << 15),
    moon_pipeline_stage_copy                                = (1u << 16),
    moon_pipeline_stage_resolve                             = (1u << 17),
    moon_pipeline_stage_blit                                = (1u << 18),
    moon_pipeline_stage_clear                               = (1u << 19),
    moon_pipeline_stage_index_input                         = (1u << 20),
    moon_pipeline_stage_pre_rasterization_shaders           = (1u << 21),
    moon_pipeline_stage_task_shader                         = (1u << 22),
    moon_pipeline_stage_mesh_shader                         = (1u << 23),
    moon_pipeline_stage_acceleration_structure_build        = (1u << 24),
    moon_pipeline_stage_ray_tracing_shader                  = (1u << 25),
} moon_pipeline_stage_bits;

typedef enum moon_access_type : u32 {
    moon_access_type_none       = 0u,
    moon_access_type_read       = (1u << 31),
    moon_access_type_write      = (1u << 30),
    moon_access_type_read_write = moon_access_type_read | moon_access_type_write,
} moon_access_type;

/** Pairs a GPU timeline with a CPU timeline value. */
typedef struct {
    moon_timeline_semaphore         timeline_semaphore;
    u64                             value;
} moon_timeline_pair;

typedef enum moon_device_type : s16 {
    moon_device_type_other = 0,
    moon_device_type_integrated_gpu,
    moon_device_type_discrete_gpu,
    moon_device_type_virtual_gpu,
    moon_device_type_cpu,
} moon_device_type;

typedef struct moon_device_limits {
    u32 max_texture_dimension_1d;
    u32 max_texture_dimension_2d;
    u32 max_texture_dimension_3d;
    u32 max_texture_array_layers;
    u32 max_uniform_buffer_range;
    u32 max_storage_buffer_range;
    u32 max_root_constant_size;
    u32 max_sampled_textures;
    u32 max_storage_textures;
    u32 max_samplers;
    u32 max_uniform_buffers;
    u32 max_storage_buffers;
    u32 max_vertex_input_bindings;
    u32 max_vertex_input_attributes;
    u32 max_viewports;
    u32 max_viewport_dimensions[2];
    u32 max_scissor_dimensions[2];
    u32 max_compute_work_group_count[3];
    u32 max_compute_work_group_size[3];
    u32 max_compute_work_group_invocations;
    u32 uniform_buffer_offset_alignment;
    u32 storage_buffer_offset_alignment;
    u32 min_texel_buffer_offset_alignment;
    u32 min_uniform_buffer_offset_alignment;
    u32 max_color_attachments;
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
    moon_implicit_feature_work_graph_mesh_tasks                         = (1u << 7),
    moon_implicit_feature_image_atomic64                                = (1u << 8),
    moon_implicit_feature_shader_atomic_float                           = (1u << 9),
    moon_implicit_feature_shader_atomic_int64                           = (1u << 10),
    moon_implicit_feature_shader_float16                                = (1u << 11),
    moon_implicit_feature_shader_int16                                  = (1u << 12),
    moon_implicit_feature_shader_int8                                   = (1u << 13),
    moon_implicit_feature_dynamic_state                                 = (1u << 14),
    moon_implicit_feature_sparse_binding                                = (1u << 15),
    moon_implicit_feature_swapchain                                     = (1u << 16),
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

#define MOON_MAX_COMPUTE_QUEUE_COUNT            8
#define MOON_MAX_TRANSFER_QUEUE_COUNT           2
#define MOON_MAX_SPARSE_BINDING_QUEUE_COUNT     1
#define MOON_MAX_VIDEO_DECODE_QUEUE_COUNT       1
#define MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT       1

#define MOON_QUEUE_MAIN_BEGIN_INDEX             0
#define MOON_QUEUE_COMPUTE_BEGIN_INDEX          1
#define MOON_QUEUE_TRANSFER_BEGIN_INDEX         (MOON_MAX_COMPUTE_QUEUE_COUNT)
#define MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX   (MOON_QUEUE_TRANSFER_BEGIN_INDEX + MOON_MAX_TRANSFER_QUEUE_COUNT)
#define MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX     (MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX + MOON_MAX_SPARSE_BINDING_QUEUE_COUNT)
#define MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX     (MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX + MOON_MAX_VIDEO_DECODE_QUEUE_COUNT)
#define MOON_QUEUE_INDEX_COUNT                  (MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX + MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT)

/** Queue types used for scheduling different type of GPU work. */
typedef enum moon_queue_type : u8  {
    moon_queue_type_none            = 0u,
    moon_queue_type_main            = (1u << 0),
    moon_queue_type_compute         = (1u << 1),
    moon_queue_type_transfer        = (1u << 2),
    moon_queue_type_sparse_binding  = (1u << 3),
    moon_queue_type_video_decode    = (1u << 4),
    moon_queue_type_video_encode    = (1u << 5),
    moon_queue_type_count           = 6u,
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

typedef u8 moon_memory_flags;
typedef enum moon_memory_flag_bits : moon_memory_flags {
    moon_memory_flag_none                           = 0,
    moon_memory_flag_dedicated_memory               = (1u << 0),
    moon_memory_flag_can_alias                      = (1u << 1),
    moon_memory_flag_host_access_sequential_write   = (1u << 2),
    moon_memory_flag_host_access_random             = (1u << 3),
    moon_memory_flag_strategy_min_memory            = (1u << 4),
    moon_memory_flag_strategy_min_time              = (1u << 5),
} moon_memory_flag_bits;

typedef struct moon_memory_requirements {
    u64                         size;
    u64                         alignment;
    u32                         type_bitmask; /** Bitmask of indices into `moon_device_details->memory_types[]`. */
} moon_memory_requirements;

typedef u8 moon_memory_property; 
typedef enum moon_memory_property_bits : moon_memory_property {
    moon_memory_property_none                                           = 0,
    moon_memory_property_device_local                                   = (1u << 0), 
    moon_memory_property_device_coherent                                = (1u << 1), 
    moon_memory_property_device_uncached                                = (1u << 2), 
    moon_memory_property_host_visible                                   = (1u << 3), 
    moon_memory_property_host_coherent                                  = (1u << 4), 
    moon_memory_property_host_cached                                    = (1u << 5), 
    moon_memory_property_lazily_allocated                               = (1u << 6), 
    moon_memory_property_protected                                      = (1u << 7), 
} moon_memory_property_bits;

typedef struct moon_device_heap_type {
    moon_memory_property property;
    u8                   heap_idx;
} moon_device_heap_type;
#define MOON_MAX_DEVICE_HEAP_TYPES 32

typedef struct moon_device_details {
    u32                                         api_version;
    u32                                         driver_version;
    u16                                         vendor_id;
    u16                                         device_id;
    moon_device_type                            device_type;
    char                                        device_name[256];
    char                                        pipeline_cache_uuid[16];
    u16                                         heap_memory_type_count;
    moon_device_heap_type                       heap_memory_types[MOON_MAX_DEVICE_HEAP_TYPES];
    moon_device_work_graph_details              work_graph_details;
    moon_device_mesh_shader_details             mesh_shader_details;
    moon_device_acceleration_structure_details  acceleration_structure_details;
    moon_device_ray_tracing_pipeline_details    ray_tracing_pipeline_details;
    moon_device_limits                          limits;
    u16                                         compute_queue_count;
    u16                                         transfer_queue_count;
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

/** List properties of available physical devices. The `out_details` argument may be nullptr to query 
 *  the details count and allocate an array of pointers to grab the device details with a second call. */
typedef void (LAKECALL *PFN_moon_list_device_details)(moon_adapter moon, u32 *out_device_count, moon_device_details const **out_details);
#define FN_MOON_LIST_DEVICE_DETAILS(backend) \
    void LAKECALL _moon_##backend##_list_device_details(moon_adapter moon, u32 *out_device_count, moon_device_details const **out_details)

/** Assemble a rendering device from given details. */
PFN_LAKE_HANDLE_ASSEMBLY(moon, device, adapter);
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_assembly)(moon_adapter moon, moon_device_assembly const *assembly, moon_device *out_device);
#define FN_MOON_DEVICE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_assembly(moon_adapter moon, moon_device_assembly const *assembly, moon_device *out_device)

/** Destroy a rendering device. */
PFN_LAKE_HANDLE_DESTRUCTOR(moon, device);
#define FN_MOON_DEVICE_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, device)

/** Retrieve the number of queues available for a given queue type. Writes 0 if the command queue is unavailable. */
typedef u32 (LAKECALL *PFN_moon_device_queue_count)(moon_device device, moon_queue_type queue_type);
#define FN_MOON_DEVICE_QUEUE_COUNT(backend) \
    u32 LAKECALL _moon_##backend##_device_queue_count(moon_device device, moon_queue_type queue_type)

/** Wait until GPU work on a given command queue is done. */
typedef void (LAKECALL *PFN_moon_device_queue_wait_idle)(moon_device device, moon_queue queue);
#define FN_MOON_DEVICE_QUEUE_WAIT_IDLE(backend) \
    void LAKECALL _moon_##backend##_device_queue_wait_idle(moon_device device, moon_queue queue)

/** Wait until all GPU work is done. */
PFN_LAKE_WORK(PFN_moon_device_wait_idle, moon_device device);
#define FN_MOON_DEVICE_WAIT_IDLE(backend) \
    FN_LAKE_WORK(_moon_##backend##_device_wait_idle, moon_device device)

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

/** Submit work into a command queue. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_submit_commands)(moon_device device, moon_device_submit const *submit);
#define FN_MOON_DEVICE_SUBMIT_COMMANDS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_submit_commands(moon_device device, moon_device_submit const *submit)

/** Describes a present. */
typedef struct moon_device_present {
    moon_binary_semaphore const    *wait_binary_semaphores;
    moon_swapchain const           *swapchains;
    u32                             wait_binary_semaphore_count;
    u16                             swapchain_count;
    moon_queue                      queue;
} moon_device_present;
static constexpr moon_device_present MOON_DEVICE_PRESENT_INIT = {0};

/** Present swapchain images. Used by a primary device that supports presentation to a window surface. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_present_frames)(moon_device device, moon_device_present const *present);
#define FN_MOON_DEVICE_PRESENT_FRAMES(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_present_frames(moon_device device, moon_device_present const *present)

/** Destroys all resources that were zombified and are ready to be destroyed. */
PFN_LAKE_WORK(PFN_moon_device_destroy_deferred, moon_device device);
#define FN_MOON_DEVICE_DESTROY_DEFERRED(backend) \
    FN_LAKE_WORK(_moon_##backend##_device_destroy_deferred, moon_device device)

typedef struct moon_device_heap_assembly {
    moon_memory_requirements        requirements;
    moon_memory_flags               flags;
} moon_device_heap_assembly;
static constexpr moon_device_heap_assembly MOON_DEVICE_HEAP_ASSEMBLY_INIT = {0};

/** Assemble and allocate heap memory on a device. */
PFN_LAKE_HANDLE_ASSEMBLY(moon, device_heap, device);
#define FN_MOON_DEVICE_HEAP_ASSEMBLY(backend) \
    FN_LAKE_HANDLE_ASSEMBLY(backend, moon, device_heap, device)

/** Destroy and deallocate heap memory on a device. */
PFN_LAKE_HANDLE_DESTRUCTOR(moon, device_heap);
#define FN_MOON_DEVICE_HEAP_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, device_heap)

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
    moon_device_heap                heap;
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

/** Writes statistics of all device heap memory in use. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_memory_report)(moon_device device, moon_memory_report *out_report);
#define FN_MOON_DEVICE_MEMORY_REPORT(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_memory_report(moon_device device, moon_memory_report *out_report)

typedef struct moon_buffer_assembly {
    u64                         size;
    moon_memory_flags           memory_flags; /**< Ignored if allocating from existing heap memory. */
    u8                      pad0[7];
    lake_small_string           name;
} moon_buffer_assembly;
static constexpr moon_buffer_assembly MOON_BUFFER_ASSEMBLY_INIT = {
    .size = 0lu,
    .memory_flags = moon_memory_flag_none,
};

typedef struct moon_buffer_assembly_from_heap {
    moon_buffer_assembly        buffer_assembly;
    moon_device_heap            device_heap;
    u64                         offset;
} moon_buffer_assembly_from_heap;
static constexpr moon_buffer_assembly_from_heap MOON_BUFFER_ASSEMBLY_FROM_HEAP_INIT = {
    .buffer_assembly = MOON_BUFFER_ASSEMBLY_INIT,
    .device_heap = nullptr,
    .offset = 0lu,
};

/** Get GPU memory requirements for a buffer. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_buffer_memory_requirements)(moon_device device, moon_buffer_assembly const *assembly, moon_memory_requirements *out_requirements);
#define FN_MOON_DEVICE_BUFFER_MEMORY_REQUIREMENTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_buffer_memory_requirements(moon_device device, moon_buffer_assembly const *assembly, moon_memory_requirements *out_requirements)

/** Create a buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_buffer)(moon_device device, moon_buffer_assembly const *assembly, moon_buffer_id *out_buffer);
#define FN_MOON_CREATE_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_buffer(moon_device device, moon_buffer_assembly const *assembly, moon_buffer_id *out_buffer)

/** Create a buffer from memory on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_buffer_from_heap)(moon_device device, moon_buffer_assembly_from_heap const *assembly, moon_buffer_id *out_buffer);
#define FN_MOON_CREATE_BUFFER_FROM_HEAP(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_buffer_from_heap(moon_device device, moon_buffer_assembly_from_heap const *assembly, moon_buffer_id *out_buffer)

/** The buffer handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_buffer_valid)(moon_device device, moon_buffer_id buffer);
#define FN_MOON_IS_BUFFER_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_buffer_valid(moon_device device, moon_buffer_id buffer)

/** Returns the host address of a mapped buffer handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_buffer_host_address)(moon_device device, moon_buffer_id buffer, void **out_host_address);
#define FN_MOON_BUFFER_HOST_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_buffer_host_address(moon_device device, moon_buffer_id buffer, void **out_host_address)

/** Returns the device address of a buffer handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_buffer_device_address)(moon_device device, moon_buffer_id buffer, moon_device_address *out_device_address);
#define FN_MOON_BUFFER_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_buffer_device_address(moon_device device, moon_buffer_id buffer, moon_device_address *out_device_address)

/** The buffer will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_buffer)(moon_device device, moon_buffer_id buffer);
#define FN_MOON_DESTROY_BUFFER(backend) \
    void LAKECALL _moon_##backend##_destroy_buffer(moon_device device, moon_buffer_id buffer)

typedef union moon_color_value {
    f32 vec[4];
    s32 sint[4];
    u32 uint[4];
} moon_color_value;

typedef struct moon_depth_stencil_value {
    f32 depth;
    u32 stencil;
} moon_depth_stencil_value;

typedef struct moon_depth_bias {
    f32 constant_factor;
    f32 clamp;
    f32 slope_factor;
} moon_depth_bias;

typedef enum moon_border_color : s8 {
    moon_border_color_float_transparent_black = 0,
    moon_border_color_int_transparent_black,
    moon_border_color_float_opaque_black,
    moon_border_color_int_opaque_black,
    moon_border_color_float_opaque_white,
    moon_border_color_int_opaque_white,
} moon_border_color;

typedef enum moon_index_format : s8 {
    moon_index_format_invalid = 0,
    moon_index_format_u16,
    moon_index_format_u32,
} moon_index_format;

typedef enum moon_compare_op : s8 {
    moon_compare_op_never = 0,
    moon_compare_op_less,
    moon_compare_op_equal,
    moon_compare_op_less_or_equal,
    moon_compare_op_not_equal,
    moon_compare_op_greater,
    moon_compare_op_greater_or_equal,
    moon_compare_op_always,
} moon_compare_op;

typedef enum moon_load_op : s8 {
    moon_load_op_load = 0,
    moon_load_op_clear,
    moon_load_op_dont_care,
    moon_load_op_none,
} moon_load_op;

typedef enum moon_store_op : s8 {
    moon_store_op_store = 0,
    moon_store_op_dont_care,
    moon_store_op_none,
} moon_store_op;

typedef enum moon_stencil_op : s8 {
    moon_stencil_op_keep = 0,
    moon_stencil_op_zero,
    moon_stencil_op_replace,
    moon_stencil_op_increment,
    moon_stencil_op_decrement,
    moon_stencil_op_increment_wrap,
    moon_stencil_op_decrement_wrap,
    moon_stencil_op_invert,
} moon_stencil_op;

typedef enum moon_stencil_face : s8 {
    moon_stencil_face_front = 0,
    moon_stencil_face_back,
} moon_stencil_face;

typedef enum moon_sharing_mode : s8 {
    moon_sharing_mode_exclusive = 0,
    moon_sharing_mode_concurrent,
} moon_sharing_mode;

/** A list of supported formats of textures, they describe how memory is laid out.
 *  Availability of formats depends on physical device and backend. */
typedef enum moon_format : s32 {
    moon_format_undefined = 0,
    moon_format_r4g4_unorm_pack8 = 1,
    moon_format_r4g4b4a4_unorm_pack16 = 2,
    moon_format_b4g4r4a4_unorm_pack16 = 3,
    moon_format_r5g6b5_unorm_pack16 = 4,
    moon_format_b5g6r5_unorm_pack16 = 5,
    moon_format_r5g5b5a1_unorm_pack16 = 6,
    moon_format_b5g5r5a1_unorm_pack16 = 7,
    moon_format_a1r5g5b5_unorm_pack16 = 8,
    moon_format_r8_unorm = 9,
    moon_format_r8_snorm = 10,
    moon_format_r8_uscaled = 11,
    moon_format_r8_sscaled = 12,
    moon_format_r8_uint = 13,
    moon_format_r8_sint = 14,
    moon_format_r8_srgb = 15,
    moon_format_r8g8_unorm = 16,
    moon_format_r8g8_snorm = 17,
    moon_format_r8g8_uscaled = 18,
    moon_format_r8g8_sscaled = 19,
    moon_format_r8g8_uint = 20,
    moon_format_r8g8_sint = 21,
    moon_format_r8g8_srgb = 22,
    moon_format_r8g8b8_unorm = 23,
    moon_format_r8g8b8_snorm = 24,
    moon_format_r8g8b8_uscaled = 25,
    moon_format_r8g8b8_sscaled = 26,
    moon_format_r8g8b8_uint = 27,
    moon_format_r8g8b8_sint = 28,
    moon_format_r8g8b8_srgb = 29,
    moon_format_b8g8r8_unorm = 30,
    moon_format_b8g8r8_snorm = 31,
    moon_format_b8g8r8_uscaled = 32,
    moon_format_b8g8r8_sscaled = 33,
    moon_format_b8g8r8_uint = 34,
    moon_format_b8g8r8_sint = 35,
    moon_format_b8g8r8_srgb = 36,
    moon_format_r8g8b8a8_unorm = 37,
    moon_format_r8g8b8a8_snorm = 38,
    moon_format_r8g8b8a8_uscaled = 39,
    moon_format_r8g8b8a8_sscaled = 40,
    moon_format_r8g8b8a8_uint = 41,
    moon_format_r8g8b8a8_sint = 42,
    moon_format_r8g8b8a8_srgb = 43,
    moon_format_b8g8r8a8_unorm = 44,
    moon_format_b8g8r8a8_snorm = 45,
    moon_format_b8g8r8a8_uscaled = 46,
    moon_format_b8g8r8a8_sscaled = 47,
    moon_format_b8g8r8a8_uint = 48,
    moon_format_b8g8r8a8_sint = 49,
    moon_format_b8g8r8a8_srgb = 50,
    moon_format_a8b8g8r8_unorm_pack32 = 51,
    moon_format_a8b8g8r8_snorm_pack32 = 52,
    moon_format_a8b8g8r8_uscaled_pack32 = 53,
    moon_format_a8b8g8r8_sscaled_pack32 = 54,
    moon_format_a8b8g8r8_uint_pack32 = 55,
    moon_format_a8b8g8r8_sint_pack32 = 56,
    moon_format_a8b8g8r8_srgb_pack32 = 57,
    moon_format_a2r10g10b10_unorm_pack32 = 58,
    moon_format_a2r10g10b10_snorm_pack32 = 59,
    moon_format_a2r10g10b10_uscaled_pack32 = 60,
    moon_format_a2r10g10b10_sscaled_pack32 = 61,
    moon_format_a2r10g10b10_uint_pack32 = 62,
    moon_format_a2r10g10b10_sint_pack32 = 63,
    moon_format_a2b10g10r10_unorm_pack32 = 64,
    moon_format_a2b10g10r10_snorm_pack32 = 65,
    moon_format_a2b10g10r10_uscaled_pack32 = 66,
    moon_format_a2b10g10r10_sscaled_pack32 = 67,
    moon_format_a2b10g10r10_uint_pack32 = 68,
    moon_format_a2b10g10r10_sint_pack32 = 69,
    moon_format_r16_unorm = 70,
    moon_format_r16_snorm = 71,
    moon_format_r16_uscaled = 72,
    moon_format_r16_sscaled = 73,
    moon_format_r16_uint = 74,
    moon_format_r16_sint = 75,
    moon_format_r16_sfloat = 76,
    moon_format_r16g16_unorm = 77,
    moon_format_r16g16_snorm = 78,
    moon_format_r16g16_uscaled = 79,
    moon_format_r16g16_sscaled = 80,
    moon_format_r16g16_uint = 81,
    moon_format_r16g16_sint = 82,
    moon_format_r16g16_sfloat = 83,
    moon_format_r16g16b16_unorm = 84,
    moon_format_r16g16b16_snorm = 85,
    moon_format_r16g16b16_uscaled = 86,
    moon_format_r16g16b16_sscaled = 87,
    moon_format_r16g16b16_uint = 88,
    moon_format_r16g16b16_sint = 89,
    moon_format_r16g16b16_sfloat = 90,
    moon_format_r16g16b16a16_unorm = 91,
    moon_format_r16g16b16a16_snorm = 92,
    moon_format_r16g16b16a16_uscaled = 93,
    moon_format_r16g16b16a16_sscaled = 94,
    moon_format_r16g16b16a16_uint = 95,
    moon_format_r16g16b16a16_sint = 96,
    moon_format_r16g16b16a16_sfloat = 97,
    moon_format_r32_uint = 98,
    moon_format_r32_sint = 99,
    moon_format_r32_sfloat = 100,
    moon_format_r32g32_uint = 101,
    moon_format_r32g32_sint = 102,
    moon_format_r32g32_sfloat = 103,
    moon_format_r32g32b32_uint = 104,
    moon_format_r32g32b32_sint = 105,
    moon_format_r32g32b32_sfloat = 106,
    moon_format_r32g32b32a32_uint = 107,
    moon_format_r32g32b32a32_sint = 108,
    moon_format_r32g32b32a32_sfloat = 109,
    moon_format_r64_uint = 110,
    moon_format_r64_sint = 111,
    moon_format_r64_sfloat = 112,
    moon_format_r64g64_uint = 113,
    moon_format_r64g64_sint = 114,
    moon_format_r64g64_sfloat = 115,
    moon_format_r64g64b64_uint = 116,
    moon_format_r64g64b64_sint = 117,
    moon_format_r64g64b64_sfloat = 118,
    moon_format_r64g64b64a64_uint = 119,
    moon_format_r64g64b64a64_sint = 120,
    moon_format_r64g64b64a64_sfloat = 121,
    moon_format_b10g11r11_ufloat_pack32 = 122,
    moon_format_e5b9g9r9_ufloat_pack32 = 123,
    moon_format_d16_unorm = 124,
    moon_format_x8_d24_unorm_pack32 = 125,
    moon_format_d32_sfloat = 126,
    moon_format_s8_uint = 127,
    moon_format_d16_unorm_s8_uint = 128,
    moon_format_d24_unorm_s8_uint = 129,
    moon_format_d32_sfloat_s8_uint = 130,
    moon_format_bc1_rgb_unorm_block = 131,
    moon_format_bc1_rgb_srgb_block = 132,
    moon_format_bc1_rgba_unorm_block = 133,
    moon_format_bc1_rgba_srgb_block = 134,
    moon_format_bc2_unorm_block = 135,
    moon_format_bc2_srgb_block = 136,
    moon_format_bc3_unorm_block = 137,
    moon_format_bc3_srgb_block = 138,
    moon_format_bc4_unorm_block = 139,
    moon_format_bc4_snorm_block = 140,
    moon_format_bc5_unorm_block = 141,
    moon_format_bc5_snorm_block = 142,
    moon_format_bc6h_ufloat_block = 143,
    moon_format_bc6h_sfloat_block = 144,
    moon_format_bc7_unorm_block = 145,
    moon_format_bc7_srgb_block = 146,
    moon_format_etc2_r8g8b8_unorm_block = 147,
    moon_format_etc2_r8g8b8_srgb_block = 148,
    moon_format_etc2_r8g8b8a1_unorm_block = 149,
    moon_format_etc2_r8g8b8a1_srgb_block = 150,
    moon_format_etc2_r8g8b8a8_unorm_block = 151,
    moon_format_etc2_r8g8b8a8_srgb_block = 152,
    moon_format_eac_r11_unorm_block = 153,
    moon_format_eac_r11_snorm_block = 154,
    moon_format_eac_r11g11_unorm_block = 155,
    moon_format_eac_r11g11_snorm_block = 156,
    moon_format_astc_4x4_unorm_block = 157,
    moon_format_astc_4x4_srgb_block = 158,
    moon_format_astc_5x4_unorm_block = 159,
    moon_format_astc_5x4_srgb_block = 160,
    moon_format_astc_5x5_unorm_block = 161,
    moon_format_astc_5x5_srgb_block = 162,
    moon_format_astc_6x5_unorm_block = 163,
    moon_format_astc_6x5_srgb_block = 164,
    moon_format_astc_6x6_unorm_block = 165,
    moon_format_astc_6x6_srgb_block = 166,
    moon_format_astc_8x5_unorm_block = 167,
    moon_format_astc_8x5_srgb_block = 168,
    moon_format_astc_8x6_unorm_block = 169,
    moon_format_astc_8x6_srgb_block = 170,
    moon_format_astc_8x8_unorm_block = 171,
    moon_format_astc_8x8_srgb_block = 172,
    moon_format_astc_10x5_unorm_block = 173,
    moon_format_astc_10x5_srgb_block = 174,
    moon_format_astc_10x6_unorm_block = 175,
    moon_format_astc_10x6_srgb_block = 176,
    moon_format_astc_10x8_unorm_block = 177,
    moon_format_astc_10x8_srgb_block = 178,
    moon_format_astc_10x10_unorm_block = 179,
    moon_format_astc_10x10_srgb_block = 180,
    moon_format_astc_12x10_unorm_block = 181,
    moon_format_astc_12x10_srgb_block = 182,
    moon_format_astc_12x12_unorm_block = 183,
    moon_format_astc_12x12_srgb_block = 184,
    moon_format_g8b8g8r8_422_unorm = 1000156000,
    moon_format_b8g8r8g8_422_unorm = 1000156001,
    moon_format_g8_b8_r8_3plane_420_unorm = 1000156002,
    moon_format_g8_b8r8_2plane_420_unorm = 1000156003,
    moon_format_g8_b8_r8_3plane_422_unorm = 1000156004,
    moon_format_g8_b8r8_2plane_422_unorm = 1000156005,
    moon_format_g8_b8_r8_3plane_444_unorm = 1000156006,
    moon_format_r10x6_unorm_pack16 = 1000156007,
    moon_format_r10x6g10x6_unorm_2pack16 = 1000156008,
    moon_format_r10x6g10x6b10x6a10x6_unorm_4pack16 = 1000156009,
    moon_format_g10x6b10x6g10x6r10x6_422_unorm_4pack16 = 1000156010,
    moon_format_b10x6g10x6r10x6g10x6_422_unorm_4pack16 = 1000156011,
    moon_format_g10x6_b10x6_r10x6_3plane_420_unorm_3pack16 = 1000156012,
    moon_format_g10x6_b10x6r10x6_2plane_420_unorm_3pack16 = 1000156013,
    moon_format_g10x6_b10x6_r10x6_3plane_422_unorm_3pack16 = 1000156014,
    moon_format_g10x6_b10x6r10x6_2plane_422_unorm_3pack16 = 1000156015,
    moon_format_g10x6_b10x6_r10x6_3plane_444_unorm_3pack16 = 1000156016,
    moon_format_r12x4_unorm_pack16 = 1000156017,
    moon_format_r12x4g12x4_unorm_2pack16 = 1000156018,
    moon_format_r12x4g12x4b12x4a12x4_unorm_4pack16 = 1000156019,
    moon_format_g12x4b12x4g12x4r12x4_422_unorm_4pack16 = 1000156020,
    moon_format_b12x4g12x4r12x4g12x4_422_unorm_4pack16 = 1000156021,
    moon_format_g12x4_b12x4_r12x4_3plane_420_unorm_3pack16 = 1000156022,
    moon_format_g12x4_b12x4r12x4_2plane_420_unorm_3pack16 = 1000156023,
    moon_format_g12x4_b12x4_r12x4_3plane_422_unorm_3pack16 = 1000156024,
    moon_format_g12x4_b12x4r12x4_2plane_422_unorm_3pack16 = 1000156025,
    moon_format_g12x4_b12x4_r12x4_3plane_444_unorm_3pack16 = 1000156026,
    moon_format_g16b16g16r16_422_unorm = 1000156027,
    moon_format_b16g16r16g16_422_unorm = 1000156028,
    moon_format_g16_b16_r16_3plane_420_unorm = 1000156029,
    moon_format_g16_b16r16_2plane_420_unorm = 1000156030,
    moon_format_g16_b16_r16_3plane_422_unorm = 1000156031,
    moon_format_g16_b16r16_2plane_422_unorm = 1000156032,
    moon_format_g16_b16_r16_3plane_444_unorm = 1000156033,
    moon_format_g8_b8r8_2plane_444_unorm = 1000330000,
    moon_format_g10x6_b10x6r10x6_2plane_444_unorm_3pack16 = 1000330001,
    moon_format_g12x4_b12x4r12x4_2plane_444_unorm_3pack16 = 1000330002,
    moon_format_g16_b16r16_2plane_444_unorm = 1000330003,
    moon_format_a4r4g4b4_unorm_pack16 = 1000340000,
    moon_format_a4b4g4r4_unorm_pack16 = 1000340001,
    moon_format_astc_4x4_sfloat_block = 1000066000,
    moon_format_astc_5x4_sfloat_block = 1000066001,
    moon_format_astc_5x5_sfloat_block = 1000066002,
    moon_format_astc_6x5_sfloat_block = 1000066003,
    moon_format_astc_6x6_sfloat_block = 1000066004,
    moon_format_astc_8x5_sfloat_block = 1000066005,
    moon_format_astc_8x6_sfloat_block = 1000066006,
    moon_format_astc_8x8_sfloat_block = 1000066007,
    moon_format_astc_10x5_sfloat_block = 1000066008,
    moon_format_astc_10x6_sfloat_block = 1000066009,
    moon_format_astc_10x8_sfloat_block = 1000066010,
    moon_format_astc_10x10_sfloat_block = 1000066011,
    moon_format_astc_12x10_sfloat_block = 1000066012,
    moon_format_astc_12x12_sfloat_block = 1000066013,
    moon_format_pvrtc1_2bpp_unorm_block_img = 1000054000,
    moon_format_pvrtc1_4bpp_unorm_block_img = 1000054001,
    moon_format_pvrtc2_2bpp_unorm_block_img = 1000054002,
    moon_format_pvrtc2_4bpp_unorm_block_img = 1000054003,
    moon_format_pvrtc1_2bpp_srgb_block_img = 1000054004,
    moon_format_pvrtc1_4bpp_srgb_block_img = 1000054005,
    moon_format_pvrtc2_2bpp_srgb_block_img = 1000054006,
    moon_format_PVRTc2_4bpp_srgb_block_img = 1000054007,
} moon_format;

typedef enum moon_color_space : s32 {
    moon_color_space_srgb_nonlinear             = 0,
    moon_color_space_display_p3_nonlinear       = 1000104001,
    moon_color_space_extended_srgb_linear       = 1000104002,
    moon_color_space_display_p3_linear          = 1000104003,
    moon_color_space_dci_p3_nonlinear           = 1000104004,
    moon_color_space_bt709_linear               = 1000104005,
    moon_color_space_bt709_nonlinear            = 1000104006,
    moon_color_space_bt2020_linear              = 1000104007,
    moon_color_space_hdr10_st2084               = 1000104008,
    moon_color_space_dolbyvision                = 1000104009,
    moon_color_space_hdr10_hlg                  = 1000104010,
    moon_color_space_adobergb_linear            = 1000104011,
    moon_color_space_adobergb_nonlinear         = 1000104012,
    moon_color_space_pass_through               = 1000104013,
    moon_color_space_extended_srgb_nonlinear    = 1000104014,
    moon_color_space_display_native_amd         = 1000213000,
} moon_color_space;

typedef u8 moon_texture_flags;
typedef enum moon_texture_flag_bits : moon_texture_flags {
    moon_texture_flag_none                  = 0,
    moon_texture_flag_allow_mutable_format  = (1u << 0),
    moon_texture_flag_compatible_cube       = (1u << 1),
    moon_texture_flag_compatible_2d_array   = (1u << 2),
    moon_texture_flag_allow_alias           = (1u << 3),
} moon_texture_flag_bits;

typedef u16 moon_texture_aspect;
typedef enum moon_texture_aspect_bits : moon_texture_aspect {
    moon_texture_aspect_none            = (1u << 0),
    moon_texture_aspect_color           = (1u << 1),
    moon_texture_aspect_depth           = (1u << 2),
    moon_texture_aspect_stencil         = (1u << 3),
    moon_texture_aspect_metadata        = (1u << 4),
    moon_texture_aspect_plane_0         = (1u << 5),
    moon_texture_aspect_plane_1         = (1u << 6),
    moon_texture_aspect_plane_2         = (1u << 7),
    moon_texture_aspect_memory_plane_0  = (1u << 8),
    moon_texture_aspect_memory_plane_1  = (1u << 9),
    moon_texture_aspect_memory_plane_2  = (1u << 10),
    moon_texture_aspect_memory_plane_3  = (1u << 11),
} moon_texture_aspect_bits;

typedef u16 moon_texture_usage;
typedef enum moon_texture_usage_bits : moon_texture_usage {
    moon_texture_usage_none                             = 0,
    moon_texture_usage_transfer_src                     = (1u << 0),
    moon_texture_usage_transfer_dst                     = (1u << 1),
    moon_texture_usage_shader_sampled                   = (1u << 2),
    moon_texture_usage_shader_storage                   = (1u << 3),
    moon_texture_usage_color_attachment                 = (1u << 4),
    moon_texture_usage_depth_stencil_attachment         = (1u << 5),
    moon_texture_usage_transient_attachment             = (1u << 6),
    moon_texture_usage_fragment_density_map             = (1u << 7),
    moon_texture_usage_fragment_shading_rate_attachment = (1u << 8),
} moon_texture_usage_bits;

typedef enum moon_texture_layout : s8 {
    moon_texture_layout_undefined = 0,
    moon_texture_layout_general,
    moon_texture_layout_transfer_src_optimal,
    moon_texture_layout_transfer_dst_optimal,
    moon_texture_layout_read_only_optimal,
    moon_texture_layout_attachment_optimal,
    moon_texture_layout_present_src,
} moon_texture_layout;

typedef u8 moon_resolve_mode;
typedef enum moon_resolve_mode_bits : moon_resolve_mode {
    moon_resolve_mode_none                          = 0,
    moon_resolve_mode_sample_zero                   = (1u << 0),
    moon_resolve_mode_average                       = (1u << 1),
    moon_resolve_mode_min                           = (1u << 2),
    moon_resolve_mode_max                           = (1u << 3),
    moon_resolve_mode_external_downsample_android   = (1u << 4),
} moon_resolve_mode_bits;

typedef struct moon_texture_mip_array_slice {
    u32 base_mip_level;
    u32 level_count;
    u32 base_array_layer;
    u32 layer_count;
} moon_texture_mip_array_slice;

typedef struct moon_texture_array_slice {
    u32 mip_level;
    u32 base_array_layer;
    u32 layer_count;
} moon_texture_array_slice;

typedef struct moon_texture_slice {
    u32 mip_level;
    u32 array_layer;
} moon_texture_slice;

typedef struct moon_texture_assembly {
    moon_texture_usage      usage;
    moon_texture_flags      flags;
    moon_memory_flags       memory_flags; /** Ignored if allocating from existing memory heap. */
    moon_sharing_mode       sharing_mode;
    u8                      dimensions;
    u8                  pad0[6];
    moon_format             format;
    lake_extent3d           extent;
    u32                     mip_level_count;
    u32                     array_layer_count;
    u32                     sample_count;
    lake_small_string       name;
} moon_texture_assembly;
static constexpr moon_texture_assembly MOON_TEXTURE_ASSEMBLY_INIT = {
    .usage = moon_texture_usage_none,
    .flags = moon_texture_flag_none,
    .memory_flags = moon_memory_flag_none,
    .sharing_mode = moon_sharing_mode_concurrent,
    .dimensions = 2,
    .format = moon_format_r8g8b8a8_srgb,
    .extent = {0u, 0u, 0u},
    .mip_level_count = 1,
    .array_layer_count = 1,
    .sample_count = 1,
};

typedef struct moon_texture_assembly_from_heap {
    moon_texture_assembly   texture_assembly;
    moon_device_heap        device_heap;
    u64                     offset;
} moon_texture_assembly_from_heap;
static constexpr moon_texture_assembly_from_heap MOON_TEXTURE_ASSEMBLY_FROM_HEAP_INIT = {
    .texture_assembly = MOON_TEXTURE_ASSEMBLY_INIT,
    .device_heap = nullptr,
    .offset = 0lu,
};

/** Get GPU memory requirements for a texture. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_texture_memory_requirements)(moon_device device, moon_texture_assembly const *assembly, moon_memory_requirements *out_requirements);
#define FN_MOON_DEVICE_TEXTURE_MEMORY_REQUIREMENTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_texture_memory_requirements(moon_device device, moon_texture_assembly const *assembly, moon_memory_requirements *out_requirements)

/** Create a texture on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture)(moon_device device, moon_texture_assembly const *assembly, moon_texture_id *out_texture);
#define FN_MOON_CREATE_TEXTURE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture(moon_device device, moon_texture_assembly const *assembly, moon_texture_id *out_texture)

/** Create a texture from memory on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture_from_heap)(moon_device device, moon_texture_assembly_from_heap const *assembly, moon_texture_id *out_texture);
#define FN_MOON_CREATE_TEXTURE_FROM_HEAP(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture_from_heap(moon_device device, moon_texture_assembly_from_heap const *assembly, moon_texture_id *out_texture)

/** The texture handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_texture_valid)(moon_device device, moon_texture_id texture);
#define FN_MOON_IS_TEXTURE_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_texture_valid(moon_device device, moon_texture_id texture)

/** The texture will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_texture)(moon_device device, moon_texture_id texture);
#define FN_MOON_DESTROY_TEXTURE(backend) \
    void LAKECALL _moon_##backend##_destroy_texture(moon_device device, moon_texture_id texture)

typedef enum moon_texture_view_type : s8 {
    moon_texture_view_type_1d = 0,
    moon_texture_view_type_2d,
    moon_texture_view_type_3d,
    moon_texture_view_type_cube,
    moon_texture_view_type_1d_array,
    moon_texture_view_type_2d_array,
    moon_texture_view_type_cube_array,
} moon_texture_view_type;

typedef struct moon_texture_view_assembly {
    moon_format                     format;
    moon_texture_view_type          type;
    u8                          pad0[3];
    moon_texture_mip_array_slice    slice;
    moon_texture_id                 texture;
    lake_small_string               name;
} moon_texture_view_assembly;
static constexpr moon_texture_view_assembly MOON_TEXTURE_VIEW_ASSEMBLY_INIT = {
    .format = moon_format_r8g8b8a8_srgb,
    .type = moon_texture_view_type_2d,
    .slice = {0},
    .texture = {0},
};

/** Create a texture view on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture_view)(moon_device device, moon_texture_view_assembly const *assembly, moon_texture_view_id *out_texture_view);
#define FN_MOON_CREATE_TEXTURE_VIEW(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture_view(moon_device device, moon_texture_view_assembly const *assembly, moon_texture_view_id *out_texture_view)

/** The texture view handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_texture_view_valid)(moon_device device, moon_texture_view_id texture_view);
#define FN_MOON_IS_TEXTURE_VIEW_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_texture_view_valid(moon_device device, moon_texture_view_id texture_view)

/** The texture view will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_texture_view)(moon_device device, moon_texture_view_id texture_view);
#define FN_MOON_DESTROY_TEXTURE_VIEW(backend) \
    void LAKECALL _moon_##backend##_destroy_texture_view(moon_device device, moon_texture_view_id texture_view)

typedef struct moon_sampled_texture {
    moon_texture_view_id        texture_view;
    moon_sampler_id             sampler;
} moon_sampled_texture;

typedef enum moon_filter_mode : s8 {
    moon_filter_mode_nearest = 0,
    moon_filter_mode_linear,
} moon_filter_mode;

typedef enum moon_sampler_address_mode : s8 {
    moon_sampler_address_mode_wrap = 0,
    moon_sampler_address_mode_mirror,
    moon_sampler_address_mode_clamp_edge,
    moon_sampler_address_mode_clamp_border,
} moon_sampler_address_mode;

typedef enum moon_sampler_reduction_mode : s8 {
    moon_sampler_reduction_mode_weighted_average = 0,
    moon_sampler_reduction_mode_min,
    moon_sampler_reduction_mode_max,
} moon_sampler_reduction_mode;

typedef u8 moon_sample_count;
typedef enum moon_sample_count_bits : moon_sample_count {
    moon_sample_count_1     = (1u << 0),   
    moon_sample_count_2     = (1u << 1),   
    moon_sample_count_4     = (1u << 2),   
    moon_sample_count_8     = (1u << 3),   
    moon_sample_count_16    = (1u << 4),   
    moon_sample_count_32    = (1u << 5),   
    moon_sample_count_64    = (1u << 6),   
} moon_sample_count_bits;

typedef struct moon_sampler_assembly {
    moon_filter_mode            magnification_filter;
    moon_filter_mode            minification_filter;
    moon_filter_mode            mipmap_filter;
    moon_sampler_reduction_mode reduction_mode;
    moon_sampler_address_mode   address_mode_u;
    moon_sampler_address_mode   address_mode_v;
    moon_sampler_address_mode   address_mode_w;
    moon_compare_op             compare_op;
    f32                         mip_lod_bias;
    f32                         min_lod;
    f32                         max_lod;
    f32                         max_anisotrophy;
    moon_border_color           border_color;
    bool                        enable_anisotrophy;
    bool                        enable_compare_op;
    bool                        enable_unnormalized_coordinates;
    lake_small_string           name;
} moon_sampler_assembly;
static constexpr moon_sampler_assembly MOON_SAMPLER_ASSEMBLY_INIT = {
    .magnification_filter = moon_filter_mode_linear,
    .minification_filter = moon_filter_mode_linear,
    .mipmap_filter = moon_filter_mode_linear,
    .reduction_mode = moon_sampler_reduction_mode_weighted_average,
    .address_mode_u = moon_sampler_address_mode_clamp_edge,
    .address_mode_v = moon_sampler_address_mode_clamp_edge,
    .address_mode_w = moon_sampler_address_mode_clamp_edge,
    .compare_op = moon_compare_op_always,
    .mip_lod_bias = 0.5f,
    .min_lod = 0.0f,
    .max_lod = 1000.0f,
    .max_anisotrophy = 0.0f,
    .border_color = moon_border_color_int_transparent_black,
    .enable_anisotrophy = false,
    .enable_compare_op = false,
    .enable_unnormalized_coordinates = false,
};

/** Create a sampler on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_sampler)(moon_device device, moon_sampler_assembly const *assembly, moon_sampler_id *out_sampler);
#define FN_MOON_CREATE_SAMPLER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_sampler(moon_device device, moon_sampler_assembly const *assembly, moon_sampler_id *out_sampler)

/** The sampler handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_sampler_valid)(moon_device device, moon_sampler_id sampler);
#define FN_MOON_IS_SAMPLER_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_sampler_valid(moon_device device, moon_sampler_id sampler)

/** The sampler will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_sampler)(moon_device device, moon_sampler_id sampler);
#define FN_MOON_DESTROY_SAMPLER(backend) \
    void LAKECALL _moon_##backend##_destroy_sampler(moon_device device, moon_sampler_id sampler)

typedef struct moon_acceleration_structure {
    u64                     size;
    lake_small_string       name;
} moon_acceleration_structure;

typedef struct moon_strided_address_region {
    moon_device_address         data;
    u64                         stride;
    u64                         size;
} moon_strided_address_region;

typedef struct moon_shader_binding_table {
    moon_strided_address_region raygen_region;
    moon_strided_address_region miss_region;
    moon_strided_address_region hit_region;
    moon_strided_address_region callable_region;
} moon_shader_binding_table;

typedef u8 moon_geometry_flags;
typedef enum moon_geometry_flag_bits : moon_geometry_flags {
    moon_geometry_flag_opaque                           = (1u << 0),
    moon_geometry_flag_no_duplicate_any_hit_invocation  = (1u << 1),
} moon_geometry_flag_bits;

typedef struct moon_blas_triangle_geometry {
    moon_device_address vertex_data;
    u64                 vertex_stride;
    moon_format         vertex_format;
    u32                 max_vertex;
    moon_device_address transform_data;
    moon_device_address index_data;
    moon_index_format   index_format;
    moon_geometry_flags geometry_flags;
    u8              pad0[2];
    u32                 count;
} moon_blas_triangle_geometry;
static constexpr moon_blas_triangle_geometry MOON_BLAS_TRIANGLE_GEOMETRY_INIT = {
    .vertex_data = 0,
    .vertex_stride = 12,
    .vertex_format = moon_format_r32g32b32_sfloat,
    .max_vertex = 0,
    .transform_data = 0,
    .index_data = 0,
    .index_format = moon_index_format_u32,
};

typedef struct moon_blas_aabb_geometry {
    moon_device_address data;
    u64                 stride;
    moon_geometry_flags geometry_flags;
    u8              pad0[3];
    u32                 count;
} moon_blas_aabb_geometry;
static constexpr moon_blas_aabb_geometry MOON_BLAS_AABB_GEOMETRY_INIT = {
    .data = 0,
    .stride = 12,
    .geometry_flags = moon_geometry_flag_opaque,
    .count = 0,
};

typedef struct moon_blas_triangle_geometry_span {
    moon_blas_triangle_geometry const  *span;
    usize                               count;
} moon_blas_triangle_geometry_span;

typedef struct moon_blas_aabb_geometry_span { 
    moon_blas_aabb_geometry const      *span; 
    usize                               count;
} moon_blas_aabb_geometry_span;

typedef union moon_blas_geometry_spans {
    moon_blas_triangle_geometry_span    triangle;
    moon_blas_aabb_geometry_span        aabb;
} moon_blas_geometry_spans;

typedef struct {
    moon_device_address                 data;
    bool                                data_is_array_of_pointers;
    moon_geometry_flags                 geometry_flags;
    u8                              pad0[2];
    u32                                 count;
} moon_tlas_instance;
static constexpr moon_tlas_instance MOON_TLAS_INSTANCE_INIT = {
    .data = 0,
    .data_is_array_of_pointers = false,
    .geometry_flags = moon_geometry_flag_opaque,
    .count = 0,
};

typedef u8 moon_build_acceleration_structure_flags;
typedef enum moon_build_acceleration_structure_flag_bits : moon_build_acceleration_structure_flags {
    moon_build_acceleration_structure_flag_allow_update         = (1u << 0),
    moon_build_acceleration_structure_flag_allow_compaction     = (1u << 1),
    moon_build_acceleration_structure_flag_prefer_fast_trace    = (1u << 2),
    moon_build_acceleration_structure_flag_prefer_fast_build    = (1u << 3),
    moon_build_acceleration_structure_flag_low_memory           = (1u << 4),
} moon_build_acceleration_structure_flag_bits;

typedef struct moon_build_acceleration_structure_sizes {
    u64 acceleration_structure_size;
    u64 update_scratch_size;
    u64 build_scratch_size;
} moon_build_acceleration_structure_sizes;

typedef struct moon_tlas_assembly {
    moon_tlas_id                            src_tlas;
    moon_tlas_id                            dst_tlas;
    moon_tlas_instance const               *instances;
    u32                                     instance_count;
    moon_build_acceleration_structure_flags flags;
    bool                                    update;
    u8                                  pad0[2];
    moon_device_address                     scratch_data;
} moon_tlas_assembly;
static constexpr moon_tlas_assembly MOON_TLAS_ASSEMBLY_INIT = {
    .src_tlas = {0},
    .dst_tlas = {0},
    .instances = nullptr,
    .instance_count = 0,
    .flags = moon_build_acceleration_structure_flag_prefer_fast_trace,
    .update = 0,
    .scratch_data = 0,
};

typedef struct moon_tlas_assembly_from_buffer {
    moon_tlas_assembly      tlas_assembly;
    moon_buffer_id          buffer;
    u64                     offset;
} moon_tlas_assembly_from_buffer;
static constexpr moon_tlas_assembly_from_buffer MOON_TLAS_ASSEMBLY_FROM_BUFFER_INIT = {
    .tlas_assembly = MOON_TLAS_ASSEMBLY_INIT,
    .buffer = {0},
    .offset = 0,
};

/** Create a top-level on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_tlas)(moon_device device, moon_tlas_assembly const *assembly, moon_tlas_id *out_tlas);
#define FN_MOON_CREATE_TLAS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_tlas(moon_device device, moon_tlas_assembly const *assembly, moon_tlas_id *out_tlas)

/** Create a top-level from buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_tlas_from_buffer)(moon_device device, moon_tlas_assembly_from_buffer const *assembly, moon_tlas_id *out_tlas);
#define FN_MOON_CREATE_TLAS_FROM_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_tlas_from_buffer(moon_device device, moon_tlas_assembly_from_buffer const *assembly, moon_tlas_id *out_tlas)

/** The tlas-level handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_tlas_valid)(moon_device device, moon_tlas_id tlas);
#define FN_MOON_IS_TLAS_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_tlas_valid(moon_device device, moon_tlas_id tlas)

/** Returns the device address of an top-level acceleration structure handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_tlas_device_address)(moon_device device, moon_tlas_id tlas, moon_device_address *out_device_address);
#define FN_MOON_TLAS_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_tlas_device_address(moon_device device, moon_tlas_id tlas, moon_device_address *out_device_address)

/** The top-level will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_tlas)(moon_device device, moon_tlas_id tlas);
#define FN_MOON_DESTROY_TLAS(backend) \
    void LAKECALL _moon_##backend##_destroy_tlas(moon_device device, moon_tlas_id tlas)

typedef struct moon_blas_assembly {
    moon_blas_id                            src_blas;
    moon_blas_id                            dst_blas;
    moon_blas_geometry_spans                geometries;
    u8                                      geometry_variant;
    moon_build_acceleration_structure_flags flags;
    bool                                    update;
    u8                                  pad0[5];
    moon_device_address                     scratch_data;
} moon_blas_assembly;
static constexpr moon_blas_assembly MOON_BLAS_ASSEMBLY_INIT = {
    .flags = moon_build_acceleration_structure_flag_prefer_fast_trace,
};

typedef struct moon_blas_assembly_from_buffer {
    moon_blas_assembly      blas_assembly;
    moon_buffer_id          buffer;
    u64                     offset;
} moon_blas_assembly_from_buffer;
static constexpr moon_blas_assembly_from_buffer MOON_BLAS_ASSEMBLY_FROM_BUFFER_INIT = {
    .blas_assembly = MOON_BLAS_ASSEMBLY_INIT,
    .buffer = {0},
    .offset = 0,
};

/** Create a bottom-level on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_blas)(moon_device device, moon_blas_assembly const *assembly, moon_blas_id *out_blas);
#define FN_MOON_CREATE_BLAS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_blas(moon_device device, moon_blas_assembly const *assembly, moon_blas_id *out_blas)

/** Create a bottom-level from buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_blas_from_buffer)(moon_device device, moon_blas_assembly_from_buffer const *assembly, moon_blas_id *out_blas);
#define FN_MOON_CREATE_BLAS_FROM_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_blas_from_buffer(moon_device device, moon_blas_assembly_from_buffer const *assembly, moon_blas_id *out_blas)

/** The bottom-level handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_blas_valid)(moon_device device, moon_blas_id blas);
#define FN_MOON_IS_BLAS_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_blas_valid(moon_device device, moon_blas_id blas)

/** Returns the device address of an bottom-level acceleration structure handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_blas_device_address)(moon_device device, moon_blas_id blas, moon_device_address *out_device_address);
#define FN_MOON_BLAS_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_blas_device_address(moon_device device, moon_blas_id blas, moon_device_address *out_device_address)

/** The bottom-level will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_blas)(moon_device device, moon_blas_id blas);
#define FN_MOON_DESTROY_BLAS(backend) \
    void LAKECALL _moon_##backend##_destroy_blas(moon_device device, moon_blas_id blas)

/* moon_pipeline_stages + moon_access_type */
typedef u32 moon_access;

static constexpr moon_access MOON_ACCESS_NONE = moon_pipeline_stage_none | moon_access_type_none;
/* READ */
static constexpr moon_access MOON_ACCESS_TOP_OF_PIPE_READ = moon_pipeline_stage_top_of_pipe | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_DRAW_INDIRECT_READ = moon_pipeline_stage_draw_indirect | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_VERTEX_SHADER_READ = moon_pipeline_stage_vertex_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_TESSELATION_CONTROL_SHADER_READ = moon_pipeline_stage_tesselation_control_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_TESSELATION_EVALUATION_SHADER_READ = moon_pipeline_stage_tesselation_evaluation_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_GEOMETRY_SHADER_READ = moon_pipeline_stage_geometry_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_FRAGMENT_SHADER_READ = moon_pipeline_stage_fragment_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_EARLY_FRAGMENT_TESTS_READ = moon_pipeline_stage_early_fragment_tests | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_LATE_FRAGMENT_TESTS_READ = moon_pipeline_stage_late_fragment_tests | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_COLOR_ATTACHMENT_OUTPUT_READ = moon_pipeline_stage_color_attachment_output | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_COMPUTE_SHADER_READ = moon_pipeline_stage_compute_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_TRANSFER_READ = moon_pipeline_stage_transfer | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_BOTTOM_OF_PIPE_READ = moon_pipeline_stage_bottom_of_pipe | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_HOST_READ = moon_pipeline_stage_host | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_ALL_GRAPHICS_READ = moon_pipeline_stage_all_graphics | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_READ = moon_pipeline_stage_all_commands | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_COPY_READ = moon_pipeline_stage_copy | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_RESOLVE_READ = moon_pipeline_stage_resolve | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_BLIT_READ = moon_pipeline_stage_blit | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_CLEAR_READ = moon_pipeline_stage_clear | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_INDEX_INPUT_READ = moon_pipeline_stage_index_input | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_PRE_RASTERIZATION_SHADERS_READ = moon_pipeline_stage_pre_rasterization_shaders | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_TASK_SHADER_READ = moon_pipeline_stage_task_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_MESH_SHADER_READ = moon_pipeline_stage_mesh_shader | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_ACCELERATION_STRUCTURE_BUILD_READ = moon_pipeline_stage_acceleration_structure_build | moon_access_type_read;
static constexpr moon_access MOON_ACCESS_RAY_TRACING_SHADER_READ = moon_pipeline_stage_ray_tracing_shader | moon_access_type_read;
/* WRITE */
static constexpr moon_access MOON_ACCESS_TOP_OF_PIPE_WRITE = moon_pipeline_stage_top_of_pipe | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_DRAW_INDIRECT_WRITE = moon_pipeline_stage_draw_indirect | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_VERTEX_SHADER_WRITE = moon_pipeline_stage_vertex_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_TESSELATION_CONTROL_SHADER_WRITE = moon_pipeline_stage_tesselation_control_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_TESSELATION_EVALUATION_SHADER_WRITE = moon_pipeline_stage_tesselation_evaluation_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_GEOMETRY_SHADER_WRITE = moon_pipeline_stage_geometry_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_FRAGMENT_SHADER_WRITE = moon_pipeline_stage_fragment_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_EARLY_FRAGMENT_TESTS_WRITE = moon_pipeline_stage_early_fragment_tests | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_LATE_FRAGMENT_TESTS_WRITE = moon_pipeline_stage_late_fragment_tests | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_COLOR_ATTACHMENT_OUTPUT_WRITE = moon_pipeline_stage_color_attachment_output | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_COMPUTE_SHADER_WRITE = moon_pipeline_stage_compute_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_TRANSFER_WRITE = moon_pipeline_stage_transfer | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_BOTTOM_OF_PIPE_WRITE = moon_pipeline_stage_bottom_of_pipe | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_HOST_WRITE = moon_pipeline_stage_host | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_ALL_GRAPHICS_WRITE = moon_pipeline_stage_all_graphics | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_WRITE = moon_pipeline_stage_all_commands | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_COPY_WRITE = moon_pipeline_stage_copy | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_RESOLVE_WRITE = moon_pipeline_stage_resolve | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_BLIT_WRITE = moon_pipeline_stage_blit | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_CLEAR_WRITE = moon_pipeline_stage_clear | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_INDEX_INPUT_WRITE = moon_pipeline_stage_index_input | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_PRE_RASTERIZATION_SHADERS_WRITE = moon_pipeline_stage_pre_rasterization_shaders | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_TASK_SHADER_WRITE = moon_pipeline_stage_task_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_MESH_SHADER_WRITE = moon_pipeline_stage_mesh_shader | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_ACCELERATION_STRUCTURE_BUILD_WRITE = moon_pipeline_stage_acceleration_structure_build | moon_access_type_write;
static constexpr moon_access MOON_ACCESS_RAY_TRACING_SHADER_WRITE = moon_pipeline_stage_ray_tracing_shader | moon_access_type_write;
/* READ/WRITE */
static constexpr moon_access MOON_ACCESS_TOP_OF_PIPE_READ_WRITE = moon_pipeline_stage_top_of_pipe | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_DRAW_INDIRECT_READ_WRITE = moon_pipeline_stage_draw_indirect | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_VERTEX_SHADER_READ_WRITE = moon_pipeline_stage_vertex_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_TESSELATION_CONTROL_SHADER_READ_WRITE = moon_pipeline_stage_tesselation_control_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_TESSELATION_EVALUATION_SHADER_READ_WRITE = moon_pipeline_stage_tesselation_evaluation_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_GEOMETRY_SHADER_READ_WRITE = moon_pipeline_stage_geometry_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_FRAGMENT_SHADER_READ_WRITE = moon_pipeline_stage_fragment_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_EARLY_FRAGMENT_TESTS_READ_WRITE = moon_pipeline_stage_early_fragment_tests | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_LATE_FRAGMENT_TESTS_READ_WRITE = moon_pipeline_stage_late_fragment_tests | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_COLOR_ATTACHMENT_OUTPUT_READ_WRITE = moon_pipeline_stage_color_attachment_output | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_COMPUTE_SHADER_READ_WRITE = moon_pipeline_stage_compute_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_TRANSFER_READ_WRITE = moon_pipeline_stage_transfer | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_BOTTOM_OF_PIPE_READ_WRITE = moon_pipeline_stage_bottom_of_pipe | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_HOST_READ_WRITE = moon_pipeline_stage_host | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_ALL_GRAPHICS_READ_WRITE = moon_pipeline_stage_all_graphics | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_READ_WRITE = moon_pipeline_stage_all_commands | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_COPY_READ_WRITE = moon_pipeline_stage_copy | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_RESOLVE_READ_WRITE = moon_pipeline_stage_resolve | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_BLIT_READ_WRITE = moon_pipeline_stage_blit | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_CLEAR_READ_WRITE = moon_pipeline_stage_clear | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_INDEX_INPUT_READ_WRITE = moon_pipeline_stage_index_input | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_PRE_RASTERIZATION_SHADERS_READ_WRITE = moon_pipeline_stage_pre_rasterization_shaders | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_TASK_SHADER_READ_WRITE = moon_pipeline_stage_task_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_MESH_SHADER_READ_WRITE = moon_pipeline_stage_mesh_shader | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_ACCELERATION_STRUCTURE_BUILD_READ_WRITE = moon_pipeline_stage_acceleration_structure_build | moon_access_type_read_write;
static constexpr moon_access MOON_ACCESS_RAY_TRACING_SHADER_READ_WRITE = moon_pipeline_stage_ray_tracing_shader | moon_access_type_read_write;

/** Pipeline barrier used for memory access. */
typedef struct {
    moon_access                     src_access;
    moon_access                     dst_access;
} moon_memory_barrier;

/** Pipeline barrier used for image access and transitions. */
typedef struct {
    moon_memory_barrier             barrier;
    moon_texture_layout             src_layout;
    moon_texture_layout             dst_layout;
    moon_texture_mip_array_slice    texture_slice;
    moon_texture_id                 texture;
} moon_texture_memory_barrier;

/** Details needed to create a timeline query pool. */
typedef struct {
    u32                             query_count;
    lake_small_string               name;
} moon_timeline_query_pool_assembly;
static constexpr moon_timeline_query_pool_assembly MOON_TIMELINE_QUERY_POOL_ASSEMBLY_INIT = {0};

PFN_LAKE_HANDLE_ASSEMBLY(moon, timeline_query_pool, device);
#define FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(backend) \
    FN_LAKE_HANDLE_ASSEMBLY(backend, moon, timeline_query_pool, device)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, timeline_query_pool);
#define FN_MOON_TIMELINE_QUERY_POOL_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, timeline_query_pool)

/** Details needed to create a timeline semaphore. */
typedef struct {
    u64                             initial_value;
    lake_small_string               name;
} moon_timeline_semaphore_assembly;
static constexpr moon_timeline_semaphore_assembly MOON_TIMELINE_SEMAPHORE_ASSEMBLY_INIT = {0};

PFN_LAKE_HANDLE_ASSEMBLY(moon, timeline_semaphore, device);
#define FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(backend) \
    FN_LAKE_HANDLE_ASSEMBLY(backend, moon, timeline_semaphore, device)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, timeline_semaphore);
#define FN_MOON_TIMELINE_SEMAPHORE_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, timeline_semaphore)

/** Details needed to create a binary semaphore. */
typedef struct {
    lake_small_string               name;
} moon_binary_semaphore_assembly;
static constexpr moon_binary_semaphore_assembly MOON_BINARY_SEMAPHORE_ASSEMBLY_INIT = {0};

PFN_LAKE_HANDLE_ASSEMBLY(moon, binary_semaphore, device);
#define FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(backend) \
    FN_LAKE_HANDLE_ASSEMBLY(backend, moon, binary_semaphore, device)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, binary_semaphore);
#define FN_MOON_BINARY_SEMAPHORE_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, binary_semaphore)

typedef u8 moon_shader_create_flags;
typedef enum moon_shader_create_flag_bits : moon_shader_create_flags {
    moon_shader_create_flag_none                        = 0u,
    moon_shader_create_flag_allow_varying_subgroup_size = (1u << 0),
    moon_shader_create_flag_require_full_subgroups      = (1u << 1),
} moon_shader_create_flag_bits;

typedef struct moon_shader_assembly {
    u32 const                  *byte_code;
    u32                         byte_code_size;
    moon_shader_create_flags    create_flags;
    u8                      pad0[6];
    bool                        has_required_subgroup_size;
    u32                         required_subgroup_size;
    lake_small_string           entry_point;
} moon_shader_assembly;
static constexpr moon_shader_assembly MOON_SHADER_ASSEMBLY_INIT = {
    .byte_code = nullptr,
    .byte_code_size = 0,
    .create_flags = moon_shader_create_flag_none,
    .has_required_subgroup_size = false, 
    .required_subgroup_size = 0,
};

typedef struct moon_compute_pipeline_assembly {
    moon_shader_assembly        shader;
    u32                         push_constant_size;
    u8                      pad0[4];
    lake_small_string           name;
} moon_compute_pipeline_assembly;
static constexpr moon_compute_pipeline_assembly MOON_COMPUTE_PIPELINE_ASSEMBLY_INIT = {
    .shader = MOON_SHADER_ASSEMBLY_INIT,
    .push_constant_size = MOON_MAX_PUSH_CONSTANT_BYTE_SIZE,
};

/** Assemble an array of compute pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_compute_pipelines_assembly)(moon_device device, u32 count, moon_compute_pipeline_assembly const *assembly_array, moon_compute_pipeline *out_pipelines);
#define FN_MOON_COMPUTE_PIPELINES_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_compute_pipelines_assembly(moon_device device, u32 count, moon_compute_pipeline_assembly const *assembly_array, moon_compute_pipeline *out_pipelines)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, compute_pipeline);
#define FN_MOON_COMPUTE_PIPELINE_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, compute_pipeline)

typedef enum moon_work_graph_node_type : s8 {
    moon_work_graph_node_type_compute = 0,
    moon_work_graph_node_type_mesh,
} moon_work_graph_node_type;

typedef union moon_work_graph_node {
    moon_compute_pipeline compute;
    moon_raster_pipeline  mesh;
} moon_work_graph_node;

typedef struct {
    moon_shader_assembly const *shaders; /* compute */
    usize                       shader_count;
    moon_work_graph_node const *nodes;
    u32                         node_count;
    u32                         push_constant_size;
    lake_small_string           name;
} moon_work_graph_pipeline_assembly;
static constexpr moon_work_graph_pipeline_assembly MOON_WORK_GRAPH_PIPELINE_ASSEMBLY_INIT = {
    .shaders = nullptr,
    .shader_count = 0,
    .nodes = nullptr,
    .node_count = 0,
    .push_constant_size = MOON_MAX_PUSH_CONSTANT_BYTE_SIZE,
};

/** Assemble an array of work graph pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_work_graph_pipelines_assembly)(moon_device device, u32 count, moon_work_graph_pipeline_assembly const *assembly_array, moon_work_graph_pipeline *out_pipelines);
#define FN_MOON_WORK_GRAPH_PIPELINES_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_work_graph_pipelines_assembly(moon_device device, u32 count, moon_work_graph_pipeline_assembly const *assembly_array, moon_work_graph_pipeline *out_pipelines)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, work_graph_pipeline);
#define FN_MOON_WORK_GRAPH_PIPELINE_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, work_graph_pipeline)

typedef struct moon_depth_test {
    moon_format     depth_attachment_format;
    bool            enable_depth_write;
    moon_compare_op depth_test_compare_op;
    f32             min_depth_bounds;
    f32             max_depth_bounds;
} moon_depth_test;
static constexpr moon_depth_test MOON_DEPTH_TEST_INIT = {
    .depth_attachment_format = moon_format_undefined,
    .enable_depth_write = 0,
    .depth_test_compare_op = moon_compare_op_less_or_equal,
    .min_depth_bounds = 0.0f,
    .max_depth_bounds = 1.0f,
};

typedef struct moon_stencil_test {
    u8              test_mask;
    u8              write_mask;
    moon_compare_op stencil_compare_op;
    moon_stencil_op stencil_op_fail;
    moon_stencil_op stencil_op_depth_fail;
    moon_stencil_op stencil_op_depth_pass;
} moon_stencil_test;
static constexpr moon_stencil_test MOON_STENCIL_TEST_INIT = {
    .test_mask = 0xffu,
    .write_mask = 0u,
    .stencil_compare_op = moon_compare_op_always,
    .stencil_op_fail = moon_stencil_op_keep,
    .stencil_op_depth_fail = moon_stencil_op_keep,
    .stencil_op_depth_pass = moon_stencil_op_keep,
};

typedef enum moon_primitive_topology : s8 {
    moon_primitive_topology_point_list = 0,
    moon_primitive_topology_line_list,
    moon_primitive_topology_line_strip,
    moon_primitive_topology_triangle_list,
    moon_primitive_topology_triangle_strip,
    moon_primitive_topology_triangle_fan,
    moon_primitive_topology_line_list_with_adjacency,
    moon_primitive_topology_line_strip_with_adjacency,
    moon_primitive_topology_triangle_list_with_adjacency,
    moon_primitive_topology_triangle_strip_with_adjacency,
    moon_primitive_topology_patch_list,
} moon_primitive_topology;

typedef enum moon_polygon_mode : s8 {
    moon_polygon_mode_fill = 0,
    moon_polygon_mode_line,
    moon_polygon_mode_point,
} moon_polygon_mode;

typedef enum moon_front_face_winding : s8 {
    moon_front_face_winding_counter_clockwise = 0,
    moon_front_face_winding_clockwise,
} moon_front_face_winding;

typedef enum moon_face_cull : u8 {
    moon_face_cull_none  = 0,
    moon_face_cull_front = (1u << 0),
    moon_face_cull_back  = (1u << 1),
    moon_face_cull_front_and_back = moon_face_cull_front | moon_face_cull_back,
} moon_face_cull;

typedef enum moon_conservative_rasterizer_mode : s8 {
    moon_conservative_rasterizer_mode_disabled = 0,
    moon_conservative_rasterizer_mode_overestimate,
    moon_conservative_rasterizer_mode_underestimate,
} moon_conservative_rasterizer_mode;

typedef struct moon_conservative_rasterizer {
    moon_conservative_rasterizer_mode   mode;
    f32                                 size;
} moon_conservative_rasterizer;

typedef struct moon_rasterizer {
    moon_primitive_topology         primitive_topology;
    moon_polygon_mode               polygon_mode;
    moon_face_cull                  face_culling;
    moon_front_face_winding         front_face_winding;
    bool                            enable_primitive_restart;
    bool                            enable_rasterizer_discard;
    bool                            enable_depth_clamp;
    bool                            enable_depth_bias;
    f32                             depth_bias_constant_factor;
    f32                             depth_bias_clamp;
    f32                             depth_bias_slope_factor;
    f32                             line_width;
    moon_conservative_rasterizer    conservative_rasterizer;
    bool                            has_conservative_rasterizer;
    u8                          pad0[1];
    moon_sample_count               static_state_sample_count;
    bool                            has_static_state_sample_count;
} moon_rasterizer;
static constexpr moon_rasterizer MOON_RASTERIZER_INIT = {
    .primitive_topology = moon_primitive_topology_triangle_list,
    .polygon_mode = moon_polygon_mode_fill,
    .face_culling = moon_face_cull_none,
    .front_face_winding = moon_front_face_winding_clockwise,
    .enable_primitive_restart = false,
    .enable_rasterizer_discard = false,
    .enable_depth_clamp = false,
    .enable_depth_bias = false,
    .depth_bias_constant_factor = 0.0f,
    .depth_bias_clamp = 0.0f,
    .depth_bias_slope_factor = 0.0f,
    .line_width = 1.0f,
    .conservative_rasterizer = { .mode = moon_conservative_rasterizer_mode_disabled, .size = 0 },
    .has_conservative_rasterizer = false,
    .static_state_sample_count = moon_sample_count_1,
    .has_static_state_sample_count = true,
};

typedef u8 moon_color_components;
typedef enum moon_color_component_bits : moon_color_components {
    moon_color_component_r = (1u << 0),
    moon_color_component_g = (1u << 1),
    moon_color_component_b = (1u << 2),
    moon_color_component_a = (1u << 3),
} moon_color_component_bits;

typedef enum moon_blend_op : s8 {
    moon_blend_op_none = 0,
    moon_blend_op_add,
    moon_blend_op_subtract,
    moon_blend_op_reverse_subtract,
    moon_blend_op_min,
    moon_blend_op_max,
} moon_blend_op;

typedef enum moon_blend_factor : s8 {
    moon_blend_factor_zero = 0,
    moon_blend_factor_one,
    moon_blend_factor_src_color,
    moon_blend_factor_one_minus_src_color,
    moon_blend_factor_dst_color,
    moon_blend_factor_one_minus_dst_color,
    moon_blend_factor_src_alpha,
    moon_blend_factor_one_minus_src_alpha,
    moon_blend_factor_dst_alpha,
    moon_blend_factor_one_minus_dst_alpha,
    moon_blend_factor_constant_color,
    moon_blend_factor_one_minus_constant_color,
    moon_blend_factor_constant_alpha,
    moon_blend_factor_one_minus_constant_alpha,
    moon_blend_factor_src_alpha_saturate,
    moon_blend_factor_src1_color,
    moon_blend_factor_one_minus_src1_color,
    moon_blend_factor_src1_alpha,
    moon_blend_factor_one_minus_src1_alpha,
} moon_blend_factor;

typedef struct moon_blend {
    moon_blend_factor       src_color_blend_factor;
    moon_blend_factor       dst_color_blend_factor;
    moon_blend_op           color_blend_op;
    moon_blend_factor       src_alpha_blend_factor;
    moon_blend_factor       dst_alpha_blend_factor;
    moon_blend_op           alpha_blend_op;
    moon_color_components   color_write_mask;
} moon_blend;
static constexpr moon_blend MOON_BLEND_INIT = {
    .src_color_blend_factor = moon_blend_factor_one,
    .dst_color_blend_factor = moon_blend_factor_zero,
    .color_blend_op = moon_blend_op_add,
    .src_alpha_blend_factor = moon_blend_factor_one,
    .dst_alpha_blend_factor = moon_blend_factor_zero,
    .alpha_blend_op = moon_blend_op_add,
    .color_write_mask = moon_color_component_r | moon_color_component_g | moon_color_component_b | moon_color_component_a,
};

typedef enum moon_tesselation_domain_origin : s8 {
    moon_tesselation_domain_origin_lower_left = 0,
    moon_tesselation_domain_origin_upper_left,
} moon_tesselation_domain_origin;

typedef struct moon_tesselation {
    u32                             control_points;
    moon_tesselation_domain_origin  origin;
    u8                          pad0[3];
} moon_tesselation;
static constexpr moon_tesselation MOON_TESSELATION_INIT = {
    .control_points = 0,
    .origin = moon_tesselation_domain_origin_lower_left,
};

typedef struct moon_raster_pipeline_attachment {
    moon_format format;
    moon_blend  blend;
    bool        has_blend;
} moon_raster_pipeline_attachment;
static constexpr moon_raster_pipeline_attachment MOON_RASTER_PIPELINE_ATTACHMENT_INIT = {
    .format = moon_format_r32g32b32a32_sfloat,
    .blend = MOON_BLEND_INIT,
    .has_blend = false,
};

typedef struct moon_raster_pipeline_assembly {
    moon_shader_assembly            mesh_shader;
    moon_shader_assembly            vertex_shader;
    moon_shader_assembly            tesselation_control_shader;
    moon_shader_assembly            tesselation_evaluation_shader;
    moon_shader_assembly            geometry_shader;
    moon_shader_assembly            fragment_shader;
    moon_shader_assembly            task_shader;
    moon_raster_pipeline_attachment color_attachments[MOON_MAX_COLOR_ATTACHMENTS];
    u8                              color_attachment_count;
    bool                            has_mesh_shader;
    bool                            has_vertex_shader;
    bool                            has_tesselation_control_shader;
    bool                            has_tesselation_evaluation_shader;
    bool                            has_geometry_shader;
    bool                            has_fragment_shader;
    bool                            has_task_shader;
    bool                            has_stencil_test;
    bool                            has_depth_test;
    bool                            has_tesselation;
    u8                          pad0[3];
    moon_stencil_test               stencil_test;
    moon_depth_test                 depth_test;
    moon_tesselation                tesselation;
    moon_rasterizer                 rasterizer;
    u8                          pad1[4];
    u32                             push_constant_size;
    lake_small_string               name;
} moon_raster_pipeline_assembly;
static constexpr moon_raster_pipeline_assembly MOON_RASTER_PIPELINE_ASSEMBLY_INIT = { 
    .mesh_shader = MOON_SHADER_ASSEMBLY_INIT,
    .vertex_shader = MOON_SHADER_ASSEMBLY_INIT,
    .tesselation_control_shader = MOON_SHADER_ASSEMBLY_INIT,
    .tesselation_evaluation_shader = MOON_SHADER_ASSEMBLY_INIT,
    .geometry_shader = MOON_SHADER_ASSEMBLY_INIT,
    .fragment_shader = MOON_SHADER_ASSEMBLY_INIT,
    .task_shader = MOON_SHADER_ASSEMBLY_INIT,
    .color_attachments = {
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
        MOON_RASTER_PIPELINE_ATTACHMENT_INIT,
    },
    .color_attachment_count = 0,
    .has_mesh_shader = false,
    .has_vertex_shader = true,
    .has_tesselation_control_shader = false,
    .has_tesselation_evaluation_shader = false,
    .has_geometry_shader = false,
    .has_fragment_shader = true,
    .has_task_shader = false,
    .has_stencil_test = false,
    .has_depth_test = false,
    .has_tesselation = false,
    .stencil_test = MOON_STENCIL_TEST_INIT,
    .depth_test = MOON_DEPTH_TEST_INIT,
    .tesselation = MOON_TESSELATION_INIT,
    .rasterizer = MOON_RASTERIZER_INIT,
    .push_constant_size = MOON_MAX_PUSH_CONSTANT_BYTE_SIZE,
};

/** Assemble an array of raster pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_raster_pipelines_assembly)(moon_device device, u32 count, moon_raster_pipeline_assembly const *assembly_array, moon_raster_pipeline *out_pipelines);
#define FN_MOON_RASTER_PIPELINES_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_raster_pipelines_assembly(moon_device device, u32 count, moon_raster_pipeline_assembly const *assembly_array, moon_raster_pipeline *out_pipelines)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, raster_pipeline);
#define FN_MOON_RASTER_PIPELINE_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, raster_pipeline)

typedef enum moon_ray_tracing_shader_group_type : s8 {
    moon_ray_tracing_shader_group_type_general = 0,
    moon_ray_tracing_shader_group_type_triangles_hit_group,
    moon_ray_tracing_shader_group_type_procedural_hit_group,
} moon_ray_tracing_shader_group_type;

typedef struct moon_ray_tracing_shader_group {
    moon_ray_tracing_shader_group_type  type;
    u8                              pad0[3];
    u32                                 general_shader_index;
    u32                                 closest_hit_shader_index;
    u32                                 any_hit_shader_index;
    u32                                 intersection_shader_index;
} moon_ray_tracing_shader_group;
static constexpr moon_ray_tracing_shader_group MOON_RAY_TRACING_SHADER_GROUP_INIT = {
    .type = moon_ray_tracing_shader_group_type_general,
    .general_shader_index = MOON_SHADER_UNUSED,
    .closest_hit_shader_index = MOON_SHADER_UNUSED,
    .any_hit_shader_index = MOON_SHADER_UNUSED,
    .intersection_shader_index = MOON_SHADER_UNUSED,
};

typedef struct {
    moon_shader_assembly const                         *ray_gen_stages;
    moon_shader_assembly const                         *miss_stages;
    moon_shader_assembly const                         *callable_stages;
    moon_shader_assembly const                         *intersection_stages;
    moon_shader_assembly const                         *closest_hit_stages;
    moon_shader_assembly const                         *any_hit_stages;
    moon_shader_assembly const                         *shader_groups;
    u32                                                 ray_gen_stage_count;
    u32                                                 miss_stage_count;
    u32                                                 callable_stage_count;
    u32                                                 intersection_stage_count;
    u32                                                 closest_hit_stage_count;
    u32                                                 any_hit_stage_count;
    u32                                                 shader_group_count;
    u32                                                 max_ray_recursion_depth;
    u32                                                 push_constant_size;
    lake_small_string                                   name;
} moon_ray_tracing_pipeline_assembly;
static constexpr moon_ray_tracing_pipeline_assembly MOON_RAY_TRACING_PIPELINE_ASSEMBLY_INIT = {
    .push_constant_size = MOON_MAX_PUSH_CONSTANT_BYTE_SIZE,
};

/** Assemble an array of raster pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_ray_tracing_pipelines_assembly)(moon_device device, u32 count, moon_ray_tracing_pipeline_assembly const *assembly_array, moon_ray_tracing_pipeline *out_pipelines);
#define FN_MOON_RAY_TRACING_PIPELINES_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_ray_tracing_pipelines_assembly(moon_device device, u32 count, moon_ray_tracing_pipeline_assembly const *assembly_array, moon_ray_tracing_pipeline *out_pipelines)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, ray_tracing_pipeline);
#define FN_MOON_RAY_TRACING_PIPELINE_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, ray_tracing_pipeline)

typedef enum moon_present_mode : s8 {
    moon_present_mode_immediate = 0,
    moon_present_mode_mailbox,
    moon_present_mode_fifo,
    moon_present_mode_fifo_relaxed,
} moon_present_mode;

typedef u16 moon_present_transform;
typedef enum moon_present_transform_bits : moon_present_transform {
    moon_present_transform_identity             = (1u << 0),
    moon_present_transform_rotate_90            = (1u << 1),
    moon_present_transform_rotate_180           = (1u << 2),
    moon_present_transform_rotate_270           = (1u << 3),
    moon_present_transform_mirror               = (1u << 4),
    moon_present_transform_mirror_rotate_90     = (1u << 5),
    moon_present_transform_mirror_rotate_180    = (1u << 6),
    moon_present_transform_mirror_rotate_270    = (1u << 7),
    moon_present_transform_inherit              = (1u << 8),
} moon_present_transform_bits;

/** A custom procedure to select the surface format from an array of supported formats.
 *  Should return an index into the `formats` array. */
typedef s32 (LAKECALL *PFN_moon_surface_format_selector)(u32 format_count, moon_format *formats);

/** Details needed to create a swapchain. */
typedef struct moon_swapchain_assembly {
    /** A window obtained from the display backend. */
    struct hadal_window_impl           *native_window;
    /** Custom selector for the surface format. If NULL, a default selector will be provided. */
    PFN_moon_surface_format_selector    surface_format_selector;
    /** Indices of timeline semaphores for presentation will be cycled by this value. */
    s16                                 max_allowed_frames_in_flight;
    /** Describes how the swapchain image will be transformed at presentation. */
    moon_present_transform              present_transform;
    /** The preferred present mode will be selected if possible. */
    moon_present_mode                   present_mode;
    /** This queue type will be selected for presentation, fallback is the main queue. */
    moon_queue_type                     queue_type;
    /** Usage bits for swapchain images. */
    moon_texture_usage                  image_usage;
    /** Name of the swapchain for debugging. */
    lake_small_string                   name;
} moon_swapchain_assembly;
static constexpr moon_swapchain_assembly MOON_SWAPCHAIN_ASSEMBLY_INIT = {
    .native_window = nullptr,
    .surface_format_selector = nullptr,
    .max_allowed_frames_in_flight = INT16_MAX,
    .present_transform = moon_present_transform_identity,
    .present_mode = moon_present_mode_mailbox,
    .image_usage = moon_texture_usage_color_attachment,
    .queue_type = moon_queue_type_main,
};

PFN_LAKE_HANDLE_ASSEMBLY(moon, swapchain, device);
#define FN_MOON_SWAPCHAIN_ASSEMBLY(backend) \
    FN_LAKE_HANDLE_ASSEMBLY(backend, moon, swapchain, device)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, swapchain);
#define FN_MOON_SWAPCHAIN_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, swapchain)

/** Limits frames in flight. Blocks until GPU catches up to the max number of frames in flight.
 *  DOES NOT WAIT for the swapchain image to be available, one must STILL use the acquire semaphore.
 *  This function DOES WAIT until there is a frame in flight available to prepare on the CPU. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_wait_for_next_frame)(moon_swapchain swapchain);
#define FN_MOON_SWAPCHAIN_WAIT_FOR_NEXT_FRAME(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_wait_for_next_frame(moon_swapchain swapchain)

/** The texture handle may change between calls. This must be called to obtain a new swapchain image 
 *  to be used for rendering. WARNING: texture handles returned from the swapchain are INVALID after 
 *  the swapchain is destroyed, or either resize or set_present_mode is called. This function may 
 *  internally call into `PFN_moon_swapchain_wait_for_next_frame`. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_acquire_next_image)(moon_swapchain swapchain, moon_texture_id *out_texture);
#define FN_MOON_SWAPCHAIN_ACQUIRE_NEXT_IMAGE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_acquire_next_image(moon_swapchain swapchain, moon_texture_id *out_texture)

/** The acquire semaphore must be waited on in the first submission that uses the last acquired image. 
 *  This semaphore may change between acquires, so it needs to be re-queried after every current_acquire_semaphore call. */
typedef LAKE_NODISCARD moon_binary_semaphore (LAKECALL *PFN_moon_swapchain_current_acquire_semaphore)(moon_swapchain swapchain);
#define FN_MOON_SWAPCHAIN_CURRENT_ACQUIRE_SEMAPHORE(backend) \
    LAKE_NODISCARD moon_binary_semaphore LAKECALL _moon_##backend##_swapchain_current_acquire_semaphore(moon_swapchain swapchain)

/** The present semaphore must be signaled in the last submission that uses the last acquired swapchain image.
 *  The present semaphore must be waited on in the present of the last acquired image. This semaphore may 
 *  change between acquires, so it needs to be re-queried after every current_acquire_semaphore call. */
typedef LAKE_NODISCARD moon_binary_semaphore (LAKECALL *PFN_moon_swapchain_current_present_semaphore)(moon_swapchain swapchain);
#define FN_MOON_SWAPCHAIN_CURRENT_PRESENT_SEMAPHORE(backend) \
    LAKE_NODISCARD moon_binary_semaphore LAKECALL _moon_##backend##_swapchain_current_present_semaphore(moon_swapchain swapchain)

/** The last submission that uses the swapchain image needs to signal the timeline with the CPU value. */
typedef LAKE_NODISCARD u64 (LAKECALL *PFN_moon_swapchain_current_cpu_timeline_value)(moon_swapchain swapchain);
#define FN_MOON_SWAPCHAIN_CURRENT_CPU_TIMELINE_VALUE(backend) \
    LAKE_NODISCARD u64 LAKECALL _moon_##backend##_swapchain_current_cpu_timeline_value(moon_swapchain swapchain)

/** The swapchain needs to know when the last use of the swapchain happens to limit the frames in flight. In the last 
 *  submission that uses the swapchain image, signal this timeline semaphore with the CPU timeline value. The CPU value 
 *  timeline is incremented whenever acquire is called. The GPU timeline must be manually incremented by the user via 
 *  a submit. The difference between CPU and GPU timeline describes how many frames in flight the GPU is behind the CPU. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_current_timeline_pair)(moon_swapchain swapchain, moon_timeline_pair *out_timeline);
#define FN_MOON_SWAPCHAIN_CURRENT_TIMELINE_PAIR(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_current_timeline_pair(moon_swapchain swapchain, moon_timeline_pair *out_timeline)

/** The swapchain needs to know when the last use of the swapchain happens to limit the frames in flight.
 *  In the last submission that uses the swapchain image, signal this timeline semaphore with the CPU timeline value. */
typedef LAKE_NODISCARD moon_timeline_semaphore (LAKECALL *PFN_moon_swapchain_gpu_timeline_semaphore)(moon_swapchain swapchain);
#define FN_MOON_SWAPCHAIN_GPU_TIMELINE_SEMAPHORE(backend) \
    LAKE_NODISCARD moon_timeline_semaphore LAKECALL _moon_##backend##_swapchain_gpu_timeline_semaphore(moon_swapchain swapchain)

/** Recreates the swapchain with new present mode. Due to wsi limitations this function will WAIT IDLE the device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_set_present_mode)(moon_swapchain swapchain, moon_present_mode present_mode);
#define FN_MOON_SWAPCHAIN_SET_PRESENT_MODE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_set_present_mode(moon_swapchain swapchain, moon_present_mode present_mode)

/** When the window size changes the swapchain is in an invalid state for new commands.
 *  Calling resize will recreate the swapchain using the updated window size. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_resize)(moon_swapchain swapchain);
#define FN_MOON_SWAPCHAIN_RESIZE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_resize(moon_swapchain swapchain)

typedef struct moon_command_recorder_assembly {
    moon_queue_type                     queue_type;
    lake_small_string                   name;
} moon_command_recorder_assembly;

PFN_LAKE_HANDLE_ASSEMBLY(moon, command_recorder, device);
#define FN_MOON_COMMAND_RECORDER_ASSEMBLY(backend) \
    FN_LAKE_HANDLE_ASSEMBLY(backend, moon, command_recorder, device)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, command_recorder);
#define FN_MOON_COMMAND_RECORDER_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, command_recorder)

typedef struct moon_staged_command_list_assembly {
    lake_small_string                   name;
} moon_staged_command_list_assembly;

PFN_LAKE_HANDLE_ASSEMBLY(moon, staged_command_list, command_recorder);
#define FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(backend) \
    FN_LAKE_HANDLE_ASSEMBLY(backend, moon, staged_command_list, command_recorder)

PFN_LAKE_HANDLE_DESTRUCTOR(moon, staged_command_list);
#define FN_MOON_STAGED_COMMAND_LIST_DESTRUCTOR(backend) \
    FN_LAKE_HANDLE_DESTRUCTOR(backend, moon, staged_command_list)

#define PFN_MOON_COMMAND(fn, ...) \
    typedef void (LAKECALL *PFN_moon_cmd_##fn)(moon_command_recorder cmd, __VA_ARGS__)
#define FN_MOON_COMMAND(fn, backend, ...) \
    void LAKECALL _moon_##backend##_cmd_##fn(moon_command_recorder cmd, __VA_ARGS__)

#define PFN_MOON_COMMAND_NO_ARGS(fn) \
    typedef void (LAKECALL *PFN_moon_cmd_##fn)(moon_command_recorder cmd)
#define FN_MOON_COMMAND_NO_ARGS(fn, backend) \
    void LAKECALL _moon_##backend##_cmd_##fn(moon_command_recorder cmd)

typedef struct moon_buffer_region {
    u64                                 src_offset;
    u64                                 dst_offset;
    u64                                 size;
} moon_buffer_region;

typedef struct moon_cmd_copy_buffer {
    moon_buffer_id                      src_buffer;
    moon_buffer_id                      dst_buffer;
    u32                                 region_count;
    moon_buffer_region const           *regions;
} moon_cmd_copy_buffer;
static constexpr moon_cmd_copy_buffer MOON_CMD_COPY_BUFFER_INIT = {
    .region_count = 1,
};

/** Copy data between buffer regions. */
PFN_MOON_COMMAND(copy_buffer, moon_cmd_copy_buffer const *params);
#define FN_MOON_CMD_COPY_BUFFER(backend) \
    FN_MOON_COMMAND(copy_buffer, backend, moon_cmd_copy_buffer const *params)

typedef struct moon_buffer_and_texture_copy {
    u64                                 buffer_offset;
    u32                                 buffer_row_length;
    u32                                 buffer_texture_height;
    moon_texture_array_slice            texture_slice;
    lake_offset3d                       texture_offset;
    lake_extent3d                       texture_extent;
} moon_buffer_and_texture_copy;

typedef struct moon_cmd_copy_buffer_and_texture {
    moon_buffer_id                      buffer;
    moon_texture_id                     texture;
    moon_texture_layout                 texture_layout;
    u32                                 region_count;
    moon_buffer_and_texture_copy const *regions;
} moon_cmd_copy_buffer_and_texture;
static constexpr moon_cmd_copy_buffer_and_texture MOON_CMD_COPY_BUFFER_TO_TEXTURE_INIT = {
    .texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};
static constexpr moon_cmd_copy_buffer_and_texture MOON_CMD_COPY_TEXTURE_TO_BUFFER_INIT = {
    .texture_layout = moon_texture_layout_transfer_src_optimal,
    .region_count = 1,
};

/** Copy data between source buffer and destination texture regions. */
PFN_MOON_COMMAND(copy_buffer_to_texture, moon_cmd_copy_buffer_and_texture const *params);
#define FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(backend) \
    FN_MOON_COMMAND(copy_buffer_to_texture, backend, moon_cmd_copy_buffer_and_texture const *params)

/** Copy data between source buffer and destination texture regions. */
PFN_MOON_COMMAND(copy_texture_to_buffer, moon_cmd_copy_buffer_and_texture const *params);
#define FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(backend) \
    FN_MOON_COMMAND(copy_texture_to_buffer, backend, moon_cmd_copy_buffer_and_texture const *params)

typedef struct moon_texture_copy {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offset;
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offset;
    lake_extent3d                       extent;
} moon_texture_copy;

typedef struct moon_cmd_copy_texture {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    u32                                 region_count;
    moon_texture_copy const            *regions;
} moon_cmd_copy_texture;
static constexpr moon_cmd_copy_texture MOON_CMD_COPY_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};

/** Copy regions between textures. */
PFN_MOON_COMMAND(copy_texture, moon_cmd_copy_texture const *params);
#define FN_MOON_CMD_COPY_TEXTURE(backend) \
    FN_MOON_COMMAND(copy_texture, backend, moon_cmd_copy_texture const *params)

typedef struct moon_texture_blit {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offsets[2];
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offsets[2];
} moon_texture_blit;

typedef struct moon_cmd_blit_texture {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    moon_filter_mode                    filter;
    u32                                 region_count;
    moon_texture_blit const            *regions;
} moon_cmd_blit_texture;
static constexpr moon_cmd_blit_texture MOON_CMD_BLIT_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .filter = moon_filter_mode_linear,
    .region_count = 1,
};

/** Copy regions of source texture into a destination texture, potentially performing 
 *  format conversion, arbitrary scaling, and filtering. */
PFN_MOON_COMMAND(blit_texture, moon_cmd_blit_texture const *params);
#define FN_MOON_CMD_BLIT_TEXTURE(backend) \
    FN_MOON_COMMAND(blit_texture, backend, moon_cmd_blit_texture const *params)

typedef struct moon_texture_resolve {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offset;
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offset;
    lake_extent3d                       extent;
} moon_texture_resolve;

typedef struct moon_cmd_resolve_texture {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    u32                                 region_count;
    moon_texture_resolve const         *regions;
} moon_cmd_resolve_texture;
static constexpr moon_cmd_resolve_texture MOON_CMD_RESOLVE_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};

/** Resolve regions of a multisample color image to a non-multisample color image. */
PFN_MOON_COMMAND(resolve_texture, moon_cmd_resolve_texture const *params);
#define FN_MOON_CMD_RESOLVE_TEXTURE(backend) \
    FN_MOON_COMMAND(resolve_texture, backend, moon_cmd_resolve_texture const *params)

typedef struct moon_cmd_clear_buffer {
    moon_buffer_id      dst_buffer;
    u64                 dst_offset;
    u64                 size;
    s32                 clear_value;
} moon_cmd_clear_buffer;
static constexpr moon_cmd_clear_buffer MOON_CMD_CLEAR_BUFFER_INIT = {0};

/** Fill a region of a buffer with a fixed value, used to zero out a buffer. */
PFN_MOON_COMMAND(clear_buffer, moon_cmd_clear_buffer const *params);
#define FN_MOON_CMD_CLEAR_BUFFER(backend) \
    FN_MOON_COMMAND(clear_buffer, backend, moon_cmd_clear_buffer const *params)

typedef struct moon_cmd_clear_texture {
    moon_texture_id     dst_texture;
    moon_texture_layout dst_texture_layout;
    moon_color_value    clear_value;
} moon_cmd_clear_texture;
static constexpr moon_cmd_clear_texture MOON_CMD_CLEAR_TEXTURE_INIT = {
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
};

/** Fill a region of a texture with a fixed value, used to zero out a texture. */
PFN_MOON_COMMAND(clear_texture, moon_cmd_clear_texture const *params);
#define FN_MOON_CMD_CLEAR_TEXTURE(backend) \
    FN_MOON_COMMAND(clear_texture, backend, moon_cmd_clear_texture const *params)

/** Destroys a buffer after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_buffer_deferred, moon_buffer_id buffer);
#define FN_MOON_CMD_DESTROY_BUFFER_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_buffer_deferred, backend, moon_buffer_id buffer)

/** Destroys a texture after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_texture_deferred, moon_texture_id texture);
#define FN_MOON_CMD_DESTROY_TEXTURE_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_texture_deferred, backend, moon_texture_id texture)

/** Destroys a texture view after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_texture_view_deferred, moon_texture_view_id texture_view);
#define FN_MOON_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_texture_view_deferred, backend, moon_texture_view_id texture_view)

/** Destroys a sampler after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_sampler_deferred, moon_sampler_id sampler);
#define FN_MOON_CMD_DESTROY_SAMPLER_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_sampler_deferred, backend, moon_sampler_id sampler)

typedef struct moon_cmd_build_acceleration_structures {
    u32                 tlas_assembly_count;
    u32                 blas_assembly_count;
    moon_tlas_assembly *tlas_assembly;
    moon_blas_assembly *blas_assembly;
} moon_cmd_build_acceleration_structures;
static constexpr moon_cmd_build_acceleration_structures MOON_CMD_BUILD_ACCELERATION_STRUCTURES_INIT = {0};

/** Build or update bottom-level and top-level acceleration structures. */
PFN_MOON_COMMAND(build_acceleration_structures, moon_cmd_build_acceleration_structures const *params);
#define FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(backend) \
    FN_MOON_COMMAND(build_acceleration_structures, backend, moon_cmd_build_acceleration_structures const *params)

typedef struct moon_cmd_root_constants {
    void const         *data;
    u32                 offset;
    u32                 size;
} moon_cmd_root_constants;
static constexpr moon_cmd_root_constants MOON_CMD_ROOT_CONSTANTS_INIT = {0};

/** Root a range of constant data to shader stages. */
PFN_MOON_COMMAND(root_constants, moon_cmd_root_constants const *params);
#define FN_MOON_CMD_ROOT_CONSTANTS(backend) \
    FN_MOON_COMMAND(root_constants, backend, moon_cmd_root_constants const *params)

/** Bind compute pipeline for dispatch commands. */
PFN_MOON_COMMAND(set_compute_pipeline, moon_compute_pipeline pipeline);
#define FN_MOON_CMD_SET_COMPUTE_PIPELINE(backend) \
    FN_MOON_COMMAND(set_compute_pipeline, backend, moon_compute_pipeline pipeline)

/** Bind raster pipeline for draw commands. */
PFN_MOON_COMMAND(set_raster_pipeline, moon_raster_pipeline pipeline);
#define FN_MOON_CMD_SET_RASTER_PIPELINE(backend) \
    FN_MOON_COMMAND(set_raster_pipeline, backend, moon_raster_pipeline pipeline)

/** Bind ray tracing pipeline for trace commands. */
PFN_MOON_COMMAND(set_ray_tracing_pipeline, moon_ray_tracing_pipeline pipeline);
#define FN_MOON_CMD_SET_RAY_TRACING_PIPELINE(backend) \
    FN_MOON_COMMAND(set_ray_tracing_pipeline, backend, moon_ray_tracing_pipeline pipeline)

/** Bind work graph pipeline for dispatch graph commands. */
PFN_MOON_COMMAND(set_work_graph_pipeline, moon_work_graph_pipeline pipeline);
#define FN_MOON_CMD_SET_WORK_GRAPH_PIPELINE(backend) \
    FN_MOON_COMMAND(set_work_graph_pipeline, backend, moon_work_graph_pipeline pipeline)

/** Configure viewport transformation dimensions. */
PFN_MOON_COMMAND(set_viewport, lake_viewport const *viewport);
#define FN_MOON_CMD_SET_VIEWPORT(backend) \
    FN_MOON_COMMAND(set_viewport, backend, lake_viewport const *viewport)

/** Configure scissor rectangle for fragment clipping. */
PFN_MOON_COMMAND(set_scissor, lake_rect2d const *scissor);
#define FN_MOON_CMD_SET_SCISSOR(backend) \
    FN_MOON_COMMAND(set_scissor, backend, lake_rect2d const *scissor)

/** Configure number of rasterization samples for raster pipelines. */
PFN_MOON_COMMAND(set_rasterization_samples, moon_sample_count sample_count);
#define FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(backend) \
    FN_MOON_COMMAND(set_rasterization_samples, backend, moon_sample_count sample_count)

/** Configure depth bias constants for depth calculations. */
PFN_MOON_COMMAND(set_depth_bias, moon_depth_bias const *depth_bias);
#define FN_MOON_CMD_SET_DEPTH_BIAS(backend) \
    FN_MOON_COMMAND(set_depth_bias, backend, moon_depth_bias const *depth_bias)

typedef struct moon_cmd_set_index_buffer {
    moon_buffer_id      buffer;
    u64                 offset;
    moon_index_format   index_format;
} moon_cmd_set_index_buffer;
static constexpr moon_cmd_set_index_buffer MOON_CMD_SET_INDEX_BUFFER_INIT = {
    .index_format = moon_index_format_u32,
};

/** Configure index buffer for indexed draw calls. */
PFN_MOON_COMMAND(set_index_buffer, moon_cmd_set_index_buffer const *params);
#define FN_MOON_CMD_SET_INDEX_BUFFER(backend) \
    FN_MOON_COMMAND(set_index_buffer, backend, moon_cmd_set_index_buffer const *params)

typedef struct moon_cmd_barriers_and_transitions {
    u32                                 barrier_count;
    u32                                 transition_count;
    moon_memory_barrier const          *barriers;
    moon_texture_memory_barrier const  *transitions;
} moon_cmd_barriers_and_transitions;
static constexpr moon_cmd_barriers_and_transitions MOON_CMD_BARRIERS_AND_TRANSITIONS_INIT = {0};

/** Insert pipeline memory barriers and texture barriers for layout transitions. Successive barriers_and_transitions 
 *  calls are combined, as soon as any other command is recorder, the barrier batch is flushed. */
PFN_MOON_COMMAND(barriers_and_transitions, moon_cmd_barriers_and_transitions const *params);
#define FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(backend) \
    FN_MOON_COMMAND(barriers_and_transitions, backend, moon_cmd_barriers_and_transitions const *params)

typedef struct moon_renderpass_attachment {
    moon_texture_view_id                texture_view;
    moon_texture_view_id                resolve_texture_view;
    moon_texture_layout                 texture_layout;
    moon_texture_layout                 resolve_texture_layout;
    moon_resolve_mode                   resolve_mode;
    moon_load_op                        load_op;
    moon_store_op                       store_op;
    bool                                has_resolve;
    moon_color_value                    clear_value;
} moon_renderpass_attachment;
static constexpr moon_renderpass_attachment MOON_RENDERPASS_ATTACHMENT_INIT = {
    .texture_layout = moon_texture_layout_attachment_optimal,
    .resolve_texture_layout = moon_texture_layout_attachment_optimal,
    .resolve_mode = moon_resolve_mode_average,
    .load_op = moon_load_op_dont_care,
    .store_op = moon_store_op_store,
    .has_resolve = false,
};

typedef struct moon_cmd_begin_renderpass {
    moon_renderpass_attachment          color_attachments[MOON_MAX_COLOR_ATTACHMENTS];
    u32                                 color_attachment_count;
    bool                                has_depth_attachment;
    bool                                has_stencil_attachment;
    moon_renderpass_attachment          depth_attachment;
    moon_renderpass_attachment          stencil_attachment;
    lake_rect2d                         render_area;
} moon_cmd_begin_renderpass;
static constexpr moon_cmd_begin_renderpass MOON_CMD_BEGIN_RENDERPASS_INIT = {
    .color_attachments = {
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
    },
    .color_attachment_count = 1,
    .has_depth_attachment = false,
    .has_stencil_attachment = false,
    .depth_attachment = MOON_RENDERPASS_ATTACHMENT_INIT,
    .stencil_attachment = MOON_RENDERPASS_ATTACHMENT_INIT,
};

/** Start a renderpass with specified render targets for draw commands. */
PFN_MOON_COMMAND(begin_renderpass, moon_cmd_begin_renderpass const *params);
#define FN_MOON_CMD_BEGIN_RENDERPASS(backend) \
    FN_MOON_COMMAND(begin_renderpass, backend, moon_cmd_begin_renderpass const *params)

/** End the current renderpass. */
PFN_MOON_COMMAND_NO_ARGS(end_renderpass);
#define FN_MOON_CMD_END_RENDERPASS(backend) \
    FN_MOON_COMMAND_NO_ARGS(end_renderpass, backend)

typedef struct moon_cmd_write_timestamps {
    moon_timeline_query_pool    timeline_query_pool;
    u32                         query_index;
    moon_pipeline_stage_bits    stage;
} moon_cmd_write_timestamps;
static constexpr moon_cmd_write_timestamps MOON_CMD_WRITE_TIMESTAMPS_INIT = {0};

/** Record timestamp values into a query pool. */
PFN_MOON_COMMAND(write_timestamps, moon_cmd_write_timestamps const *params);
#define FN_MOON_CMD_WRITE_TIMESTAMPS(backend) \
    FN_MOON_COMMAND(write_timestamps, backend, moon_cmd_write_timestamps const *params)

typedef struct moon_cmd_resolve_timestamps {
    moon_timeline_query_pool    timeline_query_pool;
    u32                         start_index;
    u32                         count;
} moon_cmd_resolve_timestamps;
static constexpr moon_cmd_resolve_timestamps MOON_CMD_RESOLVE_TIMESTAMPS_INIT = {0};

/** Reset timestamp values in the query pool. */
PFN_MOON_COMMAND(resolve_timestamps, moon_cmd_resolve_timestamps const *params);
#define FN_MOON_CMD_RESOLVE_TIMESTAMPS(backend) \
    FN_MOON_COMMAND(resolve_timestamps, backend, moon_cmd_resolve_timestamps const *params)

typedef struct moon_cmd_begin_label {
    vec4                        label_color;
    char const                 *label_name;
} moon_cmd_begin_label;
static constexpr moon_cmd_begin_label MOON_CMD_BEGIN_LABEL_INIT = {0};

/** Begin a debug label. */
PFN_MOON_COMMAND(begin_label, moon_cmd_begin_label const *params);
#define FN_MOON_CMD_BEGIN_LABEL(backend) \
    FN_MOON_COMMAND(begin_label, backend, moon_cmd_begin_label const *params)

/** End a debug label. */
PFN_MOON_COMMAND_NO_ARGS(end_label);
#define FN_MOON_CMD_END_LABEL(backend) \
    FN_MOON_COMMAND_NO_ARGS(end_label, backend)

/** Compatible with `moon_draw_mesh_tasks`. */
typedef struct moon_dispatch {
    u32                         group_x, group_y, group_z;
} moon_dispatch;

typedef struct moon_cmd_dispatch {
    u32                         payload_count;
    moon_dispatch const        *payloads;
} moon_cmd_dispatch;
static constexpr moon_cmd_dispatch MOON_CMD_DISPATCH_INIT = {
    .payload_count = 1,
};

/** Execute compute workgroups with direct parameters. */
PFN_MOON_COMMAND(dispatch, moon_cmd_dispatch const *params);
#define FN_MOON_CMD_DISPATCH(backend) \
    FN_MOON_COMMAND(dispatch, backend, moon_cmd_dispatch const *params)

typedef struct moon_cmd_dispatch_indirect {
    moon_buffer_id              indirect_buffer;
    u64                         offset;
} moon_cmd_dispatch_indirect;
static constexpr moon_cmd_dispatch_indirect MOON_CMD_DISPATCH_INDIRECT_INIT = {0};

/** Execute compute workgroups with indirect parameters buffer. */
PFN_MOON_COMMAND(dispatch_indirect, moon_cmd_dispatch_indirect const *params);
#define FN_MOON_CMD_DISPATCH_INDIRECT(backend) \
    FN_MOON_COMMAND(dispatch_indirect, backend, moon_cmd_dispatch_indirect const *params)

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

typedef struct moon_cmd_dispatch_graph {
    moon_device_address         scratch;
    u64                         scratch_size;
    moon_dispatch_graph         graph;
} moon_cmd_dispatch_graph;
static constexpr moon_cmd_dispatch_graph MOON_CMD_DISPATCH_GRAPH_INIT = {0};
static constexpr moon_cmd_dispatch_graph MOON_CMD_DISPATCH_GRAPH_INDIRECT_INIT = {0};

/** Execute dispatch nodes with direct parameters. */
PFN_MOON_COMMAND(dispatch_graph, moon_cmd_dispatch_graph const *params);
#define FN_MOON_CMD_DISPATCH_GRAPH(backend) \
    FN_MOON_COMMAND(dispatch_graph, backend, moon_cmd_dispatch_graph const *params)

/** Execute dispatch nodes and payload parameters read on the device. */
PFN_MOON_COMMAND(dispatch_graph_indirect, moon_cmd_dispatch_graph const *params);
#define FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(backend) \
    FN_MOON_COMMAND(dispatch_graph_indirect, backend, moon_cmd_dispatch_graph const *params)

typedef struct moon_cmd_dispatch_graph_count {
    moon_device_address         scratch;
    u64                         scratch_size;
    moon_device_address         graph;
} moon_cmd_dispatch_graph_count;
static constexpr moon_cmd_dispatch_graph_count MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT_INIT = {0};

/** Execute dispatch nodes with all parameters read on the device. */
PFN_MOON_COMMAND(dispatch_graph_indirect_count, moon_cmd_dispatch_graph_count const *params);
#define FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND(dispatch_graph_indirect_count, backend, moon_cmd_dispatch_graph_count const *params)

typedef struct moon_draw {
    u32                         vertex_count;
    u32                         instance_count;
    u32                         first_vertex;
    u32                         first_instance;
} moon_draw;

typedef struct moon_cmd_draw {
    u32                         draw_count;
    moon_draw const            *draw_commands;
} moon_cmd_draw;
static constexpr moon_cmd_draw MOON_CMD_DRAW_INIT = {
    .draw_count = 1,
};

/** Draw non-indexed primitives with vertex count. */
PFN_MOON_COMMAND(draw, moon_cmd_draw const *params);
#define FN_MOON_CMD_DRAW(backend) \
    FN_MOON_COMMAND(draw, backend, moon_cmd_draw const *params)

typedef struct moon_draw_indexed {
    u32                         index_count;
    u32                         instance_count;
    u32                         first_index;
    s32                         vertex_offset;
    u32                         first_instance;
} moon_draw_indexed;

typedef struct moon_cmd_draw_indexed {
    u32                         draw_count;
    moon_draw const            *draw_commands;
} moon_cmd_draw_indexed;
static constexpr moon_cmd_draw_indexed MOON_CMD_DRAW_INDEXED_INIT = {
    .draw_count = 1,
};

/** Draw indexed primitives using bound index buffer. */
PFN_MOON_COMMAND(draw_indexed, moon_cmd_draw_indexed const *params);
#define FN_MOON_CMD_DRAW_INDEXED(backend) \
    FN_MOON_COMMAND(draw_indexed, backend, moon_cmd_draw_indexed const *params)

typedef struct moon_cmd_draw_indirect {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    u32                         draw_count;
    u32                         draw_stride;
    bool                        is_indexed;
} moon_cmd_draw_indirect;
static constexpr moon_cmd_draw_indirect MOON_CMD_DRAW_INDIRECT_INIT = {0};

/** Draw primitives using indirect parameters buffer. */
PFN_MOON_COMMAND(draw_indirect, moon_cmd_draw_indirect const *params);
#define FN_MOON_CMD_DRAW_INDIRECT(backend) \
    FN_MOON_COMMAND(draw_indirect, backend, moon_cmd_draw_indirect const *params)

typedef struct moon_cmd_draw_indirect_count {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    moon_buffer_id              count_buffer;
    u64                         count_buffer_offset;
    u32                         max_draw_count;
    u32                         draw_stride;
    bool                        is_indexed;
} moon_cmd_draw_indirect_count;
static constexpr moon_cmd_draw_indirect_count MOON_CMD_DRAW_INDIRECT_COUNT_INIT = {0};

/** Draw primitives using indirect parameters buffer with device count argument. */
PFN_MOON_COMMAND(draw_indirect_count, moon_cmd_draw_indirect_count const *params);
#define FN_MOON_CMD_DRAW_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND(draw_indirect_count, backend, moon_cmd_draw_indirect_count const *params)

/** Compatible with `moon_dispatch`. */
typedef struct moon_draw_mesh_tasks {
    u32                         group_x, group_y, group_z;
} moon_draw_mesh_tasks;

typedef struct moon_cmd_draw_mesh_tasks {
    u32                         draw_count;
    moon_draw_mesh_tasks const *draw_commands;
} moon_cmd_draw_mesh_tasks;
static constexpr moon_cmd_draw_mesh_tasks MOON_CMD_DRAW_MESH_TASKS_INIT = {
    .draw_count = 1,
};

/** Execute mesh shader workgroups with direct parameters. */
PFN_MOON_COMMAND(draw_mesh_tasks, moon_cmd_draw_mesh_tasks const *params);
#define FN_MOON_CMD_DRAW_MESH_TASKS(backend) \
    FN_MOON_COMMAND(draw_mesh_tasks, backend, moon_cmd_draw_mesh_tasks const *params)

typedef struct moon_cmd_draw_mesh_tasks_indirect {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    u32                         draw_count;
    u32                         draw_stride;
} moon_cmd_draw_mesh_tasks_indirect;
static constexpr moon_cmd_draw_mesh_tasks_indirect MOON_CMD_DRAW_MESH_TASKS_INDIRECT_INIT = {0};

/** Execute mesh shader workgroups using indirect parameters buffer. */
PFN_MOON_COMMAND(draw_mesh_tasks_indirect, moon_cmd_draw_mesh_tasks_indirect const *params);
#define FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(backend) \
    FN_MOON_COMMAND(draw_mesh_tasks_indirect, backend, moon_cmd_draw_mesh_tasks_indirect const *params)

typedef struct moon_cmd_draw_mesh_tasks_indirect_count {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    moon_buffer_id              count_buffer;
    u64                         count_buffer_offset;
    u32                         max_draw_count;
    u32                         draw_stride;
} moon_cmd_draw_mesh_tasks_indirect_count;
static constexpr moon_cmd_draw_mesh_tasks_indirect_count MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_INIT = {0};

/** Execute mesh shader workgroups using indirect parameters buffer with deferred count argument. */
PFN_MOON_COMMAND(draw_mesh_tasks_indirect_count, moon_cmd_draw_mesh_tasks_indirect_count const *params);
#define FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND(draw_mesh_tasks_indirect_count, backend, moon_cmd_draw_mesh_tasks_indirect_count const *params)

typedef struct moon_ray_trace {
    u32                         width, height, depth;
} moon_ray_trace;

typedef struct moon_cmd_trace_rays {
    u32                         raygen_shader_binding_table_offset;
    u32                         miss_shader_binding_table_offset;
    u32                         miss_shader_binding_table_stride;
    u32                         hit_shader_binding_table_offset;
    moon_shader_binding_table   shader_binding_table;
    u32                         payload_count;
    moon_ray_trace const       *payloads;
} moon_cmd_trace_rays;
static constexpr moon_cmd_trace_rays MOON_CMD_TRACE_RAYS_INIT = {
    .payload_count = 1,
};

/** Execute ray tracing workload with direct parameters. */
PFN_MOON_COMMAND(trace_rays, moon_cmd_trace_rays const *params);
#define FN_MOON_CMD_TRACE_RAYS(backend) \
    FN_MOON_COMMAND(trace_rays, backend, moon_cmd_trace_rays const *params)

typedef struct moon_cmd_trace_rays_indirect {
    moon_device_address         indirect_buffer_address;
    moon_shader_binding_table   shader_binding_table;
} moon_cmd_trace_rays_indirect;
static constexpr moon_cmd_trace_rays_indirect MOON_CMD_TRACE_RAYS_INDIRECT_INIT = {0};

/** Execute ray tracing workload using indirect parameters buffer. */
PFN_MOON_COMMAND(trace_rays_indirect, moon_cmd_trace_rays_indirect const *params);
#define FN_MOON_CMD_TRACE_RAYS_INDIRECT(backend) \
    FN_MOON_COMMAND(trace_rays_indirect, backend, moon_cmd_trace_rays_indirect const *params)

/** Interface of the rendering backend. */
typedef struct moon_interface_impl {
    lake_interface_header                           header;
    PFN_moon_list_device_details                    list_device_details;
    PFN_moon_device_assembly                        device_assembly;
    PFN_moon_device_destructor                      device_destructor;
    PFN_moon_device_queue_count                     device_queue_count;
    PFN_moon_device_queue_wait_idle                 device_queue_wait_idle;
    PFN_moon_device_wait_idle                       device_wait_idle;
    PFN_moon_device_submit_commands                 device_submit_commands;
    PFN_moon_device_present_frames                  device_present_frames;
    PFN_moon_device_destroy_deferred                device_destroy_deferred;
    PFN_moon_device_heap_assembly                   device_heap_assembly;
    PFN_moon_device_heap_destructor                 device_heap_destructor;
    PFN_moon_device_buffer_memory_requirements      device_buffer_memory_requirements;
    PFN_moon_device_texture_memory_requirements     device_texture_memory_requirements;
    PFN_moon_device_memory_report                   device_memory_report;

    PFN_moon_create_buffer                          create_buffer;
    PFN_moon_create_buffer_from_heap                create_buffer_from_heap;
    PFN_moon_create_texture                         create_texture;
    PFN_moon_create_texture_from_heap               create_texture_from_heap;
    PFN_moon_create_texture_view                    create_texture_view;
    PFN_moon_create_sampler                         create_sampler;
    PFN_moon_create_tlas                            create_tlas;
    PFN_moon_create_tlas_from_buffer                create_tlas_from_buffer;
    PFN_moon_create_blas                            create_blas;
    PFN_moon_create_blas_from_buffer                create_blas_from_buffer;

    PFN_moon_is_buffer_valid                        is_buffer_valid;
    PFN_moon_is_texture_valid                       is_texture_valid;
    PFN_moon_is_texture_view_valid                  is_texture_view_valid;
    PFN_moon_is_sampler_valid                       is_sampler_valid;
    PFN_moon_is_tlas_valid                          is_tlas_valid;
    PFN_moon_is_blas_valid                          is_blas_valid;

    PFN_moon_buffer_host_address                    buffer_host_address;
    PFN_moon_buffer_device_address                  buffer_device_address;
    PFN_moon_tlas_device_address                    tlas_device_address;
    PFN_moon_blas_device_address                    blas_device_address;

    PFN_moon_destroy_buffer                         destroy_buffer;
    PFN_moon_destroy_texture                        destroy_texture;
    PFN_moon_destroy_texture_view                   destroy_texture_view;
    PFN_moon_destroy_sampler                        destroy_sampler;
    PFN_moon_destroy_tlas                           destroy_tlas;
    PFN_moon_destroy_blas                           destroy_blas;

    PFN_moon_timeline_query_pool_assembly           timeline_query_pool_assembly;
    PFN_moon_timeline_query_pool_destructor         timeline_query_pool_destructor;
    PFN_moon_timeline_semaphore_assembly            timeline_semaphore_assembly;
    PFN_moon_timeline_semaphore_destructor          timeline_semaphore_destructor;
    PFN_moon_binary_semaphore_assembly              binary_semaphore_assembly;
    PFN_moon_binary_semaphore_destructor            binary_semaphore_destructor;

    PFN_moon_compute_pipelines_assembly             compute_pipelines_assembly;
    PFN_moon_compute_pipeline_destructor            compute_pipeline_destructor;
    PFN_moon_work_graph_pipelines_assembly          work_graph_pipelines_assembly;
    PFN_moon_work_graph_pipeline_destructor         work_graph_pipeline_destructor;
    PFN_moon_raster_pipelines_assembly              raster_pipelines_assembly;
    PFN_moon_raster_pipeline_destructor             raster_pipeline_destructor;
    PFN_moon_ray_tracing_pipelines_assembly         ray_tracing_pipelines_assembly;
    PFN_moon_ray_tracing_pipeline_destructor        ray_tracing_pipeline_destructor;

    PFN_moon_swapchain_assembly                     swapchain_assembly;
    PFN_moon_swapchain_destructor                   swapchain_destructor;
    PFN_moon_swapchain_wait_for_next_frame          swapchain_wait_for_next_frame;
    PFN_moon_swapchain_acquire_next_image           swapchain_acquire_next_image;
    PFN_moon_swapchain_current_acquire_semaphore    swapchain_current_acquire_semaphore;
    PFN_moon_swapchain_current_present_semaphore    swapchain_current_present_semaphore;
    PFN_moon_swapchain_current_cpu_timeline_value   swapchain_current_cpu_timeline_value;
    PFN_moon_swapchain_current_timeline_pair        swapchain_current_timeline_pair;
    PFN_moon_swapchain_gpu_timeline_semaphore       swapchain_gpu_timeline_semaphore;
    PFN_moon_swapchain_set_present_mode             swapchain_set_present_mode;
    PFN_moon_swapchain_resize                       swapchain_resize;

    PFN_moon_command_recorder_assembly              command_recorder_assembly;
    PFN_moon_command_recorder_destructor            command_recorder_destructor;
    PFN_moon_staged_command_list_assembly           staged_command_list_assembly;
    PFN_moon_staged_command_list_destructor         staged_command_list_destructor;

    PFN_moon_cmd_copy_buffer                        cmd_copy_buffer;
    PFN_moon_cmd_copy_buffer_to_texture             cmd_copy_buffer_to_texture;
    PFN_moon_cmd_copy_texture_to_buffer             cmd_copy_texture_to_buffer;
    PFN_moon_cmd_copy_texture                       cmd_copy_texture;
    PFN_moon_cmd_blit_texture                       cmd_blit_texture;
    PFN_moon_cmd_resolve_texture                    cmd_resolve_texture;
    PFN_moon_cmd_clear_buffer                       cmd_clear_buffer;
    PFN_moon_cmd_clear_texture                      cmd_clear_texture;
    PFN_moon_cmd_destroy_buffer_deferred            cmd_destroy_buffer_deferred;
    PFN_moon_cmd_destroy_texture_deferred           cmd_destroy_texture_deferred;
    PFN_moon_cmd_destroy_texture_view_deferred      cmd_destroy_texture_view_deferred;
    PFN_moon_cmd_destroy_sampler_deferred           cmd_destroy_sampler_deferred;
    PFN_moon_cmd_build_acceleration_structures      cmd_build_acceleration_structures;
    PFN_moon_cmd_root_constants                     cmd_root_constants;
    PFN_moon_cmd_set_compute_pipeline               cmd_set_compute_pipeline;
    PFN_moon_cmd_set_raster_pipeline                cmd_set_raster_pipeline;
    PFN_moon_cmd_set_ray_tracing_pipeline           cmd_set_ray_tracing_pipeline;
    PFN_moon_cmd_set_work_graph_pipeline            cmd_set_work_graph_pipeline;
    PFN_moon_cmd_set_viewport                       cmd_set_viewport;
    PFN_moon_cmd_set_scissor                        cmd_set_scissor;
    PFN_moon_cmd_set_rasterization_samples          cmd_set_rasterization_samples;
    PFN_moon_cmd_set_depth_bias                     cmd_set_depth_bias;
    PFN_moon_cmd_set_index_buffer                   cmd_set_index_buffer;
    PFN_moon_cmd_barriers_and_transitions           cmd_barriers_and_transitions;
    PFN_moon_cmd_begin_renderpass                   cmd_begin_renderpass;
    PFN_moon_cmd_end_renderpass                     cmd_end_renderpass;
    PFN_moon_cmd_write_timestamps                   cmd_write_timestamps;
    PFN_moon_cmd_resolve_timestamps                 cmd_resolve_timestamps;
    PFN_moon_cmd_begin_label                        cmd_begin_label;
    PFN_moon_cmd_end_label                          cmd_end_label;
    PFN_moon_cmd_dispatch                           cmd_dispatch;
    PFN_moon_cmd_dispatch_indirect                  cmd_dispatch_indirect;
    PFN_moon_cmd_dispatch_graph                     cmd_dispatch_graph;
    PFN_moon_cmd_dispatch_graph_indirect            cmd_dispatch_graph_indirect;
    PFN_moon_cmd_dispatch_graph_indirect_count      cmd_dispatch_graph_indirect_count;
    PFN_moon_cmd_draw                               cmd_draw;
    PFN_moon_cmd_draw_indexed                       cmd_draw_indexed;
    PFN_moon_cmd_draw_indirect                      cmd_draw_indirect;
    PFN_moon_cmd_draw_indirect_count                cmd_draw_indirect_count;
    PFN_moon_cmd_draw_mesh_tasks                    cmd_draw_mesh_tasks;
    PFN_moon_cmd_draw_mesh_tasks_indirect           cmd_draw_mesh_tasks_indirect;
    PFN_moon_cmd_draw_mesh_tasks_indirect_count     cmd_draw_mesh_tasks_indirect_count;
    PFN_moon_cmd_trace_rays                         cmd_trace_rays;
    PFN_moon_cmd_trace_rays_indirect                cmd_trace_rays_indirect;
} moon_interface_impl;

/** A view into the rendering interface implementation. */
typedef union moon_interface {
    lake_interface_header      *header;
    struct moon_adapter_impl   *adapter;
    struct moon_interface_impl *interface;
    void                       *data;
} moon_interface;

/** Header for `struct moon_device_impl`. */
typedef struct moon_device_header {
    moon_interface              moon;
    lake_refcnt                 refcnt;
    moon_device_assembly        assembly;
    moon_device_details         details;
} moon_device_header;

LAKE_DECL_HANDLE_HEADER(moon, device_heap, device); /** struct moon_device_heap_impl */
LAKE_DECL_HANDLE_HEADER(moon, timeline_query_pool, device); /** struct moon_timeline_query_pool_impl */
LAKE_DECL_HANDLE_HEADER(moon, timeline_semaphore, device); /** struct moon_timeline_semaphore_impl */
LAKE_DECL_HANDLE_HEADER(moon, binary_semaphore, device); /** struct moon_binary_semaphore_impl */
LAKE_DECL_HANDLE_HEADER(moon, compute_pipeline, device); /** struct moon_compute_pipeline_impl */
LAKE_DECL_HANDLE_HEADER(moon, work_graph_pipeline, device); /** struct moon_work_graph_pipeline_impl */
LAKE_DECL_HANDLE_HEADER(moon, raster_pipeline, device); /** struct moon_raster_pipeline_impl */
LAKE_DECL_HANDLE_HEADER(moon, ray_tracing_pipeline, device); /** struct moon_ray_tracing_pipeline_impl */
LAKE_DECL_HANDLE_HEADER(moon, swapchain, device); /** struct moon_swapchain_impl */
LAKE_DECL_HANDLE_HEADER(moon, command_recorder, device); /** struct moon_command_recorder_impl */
LAKE_DECL_HANDLE_HEADER(moon, staged_command_list, command_recorder); /** struct moon_staged_command_list_impl */

/** Details needed to assemble a rendering backend. */
typedef struct moon_interface_assembly {
    moon_interface             *out_impl; 
} moon_interface_assembly;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MOON_VULKAN
LAKEAPI FN_LAKE_WORK(moon_interface_assembly_vulkan, moon_interface_assembly const *assembly);
#endif /* MOON_VULKAN */

#ifdef __cplusplus
}
#endif /* __cplusplus */
