#include "vk_moon.h"
#ifdef MOON_VULKAN

#include <lake/math/bits.h>
#include <stdio.h> /* snprintf */

#define MOON_GPU_TABLE_SET_BINDING 0
#define MOON_STORAGE_BUFFER_BINDING 0
#define MOON_STORAGE_TEXTURE_BINDING 1
#define MOON_SAMPLED_TEXTURE_BINDING 2
#define MOON_SAMPLER_BINDING 3
#define MOON_DEVICE_ADDRESS_BUFFER_BINDING 4
#define MOON_ACCELERATION_STRUCTURE_BINDING 5

bool is_depth_format(moon_format format)
{
    switch (format) {
        case moon_format_d16_unorm: return true;
        case moon_format_x8_d24_unorm_pack32: return true;
        case moon_format_d32_sfloat: return true;
        case moon_format_s8_uint: return true;
        case moon_format_d16_unorm_s8_uint: return true;
        case moon_format_d24_unorm_s8_uint: return true;
        case moon_format_d32_sfloat_s8_uint: return true;
        default: return false;
    }
}

bool is_stencil_format(moon_format format)
{
    switch (format) {
        case moon_format_s8_uint: return true;
        case moon_format_d16_unorm_s8_uint: return true;
        case moon_format_d24_unorm_s8_uint: return true;
        case moon_format_d32_sfloat_s8_uint: return true;
        default: return false;
    }
}

void populate_vk_image_create_info_from_assembly(
    struct moon_device_impl     *device,
    moon_texture_assembly const *assembly,
    VkImageCreateInfo           *out_vk_create_info)
{
    lake_dbg_assert(lake_is_pow2(assembly->sample_count) && assembly->sample_count <= 8, LAKE_INVALID_PARAMETERS,
            "Texture samples must be a power of 2 and between 1 and 64.");
    lake_dbg_assert(assembly->extent.width > 0 && assembly->extent.height > 0 && assembly->extent.depth > 0,
            LAKE_INVALID_PARAMETERS, "Texture (x,y,z) dimensions must be greater than 0.");
    lake_dbg_assert(assembly->array_layer_count > 0, LAKE_INVALID_PARAMETERS, "Texture array layer count must be greater than 0.");
    lake_dbg_assert(assembly->mip_level_count > 0, LAKE_INVALID_PARAMETERS, "Texture mip level count must be greater than 0.");

    VkImageType const vk_image_type = (VkImageType)assembly->dimensions - 1;
    VkImageCreateFlags vk_image_create_flags = (VkImageCreateFlags)assembly->flags;

    *out_vk_create_info = (VkImageCreateInfo){
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = vk_image_create_flags,
        .imageType = vk_image_type,
        .format = (VkFormat)assembly->format,
        .extent = {
            .width = assembly->extent.width,
            .height = assembly->extent.height,
            .depth = assembly->extent.depth,
        },
        .mipLevels = assembly->mip_level_count,
        .arrayLayers = assembly->array_layer_count,
        .samples = (VkSampleCountFlagBits)assembly->sample_count,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = (VkImageUsageFlags)assembly->usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &get_device_queue_impl(device, MOON_QUEUE_MAIN)->vk_queue_family_idx,
    };
    if (assembly->sharing_mode == moon_sharing_mode_concurrent) {
        out_vk_create_info->sharingMode = VK_SHARING_MODE_CONCURRENT;
        out_vk_create_info->queueFamilyIndexCount = device->physical_device->unique_queue_family_count;
        out_vk_create_info->pQueueFamilyIndices = device->physical_device->unique_queue_family_indices;
    }
}

void populate_vk_acceleration_structure_build_details(
    struct moon_device_impl                *device,
    usize                                   tlas_build_count,
    moon_tlas_build_details const          *tlas_build_details,
    usize                                   blas_build_count,
    moon_blas_build_details const          *blas_build_details,
    struct acceleratrion_structure_build   *out_build)
{
    lake_darray *vk_build_geometry_infos = &out_build->vk_build_geometry_infos.da;
    lake_darray *vk_geometry_infos = &out_build->vk_geometry_infos.da;
    lake_darray *primitive_counts = &out_build->primitive_counts.da;
    lake_darray *primitive_counts_ptrs = &out_build->primitive_counts_ptrs.da;
    u32 const acceleration_structure_count = tlas_build_count + blas_build_count;

    lake_darray_resize_t(vk_build_geometry_infos, VkAccelerationStructureBuildGeometryInfoKHR, acceleration_structure_count);
    lake_darray_resize_t(primitive_counts_ptrs, u32, acceleration_structure_count);

    usize geometry_infos_count = 0;
    for (u32 tlas = 0; tlas < tlas_build_count; tlas++)
        geometry_infos_count += tlas_build_details[tlas].instance_count;
    /* as both variants are spans and ABI compatible, we just unconditionally 
     * read one of the variants here */
    for (u32 blas = 0; blas < blas_build_count; blas++)
        geometry_infos_count += blas_build_details[blas].geometries.triangle.count;
    lake_darray_resize_t(vk_geometry_infos, VkAccelerationStructureGeometryKHR, geometry_infos_count);
    lake_darray_resize_t(primitive_counts, u32, geometry_infos_count);

    /* top-level acceleration structures */
    for (u32 tlas = 0; tlas < tlas_build_count; tlas++) {
        moon_tlas_build_details const *details = &tlas_build_details[tlas];
        VkAccelerationStructureGeometryKHR const *vk_geometry_ptr = 
            lake_darray_beyond_t(vk_geometry_infos, VkAccelerationStructureGeometryKHR);
        u32 const *primitive_counts_ptr = *lake_darray_beyond_t(primitive_counts_ptrs, u32 const *);

        for (u32 inst = 0; inst < details->instance_count; inst++) {
            moon_tlas_instance const *inst_detail = &details->instances[inst];
            VkAccelerationStructureGeometryInstancesDataKHR const vk_inst_data = {
                .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
                .pNext = nullptr,
                .arrayOfPointers = (VkBool32)inst_detail->data_is_array_of_pointers,
                .data = (VkDeviceOrHostAddressConstKHR)inst_detail->data,
            };
            VkAccelerationStructureGeometryKHR const vk_geometry = {
                .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
                .pNext = nullptr,
                .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
                .geometry = (VkAccelerationStructureGeometryDataKHR){
                    .instances = vk_inst_data,
                },
                .flags = (VkGeometryFlagsKHR)inst_detail->geometry_flags,
            };
            lake_darray_append_t(vk_geometry_infos, VkAccelerationStructureGeometryKHR, &vk_geometry);
            lake_darray_append_t(primitive_counts, u32, &inst_detail->count);
        }
        VkAccelerationStructureBuildGeometryInfoKHR const vk_build_geometry = {
            .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
            .pNext = nullptr,
            .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
            .flags = (VkBuildAccelerationStructureFlagsKHR)details->flags,
            .mode = details->update ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR
                : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
            .srcAccelerationStructure = !moon_id_is_empty(details->src_tlas) 
                ? acquire_tlas_slot(device, details->src_tlas)->vk_acceleration_structure : nullptr,
            .dstAccelerationStructure = !moon_id_is_empty(details->dst_tlas)
                ? acquire_tlas_slot(device, details->dst_tlas)->vk_acceleration_structure : nullptr,
            .geometryCount = details->instance_count,
            .pGeometries = vk_geometry_ptr,
            .ppGeometries = nullptr,
            .scratchData = (VkDeviceOrHostAddressKHR)details->scratch_data,
        };
        lake_darray_append_t(vk_build_geometry_infos, VkAccelerationStructureBuildGeometryInfoKHR, &vk_build_geometry);
        lake_darray_append_t(primitive_counts_ptrs, u32 const *, primitive_counts_ptr);
    }

