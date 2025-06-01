#pragma once

/** @file lake/renderer/pipelines.h
 *  @brief TODO
 *
 *  TODO docs
 */
#include <lake/renderer/moon_adapter.h>

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

typedef enum moon_work_graph_node_type : s8 {
    moon_work_graph_node_type_compute = 0,
    moon_work_graph_node_type_mesh,
} moon_work_graph_node_type;

typedef union moon_work_graph_node {
    moon_compute_pipeline compute;
    moon_raster_pipeline  mesh;
} moon_work_graph_node;

typedef struct moon_work_graph_pipeline_assembly {
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

typedef struct moon_ray_tracing_pipeline_assembly {
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
    moon_sample_count               static_state_sample_count;
    bool                            has_conservative_rasterizer;
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

/** Assemble an array of compute pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_compute_pipeline_assembly)(moon_device device, u32 count, moon_compute_pipeline_assembly const *assembly_array, moon_compute_pipeline *out_pipelines);
#define FN_MOON_COMPUTE_PIPELINE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_compute_pipeline_assembly(moon_device device, u32 count, moon_compute_pipeline_assembly const *assembly_array, moon_compute_pipeline *out_pipelines)

/** Destroy a compute pipeline. */
PFN_LAKE_WORK(PFN_moon_compute_pipeline_zero_refcnt, moon_compute_pipeline pipeline);
#define FN_MOON_COMPUTE_PIPELINE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_compute_pipeline_zero_refcnt, moon_compute_pipeline pipeline)

/** Assemble an array of work graph pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_work_graph_pipeline_assembly)(moon_device device, u32 count, moon_work_graph_pipeline_assembly const *assembly_array, moon_work_graph_pipeline *out_pipelines);
#define FN_MOON_WORK_GRAPH_PIPELINE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_work_graph_pipeline_assembly(moon_device device, u32 count, moon_work_graph_pipeline_assembly const *assembly_array, moon_work_graph_pipeline *out_pipelines)

/** Destroy a work graph pipeline. */
PFN_LAKE_WORK(PFN_moon_work_graph_pipeline_zero_refcnt, moon_work_graph_pipeline pipeline);
#define FN_MOON_WORK_GRAPH_PIPELINE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_work_graph_pipeline_zero_refcnt, moon_work_graph_pipeline pipeline)

/** Assemble an array of ray tracing pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_ray_tracing_pipeline_assembly)(moon_device device, u32 count, moon_ray_tracing_pipeline_assembly const *assembly_array, moon_ray_tracing_pipeline *out_pipelines);
#define FN_MOON_RAY_TRACING_PIPELINE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_ray_tracing_pipeline_assembly(moon_device device, u32 count, moon_ray_tracing_pipeline_assembly const *assembly_array, moon_ray_tracing_pipeline *out_pipelines)

/** Destroy a ray tracing pipeline. */
PFN_LAKE_WORK(PFN_moon_ray_tracing_pipeline_zero_refcnt, moon_ray_tracing_pipeline pipeline);
#define FN_MOON_RAY_TRACING_PIPELINE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_ray_tracing_pipeline_zero_refcnt, moon_ray_tracing_pipeline pipeline)

/** Create a default shader binding table for a given ray tracing pipeline. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_ray_tracing_pipeline_create_default_sbt)(moon_ray_tracing_pipeline pipeline, moon_shader_binding_table *out_sbt, moon_buffer_id *out_buffer);
#define FN_MOON_RAY_TRACING_PIPELINE_CREATE_DEFAULT_SBT(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_ray_tracing_pipeline_create_default_sbt(moon_ray_tracing_pipeline pipeline, moon_shader_binding_table *out_sbt, moon_buffer_id *out_buffer)

/** Acquire a blob of shader group handles for a given ray tracing pipeline. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_ray_tracing_pipeline_shader_group_handles)(moon_ray_tracing_pipeline pipeline, void *out_blob);
#define FN_MOON_RAY_TRACING_PIPELINE_SHADER_GROUP_HANDLES(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_ray_tracing_pipeline_shader_group_handles(moon_ray_tracing_pipeline pipeline, void *out_blob)

/** Assemble an array of raster pipelines. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_raster_pipeline_assembly)(moon_device device, u32 count, moon_raster_pipeline_assembly const *assembly_array, moon_raster_pipeline *out_pipelines);
#define FN_MOON_RASTER_PIPELINE_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_raster_pipeline_assembly(moon_device device, u32 count, moon_raster_pipeline_assembly const *assembly_array, moon_raster_pipeline *out_pipelines)

/** Destroy a raster pipeline. */
PFN_LAKE_WORK(PFN_moon_raster_pipeline_zero_refcnt, moon_raster_pipeline pipeline);
#define FN_MOON_RASTER_PIPELINE_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_raster_pipeline_zero_refcnt, moon_raster_pipeline pipeline)

/** Header for `struct moon_compute_pipeline_impl`. */
typedef struct moon_compute_pipeline_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_compute_pipeline_assembly      assembly;
} moon_compute_pipeline_header;

/** Header for `struct moon_work_graph_pipeline_impl`. */
typedef struct moon_work_graph_pipeline_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_work_graph_pipeline_assembly   assembly;
} moon_work_graph_pipeline_header;

/** Header for `struct moon_ray_tracing_pipeline_impl`. */
typedef struct moon_ray_tracing_pipeline_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_ray_tracing_pipeline_assembly  assembly;
} moon_ray_tracing_pipeline_header;

/** Header for `struct moon_raster_pipeline_impl`. */
typedef struct moon_raster_pipeline_header {
    moon_device                         device;
    atomic_u32                          flags;
    lake_refcnt                         refcnt;
    moon_raster_pipeline_assembly       assembly;
} moon_raster_pipeline_header;
