#pragma once

/** @file lake/renderer/timeline_sync.h
 *  @brief TODO
 *
 *  TODO docs
 */
#include <lake/renderer/render_resources.h>

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

/** Inspired by the simple vulkan synchronization library:
 *  https://github.com/Tobski/simple_vulkan_synchronization */
typedef enum moon_access : s8 {
    moon_access_none = 0,
    /* READ */
    moon_access_command_buffer_read,
    moon_access_indirect_buffer,
    moon_access_index_buffer,
    moon_access_vertex_buffer,
    moon_access_vertex_shader_read_uniform_buffer,
    moon_access_vertex_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_vertex_shader_read_other,
    moon_access_tesselation_control_shader_read_uniform_buffer,
    moon_access_tesselation_control_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_tesselation_control_shader_read_other,
    moon_access_tesselation_evaluation_shader_read_uniform_buffer,
    moon_access_tesselation_evaluation_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_tesselation_evaluation_shader_read_other,
    moon_access_geometry_shader_read_uniform_buffer,
    moon_access_geometry_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_geometry_shader_read_other,
    moon_access_task_shader_read_uniform_buffer,
    moon_access_task_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_task_shader_read_other,
    moon_access_mesh_shader_read_uniform_buffer,
    moon_access_mesh_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_mesh_shader_read_other,
    moon_access_fragment_density_map_read,
    moon_access_fragment_shading_rate_read,
    moon_access_fragment_shader_read_uniform_buffer,
    moon_access_fragment_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_fragment_shader_read_color_input_attachment,
    moon_access_fragment_shader_read_depth_stencil_input_attachment,
    moon_access_fragment_shader_read_other,
    moon_access_color_attachment_read,
    moon_access_color_attachment_advanced_blending,
    moon_access_depth_stencil_attachment_read,
    moon_access_compute_shader_read_uniform_buffer,
    moon_access_compute_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_compute_shader_read_other,
    moon_access_any_shader_read_uniform_buffer,
    moon_access_any_shader_read_uniform_buffer_or_vertex_buffer,
    moon_access_any_shader_read_sampled_texture_or_uniform_texel_buffer,
    moon_access_any_shader_read_other,
    moon_access_video_decode_read,
    moon_access_video_encode_read,
    moon_access_transfer_read,
    moon_access_host_read,
    moon_access_present,
    moon_access_conditional_rendering,
    moon_access_ray_tracing_shader_acceleration_structure_read,
    moon_access_acceleration_structure_build_read,
    moon_access_end_of_read_enum,
    /* WRITE */
    moon_access_command_buffer_write = moon_access_end_of_read_enum,
    moon_access_vertex_shader_write,
    moon_access_tesselation_control_shader_write,
    moon_access_tesselation_evaluation_shader_write,
    moon_access_geometry_shader_write,
    moon_access_fragment_shader_write,
    moon_access_task_shader_write,
    moon_access_mesh_shader_write,
    moon_access_transfer_write,
    moon_access_host_preinitialized,
    moon_access_host_write,
    moon_access_clear_write,
    moon_access_color_attachment_write,
    moon_access_depth_stencil_attachment_write,
    moon_access_depth_attachment_write_stencil_read_only,
    moon_access_stencil_attachment_write_depth_read_only,
    moon_access_compute_shader_write,
    moon_access_any_shader_write,
    moon_access_video_decode_write,
    moon_access_video_encode_write,
    moon_access_acceleration_structure_build_write,
    /* OTHER */
    moon_access_color_attachment_read_write,
    /* covers any access, usefull for debugging but should be avoided for performance reasons. */
    moon_access_general,
    moon_access_max_enum,
} moon_access;

/** Rather than a list of all possible texture layouts, this reduced list 
 *  is correlated with the access types <lake/renderer/timeline_sync.h> 
 *  to map to the correct internal layouts. The optimal layout is preferred. */
typedef enum moon_layout : s8 {
    /** Choose the most optimal layout for each usage. Performs layout transitions
     *  as appropriate for the access. */
    moon_layout_optimal = 0,
    /** Layout accessible by all memory access types on a device - no layout transitions, 
     *  except for the purpose of presentation. */
    moon_layout_general,
    /** As general, but also allows presentation engines to access it - no layout transitions. */
    moon_layout_shared_present,
} moon_layout;

