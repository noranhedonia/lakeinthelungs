#include "a_moonlit_walk.h"

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

/* TODO GBuffer layout:
 * - Normal octahedral map (rg16f)
 * - Albedo (rgb8u)
 * - AO + Roughness + Metallic (rgb8u)
 * - Velocity (rg16f)
 * - depth (d32f)
 *
 * the gbuffer pass should enable stencil to:
 * - avoid writing transparent geometry to the depth buffer
 * - avoid writing static geometry to the velocity buffer
 */
static lake_result create_render_targets(struct a_moonlit_walk *amw, moon_device device, u32 width, u32 height)
{
    (void)amw;
    (void)device;
    (void)width;
    (void)height;
    return LAKE_SUCCESS;
}

static lake_result create_compute_pipelines(struct a_moonlit_walk *amw, moon_device device)
{
    (void)amw;
    (void)device;
    return LAKE_SUCCESS;
}

static lake_result create_raster_pipelines(struct a_moonlit_walk *amw, moon_device device)
{
    (void)amw;
    (void)device;
    return LAKE_SUCCESS;
}

static lake_result create_ray_tracing_pipelines(struct a_moonlit_walk *amw, moon_device device)
{
    (void)amw;
    (void)device;
    return LAKE_SUCCESS;
}

lake_result game_prototype_init(struct a_moonlit_walk *amw)
{
    (void)g_scene_vtx_data;
    (void)g_scene_idx_data;
    (void)g_scene_primitive_data;
    u32 const width = (u32)amw->primary_window.header->assembly.width;
    u32 const height = (u32)amw->primary_window.header->assembly.height;
    moon_device device = amw->primary_device;

    lake_result result = create_render_targets(amw, device, width, height);
    if (result != LAKE_SUCCESS) {
        lake_error("Prototype init failed to create render targets.");
        return result;
    }
    result = create_compute_pipelines(amw, device);
    if (result != LAKE_SUCCESS) {
        lake_error("Prototype init failed to create compute pipelines.");
        return result;
    }
    result = create_raster_pipelines(amw, device);
    if (result != LAKE_SUCCESS) {
        lake_error("Prototype init failed to create raster pipelines.");
        return result;
    }
    if (device.header->details->implicit_features & moon_implicit_feature_ray_tracing_pipeline) {
        result = create_ray_tracing_pipelines(amw, device);
        if (result != LAKE_SUCCESS) {
            lake_error("Prototype init failed to create raster pipelines.");
            return result;
        }
    }
    return LAKE_SUCCESS;
}

void game_prototype_fini(struct a_moonlit_walk *amw)
{
    LAKE_UNUSED lake_result __ignore;
    moon_device device = amw->primary_device;
    u32 const frames_in_flight = amw->frames_in_flight;

    if (device.v == nullptr)
        return;

    /* destroy scene data */
    __ignore = amw->moon.interface->destroy_blas(device.impl, amw->scene_blas);
    __ignore = amw->moon.interface->destroy_tlas(device.impl, amw->scene_tlas);
    __ignore = amw->moon.interface->destroy_buffer(device.impl, amw->scene_idx_buffer);
    __ignore = amw->moon.interface->destroy_buffer(device.impl, amw->scene_vtx_buffer);

    /* destroy render targets */
    if (amw->render_targets != nullptr)
        for (u32 i = 0; i < frames_in_flight; i++)
            for (u32 j = 0; j < RENDER_TARGETS_COUNT; j++)
                __ignore = amw->moon.interface->destroy_texture(device.impl, amw->render_targets[i].v[j]);

    /* destroy pipelines */
    for (u32 i = 0; i < COMPUTE_PIPELINES_COUNT; i++)
        if (amw->compute_pipelines.v[i].impl != nullptr)
            moon_compute_pipeline_unref(amw->compute_pipelines.v[i]);
    for (u32 i = 0; i < RASTER_PIPELINES_COUNT; i++)
        if (amw->raster_pipelines.v[i].impl != nullptr)
            moon_raster_pipeline_unref(amw->raster_pipelines.v[i]);
    for (u32 i = 0; i < RT_PIPELINES_COUNT; i++)
        if (amw->rt_pipelines.v[i].impl != nullptr)
            moon_ray_tracing_pipeline_unref(amw->rt_pipelines.v[i]);
}