    /* bottom-level acceleration structures */
    for (u32 blas = 0; blas < blas_build_count; blas++) {
        moon_blas_build_details const *details = &blas_build_details[blas];
        VkAccelerationStructureGeometryKHR const *vk_geometry_ptr =
            lake_darray_beyond_t(vk_geometry_infos, VkAccelerationStructureGeometryKHR);
        u32 const *primitive_counts_ptr = *lake_darray_beyond_t(primitive_counts_ptrs, u32 const *);
        /* as both variants are spans and ABI compatible, we just unconditionally 
         * read one of the variants here */
        u32 const geometry_count = (u32)details->geometries.triangle.count;
        for (u32 geo = 0; geo < geometry_count; geo++) {
            VkAccelerationStructureGeometryKHR vk_geometry = {
                .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
                .pNext = nullptr,
            };
            if (details->geometry_variant == 0) { /* triangles */
                vk_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
                vk_geometry.flags = (VkGeometryTypeKHR)details->geometries.triangle.span[geo].geometry_flags;
                vk_geometry.geometry.triangles = (VkAccelerationStructureGeometryTrianglesDataKHR){
                    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
                    .pNext = nullptr,
                    .vertexFormat = (VkFormat)details->geometries.triangle.span[geo].vertex_format,
                    .vertexData = (VkDeviceOrHostAddressConstKHR)details->geometries.triangle.span[geo].vertex_data,
                    .vertexStride = details->geometries.triangle.span[geo].vertex_stride,
                    .maxVertex = details->geometries.triangle.span[geo].max_vertex,
                    .indexType = (VkIndexType)details->geometries.triangle.span[geo].index_format,
                    .indexData = (VkDeviceOrHostAddressConstKHR)details->geometries.triangle.span[geo].index_data,
                    .transformData = (VkDeviceOrHostAddressConstKHR)details->geometries.triangle.span[geo].transform_data,
                };
                lake_darray_append_t(primitive_counts, u32, &details->geometries.triangle.span[geo].count);
            } else { /* aabbs */
                vk_geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
                vk_geometry.flags = (VkGeometryTypeKHR)details->geometries.aabb.span[geo].geometry_flags;
                vk_geometry.geometry.aabbs = (VkAccelerationStructureGeometryAabbsDataKHR){
                    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR,
                    .pNext = nullptr,
                    .data = (VkDeviceOrHostAddressConstKHR)details->geometries.aabb.span[geo].data,
                    .stride = details->geometries.aabb.span[geo].stride,
                };
                lake_darray_append_t(primitive_counts, u32, &details->geometries.aabb.span[geo].count);
            }
            lake_darray_append_t(vk_geometry_infos, VkAccelerationStructureGeometryKHR, &vk_geometry);
        }
        VkAccelerationStructureBuildGeometryInfoKHR const vk_build_geometry = {
            .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
            .pNext = nullptr,
            .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
            .flags = (VkBuildAccelerationStructureFlagsKHR)details->flags,
            .mode = details->update ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR
                : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
            .srcAccelerationStructure = !moon_id_is_empty(details->src_blas) 
                ? acquire_blas_slot(device, details->src_blas)->vk_acceleration_structure : nullptr,
            .dstAccelerationStructure = !moon_id_is_empty(details->dst_blas)
                ? acquire_blas_slot(device, details->dst_blas)->vk_acceleration_structure : nullptr,
            .geometryCount = geometry_count,
            .pGeometries = vk_geometry_ptr,
            .ppGeometries = nullptr,
            .scratchData = (VkDeviceOrHostAddressKHR)details->scratch_data,
        };
        lake_darray_append_t(vk_build_geometry_infos, VkAccelerationStructureBuildGeometryInfoKHR, &vk_build_geometry);
        lake_darray_append_t(primitive_counts_ptrs, u32 const *, primitive_counts_ptr);
    }
}

