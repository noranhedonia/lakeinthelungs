#include "vk_moon.h"
#ifdef MOON_VULKAN

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
    moon_device                  device, 
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

void populate_vk_acceleration_structure_build_from_assembly(
    moon_device                                  device,
    usize                                        tlas_count,
    moon_tlas_assembly const                    *tlas_assembly,
    usize                                        blas_count,
    moon_blas_assembly const                    *blas_assembly,
    struct acceleratrion_structure_build        *out_vk_build)
{
    (void)device;
    (void)tlas_count;
    (void)tlas_assembly;
    (void)blas_count;
    (void)blas_assembly;
    (void)out_vk_build;
}

static lake_result helper_vk_buffer_assembly(
    moon_device                 device,
    moon_buffer_assembly const *assembly,
    moon_buffer_id             *out_id,
    moon_memory_heap            opt_heap,
    usize                       opt_offset)
{
    (void)device;
    (void)assembly;
    (void)out_id;
    (void)opt_heap;
    (void)opt_offset;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

static lake_result helper_vk_image_assembly(
    moon_device                  device,
    moon_texture_assembly const *assembly,
    moon_texture_id             *out_id,
    moon_memory_heap             opt_heap,
    usize                        opt_offset)
{
    (void)device;
    (void)assembly;
    (void)out_id;
    (void)opt_heap;
    (void)opt_offset;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

static lake_result helper_vk_acceleration_structure_assembly(
    moon_device                     device,
    void                           *table,
    VkAccelerationStructureTypeKHR  vk_as_type,
    void const                     *assembly,
    moon_buffer_id const           *buffer,
    u64 const                      *offset,
    void                           *out_id)
{
    (void)device;
    (void)table;
    (void)vk_as_type;
    (void)assembly;
    (void)buffer;
    (void)offset;
    (void)out_id;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_BUFFER(vulkan)
{
    return helper_vk_buffer_assembly(device, assembly, out_buffer, nullptr, 0);
}

FN_MOON_CREATE_BUFFER_FROM_MEMORY_HEAP(vulkan)
{
    return helper_vk_buffer_assembly(device, &assembly->buffer_assembly, out_buffer, assembly->memory_heap, assembly->offset);
}

FN_MOON_CREATE_TEXTURE(vulkan)
{
    return helper_vk_image_assembly(device, assembly, out_texture, nullptr, 0);
}

FN_MOON_CREATE_TEXTURE_FROM_MEMORY_HEAP(vulkan)
{
    return helper_vk_image_assembly(device, &assembly->texture_assembly, out_texture, assembly->memory_heap, assembly->offset);
}

FN_MOON_CREATE_TEXTURE_VIEW(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_texture_view;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_SAMPLER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_sampler;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_TLAS(vulkan)
{
    return helper_vk_acceleration_structure_assembly(
        device,
        nullptr, /* TODO gpu resource table, tlas_slots */
        VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
        assembly,
        nullptr,
        nullptr,
        out_tlas);
}

FN_MOON_CREATE_TLAS_FROM_BUFFER(vulkan)
{
    return helper_vk_acceleration_structure_assembly(
        device,
        nullptr, /* TODO gpu resource table, tlas_slots */
        VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
        &assembly->tlas_assembly,
        &assembly->buffer,
        &assembly->offset,
        out_tlas);
}

FN_MOON_CREATE_BLAS(vulkan)
{
    return helper_vk_acceleration_structure_assembly(
        device,
        nullptr, /* TODO gpu resource table, tlas_slots */
        VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
        assembly,
        nullptr,
        nullptr,
        out_blas);
}

FN_MOON_CREATE_BLAS_FROM_BUFFER(vulkan)
{
    return helper_vk_acceleration_structure_assembly(
        device,
        nullptr, /* TODO gpu resource table, tlas_slots */
        VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
        &assembly->blas_assembly,
        &assembly->buffer,
        &assembly->offset,
        out_blas);
}

FN_MOON_IS_BUFFER_VALID(vulkan)
{
    (void)device;
    (void)buffer;
    return false;
}

FN_MOON_IS_TEXTURE_VALID(vulkan)
{
    (void)device;
    (void)texture;
    return false;
}

FN_MOON_IS_TEXTURE_VIEW_VALID(vulkan)
{
    (void)device;
    (void)texture_view;
    return false;
}

FN_MOON_IS_SAMPLER_VALID(vulkan)
{
    (void)device;
    (void)sampler;
    return false;
}

FN_MOON_IS_TLAS_VALID(vulkan)
{
    (void)device;
    (void)tlas;
    return false;
}

FN_MOON_IS_BLAS_VALID(vulkan)
{
    (void)device;
    (void)blas;
    return false;
}

FN_MOON_BUFFER_HOST_ADDRESS(vulkan)
{
    if (!_moon_vulkan_is_buffer_valid(device, buffer))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_host_address = 0; /* TODO */
    return LAKE_SUCCESS;
}

FN_MOON_BUFFER_DEVICE_ADDRESS(vulkan)
{
    if (!_moon_vulkan_is_buffer_valid(device, buffer))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_device_address = 0; /* TODO */
    return LAKE_SUCCESS;
}

FN_MOON_TLAS_DEVICE_ADDRESS(vulkan)
{
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (!_moon_vulkan_is_tlas_valid(device, tlas))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_device_address = 0; /* TODO */
    return LAKE_SUCCESS;
}

FN_MOON_BLAS_DEVICE_ADDRESS(vulkan)
{
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (!_moon_vulkan_is_blas_valid(device, blas))
        return LAKE_ERROR_INVALID_BUFFER_ID;
    *out_device_address = 0; /* TODO */
    return LAKE_SUCCESS;
}

FN_MOON_DESTROY_BUFFER(vulkan)
{
    (void)device;
    (void)buffer;
}

FN_MOON_DESTROY_TEXTURE(vulkan)
{
    (void)device;
    (void)texture;
}

FN_MOON_DESTROY_TEXTURE_VIEW(vulkan)
{
    (void)device;
    (void)texture_view;
}

FN_MOON_DESTROY_SAMPLER(vulkan)
{
    (void)device;
    (void)sampler;
}

FN_MOON_DESTROY_TLAS(vulkan)
{
    (void)device;
    (void)tlas;
}

FN_MOON_DESTROY_BLAS(vulkan)
{
    (void)device;
    (void)blas;
}
#endif /* MOON_VULKAN */
