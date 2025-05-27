#include "vk_moon.h"
#ifdef MOON_VULKAN

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