#define IMPL_GPU_SR_POOL_TEMPLATE(T) \
    u64 T##_gpu_sr_pool__version_of_slot(struct T##_gpu_sr_pool *pool, s32 idx) \
    { \
        ssize const page = (ssize)((usize)idx >> GPU_SR_POOL_PAGE_BITS); \
        ssize const offset = (ssize)((usize)idx & GPU_SR_POOL_PAGE_MASK); \
        if (page >= lake_atomic_read(&pool->valid_page_count)) \
            return 0llu; \
        return lake_atomic_read(&pool->pages[page]->version_and_refcnt[offset]); \
    } \
    \
    bool T##_gpu_sr_pool__is_id_valid(struct T##_gpu_sr_pool *pool, moon_##T##_id id) \
    { \
        usize const idx = (usize)moon_id_get_index(id); \
        ssize const page = (ssize)((usize)idx >> GPU_SR_POOL_PAGE_BITS); \
        ssize const offset = (ssize)((usize)idx & GPU_SR_POOL_PAGE_MASK); \
        if (moon_id_get_version(id) == 0 || page >= lake_atomic_read(&pool->valid_page_count)) \
            return false; \
        u64 const version = moon_id_get_version(id); \
        u64 const slot_version = lake_atomic_read(&pool->pages[page]->version_and_refcnt[offset]); \
        return slot_version == version; \
    } \
    \
    struct T##_impl_slot *T##_gpu_sr_pool__unsafe_get(struct T##_gpu_sr_pool *pool, moon_##T##_id id) \
    { \
        ssize const idx = moon_id_get_index(id); \
        ssize const page = lake_min( \
            lake_atomic_read(&pool->valid_page_count) - 1, \
            (ssize)((usize)idx >> GPU_SR_POOL_PAGE_BITS)); \
        ssize const offset = (ssize)((usize)idx & GPU_SR_POOL_PAGE_MASK); \
        return &pool->pages[page]->slots[offset]; \
    } \
    \
    void T##_gpu_sr_pool__unsafe_destroy_zombie_slot(struct T##_gpu_sr_pool *pool, moon_##T##_id id) \
    { \
        ssize const idx = moon_id_get_index(id); \
        ssize const page = (ssize)((usize)idx >> GPU_SR_POOL_PAGE_BITS); \
        ssize const offset = (ssize)((usize)idx & GPU_SR_POOL_PAGE_MASK); \
        /* remove the zombie bit */ \
        u64 const version = GPU_SR_POOL_VERSION_COUNT_MASK & \
            lake_atomic_read(&pool->pages[page]->version_and_refcnt[offset]); \
        /* Slots that reached max version CANNOT be recycled. \
         * That is because we cannot guarantee uniqueness of ids when the version wraps back. \
         * Clear slot MUST HAPPEN before pushing into the free list. */ \
        pool->pages[page]->slots[offset] = (struct T##_impl_slot){0}; \
        /* this is the maximum value a version is allowed to reach */ \
        if (version != MOON_ID_INDEX_MASK) \
            lake_mpmc_enqueue_t(&pool->free_indices.ring, lake_mpmc_node_v, &idx); \
        lake_atomic_sub_explicit(&pool->lifetime_sync, 1, lake_memory_model_release); \
    } \
    \
    bool T##_gpu_sr_pool__try_zombify(struct T##_gpu_sr_pool *pool, moon_##T##_id id) \
    { \
        ssize const idx = moon_id_get_index(id); \
        ssize const page = (ssize)((usize)idx >> GPU_SR_POOL_PAGE_BITS); \
        if (moon_id_get_version(id) == 0 || page >= lake_atomic_read(&pool->valid_page_count)) \
            return false; \
        \
        ssize const offset = (ssize)((usize)idx & GPU_SR_POOL_PAGE_MASK); \
        u64 version = moon_id_get_version(id); \
        u64 new_version = (version + 1) | GPU_SR_POOL_VERSION_ZOMBIE_BIT; \
        \
        return lake_atomic_compare_exchange_strong_explicit( \
            &pool->pages[page]->version_and_refcnt[offset], \
            &version, new_version, lake_memory_model_relaxed, lake_memory_model_relaxed); \
    } \
    \
    struct T##_impl_slot *T##_gpu_sr_pool__try_create_slot(struct T##_gpu_sr_pool *pool, moon_##T##_id *out_id) \
    { \
        ssize idx; \
        if (!lake_mpmc_dequeue_t(&pool->free_indices.ring, lake_mpmc_node_v, &idx)) { \
            idx = lake_atomic_add_explicit(&pool->next_idx, 1, lake_memory_model_release); \
            if (idx >= pool->max_resources || idx >= (ssize)GPU_SR_POOL_MAX_RESOURCE_COUNT) \
                return nullptr; \
        } \
        ssize const page = (ssize)((usize)idx >> GPU_SR_POOL_PAGE_BITS); \
        ssize const offset = (ssize)((usize)idx & GPU_SR_POOL_PAGE_MASK); \
        \
        if (page >= lake_atomic_read_explicit(&pool->valid_page_count, lake_memory_model_acquire)) { \
            lake_spinlock_acquire(&pool->page_alloc_lock); \
            /* double lock to avoid data races */ \
            if (page >= lake_atomic_read(&pool->valid_page_count)) { \
                pool->pages[page] = __lake_malloc_t(struct T##_impl_slot_page); \
                lake_zerop(pool->pages[page]); \
                for (u32 i = 0; i < GPU_SR_POOL_PAGE_SIZE; i++) \
                    lake_atomic_write(&pool->pages[page]->version_and_refcnt[i], 1ull); \
                lake_atomic_add_explicit(&pool->valid_page_count, 1, lake_memory_model_release); \
            } \
            lake_spinlock_release(&pool->page_alloc_lock); \
        } \
        lake_atomic_add_explicit(&pool->lifetime_sync, 1, lake_memory_model_release); \
        u64 version = lake_atomic_read(&pool->pages[page]->version_and_refcnt[offset]); \
        /* remove zombie mark bit */ \
        version = version & GPU_SR_POOL_VERSION_COUNT_MASK; \
        lake_atomic_write(&pool->pages[page]->version_and_refcnt[offset], version); \
        *out_id = moon_id_make(moon_##T##_id, idx, version); \
        return &pool->pages[page]->slots[offset]; \
    }
IMPL_GPU_SR_POOL_TEMPLATE(buffer)
IMPL_GPU_SR_POOL_TEMPLATE(texture)
IMPL_GPU_SR_POOL_TEMPLATE(sampler)
IMPL_GPU_SR_POOL_TEMPLATE(tlas)
IMPL_GPU_SR_POOL_TEMPLATE(blas)

lake_result init_gpu_sr_table(struct moon_device_impl *device)
{
    lake_defer_begin();
    bool const ray_tracing_enabled = (device->header.details->implicit_features & moon_implicit_feature_basic_ray_tracing);
    u32 const max_allowed_buffers = device->header.assembly.max_allowed_buffers;
    u32 const max_allowed_textures = device->header.assembly.max_allowed_textures;
    u32 const max_allowed_samplers = device->header.assembly.max_allowed_samplers;
    u32 const max_allowed_acceleration_structures = ray_tracing_enabled ? device->header.assembly.max_allowed_acceleration_structures : (~0u);

    VkResult vk_result = VK_SUCCESS;
    lake_defer({
        if (vk_result != VK_SUCCESS) {
            lake_error("Creating the GPU shader resource table for Vulkan device `%s` failed: %s.",
                    device->header.assembly.name.str, vk_result_string(vk_result));

            for (u32 i = 0; i < MOON_PIPELINE_LAYOUT_COUNT; i++)
                if (device->gpu_sr_table.pipeline_layouts[i] != VK_NULL_HANDLE)
                    device->vkDestroyPipelineLayout(device->vk_device, device->gpu_sr_table.pipeline_layouts[i], device->vk_allocator);

            if (device->gpu_sr_table.vk_descriptor_pool)
                device->vkDestroyDescriptorPool(device->vk_device, device->gpu_sr_table.vk_descriptor_pool, device->vk_allocator);

            if (device->gpu_sr_table.vk_descriptor_set_layout)
                device->vkDestroyDescriptorSetLayout(device->vk_device, device->gpu_sr_table.vk_descriptor_set_layout, device->vk_allocator);
        } 
    });
    device->gpu_sr_table.buffer_slots.max_resources = max_allowed_buffers;
    device->gpu_sr_table.texture_slots.max_resources = max_allowed_textures;
    device->gpu_sr_table.sampler_slots.max_resources = max_allowed_samplers;
    if (ray_tracing_enabled) {
        device->gpu_sr_table.tlas_slots.max_resources = max_allowed_acceleration_structures;
        device->gpu_sr_table.blas_slots.max_resources = max_allowed_acceleration_structures * 10;
    }
    VkDescriptorPoolSize const vk_desc_pool_sizes[5] = {
        (VkDescriptorPoolSize){ /* storage buffer */
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = device->gpu_sr_table.buffer_slots.max_resources + 1,
        },
        (VkDescriptorPoolSize){ /* storage texture */
            .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = device->gpu_sr_table.texture_slots.max_resources,
        },
        (VkDescriptorPoolSize){ /* sampled texture */
            .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = device->gpu_sr_table.texture_slots.max_resources,
        },
        (VkDescriptorPoolSize){ /* sampler */
            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = device->gpu_sr_table.sampler_slots.max_resources,
        },
        (VkDescriptorPoolSize){ /* acceleration structure */
            .type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
            .descriptorCount = device->gpu_sr_table.tlas_slots.max_resources,
        },
    };
    VkDescriptorPoolCreateInfo const vk_desc_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = 1,
        .poolSizeCount = ray_tracing_enabled ? 5 : 4,
        .pPoolSizes = vk_desc_pool_sizes,
    };
    vk_result = device->vkCreateDescriptorPool(device->vk_device, &vk_desc_pool_create_info, device->vk_allocator, &device->gpu_sr_table.vk_descriptor_pool);
    if (vk_result != VK_SUCCESS) { 
        lake_defer_return vk_result_translate(vk_result); 
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        char const *desc_pool_name = "descriptor pool";
        VkDebugUtilsObjectNameInfoEXT const desc_pool_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL,
            .objectHandle = (u64)(uptr)device->gpu_sr_table.vk_descriptor_pool,
            .pObjectName = desc_pool_name,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &desc_pool_name_info);
    }
#endif /* LAKE_NDEBUG */
    VkDescriptorSetLayoutBinding const vk_desc_set_layout_bindings[6] = {
        (VkDescriptorSetLayoutBinding){ /* storage buffer */
            .binding = MOON_STORAGE_BUFFER_BINDING,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = device->gpu_sr_table.buffer_slots.max_resources,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr
        },
        (VkDescriptorSetLayoutBinding){ /* storage texture */
            .binding = MOON_STORAGE_TEXTURE_BINDING,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = device->gpu_sr_table.texture_slots.max_resources,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr
        },
        (VkDescriptorSetLayoutBinding){ /* sampled texture */
            .binding = MOON_SAMPLED_TEXTURE_BINDING,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = device->gpu_sr_table.texture_slots.max_resources,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr
        },
        (VkDescriptorSetLayoutBinding){ /* sampler */
            .binding = MOON_SAMPLER_BINDING,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = device->gpu_sr_table.sampler_slots.max_resources,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr
        },
        (VkDescriptorSetLayoutBinding){ /* device address buffer */
            .binding = MOON_DEVICE_ADDRESS_BUFFER_BINDING,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr
        },
        (VkDescriptorSetLayoutBinding){ /* acceleration structure */
            .binding = MOON_ACCELERATION_STRUCTURE_BINDING,
            .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
            .descriptorCount = device->gpu_sr_table.tlas_slots.max_resources,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr
        },
    };
    VkDescriptorBindingFlags vk_desc_binding_flags[6] = {
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
    };
    VkDescriptorSetLayoutBindingFlagsCreateInfo vk_desc_set_layout_binding_flags_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = ray_tracing_enabled ? 6 : 5,
        .pBindingFlags = vk_desc_binding_flags,
    };
    VkDescriptorSetLayoutCreateInfo const vk_desc_set_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &vk_desc_set_layout_binding_flags_create_info,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
        .bindingCount = ray_tracing_enabled ? 6 : 5, 
        .pBindings = vk_desc_set_layout_bindings,
    };
    vk_result = device->vkCreateDescriptorSetLayout(device->vk_device, &vk_desc_set_layout_create_info, device->vk_allocator, &device->gpu_sr_table.vk_descriptor_set_layout);
    if (vk_result != VK_SUCCESS) { 
        lake_defer_return vk_result_translate(vk_result); 
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        char const *desc_set_layout_name = "descriptor set layout";
        VkDebugUtilsObjectNameInfoEXT const desc_set_layout_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            .objectHandle = (u64)(uptr)device->gpu_sr_table.vk_descriptor_set_layout,
            .pObjectName = desc_set_layout_name,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &desc_set_layout_name_info);
    }
#endif /* LAKE_NDEBUG */
    VkDescriptorSetAllocateInfo const vk_desc_set_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = device->gpu_sr_table.vk_descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &device->gpu_sr_table.vk_descriptor_set_layout,
    };
    vk_result = device->vkAllocateDescriptorSets(device->vk_device, &vk_desc_set_alloc_info, &device->gpu_sr_table.vk_descriptor_set);
    if (vk_result != VK_SUCCESS) { 
        lake_defer_return vk_result_translate(vk_result); 
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        char const *desc_set_name = "descriptor set";
        VkDebugUtilsObjectNameInfoEXT const desc_set_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET,
            .objectHandle = (u64)(uptr)device->gpu_sr_table.vk_descriptor_set,
            .pObjectName = desc_set_name,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &desc_set_name_info);
    }
