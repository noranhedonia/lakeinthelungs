#define LAKE_IN_THE_LUNGS_MAIN
#include <lake/sorceress.h>

#define MAX_FRAMES_IN_FLIGHT 4

/* XXX The shader pipelines and resources like samplers, textures, buffers, acceleration structures,
 * can later be moved into higher-level structures for the renderer. For now keep things simple. */

/* The test scene contains a floor plane, and a cube placed on top of it at the center. */
static constexpr vec3 g_scene_vtx_data[] = {
    /* floor plane */
    {-100.0f, 0,  100.0f},
    { 100.0f, 0,  100.0f},
    { 100.0f, 0, -100.0f},
    {-100.0f, 0, -100.0f},
    /* cube face (+y) */
    {-1.0f, 2.0,  1.0f},
    { 1.0f, 2.0,  1.0f},
    { 1.0f, 2.0, -1.0f},
    {-1.0f, 2.0, -1.0f},
    /* cube face (+z) */
    {-1.0f, 0.0, 1.0f},
    { 1.0f, 0.0, 1.0f},
    { 1.0f, 2.0, 1.0f},
    {-1.0f, 2.0, 1.0f},
    /* cube face (-z) */
    {-1.0f, 0.0, -1.0f},
    {-1.0f, 2.0, -1.0f},
    { 1.0f, 2.0, -1.0f},
    { 1.0f, 0.0, -1.0f},
    /* cube face (-x) */
    {-1.0f, 0.0, -1.0f},
    {-1.0f, 0.0,  1.0f},
    {-1.0f, 2.0,  1.0f},
    {-1.0f, 2.0, -1.0f},
    /* cube face (+x) */
    {1.0f, 2.0, -1.0f},
    {1.0f, 2.0,  1.0f},
    {1.0f, 0.0,  1.0f},
    {1.0f, 0.0, -1.0f},
};
static constexpr int g_scene_idx_data[] = {
     0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11, 12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
};

