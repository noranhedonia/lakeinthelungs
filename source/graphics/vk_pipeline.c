#include "vk_moon.h"
#ifdef MOON_VULKAN

struct populate_compute_pipeline_work {
    struct moon_compute_pipeline_impl                   impl;
    VkComputePipelineCreateInfo                        *vk_create_info;
    VkShaderModule                                      vk_shader_module;
    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo vk_require_subgroup_size;
    VkResult                                            vk_result;
};

static FN_LAKE_WORK(populate_compute_pipeline, struct populate_compute_pipeline_work *work)
{
    struct moon_compute_pipeline_impl *impl = &work->impl;
    struct moon_device_impl const *device = impl->header.device.impl;
    moon_shader_assembly const *shader_assembly = &impl->header.assembly.shader;

    VkShaderModuleCreateInfo const vk_shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = shader_assembly->byte_code_size * sizeof(u32),
        .pCode = shader_assembly->byte_code,
    };
    work->vk_result = device->vkCreateShaderModule(device->vk_device, &vk_shader_module_create_info, device->vk_allocator, &work->vk_shader_module);
    if (work->vk_result != VK_SUCCESS)
        return;
    impl->vk_pipeline_layout = device->gpu_sr_table.pipeline_layouts[(impl->header.assembly.push_constant_size + 3) >> 2];

    work->vk_require_subgroup_size = (VkPipelineShaderStageRequiredSubgroupSizeCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO,
        .pNext = nullptr,
        .requiredSubgroupSize = impl->header.assembly.shader.has_required_subgroup_size 
            ? impl->header.assembly.shader.required_subgroup_size : 0,
    };
    *work->vk_create_info = (VkComputePipelineCreateInfo){
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = (VkPipelineShaderStageCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = impl->header.assembly.shader.has_required_subgroup_size
                ? &work->vk_require_subgroup_size : nullptr,
            .flags = (VkPipelineShaderStageCreateFlags)impl->header.assembly.shader.create_flags,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = work->vk_shader_module,
            .pName = impl->header.assembly.shader.entry_point.str,
            .pSpecializationInfo = nullptr,
        },
        .layout = impl->vk_pipeline_layout,
        .basePipelineHandle = nullptr,
        .basePipelineIndex = 0,
    };
}

static void LAKECALL destroy_compute_pipeline_assembly_shader_modules(
    struct moon_device_impl const         *device, 
    u32 const                              count, 
    struct populate_compute_pipeline_work *work)
{
    for (u32 i = 0; i < count; i++)
        if (work[i].vk_shader_module != VK_NULL_HANDLE)
            device->vkDestroyShaderModule(device->vk_device, work[i].vk_shader_module, device->vk_allocator);
}

struct populate_work_graph_pipeline_work {
    struct moon_work_graph_pipeline_impl    impl;
    VkExecutionGraphPipelineCreateInfoAMDX *vk_create_info;
    lake_darray_t(VkShaderModule)           vk_shader_modules;
    VkResult                                vk_result;
};

static FN_LAKE_WORK(populate_work_graph_pipeline, struct populate_work_graph_pipeline_work *work)
{
    VkResult vk_result = VK_ERROR_UNKNOWN;

    /* TODO */

    work->vk_result = vk_result;
}

static void LAKECALL destroy_work_graph_pipeline_assembly_shader_modules(
    struct moon_device_impl const            *device, 
    u32 const                                 count, 
    struct populate_work_graph_pipeline_work *work)
{
    for (u32 i = 0; i < count; i++) {
        lake_darray_foreach_v(work[i].vk_shader_modules, VkShaderModule, module) {
            if (*module != VK_NULL_HANDLE) device->vkDestroyShaderModule(device->vk_device, *module, device->vk_allocator);
        }
        lake_darray_fini(&work[i].vk_shader_modules.da);
    }
}

FN_MOON_WORK_GRAPH_PIPELINE_NODE_INDEX(vulkan)
{
    struct moon_device_impl *device = pipeline->header.device.impl;
    if (!(device->header.details->implicit_features & moon_implicit_feature_work_graph))
        return LAKE_ERROR_FEATURE_NOT_PRESENT;

    VkPipelineShaderStageNodeCreateInfoAMDX const vk_node_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_NODE_CREATE_INFO_AMDX,
        .pNext = nullptr,
        .pName = node->name,
        .index = node->shader_index,
    };
    return vk_result_translate(
        device->vkGetExecutionGraphPipelineNodeIndexAMDX(
            device->vk_device, 
            pipeline->vk_pipeline, 
            &vk_node_create_info, 
            out_index));
}

FN_MOON_WORK_GRAPH_PIPELINE_SCRATCH_SIZE(vulkan)
{
    struct moon_device_impl *device = pipeline->header.device.impl;
    if (!(device->header.details->implicit_features & moon_implicit_feature_work_graph))
        return LAKE_ERROR_FEATURE_NOT_PRESENT;

    VkExecutionGraphPipelineScratchSizeAMDX vk_exec_graph_scratch_size = {
        .sType = VK_STRUCTURE_TYPE_EXECUTION_GRAPH_PIPELINE_SCRATCH_SIZE_AMDX,
        .pNext = nullptr,
    };
    VkResult vk_result = device->vkGetExecutionGraphPipelineScratchSizeAMDX(
            device->vk_device, 
            pipeline->vk_pipeline, 
            &vk_exec_graph_scratch_size);
    if (vk_result == VK_SUCCESS)
        *out_scratch_size = (moon_work_graph_scratch_size){
            .min_size = vk_exec_graph_scratch_size.minSize,
            .max_size = vk_exec_graph_scratch_size.maxSize,
            .granularity = vk_exec_graph_scratch_size.sizeGranularity,
        };
    return vk_result_translate(vk_result);
}