#endif /* LAKE_NDEBUG */
    VkPipelineLayoutCreateInfo vk_pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &device->gpu_sr_table.vk_descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };
    vk_result = device->vkCreatePipelineLayout(device->vk_device, &vk_pipeline_create_info, device->vk_allocator, &device->gpu_sr_table.pipeline_layouts[0]);
    if (vk_result != VK_SUCCESS) { 
        lake_defer_return vk_result_translate(vk_result); 
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        char const *layout_name = "pipeline layout (push constant size 0)";
        VkDebugUtilsObjectNameInfoEXT const layout_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            .objectHandle = (u64)(uptr)device->gpu_sr_table.pipeline_layouts[0],
            .pObjectName = layout_name,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &layout_name_info);
    }
#endif /* LAKE_NDEBUG */

    for (u32 i = 1; i < MOON_PIPELINE_LAYOUT_COUNT; i++) {
        VkPushConstantRange const vk_push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_ALL,
            .offset = 0,
            .size = i * 4,
        };
        vk_pipeline_create_info.pushConstantRangeCount = 1;
        vk_pipeline_create_info.pPushConstantRanges = &vk_push_constant_range;

        vk_result = device->vkCreatePipelineLayout(device->vk_device, &vk_pipeline_create_info, device->vk_allocator, &device->gpu_sr_table.pipeline_layouts[i]);
        if (vk_result != VK_SUCCESS) { 
            lake_defer_return vk_result_translate(vk_result); 
        }
#ifndef LAKE_NDEBUG
        if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
            usize len = 37;
            char layout_name[48] = "pipeline layout (push constant size ";
            len += snprintf(layout_name + len, 48 - len, "%u)", i * 4);
            layout_name[len] = '\0';
            
            VkDebugUtilsObjectNameInfoEXT const layout_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                .objectHandle = (u64)(uptr)device->gpu_sr_table.pipeline_layouts[i],
                .pObjectName = layout_name,
            };
            device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &layout_name_info);
        }
#endif /* LAKE_NDEBUG */
    }
    VkDescriptorBufferInfo const write_buffer = {
        .buffer = device->device_address_buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };
    VkWriteDescriptorSet const write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = device->gpu_sr_table.vk_descriptor_set,
        .dstBinding = MOON_DEVICE_ADDRESS_BUFFER_BINDING,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &write_buffer,
        .pTexelBufferView = nullptr,
    };
    device->vkUpdateDescriptorSets(device->vk_device, 1, &write, 0, nullptr);

