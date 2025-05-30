#include "vk_moon.h"
#ifdef MOON_VULKAN

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

FN_MOON_CREATE_BUFFER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_buffer;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_BUFFER_FROM_HEAP(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_buffer;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_TEXTURE(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_texture;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_TEXTURE_FROM_HEAP(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_texture;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
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
    (void)device;
    (void)assembly;
    (void)out_tlas;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_TLAS_FROM_BUFFER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_tlas;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_BLAS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_blas;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CREATE_BLAS_FROM_BUFFER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_blas;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
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
    (void)device;
    (void)buffer;
    (void)out_host_address;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_BUFFER_DEVICE_ADDRESS(vulkan)
{
    (void)device;
    (void)buffer;
    (void)out_device_address;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_TLAS_DEVICE_ADDRESS(vulkan)
{
    (void)device;
    (void)tlas;
    (void)out_device_address;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_BLAS_DEVICE_ADDRESS(vulkan)
{
    (void)device;
    (void)blas;
    (void)out_device_address;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
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