struct primitive {
    vec4 data;
    vec4 color;
};
static constexpr struct primitive g_scene_primitive_data[] = {
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.75f, 0.8f,  0.85f, 1.0f}},
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.75f, 0.8f,  0.85f, 1.0f}},
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 1.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f,  1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f,  1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f, -1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 0.0f, 0.0f, -1.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{-1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{-1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
    {{ 1.0f, 0.0f,  0.0f, 0.0f}, {0.95f, 0.85f, 0.05f, 1.0f}},
};

/** Available methods for sampling polygonal lights. */
enum sample_polygon_technique : s8 {
    /** A mock technique, designed to have practically zero cost. 
     *  Useful for comparison of run time measurements. */
    sample_polygon_technique_baseline = 0,
    /** Uniform sampling over the area of the polygon using Turk's method. */
    sample_polygon_technique_area_turk,
    /** Sampling proportional to solid angle using Urena's method, which is limited to 
     *  rectangles. Unlike all other techniques, this one samples a light source of different 
     *  shape unless the plane space vertices are (0,0), (0,1), (1,1) (1,0). */
    sample_polygon_technique_rectangle_solid_angle_urena,
    /** Sampling proportional to solid angle using a straight forward implementation of Arvo's method. */
    sample_polygon_technique_solid_angle_arvo,
    /** Sampling proportional to solid angle using our method. */
    sample_polygon_technique_solid_angle,
    /** Sampling proportional to solid angle but with clipping to the upper hemisphere using our method. */
    sample_polygon_technique_clipped_solid_angle,
    /** Sampling proportional to solid angle without clipping to the upper hemisphere. Samples are 
     *  warped in primary sample space according to a bilinear approximation of the cosine term 
     *  as proposed by Hart et al. */
    sample_polygon_technique_bilinear_cosine_warp_hart,
    /** Like `sample_polygon_technique_bilinear_cosine_warp_hart` but with clipping. */
    sample_polygon_technique_bilinear_cosine_warp_clipping_hart,
    /** Like `sample_polygon_technique_bilinear_cosine_warp_hart` 
     *  but with a biquadratic approximation to the cosine term. */
    sample_polygon_technique_biquadratic_cosine_warp_hart,
    /** Like `sample_polygon_technique_biquatratic_cosine_warp_hart` but with clipping. */
    sample_polygon_technique_biquadratic_cosine_warp_clipping_hart,
    /** Sampling proportional to projected solid angles using Arvo's method. */
    sample_polygon_technique_projected_solid_angle_arvo,
    /** Sampling proportional to projected solid angle with our method. */
    sample_polygon_technique_projected_solid_angle,
    /** Sampling proportional to projected solid angle with a biased 
     *  but less costly version of our method. */
    sample_polygon_technique_projected_solid_angle_biased,
    /** Number of available polygon sampling techniques. */
    sample_polygon_technique_count,
};

/** Available methods to make use of a texture for computation of the radiance of a polygonal light.
 *  In any case, the value from the texture is multiplied onto the uniform surface radiance. */
enum polygon_texturing_technique : s8 {
    /** No texturing is used. */
    polygon_texturing_technique_none = 0,
    /** Plane space coordinates are used as texture coordinates. */
    polygon_texturing_technique_area,
    /** The outgoing light direction gets flipped and used for a lookup in a 
     *  light probe parameterized through spherical coordinates. */
    polygon_texturing_technique_portal,
    /** A 1D texture is interpreted as IES profile, i.e. it changes the 
     *  emission dependent on the angle. */
    polygon_texturing_technique_ies_profile,
    /** Number of available polygon texturing methods. */
    polygon_texturing_technique_count,
};

/** It represents a convex polygonal light source. The polygon is planar but oriented arbitrarily
 *  in 3D space. By design, it matches the layout of the corresponding structure in the shader. */
struct polygonal_light {
    /* TODO */
    f32 todo;
};

/** Available methods to combine diffuse and specular samples. */
enum sampling_strategies : s8 {
    /** Only the sampling strategy for diffuse samples is used, but the full BRDF is evaluated. */
    sampling_strategies_diffuse_only = 0,
    /** Sampling strategies for diffuse and specular samples are used and combined by multiple 
     *  importance sampling. The specular samples are produced in proportion to the GGX specular 
     *  BRDF by sampling the visibile normal distribution function (VNDF) but regardless of the 
     *  geometry of the polygonal light, i.e. they may miss it. */
    sampling_strategies_diffuse_ggx_mis,
    /** The diffuse sampling strategy is used with the diffuse BRDF only and the specular 
     *  sampling strategy is used with the specular BRDF only. */
    sampling_strategies_diffuse_specular_separately,
    /** Sampling strategies for diffuse and specular samples are used and combined 
     *  by multiple importance sampling. */
    sampling_strategies_diffuse_specular_mis,
    /** One of the sampling strategies for diffuse and specular is picked randomly
     *  with weights proportional to the estimated unshadowed contribution. */
    sampling_strategies_diffuse_specular_random,
    /** Number of available sampling strategies. */
    sampling_strategies_count,
};

/** Available variants of multiple importance sampling. See Veach's thesis:
 *  http://graphics.stanford.edu/papers/veach_thesis/ */
enum mis_heuristic : s8 {
    /** The balance heuristic as described by Veach (p. 264). */
    mis_heuristic_balance = 0,
    /** The power heuristic with exponent 2 as described by Veach (p. 273). */
    mis_heuristic_power,
    /** Our weighted variant of the balance heuristic that incorporates estimates 
     *  of the unshadowed reflected radiance. */
    mis_heuristic_weighted,
    /** Our optimal MIS strategy, clamped to only use non-negative weights. */
    mis_heuristic_optimal_clamped,
    /** Our optimal MIS strategy. It is a blend between balance heuristic and 
     *  weighted balance heuristic, and under some fairly weak assumptions it is 
     *  probably optimal for some degree of visibility. It uses negative weights 
     *  and produces non-zero splats (with zero-mean) for occluded rays. This way 
     *  it reduces variance a bit but introduces leaking. */
    mis_heuristic_optimal,
    /** Number of available heuristics. */
    mis_heuristic_count,
};

/** Settings for how the error of projected solid angle sampling should be visualized. */
enum error_display : s8 {
    /** The scene is rendered as is, no errors are displayed. */
    error_display_none = 0,
    /** The error of the first sample using the diffuse sampling strategy is displayed as error 
     *  in the first random number. */
    error_display_diffuse_backward,
    /** The error of the first sample using the diffuse sampling strategy is displayed as error 
     *  in the first random number, multiplied by the projected solid angle of the polygon. */
    error_display_diffuse_backward_scaled,
    /** The error of the first sample using the diffuse sampling strategy is displayed as error 
     *  in the sampled direction in radians. */
    error_display_diffuse_forward,
    /** The error of the first sample using the specular sampling strategy is displayed as error 
     *  in the first random number. */
    error_display_specular_backward,
    /** The error of the first sample using the specular sampling strategy is displayed as error 
     *  in the first random number, multiplied by the projected solid angle of the polygon. */
    error_display_specular_backward_scaled,
    /** The error of the first sample using the specular sampling strategy is displayed as error 
     *  in the sampled direction in radians. */
    error_display_specular_forward,
    /** Number of available settings. */
    error_display_count,
};

/** Options that control how the scene will be rendered, set at beginning of the rendering stage 
 *  and persists until the end of the workflow. */
struct render_settings {
    /** Constant factors for the overall brightness and surface roughness. */
    f32                             exposure_factor, roughness_factor;
    /** An estimate of how much each shading point is shadowed. Used as a parameter 
     *  to control optimal multiple importance sampling. */
    f32                             mis_visibility_estimate;
    /** The number of samples used per sampling technique. */ 
    u32                             sample_count;
    /** An error of pow(10.f, error_min_exponent) is displayed as radiant pink. */
    f32                             error_min_exponent;
    /** Whether the error of the diffuse or specular sampling strategy should be visualized. */
    enum error_display              error_display;
    /** The way in which diffuse and specular samples are combined. */
    enum sampling_strategies        sampling_strategies;
    /** The technique used to sample polygonal lights. */
    enum sample_polygon_technique   sample_polygon_technique;
    /** The heuristic used for multiple importance sampling. */
    enum mis_heuristic              mis_heuristic;
    /** Whether ray traced shadows should be used. */
    bool                            trace_shadow_rays;
    /** Whether light sources should be rendered. */
    bool                            show_light_sources;
    /** Whether screen-space elements of the graphical user interface should be rendered. */
    bool                            show_gui;
    /** Whether vertical synchronization should be used. */
    bool                            v_sync;
};

/** Samplers are used in any shaders that read from textures. */
enum sampler_type : s8 {
    /** For the gbuffer pass to read material data. */
    sampler_type_geometry = 0,
    /** For light textures in a shading pass. */
    sampler_type_light,
    /** For the font texture in an interface pass. */
    sampler_type_font,
    /** How many samplers are used in this prototype. */
    sampler_type_count,
};

/** Render targets except the swapchain images. They are duplicated per frame in flight. */
enum render_target : s8 {
    /** Normal octahedral map (rg16f). */
    render_target_normal_octahedral,
    /** Color of non-transparent geometry (rgb8u). */
    render_target_albedo_buffer,
    /** AO, roughness, metalic, emissive - material data (rgba8u). */
    render_target_orm_buffer,
    /** Velocity buffer of non-static geometry (rg16f). */
    render_target_velocity_buffer,
    /** Depth buffer of non-transparent geometry (d32f). */
    render_target_depth_buffer,
    /** How many render targets there are in a single frame in flight. */
    render_target_count,
};

/** Raster and task/mesh render passes. */
enum raster_pipeline : s8 {
    /** This pass rasterizes all geometry once and saves material data per fragment into buffers. */
    raster_pipeline_gbuffer = 0,
    /** How many raster pipelines are used in this prototype. */
    raster_pipeline_count,
};

/** Ray tracing render passes. */
enum ray_tracing_pipeline : s8 {
    /** Path tracing with Monte-Carlo integration. */
    ray_tracing_pipeline_path_trace = 0,
    /** How many ray tracing pipelines are used in this prototype. */
    ray_tracing_pipeline_count,
};

/** Describes a static scene. */
struct scene_details {
    moon_buffer_id                  buffer;
    u64                             buffer_vtx_offset, buffer_vtx_size;
    u64                             buffer_idx_offset, buffer_idx_size;
    u64                             buffer_tlas_offset, buffer_tlas_size;
    u64                             buffer_blas_offset, buffer_blas_size;
    moon_tlas_id                    tlas;
    moon_blas_id                    blas;
};

/** Implements the `sorceress` interface. */
struct sorceress_impl {
    struct sorceress_interface_impl interface;

    /* XXX All these details that depend on other interfaces must be handled via movements. */
    struct scene_details            scene;
    moon_raster_pipeline            raster_passes[raster_pipeline_count];
    moon_ray_tracing_pipeline       ray_tracing_passes[ray_tracing_pipeline_count];
    moon_texture_id                 render_targets[MAX_FRAMES_IN_FLIGHT][render_target_count];
    moon_sampler_id                 samplers[sampler_type_count];
};

static FN_SORCERESS_MOVEMENT(inthelungs) 
{
    (void)encore_composer;
}

static FN_LAKE_WORK(_sorceress_inthelungs_zero_refcnt, struct sorceress_impl *sorceress)
{
    __lake_free(sorceress);
}

FN_LAKE_INTERFACE_IMPL(sorceress, inthelungs)
{
    char const *name = "inthelungs";

    struct sorceress_impl *sorceress = __lake_malloc_t(struct sorceress_impl);
    lake_zerop(sorceress);

    sorceress->interface.header.bedrock = bedrock;
    sorceress->interface.header.zero_refcnt = (PFN_lake_work)_sorceress_inthelungs_zero_refcnt;
    sorceress->interface.header.name.len = lake_strlen(name);
    lake_memcpy(sorceress->interface.header.name.str, name, sorceress->interface.header.name.len);

    (void)g_scene_vtx_data;
    (void)g_scene_idx_data;
    (void)g_scene_primitive_data;

    sorceress->interface.movement = _sorceress_inthelungs_movement;

    lake_trace("Connected to spell::%s.", name);
    lake_inc_refcnt(&sorceress->interface.header.refcnt);
    return sorceress;
}

s32 LAKECALL lake_main(lake_bedrock *bedrock, s32 argc, char const **argv)
{
    sorceress_composition composition;
    bedrock->hints.fiber_stack_size = 128*1024;
    bedrock->hints.frames_in_flight = 3;
    bedrock->hints.enable_debug_instruments = LAKE_DEBUG_INSTRUMENTS;
    bedrock->engine_name = "sorceress";
    bedrock->app_name = "Lake in the Lungs";
    lake_log_enable_colors(true);

    s32 result = sorceress_compose_w_args(&composition, argc, argv);
    composition.sorceress_interface_impl = (PFN_lake_interface_impl)sorceress_interface_impl_inthelungs;

    if (result == LAKE_SUCCESS)
        result = lake_in_the_lungs((PFN_lake_bedrock_main)sorceress_bedrock_main, &composition, bedrock);
    return result;
}