#define GPU_SR_TABLE__IMPL_NODES(T, allowed) \
    do { \
        ssize node_count = lake_bits_next_pow2(allowed); \
        lake_mpmc_node_v *nodes = __lake_malloc_n(lake_mpmc_node_v, node_count); \
        lake_mpmc_init_t(&device->gpu_sr_table.T##_slots.free_indices.ring, lake_mpmc_node_v, node_count, nodes); \
    } while(0)

#define GPU_SR_TABLE__IMPL_ZOMBIES(T, len) \
    lake_deque_init_t(&device->T##_zombies.deq, zombie_timeline_##T, len, next_pow2_size, lake_deque_shrink_if_empty);

#define GPU_SR_TABLE__IMPL_NODES_AND_ZOMBIES(T, allowed) \
    GPU_SR_TABLE__IMPL_NODES(T, allowed); \
    GPU_SR_TABLE__IMPL_ZOMBIES(T, lake_max(next_pow2_size << 1, (s32)lake_bits_next_pow2(allowed) >> 6));

    s32 const thread_count = device->header.moon.header->bedrock->hints.worker_thread_count;
    s32 const next_pow2_size = lake_bits_next_pow2(thread_count);

    GPU_SR_TABLE__IMPL_NODES_AND_ZOMBIES(buffer, max_allowed_buffers);
    GPU_SR_TABLE__IMPL_NODES_AND_ZOMBIES(texture, max_allowed_textures);
    GPU_SR_TABLE__IMPL_ZOMBIES(texture_view, lake_max(next_pow2_size << 1, (s32)lake_bits_next_pow2(max_allowed_textures) >> 6));
    GPU_SR_TABLE__IMPL_NODES_AND_ZOMBIES(sampler, max_allowed_samplers);
    if (ray_tracing_enabled) {
        GPU_SR_TABLE__IMPL_NODES_AND_ZOMBIES(tlas, max_allowed_acceleration_structures);
        GPU_SR_TABLE__IMPL_NODES_AND_ZOMBIES(blas, max_allowed_acceleration_structures);
    }
    GPU_SR_TABLE__IMPL_ZOMBIES(event, next_pow2_size);
    GPU_SR_TABLE__IMPL_ZOMBIES(pipeline, next_pow2_size << 1);
    GPU_SR_TABLE__IMPL_ZOMBIES(semaphore, next_pow2_size << 1);
    GPU_SR_TABLE__IMPL_ZOMBIES(query_pool, next_pow2_size);
    GPU_SR_TABLE__IMPL_ZOMBIES(command_recorder, next_pow2_size);
    GPU_SR_TABLE__IMPL_ZOMBIES(memory_heap, next_pow2_size);

#undef GPU_SR_TABLE__IMPL_NODES_AND_ZOMBIES
#undef GPU_SR_TABLE__IMPL_ZOMBIES
#undef GPU_SR_TABLE__IMPL_NODES
    lake_defer_return LAKE_SUCCESS;
}

#ifndef LAKE_NDEBUG
#define GPU_SR_TABLE__DEBUG_PRINT(T, vk_name) \
    static char *LAKECALL debug_print_remaining_##T(struct T##_gpu_sr_pool const *pool, s32 valid_page_count) \
    { \
      /* TODO STRBUF ?? */ \
        constexpr s32 limit = 16384; \
        char *ret = lake_drift(limit, 1); \
        ssize o = 0lu; \
        \
        for (s32 i = 0; i < valid_page_count && o < limit-32; i++) { \
            struct T##_impl_slot_page const *page = pool->pages[i]; \
            \
            for (s32 j = 0; j < (s32)GPU_SR_POOL_PAGE_SIZE; j++) { \
                struct T##_impl_slot const *slot = &page->slots[j]; \
                \
                if (slot->vk_name != VK_NULL_HANDLE) \
                    o += snprintf(ret + o, slot->assembly.name.len, "\n  dbg name : \"%s\"", slot->assembly.name.str); \
            } \
        } \
        if (o >= limit-32) \
            o += snprintf(ret + o, 4, "..."); \
        ret[o] = '\0'; \
        return ret; \
    }
GPU_SR_TABLE__DEBUG_PRINT(buffer, vk_buffer)
GPU_SR_TABLE__DEBUG_PRINT(texture, vk_image)
GPU_SR_TABLE__DEBUG_PRINT(sampler, vk_sampler)
#undef GPU_SR_TABLE__DEBUG_PRINT

#define GPU_SR_TABLE__RELEASE_SLOTS_MEMORY_W_DBG(T) \
    s32 const sync_value = lake_atomic_read(&T##_pool->lifetime_sync); \
    if (sync_value > 0) { \
        char *remaining = debug_print_remaining_##T(T##_pool, valid_page_count); \
        lake_assert(false, LAKE_ERROR_TOO_MANY_OBJECTS, \
                "Detected leaked "#T", not all "#T"s (%d) have been released before " \
                "destroying the device. List of named survivors:\n%s", sync_value, remaining); \
    }
#else
#define GPU_SR_TABLE__RELEASE_SLOTS_MEMORY_W_DBG(T) LAKE_MAGIC_NOTHING()
#endif /* LAKE_NDEBUG */

void fini_gpu_sr_table(struct moon_device_impl *device)
{
    s32 valid_page_count;

#define GPU_SR_TABLE__RELEASE_SLOTS_MEMORY(T, dbg) \
    do { \
        struct T##_gpu_sr_pool *T##_pool = &device->gpu_sr_table.T##_slots; \
    valid_page_count = lake_atomic_read(&T##_pool->valid_page_count); \
        dbg \
        for (s32 i = 0; i < valid_page_count; i++) \
            __lake_free(T##_pool->pages[i]); \
        if (T##_pool->free_indices.ring.buffer != nullptr) \
            __lake_free(T##_pool->free_indices.ring.buffer); \
    } while(0);

    GPU_SR_TABLE__RELEASE_SLOTS_MEMORY(buffer, GPU_SR_TABLE__RELEASE_SLOTS_MEMORY_W_DBG(buffer))
    GPU_SR_TABLE__RELEASE_SLOTS_MEMORY(texture, GPU_SR_TABLE__RELEASE_SLOTS_MEMORY_W_DBG(texture))
    GPU_SR_TABLE__RELEASE_SLOTS_MEMORY(sampler, GPU_SR_TABLE__RELEASE_SLOTS_MEMORY_W_DBG(sampler))

    if (device->header.details->implicit_features & moon_implicit_feature_basic_ray_tracing) {
        GPU_SR_TABLE__RELEASE_SLOTS_MEMORY(tlas, LAKE_MAGIC_NOTHING())
        GPU_SR_TABLE__RELEASE_SLOTS_MEMORY(blas, LAKE_MAGIC_NOTHING())
    }
#undef GPU_SR_TABLE__RELEASE_SLOTS_MEMORY_W_DBG
#undef GPU_SR_TABLE__RELEASE_SLOTS_MEMORY

    lake_deque_fini(&device->buffer_zombies.deq);
    lake_deque_fini(&device->texture_zombies.deq);
    lake_deque_fini(&device->texture_view_zombies.deq);
    lake_deque_fini(&device->sampler_zombies.deq);
    lake_deque_fini(&device->tlas_zombies.deq);
    lake_deque_fini(&device->blas_zombies.deq);
    lake_deque_fini(&device->event_zombies.deq);
    lake_deque_fini(&device->pipeline_zombies.deq);
    lake_deque_fini(&device->semaphore_zombies.deq);
    lake_deque_fini(&device->query_pool_zombies.deq);
    lake_deque_fini(&device->command_recorder_zombies.deq);
    lake_deque_fini(&device->memory_heap_zombies.deq);

    for (u32 i = 0; i < MOON_PIPELINE_LAYOUT_COUNT; i++)
        device->vkDestroyPipelineLayout(device->vk_device, device->gpu_sr_table.pipeline_layouts[i], device->vk_allocator);

    device->vkDestroyDescriptorSetLayout(device->vk_device, device->gpu_sr_table.vk_descriptor_set_layout, device->vk_allocator);
    device->vkResetDescriptorPool(device->vk_device, device->gpu_sr_table.vk_descriptor_pool, 0);
    device->vkDestroyDescriptorPool(device->vk_device, device->gpu_sr_table.vk_descriptor_pool, device->vk_allocator);
}

void write_descriptor_set_buffer(
    struct moon_device_impl    *device,
    VkDescriptorSet             vk_desc_set,
    VkBuffer                    vk_buffer,
    VkDeviceSize                offset,
    VkDeviceSize                range,
    u32                         idx)
{
    VkDescriptorBufferInfo const vk_desc_buffer_info = {
        .buffer = vk_buffer,
        .offset = offset,
        .range = range,
    };
    VkWriteDescriptorSet const vk_write_desc_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = vk_desc_set,
        .dstBinding = MOON_STORAGE_BUFFER_BINDING,
        .dstArrayElement = idx,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &vk_desc_buffer_info,
        .pTexelBufferView = nullptr,
    };
    device->vkUpdateDescriptorSets(device->vk_device, 1, &vk_write_desc_set, 0, nullptr);
}

void write_descriptor_set_image(
    struct moon_device_impl    *device,
    VkDescriptorSet             vk_desc_set,
    VkImageView                 vk_image_view,
    moon_texture_usage          usage,
    u32                         idx)
{
    u32 desc_set_write_count = 0;
    VkWriteDescriptorSet desc_set_writes[2];

    VkDescriptorImageInfo const vk_desc_image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = vk_image_view,
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };
    VkWriteDescriptorSet const vk_write_desc_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = vk_desc_set,
        .dstBinding = MOON_STORAGE_TEXTURE_BINDING,
        .dstArrayElement = idx,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = &vk_desc_image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };
    if ((usage & moon_texture_usage_shader_storage) != moon_texture_usage_none)
        desc_set_writes[desc_set_write_count++] = vk_write_desc_set;
    
    VkDescriptorImageInfo const vk_desc_image_info_sampled = {
        .sampler = VK_NULL_HANDLE,
        .imageView = vk_image_view,
        .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    };
    VkWriteDescriptorSet const vk_write_desc_set_sampled = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = vk_desc_set,
        .dstBinding = MOON_SAMPLED_TEXTURE_BINDING,
        .dstArrayElement = idx,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = &vk_desc_image_info_sampled,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };
    if ((usage & moon_texture_usage_shader_sampled) != moon_texture_usage_none)
        desc_set_writes[desc_set_write_count++] = vk_write_desc_set_sampled;
    
    device->vkUpdateDescriptorSets(device->vk_device, desc_set_write_count, desc_set_writes, 0, nullptr);
}

void write_descriptor_set_sampler(
    struct moon_device_impl    *device,
    VkDescriptorSet             vk_desc_set,
    VkSampler                   vk_sampler,
    u32                         idx)
{
    VkDescriptorImageInfo const vk_desc_image_info = {
        .sampler = vk_sampler,
        .imageView = VK_NULL_HANDLE,
        .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    VkWriteDescriptorSet const vk_write_desc_set_storage = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = vk_desc_set,
        .dstBinding = MOON_SAMPLER_BINDING,
        .dstArrayElement = idx,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .pImageInfo = &vk_desc_image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };
    device->vkUpdateDescriptorSets(device->vk_device, 1, &vk_write_desc_set_storage, 0, nullptr);
}

void write_descriptor_set_acceleration_structure(
    struct moon_device_impl    *device,
    VkDescriptorSet             vk_desc_set,
    VkAccelerationStructureKHR  vk_acceleration_structure,
    u32                         idx)
{
    VkWriteDescriptorSetAccelerationStructureKHR vk_write_desc_set_as = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
        .pNext = nullptr,
        .accelerationStructureCount = 1,
        .pAccelerationStructures = &vk_acceleration_structure,
    };
    VkWriteDescriptorSet const vk_write_desc_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = &vk_write_desc_set_as,
        .dstSet = vk_desc_set,
        .dstBinding = MOON_ACCELERATION_STRUCTURE_BINDING,
        .dstArrayElement = idx,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        .pImageInfo = nullptr,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };
    device->vkUpdateDescriptorSets(device->vk_device, 1, &vk_write_desc_set, 0, nullptr);
}

static lake_result helper_vk_buffer_assembly(
    struct moon_device_impl        *device,
    moon_buffer_assembly const     *assembly,
    moon_buffer_id                 *out_id,
    struct moon_memory_heap_impl   *opt_heap,
    usize                           opt_offset)
{
    lake_result result = LAKE_SUCCESS;

    if (assembly->size == 0)
        return LAKE_ERROR_INVALID_ASSEMBLY_DETAILS;

    moon_buffer_id id = {0};
    struct buffer_impl_slot *slot = buffer_gpu_sr_pool__try_create_slot(&device->gpu_sr_table.buffer_slots, &id);
    if (slot == nullptr)
        return LAKE_ERROR_EXCEEDED_MAX_BUFFERS;

    slot->assembly = *assembly;

    VkBufferCreateInfo const vk_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = (VkDeviceSize)slot->assembly.size,
        .usage = get_buffer_usage_flags(device),
        .sharingMode = VK_SHARING_MODE_CONCURRENT, /* FIXME buffers are always shared for now */
        .queueFamilyIndexCount = device->physical_device->unique_queue_family_count,
        .pQueueFamilyIndices = device->physical_device->unique_queue_family_indices,
    };
    bool host_accessible = false;
    VmaAllocationInfo vma_allocation_info = {0};
    if (opt_heap == nullptr) {
        VmaAllocationCreateFlags vma_allocation_flags = assembly->memory_flags;
        if (((vma_allocation_flags & VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT) != 0u) ||
            ((vma_allocation_flags & VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT) != 0u) ||
            ((vma_allocation_flags & VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT) != 0u))
        {
            vma_allocation_flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
            host_accessible = true;
        }
        VmaAllocationCreateInfo const vma_allocation_create_info = {
            .flags = vma_allocation_flags,
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = UINT32_MAX,
            .pool = nullptr,
            .pUserData = nullptr,
            .priority = 0.5f,
        };
        result = vk_result_translate(
            vmaCreateBuffer(
                device->vma_allocator,
                &vk_buffer_create_info,
                &vma_allocation_create_info,
                &slot->vk_buffer,
                &slot->vma_allocation,
                &vma_allocation_info));
    } else {
        slot->optional_heap = opt_heap;
        lake_inc_refcnt(&opt_heap->header.refcnt);

        result = vk_result_translate(
            device->vkCreateBuffer(
                device->vk_device,
                &vk_buffer_create_info,
                device->vk_allocator,
                &slot->vk_buffer));
        if (result != LAKE_SUCCESS) {
            buffer_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.buffer_slots, id);
            if (slot->vk_buffer != VK_NULL_HANDLE)
                device->vkDestroyBuffer(device->vk_device, slot->vk_buffer, device->vk_allocator);
            slot->vk_buffer = VK_NULL_HANDLE;
            return result;
        }

        result = vk_result_translate(
            vmaBindBufferMemory2(
                device->vma_allocator, 
                opt_heap->vma_allocation,
                opt_offset,
                slot->vk_buffer,
                nullptr));
    }
    if (result != LAKE_SUCCESS) {
        buffer_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.buffer_slots, id);
        if (slot->vk_buffer != VK_NULL_HANDLE)
            device->vkDestroyBuffer(device->vk_device, slot->vk_buffer, device->vk_allocator);
        slot->vk_buffer = VK_NULL_HANDLE;
        return result;
    }
    VkBufferDeviceAddressInfo const vk_buffer_device_address_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .pNext = nullptr,
        .buffer = slot->vk_buffer,
    };
    slot->device_address = device->vkGetBufferDeviceAddress(device->vk_device, &vk_buffer_device_address_info);
    slot->host_address = host_accessible ? vma_allocation_info.pMappedData : nullptr;
    device->device_address_buffer_host[moon_id_get_index(id)] = slot->device_address;
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT image_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_BUFFER,
            .objectHandle = (u64)(uptr)slot->vk_buffer,
            .pObjectName = assembly->name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &image_name_info);
    }