struct populate_ray_tracing_pipeline_work {
    struct moon_ray_tracing_pipeline_impl               impl;
    VkRayTracingPipelineCreateInfoKHR                  *vk_create_info;
    lake_darray_t(VkShaderModule)                       vk_shader_modules;
    lake_darray_t(VkPipelineShaderStageCreateInfo)      vk_stages;
    lake_darray_t(VkRayTracingShaderGroupCreateInfoKHR) vk_groups;
    VkResult                                            vk_result;
};

static FN_LAKE_WORK(populate_ray_tracing_pipeline, struct populate_ray_tracing_pipeline_work *work)
{
    struct moon_ray_tracing_pipeline_impl *impl = &work->impl;
    struct moon_device_impl const *device = impl->header.device.impl;
    moon_device_ray_tracing_pipeline_details const *details = &device->header.details->ray_tracing_pipeline_details;

    impl->shader_group_count = lake_darray_size(&impl->header.assembly.shader_groups.da);
    /* TODO */

    u32 const raygen_count = lake_darray_size(&impl->header.assembly.ray_gen_stages.da);
    u32 const intersection_count = lake_darray_size(&impl->header.assembly.intersection_stages.da);
    u32 const any_hit_count = lake_darray_size(&impl->header.assembly.any_hit_stages.da);
    u32 const callable_count = lake_darray_size(&impl->header.assembly.callable_stages.da);
    u32 const closest_hit_count = lake_darray_size(&impl->header.assembly.closest_hit_stages.da);
    u32 const miss_hit_count = lake_darray_size(&impl->header.assembly.miss_stages.da);
    u32 const all_stages_count = 
        raygen_count +
        intersection_count +
        any_hit_count +
        callable_count +
        closest_hit_count +
        miss_hit_count;
    LAKE_UNUSED u32 const first_callable_idx = raygen_count + intersection_count + any_hit_count;
    LAKE_UNUSED u32 const last_callable_idx = raygen_count + intersection_count + any_hit_count + callable_count;
    LAKE_UNUSED u32 const first_miss_idx = raygen_count + intersection_count + any_hit_count + callable_count + closest_hit_count;
    (void)all_stages_count;

    work->vk_result = VK_ERROR_UNKNOWN; /* TODO */
    if (work->vk_result != VK_SUCCESS)
        return;
    impl->vk_pipeline_layout = device->gpu_sr_table.pipeline_layouts[(impl->header.assembly.push_constant_size + 3) >> 2];

    *work->vk_create_info = (VkRayTracingPipelineCreateInfoKHR){
        .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = 0,
        .pStages = nullptr, /* TODO */
        .groupCount = 0,
        .pGroups = nullptr, /* TODO */
        .maxPipelineRayRecursionDepth = lake_min(impl->header.assembly.max_ray_recursion_depth, details->max_ray_recursion_depth),
        .pLibraryInfo = nullptr,
        .pLibraryInterface = nullptr,
        .pDynamicState = nullptr,
        .layout = impl->vk_pipeline_layout,
        .basePipelineHandle = nullptr,
        .basePipelineIndex = 0,
    };
}

static void LAKECALL destroy_ray_tracing_pipeline_assembly_shader_modules(
    struct moon_device_impl const             *device, 
    u32 const                                  count, 
    struct populate_ray_tracing_pipeline_work *work)
{
    for (u32 i = 0; i < count; i++) {
        lake_darray_foreach_v(work[i].vk_shader_modules, VkShaderModule, module) {
            if (*module != VK_NULL_HANDLE) device->vkDestroyShaderModule(device->vk_device, *module, device->vk_allocator);
        }
        lake_darray_fini(&work[i].vk_shader_modules.da);
    }
}

