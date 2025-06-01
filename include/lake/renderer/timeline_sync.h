#pragma once

/** @file lake/renderer/timeline_sync.h
 *  @brief TODO
 *
 *  TODO docs
 */
#include <lake/renderer/moon_adapter.h>

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
typedef struct moon_timeline_pair {
    moon_timeline_semaphore         timeline_semaphore;
    u64                             value;
} moon_timeline_pair;

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
typedef struct moon_memory_barrier {
    moon_access                     src_access;
    moon_access                     dst_access;
} moon_memory_barrier;

/** Pipeline barrier used for image access and transitions. */
typedef struct moon_texture_memory_barrier {
    moon_memory_barrier             barrier;
    moon_texture_layout             src_layout;
    moon_texture_layout             dst_layout;
    moon_texture_mip_array_slice    texture_slice;
    moon_texture_id                 texture;
} moon_texture_memory_barrier;

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
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_query_pool_assembly)(moon_device device, moon_timeline_query_pool_assembly const *assembly, moon_timeline_query_pool *out_timeline_query_pool);
#define FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_query_pool_assembly(moon_device device, moon_timeline_query_pool_assembly const *assembly, moon_timeline_query_pool *out_timeline_query_pool)

/** Destroy a timeline query pool. */
PFN_LAKE_WORK(PFN_moon_timeline_query_pool_zero_refcnt, moon_timeline_query_pool timeline_query_pool);
#define FN_MOON_TIMELINE_QUERY_POOL_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_timeline_query_pool_zero_refcnt, moon_timeline_query_pool timeline_query_pool)

/** Query results from a timeline query pool. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_query_pool_query_results)(moon_timeline_query_pool timeline_query_pool, u32 start, u32 count, u64 *out_results);
#define FN_MOON_TIMELINE_QUERY_POOL_QUERY_RESULTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_query_pool_query_results(moon_timeline_query_pool timeline_query_pool, u32 start, u32 count, u64 *out_results)

/** Assemble a timeline semaphore. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_assembly)(moon_device device, moon_timeline_semaphore_assembly const *assembly, moon_timeline_semaphore *out_timeline_semaphore);
#define FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_assembly(moon_device device, moon_timeline_semaphore_assembly const *assembly, moon_timeline_semaphore *out_timeline_semaphore)

/** Destroy a timeline semaphore. */
PFN_LAKE_WORK(PFN_moon_timeline_semaphore_zero_refcnt, moon_timeline_semaphore timeline_semaphore);
#define FN_MOON_TIMELINE_SEMAPHORE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_timeline_semaphore_zero_refcnt, moon_timeline_semaphore timeline_semaphore)

/** Atomically read the current timeline value from a timeline semaphore. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_read_value)(moon_timeline_semaphore timeline_semaphore, u64 *out_value);
#define FN_MOON_TIMELINE_SEMAPHORE_READ_VALUE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_read_value(moon_timeline_semaphore timeline_semaphore, u64 *out_value)

/** Atomically write into a timeline semaphore's timeline value. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_write_value)(moon_timeline_semaphore timeline_semaphore, u64 value);
#define FN_MOON_TIMELINE_SEMAPHORE_WRITE_VALUE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_write_value(moon_timeline_semaphore timeline_semaphore, u64 value)

/** Wait until a timeline semaphore's value reaches a value, or until a timeout. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_timeline_semaphore_wait_for_value)(moon_timeline_semaphore timeline_semaphore, u64 value, u64 timeout);
#define FN_MOON_TIMELINE_SEMAPHORE_WAIT_FOR_VALUE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_timeline_semaphore_wait_for_value(moon_timeline_semaphore timeline_semaphore, u64 value, u64 timeout)

/** Assemble a binary semaphore. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_binary_semaphore_assembly)(moon_device device, moon_binary_semaphore_assembly const *assembly, moon_binary_semaphore *out_binary_semaphore);
#define FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_binary_semaphore_assembly(moon_device device, moon_binary_semaphore_assembly const *assembly, moon_binary_semaphore *out_binary_semaphore)

/** Destroy a binary semaphore. */
PFN_LAKE_WORK(PFN_moon_binary_semaphore_zero_refcnt, moon_binary_semaphore binary_semaphore);
#define FN_MOON_BINARY_SEMAPHORE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_binary_semaphore_zero_refcnt, moon_binary_semaphore binary_semaphore)

/** Assemble an event. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_event_assembly)(moon_device device, moon_event_assembly const *assembly, moon_event *out_event);
#define FN_MOON_EVENT_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_event_assembly(moon_device device, moon_event_assembly const *assembly, moon_event *out_event)

/** Destroy an event. */
PFN_LAKE_WORK(PFN_moon_event_zero_refcnt, moon_event event);
#define FN_MOON_EVENT_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_event_zero_refcnt, moon_event event)

/** Header for `struct moon_timeline_query_pool_impl`. */
typedef struct moon_timeline_query_pool_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_timeline_query_pool_assembly   assembly;
} moon_timeline_query_pool_header;

/** Header for `struct moon_timeline_semaphore_impl`. */
typedef struct moon_timeline_semaphore_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_timeline_semaphore_assembly    assembly;
} moon_timeline_semaphore_header;

/** Header for `struct moon_binary_semaphore_impl`. */
typedef struct moon_binary_semaphore_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_binary_semaphore_assembly      assembly;
} moon_binary_semaphore_header;

/** Header for `struct moon_event_impl`. */
typedef struct moon_event_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_event_assembly                 assembly;
} moon_event_header;