#endif /* LAKE_NDEBUG */
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_buffer(device, 
            device->gpu_sr_table.vk_descriptor_set, 
            slot->vk_buffer, 0,
            (VkDeviceSize)slot->assembly.size,
            moon_id_get_index(id));
    *out_id = id;
    return result;
}

FN_MOON_CREATE_BUFFER(vulkan)
{
    return helper_vk_buffer_assembly(device, assembly, out_buffer, nullptr, 0);
}

FN_MOON_CREATE_BUFFER_FROM_MEMORY_HEAP(vulkan)
{
    return helper_vk_buffer_assembly(device, &assembly->buffer_assembly, out_buffer, assembly->memory_heap, assembly->offset);
}

static lake_result helper_vk_image_assembly(
    struct moon_device_impl        *device,
    moon_texture_assembly const    *assembly,
    moon_texture_id                *out_id,
    struct moon_memory_heap_impl   *opt_heap,
    usize                           opt_offset)
{
    lake_result result = LAKE_SUCCESS;

    if (!(assembly->dimensions >= 1 && assembly->dimensions <= 3))
        return LAKE_ERROR_INVALID_ASSEMBLY_DETAILS;

    moon_texture_id id = {0};
    struct texture_impl_slot *slot = texture_gpu_sr_pool__try_create_slot(&device->gpu_sr_table.texture_slots, &id);
    if (slot == nullptr)
        return LAKE_ERROR_EXCEEDED_MAX_TEXTURES;

    VkImageViewType vk_image_view_type = {0};
    if (assembly->array_layer_count > 1) {
        vk_image_view_type = (VkImageViewType)assembly->dimensions + 3;
    } else {
        vk_image_view_type = (VkImageViewType)assembly->dimensions - 1;
    }
    slot->assembly = *assembly;
    slot->view_slot.assembly = (moon_texture_view_assembly){
        .type = (moon_texture_view_type)vk_image_view_type,
        .format = assembly->format,
        .texture = id,
        .slice = (moon_texture_mip_array_slice){
            .base_mip_level = 0,
            .level_count = assembly->mip_level_count,
            .base_array_layer = 0,
            .layer_count = assembly->array_layer_count,
        },
        .name = assembly->name,
    };
    slot->aspect_flags = infer_aspect_from_format(assembly->format);

    VkImageViewCreateInfo vk_image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewType = vk_image_view_type,
        .format = (VkFormat)assembly->format,
        .components = (VkComponentMapping){
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange = {
            .aspectMask = slot->aspect_flags,
            .baseMipLevel = 0,
            .levelCount = assembly->mip_level_count,
            .baseArrayLayer = 0,
            .layerCount = assembly->array_layer_count,
        },
    };
    VkImageCreateInfo vk_image_create_info;
    populate_vk_image_create_info_from_assembly(device, assembly, &vk_image_create_info);

    if (opt_heap == nullptr) {
        VmaAllocationCreateInfo const vma_allocation_create_info = {
            .flags = (VmaAllocationCreateFlags)assembly->memory_flags,
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = UINT32_MAX,
            .pool = nullptr,
            .pUserData = nullptr,
            .priority = 0.5f,
        };
        result = vk_result_translate(
            vmaCreateImage(
                device->vma_allocator,
                &vk_image_create_info,
                &vma_allocation_create_info,
                &slot->vk_image,
                &slot->vma_allocation,
                nullptr));
        vk_image_view_create_info.image = slot->vk_image;

        /* don't continue if the memory mapping failed */
        if (result == LAKE_SUCCESS) 
            result = vk_result_translate(
                device->vkCreateImageView(
                    device->vk_device,
                    &vk_image_view_create_info,
                    device->vk_allocator,
                    &slot->view_slot.vk_image_view));
    } else {
        slot->optional_heap = opt_heap;
        lake_inc_refcnt(&opt_heap->header.refcnt);
        result = vk_result_translate(
            device->vkCreateImage(
                device->vk_device,
                &vk_image_create_info,
                device->vk_allocator,
                &slot->vk_image));
        if (result != LAKE_SUCCESS) {
            texture_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.texture_slots, id);
            if (slot->view_slot.vk_image_view != VK_NULL_HANDLE)
                device->vkDestroyImageView(device->vk_device, slot->view_slot.vk_image_view, device->vk_allocator);
            slot->view_slot.vk_image_view = VK_NULL_HANDLE;
            if (slot->vk_image != VK_NULL_HANDLE)
                device->vkDestroyImage(device->vk_device, slot->vk_image, device->vk_allocator);
            slot->vk_image = VK_NULL_HANDLE;
            return result;
        }
        vk_image_view_create_info.image = slot->vk_image;

        result = vk_result_translate(
            vmaBindImageMemory2(
                device->vma_allocator,
                opt_heap->vma_allocation,
                opt_offset,
                slot->vk_image,
                nullptr));
        /* don't continue if the memory mapping failed */
        if (result == LAKE_SUCCESS) 
            result = vk_result_translate(
                device->vkCreateImageView(
                    device->vk_device,
                    &vk_image_view_create_info,
                    device->vk_allocator,
                    &slot->view_slot.vk_image_view));
    }
    if (result != LAKE_SUCCESS) {
        texture_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.texture_slots, id);
        if (slot->view_slot.vk_image_view != VK_NULL_HANDLE)
            device->vkDestroyImageView(device->vk_device, slot->view_slot.vk_image_view, device->vk_allocator);
        slot->view_slot.vk_image_view = VK_NULL_HANDLE;
        if (slot->vk_image != VK_NULL_HANDLE)
            device->vkDestroyImage(device->vk_device, slot->vk_image, device->vk_allocator);
        slot->vk_image = VK_NULL_HANDLE;
        return result;
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT image_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_IMAGE,
            .objectHandle = (u64)(uptr)slot->vk_image,
            .pObjectName = assembly->name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &image_name_info);

        VkDebugUtilsObjectNameInfoEXT image_view_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_IMAGE_VIEW,
            .objectHandle = (u64)(uptr)slot->view_slot.vk_image_view,
            .pObjectName = assembly->name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &image_view_name_info);
    }
#endif /* LAKE_NDEBUG */
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_image(device, 
            device->gpu_sr_table.vk_descriptor_set, 
            slot->view_slot.vk_image_view,
            slot->assembly.usage, 
            moon_id_get_index(id));
    *out_id = id;
    return result;
}

FN_MOON_CREATE_TEXTURE(vulkan)
{
    return helper_vk_image_assembly(device, assembly, out_texture, nullptr, 0);
}

FN_MOON_CREATE_TEXTURE_FROM_MEMORY_HEAP(vulkan)
{
    return helper_vk_image_assembly(device, &assembly->texture_assembly, out_texture, assembly->memory_heap, assembly->offset);
}