/** Global barriers define a set of accesses on multiple resources at once.
 *  If a buffer or texture doesn't require a queue ownership transfer, or an 
 *  texture doesn't require a layout transition (e.g. you're using the general 
 *  or present layouts) then a global barrier should be preferred. Simply 
 *  define the source and destination access types of resources affected. */
typedef struct moon_global_barrier {
    u32                     src_access_count;
    u32                     dst_access_count;
    moon_access const      *src_accesses;
    moon_access const      *dst_accesses;
} moon_global_barrier;

/** Buffer barriers should only be used when a queue type ownership transfer
 *  is required - prefer global barriers at all other times.
 *
 *  Access types are defined in the same way as for a global memory barrier, but
 *  they only affect the buffer range identified by buffer, offset and size,
 *  rather than all resources.
 *  
 *  A buffer barrier defining a queue ownership transfer needs to be executed
 *  twice - once by a queue in the source queue family, and then once again by a
 *  queue in the destination queue family, with a semaphore guaranteeing
 *  execution order between them. */
typedef struct moon_buffer_barrier {
    moon_queue_type         src_queue_type;
    moon_queue_type         dst_queue_type;
    u32                     src_access_count;
    u32                     dst_access_count;
    moon_access const      *src_accesses;
    moon_access const      *dst_accesses;
    moon_buffer_id          buffer;
    u64                     offset;
    u64                     size;
} moon_buffer_barrier;

/** Texture barriers should only be used when a queue type ownership transfer
 *  or an texture layout transition is required - prefer global barriers at 
 *  all other times. In general it is better to use texture barriers with 
 *  moon_texture_layout_*_optimal than it is to use global memory barriers 
 *  with textures using either of the general or present layouts.
 *
 *  Access types are defined in the same way as for a global memory barrier, 
 *  but they only affect the texture subresource range, rather than all resources.
 *
 *  An image barrier defining a queue ownership transfer needs to be executed
 *  twice - once by a queue in the source queue family, and then once again by a
 *  queue in the destination queue family, with a semaphore guaranteeing
 *  execution order between them.
 *
 *  If discardContents is set to true, the contents of the image become
 *  undefined after the barrier is executed, which can result in a performance
 *  boost over attempting to preserve the contents. This is particularly useful 
 *  for transient images where the contents are going to be immediately 
 *  overwritten. A good example of when to use this is when an application 
 *  re-uses a presented texture after acquiring the next swapchain image. */
typedef struct moon_texture_barrier {
    bool                            discard_contents;
    moon_queue_type                 src_queue_type;
    moon_queue_type                 dst_queue_type;
    moon_layout                     src_layout;
    moon_layout                     dst_layout;
    u32                             src_access_count;
    u32                             dst_access_count;
    moon_access const              *src_accesses;
    moon_access const              *dst_accesses;
    moon_texture_mip_array_slice    texture_slice;
    moon_texture_id                 texture;
} moon_texture_barrier;

/** Pairs a GPU timeline with a CPU timeline value. */
typedef struct moon_timeline_pair {
    moon_timeline_semaphore         timeline_semaphore;
    u64                             value;
} moon_timeline_pair;

/** Details needed to create a timeline query pool. */
typedef struct moon_timeline_query_pool_assembly {
    u32                             query_count;
    lake_small_string               name;
} moon_timeline_query_pool_assembly;
static constexpr moon_timeline_query_pool_assembly MOON_TIMELINE_QUERY_POOL_ASSEMBLY_INIT = {0};

/** Details needed to create a timeline semaphore. */
typedef struct moon_timeline_semaphore_assembly {
    u64                             initial_value;
    lake_small_string               name;
} moon_timeline_semaphore_assembly;
static constexpr moon_timeline_semaphore_assembly MOON_TIMELINE_SEMAPHORE_ASSEMBLY_INIT = {0};

/** Details needed to create a binary semaphore. */
typedef struct moon_binary_semaphore_assembly {
    lake_small_string               name;
} moon_binary_semaphore_assembly;
static constexpr moon_binary_semaphore_assembly MOON_BINARY_SEMAPHORE_ASSEMBLY_INIT = {0};

/** Details needed to create an event. */
typedef struct moon_event_assembly {
    lake_small_string               name;
} moon_event_assembly;
static constexpr moon_event_assembly MOON_EVENT_ASSEMBLY_INIT = {0};

