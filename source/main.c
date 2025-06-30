#define LAKE_IN_THE_LUNGS_MAIN
#include "main.h"

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

FN_SORCERESS_ACQUIRE_WORK(lungs)
{
    u32 const work_count = MAX_FRAMES_IN_FLIGHT;
    u32 const render_device_count = sorceress->renderer.device_count;
    struct sorceress_work_impl *work = lake_drift_n(struct sorceress_work_impl, work_count);
    lake_render_submit *submits = lake_drift_n(lake_render_submit, work_count * render_device_count);

    lake_memset(work, 0, sizeof(struct sorceress_work_impl) * work_count);
    lake_memset(submits, 0, sizeof(lake_render_submit) * work_count * render_device_count);

    for (u32 i = 0; i < work_count; i ++) {
        work[i].header.sorceress.impl = sorceress;
        work[i].header.last_work = &work[(i + (work_count-2)) & (work_count-1)];
        work[i].header.next_work = &work[(i + work_count) & (work_count-1)];
        work[i].submits = &submits[work_count * render_device_count];
    }
    *out_work_count = work_count;
    *out_work = work;
    return sizeof(struct sorceress_work_impl);
}

FN_SORCERESS_RELEASE_WORK(lungs)
{
    lake_dbg_assert(lake_is_pow2(work_count) && work, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    struct sorceress_impl *sorceress = work->header.sorceress.impl;

    for (u32 i = 0; i < work_count; i++) {
        (void)sorceress;
    }
}

static FN_LAKE_WORK(_sorceress_lungs_zero_refcnt, struct sorceress_impl *sorceress)
{
    (void)g_scene_vtx_data;
    (void)g_scene_idx_data;
    (void)g_scene_primitive_data;
    __lake_free(sorceress);
}

static FN_LAKE_INTERFACE_IMPL(sorceress, lungs)
{
    char const *name = "lungs";

    struct sorceress_impl *sorceress = __lake_malloc_t(struct sorceress_impl);
    lake_zerop(sorceress);

    sorceress->interface.header.bedrock = bedrock;
    sorceress->interface.header.zero_refcnt = (PFN_lake_work)_sorceress_lungs_zero_refcnt;
    sorceress->interface.header.name.len = lake_strlen(name);
    lake_memcpy(sorceress->interface.header.name.str, name, sorceress->interface.header.name.len);

    sorceress->interface.acquire_work = _sorceress_lungs_acquire_work;
    sorceress->interface.release_work = _sorceress_lungs_release_work;
    sorceress->interface.stage_gameplay = _sorceress_lungs_stage_gameplay;
    sorceress->interface.stage_rendering = _sorceress_lungs_stage_rendering;
    sorceress->interface.stage_gpuexec = _sorceress_lungs_stage_gpuexec;

    lake_trace("Connected to sorceress::%s.", name);
    lake_inc_refcnt(&sorceress->interface.header.refcnt);
    return sorceress;
}

s32 LAKECALL lake_main(lake_bedrock *bedrock)
{
    bedrock->hints.fiber_stack_size = 128*1024;
    bedrock->hints.frames_in_flight = 3;
    bedrock->engine_name = "sorceress";
    bedrock->app_name = "Lake in the Lungs";
    lake_log_enable_colors(true);

    return lake_in_the_lungs(
        (PFN_lake_bedrock_main)sorceress_bedrock_main, 
        (PFN_lake_interface_impl)sorceress_interface_impl_lungs, 
        bedrock);
}