lake_result create_texture_from_swapchain_image(
    struct moon_device_impl        *device,
    VkImage                         swapchain_image, 
    VkFormat                        format,
    u32                             idx,
    moon_texture_usage              usage,
    moon_texture_assembly const    *assembly,
    moon_texture_id                *out_texture)
{
    lake_result result = LAKE_SUCCESS;

    moon_texture_id id = {0};
    struct texture_impl_slot *slot = texture_gpu_sr_pool__try_create_slot(&device->gpu_sr_table.texture_slots, &id);
    if (slot == nullptr)
        return LAKE_ERROR_EXCEEDED_MAX_TEXTURES;

    lake_dbg_assert(swapchain_image != VK_NULL_HANDLE, LAKE_ERROR_MEMORY_MAP_FAILED, "%s", assembly->name.str);
    slot->vk_image = swapchain_image;
    slot->view_slot.assembly = (moon_texture_view_assembly){
        .type = moon_texture_view_type_2d,
        .format = assembly->format,
        .texture = id,
        .slice = (moon_texture_mip_array_slice){
            .base_mip_level = 0,
            .level_count = assembly->mip_level_count,
            .base_array_layer = 0,
            .layer_count = assembly->array_layer_count,
        },
        .name = assembly->name,
    };
    slot->aspect_flags = infer_aspect_from_format(assembly->format);
    slot->swapchain_image_idx = idx;
    slot->assembly = *assembly;

    slot->view_slot.assembly = (moon_texture_view_assembly){
        .texture = id,
        .format = (moon_format)format,
        .slice = {
            .base_mip_level = 0,
            .level_count = 1,
            .base_array_layer = 0,
            .layer_count = 1,
        },
        .name = slot->assembly.name,
    };
    VkImageViewCreateInfo const vk_image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = swapchain_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };
    result = vk_result_translate(
        device->vkCreateImageView(
            device->vk_device,
            &vk_image_view_create_info,
            device->vk_allocator,
            &slot->view_slot.vk_image_view));
    if (result != LAKE_SUCCESS) {
        texture_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.texture_slots, id);
        if (slot->view_slot.vk_image_view != VK_NULL_HANDLE)
            device->vkDestroyImageView(device->vk_device, slot->view_slot.vk_image_view, device->vk_allocator);
        slot->view_slot.vk_image_view = VK_NULL_HANDLE;
        return result;
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT image_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_IMAGE,
            .objectHandle = (u64)(uptr)slot->vk_image,
            .pObjectName = assembly->name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &image_name_info);

        VkDebugUtilsObjectNameInfoEXT image_view_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_IMAGE_VIEW,
            .objectHandle = (u64)(uptr)slot->view_slot.vk_image_view,
            .pObjectName = assembly->name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &image_view_name_info);
    }
#endif /* LAKE_NDEBUG */
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_image(device, 
            device->gpu_sr_table.vk_descriptor_set, 
            slot->view_slot.vk_image_view,
            usage, 
            moon_id_get_index(id));
    *out_texture = id;
    return result;
}

FN_MOON_CREATE_TEXTURE_VIEW(vulkan)
{
    lake_result result = LAKE_SUCCESS;
    union {
        moon_texture_id texture;
        moon_texture_view_id view;
    } id;
    struct texture_impl_slot *slot = texture_gpu_sr_pool__try_create_slot(&device->gpu_sr_table.texture_slots, &id.texture);
    if (slot == nullptr)
        return LAKE_ERROR_EXCEEDED_MAX_TEXTURES;

    struct texture_impl_slot const *parent = acquire_texture_slot(device, assembly->texture);

    if (assembly->slice.layer_count > 1) {
        bool const array_type = assembly->type == moon_texture_view_type_1d_array ||
                                assembly->type == moon_texture_view_type_2d_array ||
                                assembly->type == moon_texture_view_type_cube ||
                                assembly->type == moon_texture_view_type_cube_array;
        if (!array_type)
            result = LAKE_ERROR_INVALID_ASSEMBLY_DETAILS;
    }
    if (result != LAKE_SUCCESS) {
        texture_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.texture_slots, id.texture);
        if (slot->view_slot.vk_image_view != VK_NULL_HANDLE)
            device->vkDestroyImageView(device->vk_device, slot->view_slot.vk_image_view, device->vk_allocator);
        slot->view_slot.vk_image_view = VK_NULL_HANDLE;
        return result;
    }
    *slot = (struct texture_impl_slot){0};
    slot->view_slot.assembly = *assembly;
    slot->view_slot.assembly.slice = validate_texture_slice(device, &assembly->slice, id.view);
    
    VkImageViewCreateInfo const vk_image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = parent->vk_image,
        .viewType = (VkImageViewType)slot->view_slot.assembly.type,
        .format = (VkFormat)slot->view_slot.assembly.format,
        .components = (VkComponentMapping){
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange = make_subresource_range(&slot->view_slot.assembly.slice, parent->aspect_flags),
    };
    result = vk_result_translate(
        device->vkCreateImageView(
            device->vk_device,
            &vk_image_view_create_info,
            device->vk_allocator,
            &slot->view_slot.vk_image_view));
    if (result != LAKE_SUCCESS) {
        texture_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.texture_slots, id.texture);
        if (slot->view_slot.vk_image_view != VK_NULL_HANDLE)
            device->vkDestroyImageView(device->vk_device, slot->view_slot.vk_image_view, device->vk_allocator);
        slot->view_slot.vk_image_view = VK_NULL_HANDLE;
        return result;
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT const name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_IMAGE_VIEW,
            .objectHandle = (u64)(uptr)slot->view_slot.vk_image_view,
            .pObjectName = assembly->name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &name_info);
    }
#endif /* LAKE_NDEBUG */
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_image(device, 
            device->gpu_sr_table.vk_descriptor_set, 
            slot->view_slot.vk_image_view,
            parent->assembly.usage, 
            moon_id_get_index(id.view));
    *out_texture_view = id.view;
    return result;
}

FN_MOON_CREATE_SAMPLER(vulkan)
{
    lake_result result = LAKE_SUCCESS;
    lake_dbg_assert(!((VkFilter)assembly->mipmap_filter != VK_FILTER_CUBIC_IMG), LAKE_ERROR_INVALID_ASSEMBLY_DETAILS, nullptr);
    
    moon_sampler_id id = {0};
    struct sampler_impl_slot *slot = sampler_gpu_sr_pool__try_create_slot(&device->gpu_sr_table.sampler_slots, &id);
    if (slot == nullptr)
        return LAKE_ERROR_EXCEEDED_MAX_SAMPLERS;

    VkSamplerReductionModeCreateInfo vk_sampler_reduction_mode_create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO,
        .pNext = nullptr,
        .reductionMode = (VkSamplerReductionMode)assembly->reduction_mode,
    };
    VkSamplerCreateInfo const vk_sampler_create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = &vk_sampler_reduction_mode_create_info,
        .flags = 0,
        .magFilter = (VkFilter)assembly->magnification_filter,
        .minFilter = (VkFilter)assembly->minification_filter,
        .mipmapMode = (VkSamplerMipmapMode)assembly->mipmap_filter,
        .addressModeU = (VkSamplerAddressMode)assembly->address_mode_u,
        .addressModeV = (VkSamplerAddressMode)assembly->address_mode_v,
        .addressModeW = (VkSamplerAddressMode)assembly->address_mode_w,
        .mipLodBias = assembly->mip_lod_bias,
        .anisotropyEnable = assembly->enable_anisotrophy,
        .maxAnisotropy = assembly->max_anisotrophy,
        .compareEnable = assembly->enable_compare_op,
        .compareOp = (VkCompareOp)assembly->compare_op,
        .minLod = assembly->min_lod,
        .maxLod = assembly->max_lod,
        .borderColor = (VkBorderColor)assembly->border_color,
        .unnormalizedCoordinates = assembly->enable_unnormalized_coordinates,
    };
    result = vk_result_translate(
            device->vkCreateSampler(
                device->vk_device, 
                &vk_sampler_create_info,
                device->vk_allocator,
                &slot->vk_sampler));
    if (result != LAKE_SUCCESS) {
        sampler_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.sampler_slots, id);
        if (slot->vk_sampler != VK_NULL_HANDLE)
            device->vkDestroySampler(device->vk_device, slot->vk_sampler, device->vk_allocator);
        slot->vk_sampler = VK_NULL_HANDLE;
        return result;
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT const sampler_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_SAMPLER,
            .objectHandle = (u64)(uptr)slot->vk_sampler,
            .pObjectName = assembly->name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &sampler_name_info);
    }