/** Assemble a timeline query pool. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_query_pool_assembly)(struct moon_device_impl *device, moon_timeline_query_pool_assembly const *assembly, struct moon_timeline_query_pool_impl **out_timeline_query_pool);
#define FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_query_pool_assembly(struct moon_device_impl *device, moon_timeline_query_pool_assembly const *assembly, struct moon_timeline_query_pool_impl **out_timeline_query_pool)

/** Destroy a timeline query pool. */
PFN_LAKE_WORK(PFN_moon_timeline_query_pool_zero_refcnt, struct moon_timeline_query_pool_impl *timeline_query_pool);
#define FN_MOON_TIMELINE_QUERY_POOL_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_timeline_query_pool_zero_refcnt, struct moon_timeline_query_pool_impl *timeline_query_pool)

/** Query results from a timeline query pool. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_query_pool_query_results)(struct moon_timeline_query_pool_impl *timeline_query_pool, u32 start, u32 count, u64 *out_results);
#define FN_MOON_TIMELINE_QUERY_POOL_QUERY_RESULTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_query_pool_query_results(struct moon_timeline_query_pool_impl *timeline_query_pool, u32 start, u32 count, u64 *out_results)

/** Assemble a timeline semaphore. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_assembly)(struct moon_device_impl *device, moon_timeline_semaphore_assembly const *assembly, struct moon_timeline_semaphore_impl **out_timeline_semaphore);
#define FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_assembly(struct moon_device_impl *device, moon_timeline_semaphore_assembly const *assembly, struct moon_timeline_semaphore_impl **out_timeline_semaphore)

/** Destroy a timeline semaphore. */
PFN_LAKE_WORK(PFN_moon_timeline_semaphore_zero_refcnt, struct moon_timeline_semaphore_impl *timeline_semaphore);
#define FN_MOON_TIMELINE_SEMAPHORE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_timeline_semaphore_zero_refcnt, struct moon_timeline_semaphore_impl *timeline_semaphore)

/** Atomically read the current timeline value from a timeline semaphore. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_read_value)(struct moon_timeline_semaphore_impl *timeline_semaphore, u64 *out_value);
#define FN_MOON_TIMELINE_SEMAPHORE_READ_VALUE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_read_value(struct moon_timeline_semaphore_impl *timeline_semaphore, u64 *out_value)

/** Atomically write into a timeline semaphore's timeline value. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_write_value)(struct moon_timeline_semaphore_impl *timeline_semaphore, u64 value);
#define FN_MOON_TIMELINE_SEMAPHORE_WRITE_VALUE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_write_value(struct moon_timeline_semaphore_impl *timeline_semaphore, u64 value)

/** Wait until a timeline semaphore's value reaches a value, or until a timeout. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_wait_for_value)(struct moon_timeline_semaphore_impl *timeline_semaphore, u64 value, u64 timeout);
#define FN_MOON_TIMELINE_SEMAPHORE_WAIT_FOR_VALUE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_wait_for_value(struct moon_timeline_semaphore_impl *timeline_semaphore, u64 value, u64 timeout)

/** Assemble a binary semaphore. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_binary_semaphore_assembly)(struct moon_device_impl *device, moon_binary_semaphore_assembly const *assembly, struct moon_binary_semaphore_impl **out_binary_semaphore);
#define FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_binary_semaphore_assembly(struct moon_device_impl *device, moon_binary_semaphore_assembly const *assembly, struct moon_binary_semaphore_impl **out_binary_semaphore)

/** Destroy a binary semaphore. */
PFN_LAKE_WORK(PFN_moon_binary_semaphore_zero_refcnt, struct moon_binary_semaphore_impl *binary_semaphore);
#define FN_MOON_BINARY_SEMAPHORE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_binary_semaphore_zero_refcnt, struct moon_binary_semaphore_impl *binary_semaphore)

/** Assemble an event. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_event_assembly)(struct moon_device_impl *device, moon_event_assembly const *assembly, struct moon_event_impl **out_event);
#define FN_MOON_EVENT_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_event_assembly(struct moon_device_impl *device, moon_event_assembly const *assembly, struct moon_event_impl **out_event)

/** Destroy an event. */
PFN_LAKE_WORK(PFN_moon_event_zero_refcnt, struct moon_event_impl *event);
#define FN_MOON_EVENT_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_event_zero_refcnt, struct moon_event_impl *event)