FN_MOON_RAY_TRACING_PIPELINE_CREATE_DEFAULT_SBT(vulkan)
{
    moon_ray_tracing_pipeline_assembly *assembly = &pipeline->header.assembly;
    struct moon_device_impl *device = pipeline->header.device.impl;
    moon_device_ray_tracing_pipeline_details const *rt_details = &device->header.details->ray_tracing_pipeline_details;

    u32 const raygen_count = lake_darray_size(&assembly->ray_gen_stages.da);
    u32 const intersection_count = lake_darray_size(&assembly->intersection_stages.da);
    u32 const any_hit_count = lake_darray_size(&assembly->any_hit_stages.da);
    u32 const callable_hit_count = lake_darray_size(&assembly->callable_stages.da);
    u32 const closest_hit_count = lake_darray_size(&assembly->closest_hit_stages.da);
    u32 const miss_hit_count = lake_darray_size(&assembly->miss_stages.da);

    /* Because the shaders are provided in order, we can calculate the 
     * start and end range of shader indices that fall into each group. */
    u32 const all_stages_count =
        raygen_count +
        intersection_count +
        any_hit_count +
        callable_hit_count +
        closest_hit_count +
        miss_hit_count;
    u32 const first_callable_idx = raygen_count + intersection_count + any_hit_count;
    u32 const last_callable_idx = first_callable_idx + callable_hit_count;
    u32 const first_miss_idx = last_callable_idx + closest_hit_count;

    u32 raygen_group_count = 0;
    u32 miss_group_count = 0;
    u32 hit_group_count = 0;
    u32 callable_group_count = 0;

    /* We need to know into which type a shader group belongs. That is what the 
     * following code is for. We iterate over all the shader groups provided by 
     * the user and get their type based on the index shader range. We need to 
     * group them based on types because that is how the default generated 
     * shader binding table is layed out. */
    for (u32 i = 0; i < pipeline->shader_group_count; i++) {
        moon_ray_tracing_shader_group const *shader_group = &pipeline->shader_groups[i];

        /* Hit groups are the only ones that need the type explicitly specified,
         * thus we can immediately deduce the group type from this. */
        if (shader_group->type == moon_ray_tracing_shader_group_type_triangles_hit_group ||
            shader_group->type == moon_ray_tracing_shader_group_type_procedural_hit_group)
        {
            hit_group_count++;
        } else {
            if (shader_group->general_shader_index != MOON_SHADER_UNUSED &&
                shader_group->general_shader_index < raygen_count)
            {
                /* group indexes raygen shader --> it is a raygen group */
                raygen_group_count++;
            } else if (shader_group->general_shader_index >= first_miss_idx &&
                       shader_group->general_shader_index < all_stages_count)
            {
                /* group indexes miss shader --> it is a miss group */
                miss_group_count++;
            } else if (shader_group->general_shader_index >= first_callable_idx &&
                       shader_group->general_shader_index < last_callable_idx)
            {
                /* group indexes callable shader --> it is a callable group */
                callable_group_count++;
            } else {
                /* group indexes invalid shader */
                return LAKE_ERROR_INVALID_SHADER;
            }
        }
    }
    u32 const ray_count_number = raygen_group_count;
    u32 const miss_count_number = miss_group_count;
    u32 const hit_count_number = hit_group_count;
    u32 const callable_count_number = callable_group_count;
    u32 const handle_count = 
        ray_count_number + 
        miss_count_number + 
        hit_count_number +
        callable_count_number;
    u32 const group_handle_size = rt_details->shader_group_handle_size;
    u32 const group_handle_alignment = rt_details->shader_group_handle_alignment;
    u32 const group_base_alignment = rt_details->shader_group_base_alignment;
    /* The shader binding table (buffer) need to have starting groups to be 
     * aligned and handles in the group to be aligned. */
    u64 const handle_size_aligned = lake_align(group_handle_size, group_handle_alignment);

    moon_strided_address_region *raygen_region = &out_sbt->raygen_region;
    moon_strided_address_region *miss_region = &out_sbt->miss_region;
    moon_strided_address_region *hit_region = &out_sbt->hit_region;
    moon_strided_address_region *callable_region = &out_sbt->callable_region;

    raygen_region->stride = lake_align(handle_size_aligned, group_base_alignment);
    raygen_region->size = raygen_region->stride; /* the raygen size must be equal to its stride */
    miss_region->stride = handle_size_aligned;
    miss_region->size = lake_align(miss_count_number * handle_size_aligned, group_base_alignment);
    hit_region->stride = handle_size_aligned;
    hit_region->size = lake_align(hit_count_number * handle_size_aligned, group_base_alignment);
    callable_region->stride = handle_size_aligned;
    callable_region->size = lake_align(callable_count_number * handle_size_aligned, group_base_alignment);

    /* get the shader group handles */
    u32 const data_size = handle_count * group_handle_size;
    u8 *shader_handle_storage = (u8 *)__lake_malloc(data_size, 1);

    VkDeviceSize const sbt_size = 
        raygen_region->size + 
        miss_region->size + 
        hit_region->size +
        callable_region->size;
    /* https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkGetRayTracingShaderGroupHandlesNV.html */
    lake_result result = vk_result_translate(
        device->vkGetRayTracingShaderGroupHandlesKHR(
            device->vk_device,
            pipeline->vk_pipeline,
            0,
            handle_count,
            data_size,
            (void *)shader_handle_storage));
    if (result != LAKE_SUCCESS) {
        __lake_free(shader_handle_storage);
        return result;
    }
    /* allocate a buffer for storing the shader binding table */
    moon_buffer_assembly sbt_assembly = {
        .size = sbt_size,
        .memory_flags = moon_memory_flag_host_access_sequential_write,
        .name = assembly->name,
    };
    /* we need to store the buffer id somewhere so we can cleanup after the pipeline is destroyed. */
    result = _moon_vulkan_create_buffer(device, &sbt_assembly, out_buffer);
    if (result != LAKE_SUCCESS) {
fini_sbt:
        __lake_free(shader_handle_storage);
        if (!moon_id_is_empty(*out_buffer)) {
            lake_result destroy_result = _moon_vulkan_destroy_buffer(device, *out_buffer);
            if (destroy_result != LAKE_SUCCESS)
                return destroy_result;
        }
        return result;
    }
    moon_buffer_id sbt_buffer = *out_buffer;
    u8 *sbt_buffer_ptr = nullptr;
    result = _moon_vulkan_buffer_host_address(device, sbt_buffer, (void **)&sbt_buffer_ptr);
    if (result != LAKE_SUCCESS) 
        goto fini_sbt;

    /* find the SBT addresses of each group */
    VkDeviceAddress sbt_address = 0;
    result = _moon_vulkan_buffer_device_address(device, sbt_buffer, &sbt_address);
    if (result != LAKE_SUCCESS) 
        goto fini_sbt;

    raygen_region->device_address = sbt_address;
    miss_region->device_address = sbt_address + raygen_region->size;
    hit_region->device_address = sbt_address + raygen_region->size + miss_region->size;
    callable_region->device_address = sbt_address + raygen_region->size + miss_region->size + hit_region->size;

    u64 o = 0;
    u8 *sbt_ptr_iter = sbt_buffer_ptr;
    /* raygen shaders load data */
    for (u32 i = 0; i < ray_count_number; i++) {
        lake_memcpy(sbt_ptr_iter, shader_handle_storage + o, group_handle_size);
        sbt_ptr_iter += raygen_region->stride;
        o += group_handle_size;
    }

    /* miss shaders (base ptr + raygen size) */
    sbt_ptr_iter = sbt_buffer_ptr + raygen_region->size;
    /* miss shaders load data */
    for (u32 i = 0; i < miss_count_number; i++) {
        lake_memcpy(sbt_ptr_iter, shader_handle_storage + o, group_handle_size);
        sbt_ptr_iter += miss_region->stride;
        o += group_handle_size;
    }

    /* hit shaders (base ptr + raygen size + miss size) */
    sbt_ptr_iter = sbt_buffer_ptr + raygen_region->size + miss_region->size;
    /* closest hit + any hit + intersection shaders load data */
    for (u32 i = 0; i < hit_count_number; i++) {
        lake_memcpy(sbt_ptr_iter, shader_handle_storage + o, group_handle_size);
        sbt_ptr_iter += hit_region->stride;
        o += group_handle_size;
    }

    /* callable shaders (base ptr + raygen size + miss size + hit size) */
    sbt_ptr_iter = sbt_buffer_ptr + raygen_region->size + miss_region->size + hit_region->size;
    /* callable shaders load data */
    for (u32 i = 0; i < callable_count_number; i++) {
        lake_memcpy(sbt_ptr_iter, shader_handle_storage + o, group_handle_size);
        sbt_ptr_iter += callable_region->stride;
        o += group_handle_size;
    }
    return LAKE_SUCCESS;
}