#endif /* LAKE_NDEBUG */
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_sampler(device, device->gpu_sr_table.vk_descriptor_set, slot->vk_sampler, moon_id_get_index(id));
    *out_sampler = id;
    return result;
}

lake_static_assert(sizeof(struct tlas_impl_slot) == sizeof(struct blas_impl_slot), "tlas and blas must stay abi compatible");

static lake_result helper_vk_acceleration_structure_assembly(
    struct moon_device_impl        *device,
    void                           *as_slot,
    u64                             as_id,
    VkAccelerationStructureTypeKHR  vk_as_type,
    moon_buffer_id const           *buffer,
    u64 const                      *offset)
{
    lake_result result = LAKE_SUCCESS;
    /* tlas and blas must stay abi compatible */
    struct tlas_impl_slot *slot = (struct tlas_impl_slot *)as_slot;
    moon_tlas_id id = { .handle = as_id };

    if (buffer) {
        slot->buffer_id = *buffer;
        slot->offset = *offset;
        slot->owns_buffer = false;
    } else {
        char const *buffer_postfix = " buf";
        moon_buffer_assembly buffer_assembly = { .size = slot->assembly.size };

        lake_memcpy(&buffer_assembly.name, &slot->assembly.name, sizeof(lake_small_string));
        for (u32 i = 0; i < 4 && buffer_assembly.name.len < LAKE_SMALL_STRING_CAPACITY-1; i++) 
            buffer_assembly.name.str[buffer_assembly.name.len++] = buffer_postfix[i];
        buffer_assembly.name.str[lake_max(buffer_assembly.name.len, LAKE_SMALL_STRING_CAPACITY-1)] = '\0';

        result = _moon_vulkan_create_buffer(device, &buffer_assembly, &slot->buffer_id);
        if (result != LAKE_SUCCESS)
            return result;

        slot->offset = 0;
        slot->owns_buffer = true;
    }
    slot->vk_buffer = acquire_buffer_slot(device, slot->buffer_id)->vk_buffer;

    VkAccelerationStructureCreateInfoKHR vk_as_create_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .createFlags = 0, /* VK_ACCELERATION_STRUCTURE_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_KHR, */
        .buffer = slot->vk_buffer,
        .offset = slot->offset,
        .size = slot->assembly.size,
        .type = vk_as_type,
        .deviceAddress = 0,
    };
    result = vk_result_translate(
        device->vkCreateAccelerationStructureKHR(
            device->vk_device, 
            &vk_as_create_info, 
            device->vk_allocator, 
            &slot->vk_acceleration_structure));
    if (result != LAKE_SUCCESS)
        return result;

    VkAccelerationStructureDeviceAddressInfoKHR vk_as_device_address_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
        .pNext = nullptr,
        .accelerationStructure = slot->vk_acceleration_structure,
    };
    slot->device_address = device->vkGetAccelerationStructureDeviceAddressKHR(device->vk_device, &vk_as_device_address_info);

    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT const debug_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR,
            .objectHandle = (u64)(uptr)slot->vk_acceleration_structure,
            .pObjectName = slot->assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_name_info);
    }
    if (vk_as_type == VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR) {
        /* Does not need external sync given we use update after bind.
         * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
        write_descriptor_set_acceleration_structure(device, device->gpu_sr_table.vk_descriptor_set, slot->vk_acceleration_structure, moon_id_get_index(id));
    }
    return result;
}

#define IMPL_CREATE_ACCELERATION_STRUCTURE(T, name, asm_write, vk_type, buf, off) \
    FN_MOON_CREATE_##name(vulkan) \
    { \
        lake_result result; \
        moon_##T##_id id = {0}; \
        struct T##_gpu_sr_pool *pool = &device->gpu_sr_table.T##_slots; \
        \
        if (!(device->header.details->implicit_features & moon_implicit_feature_basic_ray_tracing)) \
            return LAKE_ERROR_RAY_TRACING_REQUIRED; \
        \
        struct T##_impl_slot *slot = T##_gpu_sr_pool__try_create_slot(pool, &id); \
        if (slot == nullptr) \
            return LAKE_ERROR_EXCEEDED_MAX_ACCELERATION_STRUCTURES; \
        \
        slot->assembly = asm_write; \
        \
        result = helper_vk_acceleration_structure_assembly(device, slot, id.handle, VK_ACCELERATION_STRUCTURE_TYPE_##vk_type##_KHR, buf, off); \
        if (result != LAKE_SUCCESS) { \
            T##_gpu_sr_pool__unsafe_destroy_zombie_slot(pool, id); \
            if (slot->owns_buffer && !moon_id_is_empty(slot->buffer_id)) { \
                lake_result _ignore = _moon_vulkan_destroy_buffer(device, slot->buffer_id); \
                (void)_ignore; \
            } \
        } \
        *out_##T = id; \
        return result; \
    }
IMPL_CREATE_ACCELERATION_STRUCTURE(tlas, TLAS, *assembly, TOP_LEVEL, nullptr, nullptr)
IMPL_CREATE_ACCELERATION_STRUCTURE(tlas, TLAS_FROM_BUFFER, assembly->tlas_assembly, TOP_LEVEL, &assembly->buffer, &assembly->offset)
IMPL_CREATE_ACCELERATION_STRUCTURE(blas, BLAS, *assembly, BOTTOM_LEVEL, nullptr, nullptr)
IMPL_CREATE_ACCELERATION_STRUCTURE(blas, BLAS_FROM_BUFFER, assembly->blas_assembly, BOTTOM_LEVEL, &assembly->buffer, &assembly->offset)

#define IMPL_RESOURCE_API(T, NAME, Tpool, slot_name) \
    FN_MOON_IS_##NAME##_VALID(vulkan) { \
        return Tpool##_gpu_sr_pool__is_id_valid(&device->gpu_sr_table.slot_name, (moon_##Tpool##_id){ .handle = T.handle }); \
    } \
    FN_MOON_READ_##NAME##_ASSEMBLY(vulkan) { \
        struct T##_impl_slot const *slot = acquire_##T##_slot(device, T); \
        if (_moon_vulkan_is_##T##_valid(device, T)) { \
            *out_##assembly = slot->assembly; \
            return LAKE_SUCCESS; \
        } \
        return LAKE_ERROR_INVALID_##NAME##_ID; \
    } \
    FN_MOON_DESTROY_##NAME(vulkan) { \
        bool success = Tpool##_gpu_sr_pool__try_zombify(&device->gpu_sr_table.slot_name, moon_id_t(moon_##Tpool##_id, T)); \
        if (success) { \
            zombify_##T(device, T); \
            return LAKE_SUCCESS; \
        } \
        return LAKE_ERROR_INVALID_##NAME##_ID; \
    }
IMPL_RESOURCE_API(buffer, BUFFER, buffer, buffer_slots);
IMPL_RESOURCE_API(texture, TEXTURE, texture, texture_slots);
IMPL_RESOURCE_API(texture_view, TEXTURE_VIEW, texture, texture_slots);
IMPL_RESOURCE_API(sampler, SAMPLER, sampler, sampler_slots);
IMPL_RESOURCE_API(tlas, TLAS, tlas, tlas_slots);
IMPL_RESOURCE_API(blas, BLAS, blas, blas_slots);

FN_MOON_BUFFER_HOST_ADDRESS(vulkan)
{
    if (!_moon_vulkan_is_buffer_valid(device, buffer))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_host_address = acquire_buffer_slot(device, buffer)->host_address;
    return LAKE_SUCCESS;
}

FN_MOON_BUFFER_DEVICE_ADDRESS(vulkan)
{
    if (!_moon_vulkan_is_buffer_valid(device, buffer))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_device_address = acquire_buffer_slot(device, buffer)->device_address;
    return LAKE_SUCCESS;
}

FN_MOON_TLAS_DEVICE_ADDRESS(vulkan)
{
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (!_moon_vulkan_is_tlas_valid(device, tlas))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_device_address = acquire_tlas_slot(device, tlas)->device_address;
    return LAKE_SUCCESS;
}

FN_MOON_BLAS_DEVICE_ADDRESS(vulkan)
{
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (!_moon_vulkan_is_blas_valid(device, blas))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_device_address = acquire_blas_slot(device, blas)->device_address;
    return LAKE_SUCCESS;
}
#endif /* MOON_VULKAN */