FN_MOON_RAY_TRACING_PIPELINE_SHADER_GROUP_HANDLES(vulkan)
{
    struct moon_device_impl *device = pipeline->header.device.impl;

    u32 const handle_count = pipeline->shader_group_count;
    u32 const handle_size = device->header.details->ray_tracing_pipeline_details.shader_group_handle_size;
    u32 const data_size = handle_count * handle_size;

    return vk_result_translate(
        device->vkGetRayTracingShaderGroupHandlesKHR(
            device->vk_device,
            pipeline->vk_pipeline,
            0,
            handle_count,
            data_size,
            out_blob));
}

#define MAX_RASTER_STAGES 7
#define MAX_RASTER_DYNAMIC_STATE 4
struct populate_raster_pipeline_work {
    struct moon_raster_pipeline_impl                        impl;
    VkGraphicsPipelineCreateInfo                           *vk_create_info;
    VkShaderModule                                          vk_shader_modules[MAX_RASTER_STAGES];
    VkPipelineShaderStageCreateInfo                         vk_shader_stages[MAX_RASTER_STAGES];
    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo     vk_require_subgroup_sizes[MAX_RASTER_STAGES];
    VkPipelineVertexInputStateCreateInfo                    vk_vertex_input_state;
    VkPipelineInputAssemblyStateCreateInfo                  vk_input_assembly_state;
    VkPipelineTessellationDomainOriginStateCreateInfo       vk_tessellation_domain_origin_state;
    VkPipelineTessellationStateCreateInfo                   vk_tessellation_state;
    VkPipelineRasterizationStateCreateInfo                  vk_raster_state;
    VkPipelineRasterizationConservativeStateCreateInfoEXT   vk_conservative_raster_state;
    VkPipelineMultisampleStateCreateInfo                    vk_multisample_state;
    VkPipelineDepthStencilStateCreateInfo                   vk_depth_stencil_state;
    VkPipelineColorBlendAttachmentState                     vk_color_blend_attachment_states[MOON_MAX_COLOR_ATTACHMENTS];
    VkPipelineColorBlendStateCreateInfo                     vk_color_blend_state;
    VkPipelineViewportStateCreateInfo                       vk_viewport_state;
    VkPipelineDynamicStateCreateInfo                        vk_dynamic_state;
    VkPipelineRenderingCreateInfo                           vk_pipeline_rendering;
    VkViewport                                              default_vk_viewport;
    VkRect2D                                                default_vk_scissor;
    VkDynamicState                                          vk_dynamic_state_data[MAX_RASTER_DYNAMIC_STATE];
    VkFormat                                                vk_color_attachment_formats[MOON_MAX_COLOR_ATTACHMENTS];
    VkResult                                                vk_result;
};

static FN_LAKE_WORK(populate_raster_pipeline, struct populate_raster_pipeline_work *work)
{
    struct moon_raster_pipeline_impl *impl = &work->impl;
    struct moon_device_impl const *device = impl->header.device.impl;
    struct moon_raster_pipeline_assembly const *assembly = &impl->header.assembly;

    u32 stage_count = 0;
    u32 dynamic_state_count = 0;
    u32 color_attachment_count = 0;

    moon_shader_assembly const *shaders_assembly[MAX_RASTER_STAGES];
    VkPipelineStageFlags vk_stage_flags[MAX_RASTER_STAGES];
#define TRY_SHADER_ASSEMBLY(T, NAME) \
    if (assembly->has_##T##_shader) { \
        u32 idx = stage_count++; \
        shaders_assembly[idx] = (moon_shader_assembly const *)&assembly->T##_shader; \
        vk_stage_flags[idx] = VK_SHADER_STAGE_##NAME; \
    }
    TRY_SHADER_ASSEMBLY(vertex, VERTEX_BIT)
    TRY_SHADER_ASSEMBLY(tessellation_control, TESSELLATION_CONTROL_BIT)
    TRY_SHADER_ASSEMBLY(tessellation_evaluation, TESSELLATION_EVALUATION_BIT)
    TRY_SHADER_ASSEMBLY(geometry, GEOMETRY_BIT)
    TRY_SHADER_ASSEMBLY(fragment, FRAGMENT_BIT)
    if ((device->header.details->implicit_features & moon_implicit_feature_mesh_shader) != moon_implicit_feature_none) {
        TRY_SHADER_ASSEMBLY(task, TASK_BIT_EXT);
        TRY_SHADER_ASSEMBLY(mesh, MESH_BIT_EXT);
    } else {
        if (assembly->has_mesh_shader || assembly->has_task_shader) {
            lake_error("Device `%s` raster does not support mesh or task shaders, `%s` pipeline assembly invalid.",
                device->header.assembly.name.str, assembly->name.str);
            work->vk_result = VK_ERROR_FEATURE_NOT_PRESENT;
            return;
        }
    }
#undef TRY_SHADER_ASSEMBLY
    for (u32 i = 0; i < stage_count; i++) {
        moon_shader_assembly const *shader_assembly = shaders_assembly[i];
        VkShaderModuleCreateInfo vk_shader_module_create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = shader_assembly->byte_code_size * sizeof(u32),
            .pCode = shader_assembly->byte_code,
        };
        work->vk_result = device->vkCreateShaderModule(
                device->vk_device,
                &vk_shader_module_create_info,
                device->vk_allocator,
                &work->vk_shader_modules[i]);
        /* created modules will be destroyed in main assembly function */
        if (work->vk_result != VK_SUCCESS) return;

        work->vk_require_subgroup_sizes[i] = (VkPipelineShaderStageRequiredSubgroupSizeCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO,
            .pNext = nullptr,
            .requiredSubgroupSize = shader_assembly->has_required_subgroup_size
                ? shader_assembly->required_subgroup_size : 0,
        };
        work->vk_shader_stages[i] = (VkPipelineShaderStageCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = shader_assembly->has_required_subgroup_size
                ? &work->vk_require_subgroup_sizes[i] : nullptr,
            .flags = (VkPipelineShaderStageCreateFlags)shader_assembly->create_flags,
            .stage = vk_stage_flags[i],
            .module = work->vk_shader_modules[i],
            .pName = shader_assembly->entry_point.str,
            .pSpecializationInfo = nullptr,
        };
    }
    impl->vk_pipeline_layout = device->gpu_sr_table.pipeline_layouts[(impl->header.assembly.push_constant_size + 3) >> 2];

    work->vk_vertex_input_state = (VkPipelineVertexInputStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };
    work->vk_input_assembly_state = (VkPipelineInputAssemblyStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = (VkPrimitiveTopology)assembly->rasterizer.primitive_topology,
        .primitiveRestartEnable = assembly->rasterizer.enable_primitive_restart,
    };
    work->vk_tessellation_domain_origin_state = (VkPipelineTessellationDomainOriginStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE_CREATE_INFO,
        .pNext = nullptr,
        .domainOrigin = assembly->has_tessellation ? (VkTessellationDomainOrigin)assembly->tessellation.origin : 0,
    };
    work->vk_tessellation_state = (VkPipelineTessellationStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .pNext = &work->vk_tessellation_domain_origin_state,
        .flags = 0,
        .patchControlPoints = assembly->has_tessellation 
            ? assembly->tessellation.control_points : 0,
    };
    work->vk_raster_state = (VkPipelineRasterizationStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = assembly->rasterizer.enable_depth_clamp,
        .rasterizerDiscardEnable = assembly->rasterizer.enable_rasterizer_discard,
        .polygonMode = (VkPolygonMode)assembly->rasterizer.polygon_mode,
        .cullMode = (VkCullModeFlags)assembly->rasterizer.face_culling,
        .frontFace = (VkFrontFace)assembly->rasterizer.front_face_winding,
        .depthBiasEnable = assembly->rasterizer.enable_depth_bias,
        .depthBiasConstantFactor = assembly->rasterizer.depth_bias_constant_factor,
        .depthBiasSlopeFactor = assembly->rasterizer.depth_bias_slope_factor,
        .depthBiasClamp = assembly->rasterizer.depth_bias_clamp,
        .lineWidth = assembly->rasterizer.line_width,
    };
    work->vk_conservative_raster_state = (VkPipelineRasterizationConservativeStateCreateInfoEXT){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT,
        .extraPrimitiveOverestimationSize = 0.f,
    };
    if (assembly->rasterizer.has_conservative_rasterizer && (device->header.details->implicit_features & moon_implicit_feature_conservative_rasterization)) {
        work->vk_conservative_raster_state.conservativeRasterizationMode = 
            (VkConservativeRasterizationModeEXT)assembly->rasterizer.conservative_rasterizer.mode;
        work->vk_conservative_raster_state.extraPrimitiveOverestimationSize = assembly->rasterizer.conservative_rasterizer.size;
        work->vk_raster_state.pNext = &work->vk_conservative_raster_state;
    }
    work->vk_multisample_state = (VkPipelineMultisampleStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = assembly->rasterizer.has_static_state_sample_count
            ? (VkSampleCountFlags)assembly->rasterizer.static_state_sample_count : 0,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE, 
        .alphaToOneEnable = VK_FALSE,
    };
    work->vk_depth_stencil_state = (VkPipelineDepthStencilStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = assembly->has_depth_test,
        .depthWriteEnable = assembly->has_depth_test 
            ? assembly->depth_test.enable_depth_write : 0,
        .depthCompareOp = assembly->has_depth_test 
            ? (VkCompareOp)assembly->depth_test.depth_test_compare_op : 0,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = assembly->has_stencil_test,
        .front = {0}, /* TODO stencil */
        .back = {0}, /* TODO stencil */
        .minDepthBounds = assembly->has_depth_test 
            ? assembly->depth_test.min_depth_bounds : 0,
        .maxDepthBounds = assembly->has_depth_test 
            ? assembly->depth_test.max_depth_bounds : 0,
    };
    lake_dbg_assert(color_attachment_count <= MOON_MAX_COLOR_ATTACHMENTS, LAKE_ERROR_OUT_OF_RANGE, "Too many color attachments, bump the limit. Current max is %u.", MOON_MAX_COLOR_ATTACHMENTS);

    for (u32 i = 0; i < color_attachment_count; i++) {
        VkBool32 const has_blend = assembly->color_attachments[i].has_blend;
        work->vk_color_blend_attachment_states[i] = (VkPipelineColorBlendAttachmentState){
            .blendEnable = has_blend,
            .srcColorBlendFactor = has_blend 
                ? (VkBlendFactor)assembly->color_attachments[i].blend.src_color_blend_factor : 0,
            .dstColorBlendFactor = has_blend 
                ? (VkBlendFactor)assembly->color_attachments[i].blend.dst_color_blend_factor : 0,
            .colorBlendOp = has_blend 
                ? (VkBlendOp)assembly->color_attachments[i].blend.color_blend_op : 0,
            .srcAlphaBlendFactor = has_blend 
                ? (VkBlendFactor)assembly->color_attachments[i].blend.src_alpha_blend_factor : 0,
            .dstAlphaBlendFactor = has_blend 
                ? (VkBlendFactor)assembly->color_attachments[i].blend.dst_alpha_blend_factor : 0,
            .alphaBlendOp = has_blend 
                ? (VkBlendOp)assembly->color_attachments[i].blend.alpha_blend_op : 0,
            .colorWriteMask = has_blend 
                ? (VkColorComponentFlags)assembly->color_attachments[i].blend.color_write_mask : 0,
        };
        work->vk_color_attachment_formats[i] = (VkFormat)assembly->color_attachments[i].format;
    }
    work->vk_color_blend_state = (VkPipelineColorBlendStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_CLEAR,
        .attachmentCount = color_attachment_count,
        .pAttachments = work->vk_color_blend_attachment_states,
        .blendConstants = { 1.f, 1.f, 1.f, 1.f },
    };
    work->default_vk_viewport = (VkViewport){
        .x = 0, 
        .y = 0, 
        .width = 1, 
        .height = 1, 
        .minDepth = 0, 
        .maxDepth = 0,
    };
    work->default_vk_scissor = (VkRect2D){
        .offset = {0, 0}, 
        .extent = {1, 1},
    };
    work->vk_viewport_state = (VkPipelineViewportStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &work->default_vk_viewport,
        .scissorCount = 1,
        .pScissors = &work->default_vk_scissor,
    };
    work->vk_dynamic_state_data[dynamic_state_count++] = VK_DYNAMIC_STATE_VIEWPORT;
    work->vk_dynamic_state_data[dynamic_state_count++] = VK_DYNAMIC_STATE_SCISSOR;
    work->vk_dynamic_state_data[dynamic_state_count++] = VK_DYNAMIC_STATE_DEPTH_BIAS;
    if ((device->header.details->implicit_features & moon_implicit_feature_dynamic_state) 
        && !assembly->rasterizer.has_static_state_sample_count)
    {
        work->vk_dynamic_state_data[dynamic_state_count++] = VK_DYNAMIC_STATE_RASTERIZATION_SAMPLES_EXT;
    }
    work->vk_dynamic_state = (VkPipelineDynamicStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = dynamic_state_count,
        .pDynamicStates = work->vk_dynamic_state_data,
    };
    VkFormat const depth_test_format = assembly->has_depth_test ? (VkFormat)assembly->depth_test.depth_attachment_format : VK_FORMAT_UNDEFINED;
    VkFormat const stencil_test_format = VK_FORMAT_UNDEFINED;
    work->vk_pipeline_rendering = (VkPipelineRenderingCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = color_attachment_count,
        .pColorAttachmentFormats = work->vk_color_attachment_formats,
        .depthAttachmentFormat = depth_test_format,
        .stencilAttachmentFormat = stencil_test_format,
    };
    *work->vk_create_info = (VkGraphicsPipelineCreateInfo){
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &work->vk_pipeline_rendering,
        .flags = 0,
        .stageCount = stage_count,
        .pStages = work->vk_shader_stages,
        .pVertexInputState = &work->vk_vertex_input_state,
        .pInputAssemblyState = &work->vk_input_assembly_state,
        .pTessellationState = &work->vk_tessellation_state,
        .pViewportState = &work->vk_viewport_state,
        .pRasterizationState = &work->vk_raster_state,
        .pMultisampleState = &work->vk_multisample_state,
        .pDepthStencilState = &work->vk_depth_stencil_state,
        .pColorBlendState = &work->vk_color_blend_state,
        .pDynamicState = &work->vk_dynamic_state,
        .layout = impl->vk_pipeline_layout,
        .renderPass = nullptr,
        .subpass = 0,
        .basePipelineHandle = nullptr,
        .basePipelineIndex = 0,
    };
}

static void LAKECALL destroy_raster_pipeline_assembly_shader_modules(
    struct moon_device_impl const        *device, 
    u32 const                             count, 
    struct populate_raster_pipeline_work *work)
{
    for (u32 i = 0; i < count; i++) {
        for (u32 j = 0; j < MAX_RASTER_STAGES; j++) {
            VkShaderModule module = work[i].vk_shader_modules[j];
            if (module == VK_NULL_HANDLE)
                break;
            device->vkDestroyShaderModule(device->vk_device, module, device->vk_allocator);
        }
    }
}

#ifndef LAKE_NDEBUG
#define IMPL_PIPELINE__REFCNT_DEBUG(T) \
    lake_dbg_assert(pipeline != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr); \
    s32 refcnt = lake_atomic_read(&pipeline->header.refcnt); \
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, #T " pipeline `%s` reference count is %d.", pipeline->header.assembly.name.str, refcnt)

#define IMPL_PIPELINE__NAME_DEBUG(T) \
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) { \
        char const *name = work[i].impl.header.assembly.name.str; \
        VkDebugUtilsObjectNameInfoEXT const name_info = { \
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT, \
            .pNext = nullptr, \
            .objectType = VK_OBJECT_TYPE_PIPELINE, \
            .objectHandle = (u64)(uptr)work[i].impl.vk_pipeline, \
            .pObjectName = name, \
        }; \
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &name_info); \
    }
#else
#define IMPL_PIPELINE__REFCNT_DEBUG(T) LAKE_MAGIC_NOTHING()
#define IMPL_PIPELINE__NAME_DEBUG(T) LAKE_MAGIC_NOTHING()
#endif /* LAKE_NDEBUG */
#define IMPL_PIPELINE__RT_DEFERRED_OPERATION nullptr,

#define IMPL_PIPELINE(T, NAME, vk_ci, fn_vk_create_pipelines, RT_deferred_operation, ...) \
    FN_MOON_##NAME##_PIPELINE_ASSEMBLY(vulkan) \
    { \
        vk_ci                               *vk_create_infos; \
        VkPipeline                          *vk_pipelines; \
        struct populate_##T##_pipeline_work *work; \
        lake_work_details                   *work_details; \
        u8                                  *raw; \
        moon_implicit_features               implicit_features = device->header.details->implicit_features; \
        \
        __VA_ARGS__ \
        { /* calculate scratch memory */ \
            if (count == 0) return LAKE_INVALID_PARAMETERS; \
            \
            usize const vk_create_infos_bytes = lake_align(sizeof(vk_ci) * count, 16); \
            usize const vk_pipelines_bytes = lake_align(sizeof(VkPipeline) * count, 16); \
            usize const work_bytes = lake_align(sizeof(struct populate_##T##_pipeline_work) * count, 16); \
            usize const work_details_bytes = lake_align(sizeof(lake_work_details) * count, 16); \
            usize const total_bytes = \
                vk_create_infos_bytes + \
                vk_pipelines_bytes + \
                work_bytes + \
                work_details_bytes; \
            \
            usize o = 0; \
            raw = (u8 *)__lake_malloc(total_bytes, 16); \
            \
            vk_create_infos = (vk_ci *)&raw[o]; \
            o += vk_create_infos_bytes; \
            vk_pipelines = (VkPipeline *)&raw[o]; \
            o += vk_pipelines_bytes; \
            work = (struct populate_##T##_pipeline_work *)&raw[o]; \
            o += work_bytes; \
            work_details = (lake_work_details *)&raw[o]; \
            lake_dbg_assert(o + work_details_bytes == total_bytes, LAKE_PANIC, nullptr); \
        } \
        for (u32 i = 0; i < count; i++) { \
            work[i].impl = (struct moon_##T##_pipeline_impl){ \
                .header = { \
                    .device.impl = device, \
                    .assembly = assembly_array[i], \
                    .zero_refcnt = (PFN_lake_work)_moon_vulkan_##T##_pipeline_zero_refcnt, \
                }, \
            }, \
            work[i].vk_create_info = &vk_create_infos[i]; \
            work_details[i].procedure = (PFN_lake_work)populate_##T##_pipeline; \
            work_details[i].argument = &work[i]; \
            work_details[i].name = "populate " #T " pipeline create info"; \
            vk_pipelines[i] = VK_NULL_HANDLE; \
        } \
        /* populate the Vk*PipelineCreateInfo's for all assembly */ \
        lake_submit_work_and_yield(count, work_details); \
        \
        /* check the create infos */ \
        VkResult vk_result = VK_SUCCESS; \
        for (u32 i = 0; i < count; i++) { \
            if (work[i].vk_result != VK_SUCCESS) { \
                lake_error("The " #T " pipeline assembly `%s`, %u out of %u, was dismissed: %s.", \
                        work[i].impl.header.assembly.name.str, i, count, vk_result_string(vk_result)); \
                vk_result = work[i].vk_result; \
            } \
        } \
        /* won't be called if the create info populate failed at any pipeline assembly */ \
        if (vk_result == VK_SUCCESS) { \
            /* create the pipelines */ \
            vk_result = device->fn_vk_create_pipelines(\
                    device->vk_device, \
                    RT_deferred_operation /* only ray tracing */ \
                    nullptr, /* no cache for now */ \
                    count, \
                    vk_create_infos, \
                    device->vk_allocator, \
                    vk_pipelines); \
        } \
        /* destroy shader modules as we don't need them anymore */ \
        destroy_##T##_pipeline_assembly_shader_modules(device, count, work); \
        \
        if (vk_result != VK_SUCCESS) { \
            /* destroy all pipelines */ \
            for (u32 i = 0; i < count; i++) \
                if (vk_pipelines[i] != VK_NULL_HANDLE) \
                    device->vkDestroyPipeline(device->vk_device, vk_pipelines[i], device->vk_allocator); \
            __lake_free(raw); \
            return vk_result_translate(vk_result); \
        } \
        /* write our pipelines to handles */ \
        for (u32 i = 0; i < count; i++) { \
            work[i].impl.vk_pipeline = vk_pipelines[i]; \
            IMPL_PIPELINE__NAME_DEBUG(T) \
            struct moon_##T##_pipeline_impl *out = __lake_malloc_t(struct moon_##T##_pipeline_impl); \
            lake_memcpy(out, &work[i].impl, sizeof(struct moon_##T##_pipeline_impl)); \
            lake_inc_refcnt(&device->header.refcnt); \
            lake_inc_refcnt(&out->header.refcnt); \
            out_pipelines[i] = out; \
        } \
        __lake_free(raw); \
        return LAKE_SUCCESS; \
    } \
    \
    FN_MOON_##NAME##_PIPELINE_ZERO_REFCNT(vulkan) \
    { \
        IMPL_PIPELINE__REFCNT_DEBUG(T); \
        struct moon_device_impl *device = pipeline->header.device.impl; \
        zombie_timeline_pipeline submit = { \
            .first = lake_atomic_read(&device->submit_timeline), \
            .second = { .vk_pipeline = pipeline->vk_pipeline }, \
        }; \
        lake_spinlock *lock = &device->zombies_locks[zombie_timeline_pipeline_idx]; \
        lake_deque_unshift_v_locked(device->pipeline_zombies, zombie_timeline_pipeline, submit, lock); \
        \
        moon_device_unref(pipeline->header.device); \
        __lake_free(pipeline); \
    }
IMPL_PIPELINE(compute, COMPUTE, VkComputePipelineCreateInfo, vkCreateComputePipelines, LAKE_MAGIC_NOTHING(), (void)implicit_features; )

IMPL_PIPELINE(work_graph, WORK_GRAPH, VkExecutionGraphPipelineCreateInfoAMDX, vkCreateExecutionGraphPipelinesAMDX, LAKE_MAGIC_NOTHING(), 
        if (!(implicit_features & moon_implicit_feature_work_graph)) return LAKE_ERROR_FEATURE_NOT_PRESENT; )

IMPL_PIPELINE(ray_tracing, RAY_TRACING, VkRayTracingPipelineCreateInfoKHR, vkCreateRayTracingPipelinesKHR, IMPL_PIPELINE__RT_DEFERRED_OPERATION, 
        if (!(implicit_features & moon_implicit_feature_ray_tracing_pipeline)) return LAKE_ERROR_FEATURE_NOT_PRESENT; )

IMPL_PIPELINE(raster, RASTER, VkGraphicsPipelineCreateInfo, vkCreateGraphicsPipelines, LAKE_MAGIC_NOTHING(), (void)implicit_features; )
#endif /* MOON_VULKAN */
