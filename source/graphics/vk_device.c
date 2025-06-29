#include "vk_moon.h"
#ifdef MOON_VULKAN

#include <stdio.h> /* snprintf */

FN_MOON_DEVICE_ASSEMBLY(vulkan)
{
    lake_result result = LAKE_SUCCESS;

    s32 pd_idx = assembly->device_idx;
    if (pd_idx >= moon->physical_devices.da.size)
        return LAKE_ERROR_INVALID_DEVICE_INDEX;

    struct physical_device const *pd = lake_darray_elem_v(moon->physical_devices, pd_idx);
    moon_device_details const *details = &pd->details; 

    /* check features */
    if (details->missing_required_features != moon_missing_required_feature_none)
        return LAKE_ERROR_DEVICE_NOT_SUPPORTED;
    if ((assembly->explicit_features & details->explicit_features) != assembly->explicit_features)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;

    /* check limits */
    if (assembly->max_allowed_buffers > details->limits.max_descriptor_set_storage_buffers || assembly->max_allowed_buffers == 0)
        return LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_BUFFER_COUNT;
    usize const max_device_supported_images_in_set = lake_min(details->limits.max_descriptor_set_sampled_textures, details->limits.max_descriptor_set_storage_textures);
    if (assembly->max_allowed_textures > max_device_supported_images_in_set || assembly->max_allowed_buffers == 0)
        return LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_TEXTURE_COUNT;
    if (assembly->max_allowed_samplers > details->limits.max_descriptor_set_samplers || assembly->max_allowed_samplers == 0)
        return LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_SAMPLER_COUNT;
    /* acceleration structures only matter if the device supports ray tracing */
    if (((details->implicit_features & moon_implicit_feature_basic_ray_tracing) != 0) && 
        (assembly->max_allowed_acceleration_structures 
            > details->acceleration_structure_details.max_descriptor_set_update_after_bind_acceleration_structures 
        || assembly->max_allowed_acceleration_structures == 0)) 
    {
        return LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_ACCELERATION_STRUCTURE_COUNT;
    }

    struct moon_device_impl *device = __lake_malloc_t(struct moon_device_impl);
    lake_zerop(device);

    device->header.zero_refcnt = (PFN_lake_work)_moon_vulkan_device_zero_refcnt;
    device->header.assembly = *assembly;
    device->header.details = details;
    device->header.moon.impl = moon;
    device->vk_allocator = &moon->vk_allocator;
    device->physical_device = pd;

    /* create the vulkan device */
    result = vk_result_translate(create_vk_device_from_physical_device(device, pd, assembly->explicit_features));
    if (result != LAKE_SUCCESS) {
        lake_error("Device assembly `%s` creating a VkDevice failed.", details->device_name);
        __lake_free(device);
        return result;
    }
    lake_dbg_assert(device->vk_device != nullptr, LAKE_ERROR_DEVICE_LOST, nullptr);

    device->queues[MOON_QUEUE_MAIN_BEGIN_INDEX] = (struct queue_impl){ .queue_type = moon_queue_type_main, .queue_idx = 0 };
    for (u32 i = 0, j = MOON_QUEUE_COMPUTE_BEGIN_INDEX; i < MOON_MAX_COMPUTE_QUEUE_COUNT; i++, j++)
        device->queues[j] = (struct queue_impl){ .queue_type = moon_queue_type_compute, .queue_idx = i };
    for (u32 i = 0, j = MOON_QUEUE_TRANSFER_BEGIN_INDEX; i < MOON_MAX_TRANSFER_QUEUE_COUNT; i++, j++)
        device->queues[j] = (struct queue_impl){ .queue_type = moon_queue_type_transfer, .queue_idx = i };
    for (u32 i = 0, j = MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX; i < MOON_MAX_SPARSE_BINDING_QUEUE_COUNT; i++, j++)
        device->queues[j] = (struct queue_impl){ .queue_type = moon_queue_type_sparse_binding, .queue_idx = i };
    for (u32 i = 0, j = MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX; i < MOON_MAX_VIDEO_DECODE_QUEUE_COUNT; i++, j++)
        device->queues[j] = (struct queue_impl){ .queue_type = moon_queue_type_video_decode, .queue_idx = i };
    for (u32 i = 0, j = MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX; i < MOON_MAX_VIDEO_ENCODE_QUEUE_COUNT; i++, j++)
        device->queues[j] = (struct queue_impl){ .queue_type = moon_queue_type_video_encode, .queue_idx = i };

    /* fill the queue infos */
    for (u32 i = 0; i < MOON_QUEUE_INDEX_COUNT; i++) {
        struct queue_impl *queue = &device->queues[i];

        if (queue->queue_idx >= device->physical_device->queue_families[queue->queue_type].queue_count)
            continue;

        s32 const vk_queue_family = device->physical_device->queue_families[queue->queue_type].vk_index;
        queue->vk_queue_family_idx = vk_queue_family;

        VkSemaphoreTypeCreateInfo timeline_sem_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .pNext = nullptr,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = 0,
        };
        VkSemaphoreCreateInfo const vk_sem_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = (void *)&timeline_sem_create_info,
            .flags = 0,
        };

        device->vkGetDeviceQueue(device->vk_device, vk_queue_family, queue->queue_idx, &queue->vk_queue);
        if (queue->vk_queue == VK_NULL_HANDLE) {
            lake_error("Device assembly `%s` could not query device queue: %d family, %d idx.", pd->details.device_name, queue->vk_queue_family_idx, queue->queue_idx);
            device->vkDestroyDevice(device->vk_device, device->vk_allocator);
            __lake_free(device);
            return LAKE_ERROR_COULD_NOT_QUERY_DEVICE_QUEUE;
        }
        VERIFY_VK_ERROR(device->vkCreateSemaphore(device->vk_device, &vk_sem_create_info, device->vk_allocator, &queue->gpu_local_timeline));
    }

    VkCommandPool init_cmd_pool = VK_NULL_HANDLE;
    VkCommandBuffer init_cmd_buffer = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo const vk_cmd_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = get_device_queue_impl(device, MOON_QUEUE_MAIN)->vk_queue_family_idx,
    };
    VERIFY_VK_ERROR(device->vkCreateCommandPool(device->vk_device, &vk_cmd_pool_create_info, device->vk_allocator, &init_cmd_pool));

    VkCommandBufferAllocateInfo const vk_cmd_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = init_cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VERIFY_VK_ERROR(device->vkAllocateCommandBuffers(device->vk_device, &vk_cmd_buffer_allocate_info, &init_cmd_buffer));

    VkCommandBufferBeginInfo const vk_cmd_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = 0,
    };
    VERIFY_VK_ERROR(device->vkBeginCommandBuffer(init_cmd_buffer, &vk_cmd_buffer_begin_info));

    /* create the GPU allocator using VMA */
    VmaVulkanFunctions const vma_vulkan_functions = {
        .vkGetInstanceProcAddr = moon->vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = moon->vkGetDeviceProcAddr,
        .vkGetPhysicalDeviceProperties = moon->vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = moon->vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory = device->vkAllocateMemory,
        .vkFreeMemory = device->vkFreeMemory,
        .vkMapMemory = device->vkMapMemory,
        .vkUnmapMemory = device->vkUnmapMemory,
        .vkFlushMappedMemoryRanges = device->vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges = device->vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = device->vkBindBufferMemory,
        .vkBindImageMemory = device->vkBindImageMemory,
        .vkGetBufferMemoryRequirements = device->vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements = device->vkGetImageMemoryRequirements,
        .vkCreateBuffer = device->vkCreateBuffer,
        .vkDestroyBuffer = device->vkDestroyBuffer,
        .vkCreateImage = device->vkCreateImage,
        .vkDestroyImage = device->vkDestroyImage,
        .vkCmdCopyBuffer = device->vkCmdCopyBuffer,
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
        .vkGetBufferMemoryRequirements2KHR = device->vkGetBufferMemoryRequirements2,
        .vkGetImageMemoryRequirements2KHR = device->vkGetImageMemoryRequirements2,
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
        .vkBindBufferMemory2KHR = device->vkBindBufferMemory2,
        .vkBindImageMemory2KHR = device->vkBindImageMemory2,
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
        .vkGetPhysicalDeviceMemoryProperties2KHR = moon->vkGetPhysicalDeviceMemoryProperties2,
#endif
#if VMA_KHR_MAINTENANCE4 || VMA_VULKAN_VERSION >= 1003000
        .vkGetDeviceBufferMemoryRequirements = device->vkGetDeviceBufferMemoryRequirements,
        .vkGetDeviceImageMemoryRequirements = device->vkGetDeviceImageMemoryRequirements,
#endif
        .vkGetMemoryWin32HandleKHR = nullptr,
    };

    VmaAllocatorCreateInfo const vma_allocator_create_info = {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = pd->vk_physical_device,
        .device = device->vk_device,
        .preferredLargeHeapBlockSize = 0, /* sets to VMA internal default (256 MB). */
        .pAllocationCallbacks = device->vk_allocator,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = &vma_vulkan_functions,
        .instance = moon->vk_instance,
        .vulkanApiVersion = pd->details.api_version,
        .pTypeExternalMemoryHandleTypes = 0,
    };
    result = vk_result_translate(vmaCreateAllocator(&vma_allocator_create_info, &device->vma_allocator));
    if (result != LAKE_SUCCESS) {
        lake_error("Device assembly `%s` could not create a VMA allocator.", pd->details.device_name);
        device->vkDestroyCommandPool(device->vk_device, init_cmd_pool, device->vk_allocator);
        device->vkDestroyDevice(device->vk_device, device->vk_allocator);
        __lake_free(device);
        return result;
    }

    result = LAKE_SUCCESS;
    if (false) {
deferred_null_cleanup:
        lake_error("Device assembly `%s` failed to prepare GPU resource allocations.", pd->details.device_name);
        if (device->device_address_buffer)
            vmaDestroyBuffer(device->vma_allocator, device->device_address_buffer, device->device_address_buffer_allocation);
        if (device->vk_null_sampler)
            device->vkDestroySampler(device->vk_device, device->vk_null_sampler, device->vk_allocator);
        if (device->vk_null_image_view)
            device->vkDestroyImageView(device->vk_device, device->vk_null_image_view, device->vk_allocator);
        if (device->vk_null_image)
            vmaDestroyImage(device->vma_allocator, device->vk_null_image, device->vk_null_image_allocation);
        if (device->vk_null_buffer)
            vmaDestroyBuffer(device->vma_allocator, device->vk_null_buffer, device->vk_null_buffer_allocation);
        vmaDestroyAllocator(device->vma_allocator);
        device->vkDestroyCommandPool(device->vk_device, init_cmd_pool, device->vk_allocator);
        device->vkDestroyDevice(device->vk_device, device->vk_allocator);
        __lake_free(device);
        return result;

    } else { /* create null resources */
        u8 const buffer_data[4] = { 0xff, 0x00, 0xff, 0xff };
        VmaAllocationInfo vma_allocation_info = {0};

        VkBufferCreateInfo const null_buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = sizeof(u8) * 4,
            .usage = get_buffer_usage_flags(device),
            .sharingMode = VK_SHARING_MODE_CONCURRENT,
            .queueFamilyIndexCount = pd->unique_queue_family_count,
            .pQueueFamilyIndices = pd->unique_queue_family_indices,
        };
        VmaAllocationCreateFlags vma_allocation_flags = 
            VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT |
            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        VmaAllocationCreateInfo const null_buffer_allocation_create_info = {
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
            vmaCreateBuffer(device->vma_allocator, 
                &null_buffer_create_info, 
                &null_buffer_allocation_create_info,
                &device->vk_null_buffer,
                &device->vk_null_buffer_allocation,
                &vma_allocation_info));
        if (result != LAKE_SUCCESS) 
            goto deferred_null_cleanup;
#ifndef LAKE_NDEBUG
        if (moon->vk_debug_messenger != VK_NULL_HANDLE) {
            VkDebugUtilsObjectNameInfoEXT const debug_buffer_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = VK_OBJECT_TYPE_BUFFER,
                .objectHandle = (u64)(uptr)device->vk_null_buffer,
                .pObjectName = "moon null_buffer",
            };
            device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_buffer_name_info);
        }
#endif /* LAKE_NDEBUG */
        lake_dbg_assert(vma_allocation_info.pMappedData != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
        u8 *mapped = (u8 *)vma_allocation_info.pMappedData;
        for (u32 i = 0; i < lake_arraysize(buffer_data); i++)
            mapped[i] = buffer_data[i];

        moon_texture_assembly const texture_assembly = {
            .dimensions = 2,
            .format = moon_format_r8g8b8a8_unorm,
            .extent = { 1, 1, 1 },
            .mip_level_count = 1,
            .array_layer_count = 1,
            .sample_count = moon_sample_count_1,
            .usage = moon_texture_usage_shader_sampled | moon_texture_usage_shader_storage | moon_texture_usage_transfer_dst,
            .sharing_mode = moon_sharing_mode_concurrent,
            .memory_flags = moon_memory_flag_dedicated_memory,
        };
        VmaAllocationCreateInfo const null_image_allocation_create_info = {
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = UINT32_MAX,
            .pool = nullptr,
            .pUserData = nullptr,
            .priority = 0.5f,
        };
        VkImageCreateInfo vk_image_create_info;
        populate_vk_image_create_info_from_assembly(device, &texture_assembly, &vk_image_create_info);

        result = vk_result_translate(
            vmaCreateImage(device->vma_allocator,
                &vk_image_create_info,
                &null_image_allocation_create_info,
                &device->vk_null_image,
                &device->vk_null_image_allocation,
                nullptr));
        if (result != LAKE_SUCCESS) 
            goto deferred_null_cleanup;
#ifndef LAKE_NDEBUG
        if (moon->vk_debug_messenger != VK_NULL_HANDLE) {
            VkDebugUtilsObjectNameInfoEXT const debug_image_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = VK_OBJECT_TYPE_IMAGE,
                .objectHandle = (u64)(uptr)device->vk_null_image,
                .pObjectName = "moon null_texture",
            };
            device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_image_name_info);
        }
#endif /* LAKE_NDEBUG */
        VkImageViewCreateInfo const vk_image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = device->vk_null_image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = vk_image_create_info.format,
            .components = (VkComponentMapping){
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = vk_image_create_info.mipLevels,
                .baseArrayLayer = 0,
                .layerCount = vk_image_create_info.arrayLayers,
            },
        };

        result = vk_result_translate(
            device->vkCreateImageView(
                device->vk_device, 
                &vk_image_view_create_info,
                device->vk_allocator,
                &device->vk_null_image_view));
        if (result != LAKE_SUCCESS) 
            goto deferred_null_cleanup;
#ifndef LAKE_NDEBUG
        if (moon->vk_debug_messenger != VK_NULL_HANDLE) {
            VkDebugUtilsObjectNameInfoEXT const debug_image_view_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = VK_OBJECT_TYPE_IMAGE_VIEW,
                .objectHandle = (u64)(uptr)device->vk_null_image_view,
                .pObjectName = "moon null_texture_view",
            };
            device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_image_view_name_info);
        }
#endif /* LAKE_NDEBUG */

        VkImageMemoryBarrier vk_image_mem_barrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .oldLayout = 0,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = device->vk_null_image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        device->vkCmdPipelineBarrier(
            init_cmd_buffer, VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &vk_image_mem_barrier);

        VkBufferImageCopy const vk_buffer_image_copy = {
            .bufferOffset = 0u,
            .bufferRowLength = 0u,
            .bufferImageHeight = 0u,
            .imageSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .imageOffset = { 0, 0, 0 },
            .imageExtent = { 1, 1, 1 },
        };
        device->vkCmdCopyBufferToImage(
            init_cmd_buffer, device->vk_null_buffer, device->vk_null_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_buffer_image_copy);
        vk_image_mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vk_image_mem_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        vk_image_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        vk_image_mem_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        vk_image_mem_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_image_mem_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        device->vkCmdPipelineBarrier(
            init_cmd_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &vk_image_mem_barrier);

        VkSamplerCreateInfo const vk_sampler_create_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 0,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0,
            .maxLod = 0,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };

        result = vk_result_translate(device->vkCreateSampler(device->vk_device, &vk_sampler_create_info, device->vk_allocator, &device->vk_null_sampler));
        if (result != LAKE_SUCCESS) 
            goto deferred_null_cleanup;
#ifndef LAKE_NDEBUG
        if (moon->vk_debug_messenger != VK_NULL_HANDLE) {
            VkDebugUtilsObjectNameInfoEXT const debug_sampler_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = VK_OBJECT_TYPE_SAMPLER,
                .objectHandle = (u64)(uptr)device->vk_null_sampler,
                .pObjectName = "moon null_sampler",
            };
            device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_sampler_name_info);
        }
#endif /* LAKE_NDEBUG */
        VkBufferUsageFlags const da_buffer_usage_flags = 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        VkBufferCreateInfo const da_buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = device->header.assembly.max_allowed_buffers * sizeof(u64),
            .usage = da_buffer_usage_flags,
            .sharingMode = VK_SHARING_MODE_CONCURRENT, /* TODO buffers are shared across all queues for now */
            .queueFamilyIndexCount = pd->unique_queue_family_count, /* TODO buffers are shared across all queues for now */
            .pQueueFamilyIndices = pd->unique_queue_family_indices,
        };
        VmaAllocationCreateInfo const da_allocation_create_info = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = UINT32_MAX,
            .pool = nullptr,
            .pUserData = nullptr,
            .priority = 0.5f,
        };

        result = vk_result_translate(
            vmaCreateBuffer(device->vma_allocator,
                &da_buffer_create_info,
                &da_allocation_create_info,
                &device->device_address_buffer,
                &device->device_address_buffer_allocation,
                nullptr));
        if (result != LAKE_SUCCESS) 
            goto deferred_null_cleanup;

        result = vk_result_translate(
            vmaMapMemory(device->vma_allocator,
                device->device_address_buffer_allocation,
                (void **)&device->device_address_buffer_host));
        if (result != LAKE_SUCCESS) 
            goto deferred_null_cleanup;
    }
    lake_san_assert(result == LAKE_SUCCESS, LAKE_PANIC, nullptr);

    /* set debug names */
#ifndef LAKE_NDEBUG
    if (moon->vk_debug_messenger != VK_NULL_HANDLE && device->header.assembly.name.len > 0) {
        char const *debug_name = device->header.assembly.name.str;
        VkDebugUtilsObjectNameInfoEXT const debug_device_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_DEVICE,
            .objectHandle = (u64)(uptr)device->vk_device,
            .pObjectName = debug_name,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_device_name_info);

        VkDebugUtilsObjectNameInfoEXT const debug_device_address_buffer_name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_BUFFER,
            .objectHandle = (u64)(uptr)device->device_address_buffer,
            .pObjectName = debug_name,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_device_address_buffer_name_info);

        for (u32 i = 0; i < MOON_QUEUE_INDEX_COUNT; i++) {
            struct queue_impl *queue = &device->queues[i];

            if (queue->vk_queue == VK_NULL_HANDLE)
                continue;

            lake_small_string debug_queue_name = { "[MOON DEVICE] Queue ", sizeof("[MOON DEVICE] Queue ")};
            debug_queue_name.len += snprintf(debug_queue_name.str + debug_queue_name.len, 
                LAKE_SMALL_STRING_CAPACITY - debug_queue_name.len, "%s", moon_queue_type_to_string(queue->queue_type));
            debug_queue_name.str[debug_queue_name.len] = '\0';

            VkDebugUtilsObjectNameInfoEXT const debug_queue_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = VK_OBJECT_TYPE_QUEUE,
                .objectHandle = (u64)(uptr)queue->vk_queue,
                .pObjectName = debug_queue_name.str,
            };
            device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_queue_name_info);

            lake_small_string debug_timeline_name = { "[MOON DEVICE] Queue timeline semaphore ", sizeof("[MOON DEVICE] Queue timeline semaphore ")};
            debug_timeline_name.len += snprintf(debug_timeline_name.str + debug_timeline_name.len, 
                LAKE_SMALL_STRING_CAPACITY - debug_timeline_name.len, "%s", moon_queue_type_to_string(queue->queue_type));
            debug_timeline_name.str[debug_timeline_name.len] = '\0';

            VkDebugUtilsObjectNameInfoEXT const debug_timeline_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = VK_OBJECT_TYPE_SEMAPHORE,
                .objectHandle = (u64)(uptr)queue->gpu_local_timeline,
                .pObjectName = debug_timeline_name.str,
            };
            device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &debug_timeline_name_info);
        }
    }
#endif /* LAKE_NDEBUG */
    result = init_gpu_sr_table(device);
    if (result != LAKE_SUCCESS)
        goto deferred_null_cleanup;

    /* submit initial commands to set up the moon device */
    VERIFY_VK_ERROR(device->vkEndCommandBuffer(init_cmd_buffer));
    VkPipelineStageFlags wait_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkSubmitInfo init_submit = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = &wait_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &init_cmd_buffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr,
    };
    VERIFY_VK_ERROR(device->vkQueueSubmit(get_device_queue_impl(device, MOON_QUEUE_MAIN)->vk_queue, 1, &init_submit, nullptr));

    /* wait for commands from the init list to complete, and destroy the command pool */
    VERIFY_VK_ERROR(device->vkDeviceWaitIdle(device->vk_device));
    device->vkDestroyCommandPool(device->vk_device, init_cmd_pool, device->vk_allocator);

    lake_trace("Created Moon device `%s` from %s.", device->header.assembly.name.str, device->header.details->device_name);
    lake_inc_refcnt(&moon->interface.header.refcnt);
    lake_inc_refcnt(&device->header.refcnt);
    *out_device = device;
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(device != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&device->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Device `%s` reference count is %d.", device->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_impl *moon = device->header.moon.impl;

    lake_result result = _moon_vulkan_device_wait_idle(device);
    lake_dbg_assert(result == LAKE_SUCCESS, result, "Failed to wait idle.");
    result = _moon_vulkan_device_commit_deferred_destructors(device);
    lake_dbg_assert(result == LAKE_SUCCESS, result, "Failed to call deferred GPU resources destructors.");

    for (s32 i = 0; i < moon_queue_type_count; i++) {
        struct command_pool_arena *arena = &device->command_pool_arenas[i];
        if (arena != nullptr) {
            lake_darray_foreach_v(arena->pools_and_buffers, VkCommandPool, vk_cmd_pool)
                device->vkDestroyCommandPool(device->vk_device, *vk_cmd_pool, device->vk_allocator);
            lake_darray_fini(&arena->pools_and_buffers.da);
        }
    }
    vmaUnmapMemory(device->vma_allocator, device->device_address_buffer_allocation);
    vmaDestroyBuffer(device->vma_allocator, device->device_address_buffer, device->device_address_buffer_allocation);
    fini_gpu_sr_table(device);

    vmaDestroyImage(device->vma_allocator, device->vk_null_image, device->vk_null_image_allocation);
    vmaDestroyBuffer(device->vma_allocator, device->vk_null_buffer, device->vk_null_buffer_allocation);
    vmaDestroyAllocator(device->vma_allocator);
    device->vkDestroySampler(device->vk_device, device->vk_null_sampler, device->vk_allocator);
    device->vkDestroyImageView(device->vk_device, device->vk_null_image_view, device->vk_allocator);

    for (u32 i = 0; i < MOON_QUEUE_INDEX_COUNT; i++)
        if (device->queues[i].gpu_local_timeline != VK_NULL_HANDLE)
            device->vkDestroySemaphore(device->vk_device, device->queues[i].gpu_local_timeline, device->vk_allocator);

    device->vkDestroyDevice(device->vk_device, device->vk_allocator);
    lake_trace("Destroyed Moon device `%s`.", device->header.assembly.name.str);

    lake_dec_refcnt(&moon->interface.header.refcnt, moon, moon->interface.header.zero_refcnt);
    __lake_free(device);
}

FN_MOON_DEVICE_QUEUE_COUNT(vulkan)
{
    lake_dbg_assert(out_queue_count != nullptr, LAKE_INVALID_PARAMETERS, nullptr);
    if (queue_type >= moon_queue_type_count)
        return LAKE_ERROR_INVALID_QUEUE;
    *out_queue_count = device->physical_device->queue_families[queue_type].queue_count;
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_QUEUE_WAIT_IDLE(vulkan)
{
    if (!is_device_queue_valid(device, queue))
        return LAKE_ERROR_INVALID_QUEUE;
    return vk_result_translate(device->vkQueueWaitIdle(get_device_queue_impl(device, queue)->vk_queue));
}

FN_MOON_DEVICE_WAIT_IDLE(vulkan)
{
    return vk_result_translate(device->vkDeviceWaitIdle(device->vk_device));
}

LAKE_NONNULL_ALL
static lake_result LAKECALL get_oldest_pending_submit(struct moon_device_impl const *device, struct queue_impl *queue, u64 *out_pending_submit)
{
    *out_pending_submit = UINT64_MAX;
    if (queue->gpu_local_timeline != VK_NULL_HANDLE) {
        u64 latest_gpu = 0;
        VkResult vk_result = device->vkGetSemaphoreCounterValue(device->vk_device, queue->gpu_local_timeline, &latest_gpu);
        if (vk_result != VK_SUCCESS)
            return vk_result_translate(vk_result);

        u64 latest_cpu = lake_atomic_read_explicit(&queue->latest_pending_submit_timeline_value, lake_memory_model_acquire);
        /* CPU ahead of GPU ? */
        if (latest_cpu > latest_gpu)
            *out_pending_submit = latest_gpu;
    }
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_SUBMIT_COMMANDS(vulkan)
{
    if (!is_device_queue_valid(device, submit->queue))
        return LAKE_ERROR_INVALID_QUEUE;
    if (submit->queue.idx >= device->physical_device->queue_families[submit->queue.type].queue_count)
        return LAKE_ERROR_INVALID_QUEUE;

    lake_spinlock *lifetime_lock = &device->gpu_sr_table.lifetime_lock;
    /* TODO implement a semaphore instead of a lifetime spinlock, to allow non-blocking read-only access 
     * during parallel submittions, but protect them in case of pending writes. */
    lake_spinlock_acquire(lifetime_lock);

    for (u32 i = 0; i < submit->staged_command_list_count; i++) {
        struct moon_staged_command_list_impl const *cmd_list = submit->staged_command_lists[i];

        if (cmd_list->header.cmd.header->assembly.queue_type != submit->queue.type)
            return LAKE_ERROR_QUEUE_SCHEDULING_TYPE_MISMATCH;

        lake_darray_foreach_v(cmd_list->data.used_buffers, moon_buffer_id, id)
            if (!_moon_vulkan_is_buffer_valid(device, *id)) 
                return LAKE_ERROR_INVALID_BUFFER_ID;

        lake_darray_foreach_v(cmd_list->data.used_textures, moon_texture_id, id)
            if (!_moon_vulkan_is_texture_valid(device, *id))
                return LAKE_ERROR_INVALID_TEXTURE_ID;

        lake_darray_foreach_v(cmd_list->data.used_texture_views, moon_texture_view_id, id)
            if (!_moon_vulkan_is_texture_view_valid(device, *id))
                return LAKE_ERROR_INVALID_TEXTURE_VIEW_ID;

        lake_darray_foreach_v(cmd_list->data.used_samplers, moon_sampler_id, id)
            if (!_moon_vulkan_is_sampler_valid(device, *id))
                return LAKE_ERROR_INVALID_SAMPLER_ID;
    }
    struct queue_impl *queue = get_device_queue_impl(device, submit->queue);
    u64 const current_timeline_value = lake_atomic_add_explicit(&device->submit_timeline, 1, lake_memory_model_release) + 1;
    lake_atomic_write_explicit(&queue->latest_pending_submit_timeline_value, current_timeline_value, lake_memory_model_release);
    lake_spinlock_release(lifetime_lock);

    u32 const sem_signal_count = 1 + submit->signal_binary_semaphore_count + submit->signal_timeline_semaphore_count;
    u32 const sem_wait_count = submit->wait_binary_semaphore_count + submit->wait_timeline_semaphore_count;

    usize                   o;
    u8                     *raw;
    VkCommandBuffer        *submit_vk_cmd_buffers;
    VkSemaphore            *submit_vk_sem_signals;
    u64                    *submit_vk_sem_signal_values;
    VkSemaphore            *submit_vk_sem_waits;
    VkPipelineStageFlags   *submit_vk_sem_wait_stage_masks;
    u64                    *submit_vk_sem_wait_values;
    { /* get scratch memory */
        usize const cmd_buffers_bytes = lake_align(sizeof(VkCommandBuffer) * submit->staged_command_list_count, 16);
        usize const sem_signals_bytes = lake_align(sizeof(VkSemaphore) * sem_signal_count, 16);
        usize const sem_signal_values_bytes = lake_align(sizeof(u64) * sem_signal_count, 16);
        usize const sem_waits_bytes = lake_align(sizeof(VkSemaphore) * sem_wait_count, 16);
        usize const sem_wait_stage_masks_bytes = lake_align(sizeof(VkPipelineStageFlags) * sem_wait_count, 16);
        usize const sem_wait_values_bytes = lake_align(sizeof(u64) * sem_wait_count, 16);
        usize const total_bytes = 
            cmd_buffers_bytes +
            sem_signals_bytes +
            sem_signal_values_bytes +
            sem_waits_bytes +
            sem_wait_stage_masks_bytes +
            sem_wait_values_bytes;

        raw = (u8 *)lake_drift_alias(total_bytes, 16);

        o = 0;
        submit_vk_cmd_buffers = (VkCommandBuffer *)&raw[o];
        o += cmd_buffers_bytes;
        submit_vk_sem_signals = (VkSemaphore *)&raw[o];
        o += sem_signals_bytes;
        submit_vk_sem_signal_values = (u64 *)&raw[o];
        o += sem_signal_values_bytes;
        submit_vk_sem_waits = (VkSemaphore *)&raw[o];
        o += sem_waits_bytes;
        submit_vk_sem_wait_stage_masks = (VkPipelineStageFlags *)&raw[o];
        o += sem_wait_stage_masks_bytes;
        submit_vk_sem_wait_values = (u64 *)&raw[o];
        lake_san_assert(o + sem_wait_values_bytes, LAKE_PANIC, nullptr);
    }

    o = 0;
    submit_vk_sem_signals[o] = queue->gpu_local_timeline;
    submit_vk_sem_signal_values[o] = current_timeline_value;
    o++;

    for (u32 i = 0; i < submit->signal_timeline_semaphore_count; i++) {
        moon_timeline_pair const *pair = &submit->signal_timeline_semaphores[i];
        submit_vk_sem_signals[o] = pair->timeline_semaphore.impl->vk_semaphore;
        submit_vk_sem_signal_values[o] = pair->value;
        o++;
    }
    for (u32 i = 0; i < submit->signal_binary_semaphore_count; i++) {
        submit_vk_sem_signals[o] = submit->signal_binary_semaphores[i]->vk_semaphore;
        submit_vk_sem_signal_values[o] = 0; /* the vulkan spec requires dummy values for binary semaphores */
        o++;
    }
    lake_dbg_assert(o == sem_signal_count, LAKE_ERROR_OUT_OF_RANGE, nullptr);

    o = 0;
    for (u32 i = 0; i < submit->wait_timeline_semaphore_count; i++) {
        moon_timeline_pair const *pair = &submit->wait_timeline_semaphores[i];
        submit_vk_sem_waits[o] = pair->timeline_semaphore.impl->vk_semaphore;
        submit_vk_sem_wait_stage_masks[o] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        submit_vk_sem_wait_values[o] = pair->value;
        o++;
    }
    for (u32 i = 0; i < submit->wait_binary_semaphore_count; i++) {
        submit_vk_sem_waits[o] = submit->wait_binary_semaphores[i]->vk_semaphore;
        submit_vk_sem_wait_stage_masks[o] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        submit_vk_sem_wait_values[o] = 0; /* the vulkan spec requires dummy values for binary semaphores */
        o++;
    }
    lake_dbg_assert(o == sem_wait_count, LAKE_ERROR_OUT_OF_RANGE, nullptr);

    VkTimelineSemaphoreSubmitInfo vk_timeline_info = {
        .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreValueCount = sem_wait_count,
        .pWaitSemaphoreValues = submit_vk_sem_wait_values,
        .signalSemaphoreValueCount = sem_signal_count,
        .pSignalSemaphoreValues = submit_vk_sem_signal_values,
    };
    VkSubmitInfo const vk_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = (void *)&vk_timeline_info,
        .waitSemaphoreCount = sem_wait_count,
        .pWaitSemaphores = submit_vk_sem_waits,
        .pWaitDstStageMask = submit_vk_sem_wait_stage_masks,
        .commandBufferCount = submit->staged_command_list_count,
        .pCommandBuffers = submit_vk_cmd_buffers,
        .signalSemaphoreCount = sem_signal_count,
        .pSignalSemaphores = submit_vk_sem_signals,
    };
    lake_result result = vk_result_translate(
        device->vkQueueSubmit(
            queue->vk_queue, 
            1, 
            &vk_submit_info, 
            VK_NULL_HANDLE));
    return result;
}

FN_MOON_DEVICE_PRESENT_FRAMES(vulkan)
{
    if (!is_device_queue_valid(device, present->queue))
        return LAKE_ERROR_INVALID_QUEUE;
    if (present->queue.idx >= device->physical_device->queue_families[present->queue.type].queue_count)
        return LAKE_ERROR_INVALID_QUEUE;

    u8             *raw;
    VkSemaphore    *submit_vk_sem_waits;
    VkSwapchainKHR *vk_swapchains;
    u32            *vk_image_indices;
    { /* get scratch memory */
        usize const submit_vk_sem_waits_bytes = lake_align(sizeof(VkSemaphore) * present->wait_binary_semaphore_count, 16);
        usize const vk_swapchains_bytes = lake_align(sizeof(VkSwapchainKHR) * present->swapchain_count, 16);
        usize const vk_image_indices_bytes = lake_align(sizeof(u32) * present->swapchain_count, 16);
        usize const total_bytes =
            submit_vk_sem_waits_bytes +
            vk_swapchains_bytes +
            vk_image_indices_bytes;

        usize o = 0;
        raw = (u8 *)lake_drift_alias(total_bytes, 16);

        submit_vk_sem_waits = (VkSemaphore *)&raw[vk_swapchains_bytes];
        o += submit_vk_sem_waits_bytes;
        vk_swapchains = (VkSwapchainKHR *)&raw[o];
        o += vk_swapchains_bytes;
        vk_image_indices = (u32 *)&raw[o];
        lake_san_assert(o + vk_image_indices_bytes, LAKE_PANIC, nullptr);
    }
    for (u32 i = 0; i < present->wait_binary_semaphore_count; i++)
        submit_vk_sem_waits[i] = present->wait_binary_semaphores[i]->vk_semaphore;
    for (u32 i = 0; i < present->swapchain_count; i++) {
        struct moon_swapchain_impl const *swapchain = present->swapchains[i];

        lake_dbg_assert(present->queue.type != swapchain->header.assembly.queue_type, LAKE_ERROR_QUEUE_SCHEDULING_TYPE_MISMATCH, 
            "Swapchains queue types must match with the present queue");

        vk_swapchains[i] = swapchain->vk_swapchain;
        vk_image_indices[i] = swapchain->current_image_idx;
    }
    VkPresentInfoKHR const vk_present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr, 
        .waitSemaphoreCount = present->wait_binary_semaphore_count,
        .pWaitSemaphores = submit_vk_sem_waits,
        .swapchainCount = present->swapchain_count,
        .pSwapchains = vk_swapchains,
        .pImageIndices = vk_image_indices,
        .pResults = nullptr,
    };
    lake_result result = vk_result_translate(
        device->vkQueuePresentKHR(
            get_device_queue_impl(device, present->queue)->vk_queue, 
            &vk_present_info));
    return result;
}

FN_MOON_DEVICE_COMMIT_DEFERRED_DESTRUCTORS(vulkan)
{
    lake_spinlock *lifetime_lock = &device->gpu_sr_table.lifetime_lock;
    lake_spinlock *zombie_lock = device->zombies_locks;
    u64 min_pending_timeline_value = UINT64_MAX;

    lake_spinlock_acquire(lifetime_lock);
    for (u32 i = 0; i < MOON_QUEUE_INDEX_COUNT; i++) {
        struct queue_impl *queue = &device->queues[i];

        u64 latest_pending_submit;
        lake_result result = get_oldest_pending_submit(device, queue, &latest_pending_submit);
        if (result != LAKE_SUCCESS) {
            lake_spinlock_release(lifetime_lock);
            return result;
        }
        if (latest_pending_submit != UINT64_MAX)
            min_pending_timeline_value = lake_min(min_pending_timeline_value, latest_pending_submit);
    }
#define COMMIT_DESTRUCTORS(T, ...) \
    zombie_lock = &device->zombies_locks[zombie_timeline_##T##_idx]; \
    lake_spinlock_acquire(zombie_lock); \
    \
    while (!lake_deque_empty_v(device->T##_zombies)) { \
        zombie_timeline_##T zombie = {0}; \
        lake_deque_pop_v(device->T##_zombies, zombie_timeline_##T, &zombie); \
        \
        /* Zombies are sorted. When we see a single zombie that is too young, 
         * we can dismiss the rest as they are the same age or younger. */ \
        if (zombie.first >= min_pending_timeline_value) { \
            lake_deque_op_t(&device->buffer_zombies.deq, zombie_timeline_##T, lake_deque_op_push); \
            break; \
        } \
        __VA_ARGS__ \
    } \
    lake_spinlock_release(zombie_lock);

    COMMIT_DESTRUCTORS(buffer, buffer_destructor(device, zombie.second); );
    COMMIT_DESTRUCTORS(texture, texture_destructor(device, zombie.second); );
    COMMIT_DESTRUCTORS(texture_view, texture_view_destructor(device, zombie.second); );
    COMMIT_DESTRUCTORS(sampler, sampler_destructor(device, zombie.second); );
    COMMIT_DESTRUCTORS(tlas, tlas_destructor(device, zombie.second); );
    COMMIT_DESTRUCTORS(blas, blas_destructor(device, zombie.second); );
    COMMIT_DESTRUCTORS(event, device->vkDestroyEvent(device->vk_device, zombie.second.vk_event, device->vk_allocator); );
    COMMIT_DESTRUCTORS(pipeline, device->vkDestroyPipeline(device->vk_device, zombie.second.vk_pipeline, device->vk_allocator); );
    COMMIT_DESTRUCTORS(semaphore, device->vkDestroySemaphore(device->vk_device, zombie.second.vk_semaphore, device->vk_allocator); );
    COMMIT_DESTRUCTORS(query_pool, device->vkDestroyQueryPool(device->vk_device, zombie.second.vk_query_pool, device->vk_allocator); );
    COMMIT_DESTRUCTORS(memory_heap, vmaFreeMemory(device->vma_allocator, zombie.second.vma_allocation); );
    COMMIT_DESTRUCTORS(command_recorder, 
            struct command_pool_arena *arena = &device->command_pool_arenas[zombie.second.queue_type];
            device->vkFreeCommandBuffers(device->vk_device, zombie.second.vk_cmd_pool,
                (u32)zombie.second.allocated_command_buffers.da.size, 
                zombie.second.allocated_command_buffers.v);
            VERIFY_VK_ERROR(device->vkResetCommandPool(device->vk_device, zombie.second.vk_cmd_pool, 0));
            lake_darray_append_v_locked(arena->pools_and_buffers, VkCommandPool, &zombie.second.vk_cmd_pool, 1, &arena->spinlock);
    );
    lake_spinlock_release(lifetime_lock);
#undef COMMIT_DESTRUCTORS
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_BUFFER_MEMORY_REQUIREMENTS(vulkan)
{
    VkBufferCreateInfo const vk_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = (VkDeviceSize)assembly->size,
        .usage = get_buffer_usage_flags(device),
        .sharingMode = VK_SHARING_MODE_CONCURRENT, /* buffers are always shared */
        .queueFamilyIndexCount = device->physical_device->unique_queue_family_count, /* buffers are always shared across all queues */
        .pQueueFamilyIndices = device->physical_device->unique_queue_family_indices,
    };
    VkDeviceBufferMemoryRequirements vk_device_buffer_memory_requirements = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
        .pNext = nullptr,
        .pCreateInfo = &vk_buffer_create_info,
    };
    VkMemoryRequirements2 vk_mem_requirements2 = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
        .pNext = nullptr,
        .memoryRequirements = {0},
    };
    device->vkGetDeviceBufferMemoryRequirements(device->vk_device, &vk_device_buffer_memory_requirements, &vk_mem_requirements2);
    *out_requirements = (moon_memory_requirements){
        .alignment = vk_mem_requirements2.memoryRequirements.alignment,
        .size = vk_mem_requirements2.memoryRequirements.size,
        .type_bitmask = vk_mem_requirements2.memoryRequirements.memoryTypeBits,
    };
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_TEXTURE_MEMORY_REQUIREMENTS(vulkan)
{
    VkImageCreateInfo vk_image_create_info = {0};
    populate_vk_image_create_info_from_assembly(device, assembly, &vk_image_create_info);

    VkDeviceImageMemoryRequirements vk_device_image_memory_requirements = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS, 
        .pNext = nullptr,
        .pCreateInfo = &vk_image_create_info,
        .planeAspect = infer_aspect_from_format(assembly->format)
    };
    VkMemoryRequirements2 vk_mem_requirements2 = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
        .pNext = nullptr,
        .memoryRequirements = {0},
    };
    device->vkGetDeviceImageMemoryRequirements(device->vk_device, &vk_device_image_memory_requirements, &vk_mem_requirements2);
    *out_requirements = (moon_memory_requirements){
        .alignment = vk_mem_requirements2.memoryRequirements.alignment,
        .size = vk_mem_requirements2.memoryRequirements.size,
        .type_bitmask = vk_mem_requirements2.memoryRequirements.memoryTypeBits,
    };
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_TLAS_BUILD_SIZES(vulkan)
{
    if (!(device->header.details->implicit_features & moon_implicit_feature_basic_ray_tracing))
        return LAKE_ERROR_RAY_TRACING_REQUIRED;

    struct acceleratrion_structure_build build = {0};
    populate_vk_acceleration_structure_build_details(device, 1, details, 0, nullptr, &build);

    VkAccelerationStructureBuildSizesInfoKHR vk_acceleration_structure_build_sizes_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
        .pNext = nullptr,
    };
    device->vkGetAccelerationStructureBuildSizesKHR(
        device->vk_device, 
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        build.vk_build_geometry_infos.v,
        build.primitive_counts.v,
        &vk_acceleration_structure_build_sizes_info);

    out_sizes->acceleration_structure_size = vk_acceleration_structure_build_sizes_info.accelerationStructureSize;
    out_sizes->build_scratch_size = vk_acceleration_structure_build_sizes_info.buildScratchSize;
    out_sizes->update_scratch_size = vk_acceleration_structure_build_sizes_info.updateScratchSize;
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_BLAS_BUILD_SIZES(vulkan)
{
    if (!(device->header.details->implicit_features & moon_implicit_feature_basic_ray_tracing))
        return LAKE_ERROR_RAY_TRACING_REQUIRED;

    struct acceleratrion_structure_build build = {0};
    populate_vk_acceleration_structure_build_details(device, 0, nullptr, 1, details, &build);

    VkAccelerationStructureBuildSizesInfoKHR vk_acceleration_structure_build_sizes_info = {
        .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
        .pNext = nullptr,
    };
    device->vkGetAccelerationStructureBuildSizesKHR(
        device->vk_device, 
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        build.vk_build_geometry_infos.v,
        build.primitive_counts.v,
        &vk_acceleration_structure_build_sizes_info);

    out_sizes->acceleration_structure_size = vk_acceleration_structure_build_sizes_info.accelerationStructureSize;
    out_sizes->build_scratch_size = vk_acceleration_structure_build_sizes_info.buildScratchSize;
    out_sizes->update_scratch_size = vk_acceleration_structure_build_sizes_info.updateScratchSize;
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_MEMORY_REPORT(vulkan)
{
    if (report == nullptr)
        return LAKE_ERROR_MEMORY_MAP_FAILED;

    lake_result result = LAKE_SUCCESS;
    u32 const buffer_list_alloc_size = report->buffer_count;
    u32 const texture_list_alloc_size = report->texture_count;
    u32 const tlas_list_alloc_size = report->tlas_count;
    u32 const blas_list_alloc_size = report->blas_count;
    u32 const heap_list_alloc_size = report->heap_count;

    report->buffer_count = 0;
    report->texture_count = 0;
    report->tlas_count = 0;
    report->blas_count = 0;
    report->heap_count = 0;
    report->total_buffer_memory_use = 0;
    report->total_texture_memory_use = 0;
    report->total_aliased_tlas_memory_use = 0;
    report->total_aliased_blas_memory_use = 0;
    report->total_heap_memory_use = 0;

    for (s32 i = 0; i < device->gpu_sr_table.buffer_slots.next_idx; i++) {
        struct buffer_gpu_sr_pool *pool = &device->gpu_sr_table.buffer_slots;
        u64 version = buffer_gpu_sr_pool__version_of_slot(pool, i);

        if ((version & GPU_SR_POOL_VERSION_ZOMBIE_BIT) == 0) {
            moon_buffer_id id = moon_id_make(moon_buffer_id, i, version);
            struct buffer_impl_slot *slot = buffer_gpu_sr_pool__unsafe_get(pool, id);
            if (slot->vk_buffer == VK_NULL_HANDLE)
                continue;

            u32 out_idx = report->buffer_count++;
            bool const aliased = slot->vma_allocation == VK_NULL_HANDLE;
            u64 memory_size = 0llu;

            if (!aliased) {
                VmaAllocationInfo vma_alloc_info = {0};
                vmaGetAllocationInfo(device->vma_allocator, slot->vma_allocation, &vma_alloc_info);
                memory_size = vma_alloc_info.size;
                report->total_buffer_memory_use += memory_size;
            } else {
                moon_memory_requirements requirements = {0};
                result = _moon_vulkan_device_buffer_memory_requirements(device, &slot->assembly, &requirements);
                if (result != LAKE_SUCCESS)
                    return result;
                memory_size = requirements.size;
            }
            if (slot->optional_heap != nullptr) {
                /* TODO memory heaps */
            }
            if (report->buffer_list != nullptr && out_idx < buffer_list_alloc_size) {
                report->buffer_list[out_idx] = (moon_memory_buffer_size_pair){
                    .buffer = id, 
                    .size = memory_size, 
                    .heap_allocated = aliased,
                };
            }
        }
    }
    for (s32 i = 0; i < device->gpu_sr_table.texture_slots.next_idx; i++) {
        struct texture_gpu_sr_pool *pool = &device->gpu_sr_table.texture_slots;
        u64 version = texture_gpu_sr_pool__version_of_slot(pool, i);
        
        if ((version & GPU_SR_POOL_VERSION_ZOMBIE_BIT) == 0) {
            moon_texture_id id = moon_id_make(moon_texture_id, i, version);
            struct texture_impl_slot *slot = texture_gpu_sr_pool__unsafe_get(pool, id);
            if (slot->vk_image == VK_NULL_HANDLE)
                continue;

            u32 out_idx = report->texture_count++;
            bool const aliased = slot->vma_allocation == VK_NULL_HANDLE;
            u64 memory_size = 0llu;

            if (!aliased) {
                VmaAllocationInfo vma_alloc_info = {0};
                vmaGetAllocationInfo(device->vma_allocator, slot->vma_allocation, &vma_alloc_info);
                memory_size = vma_alloc_info.size;
                report->total_buffer_memory_use += memory_size;
            } else {
                moon_memory_requirements requirements = {0};
                result = _moon_vulkan_device_texture_memory_requirements(device, &slot->assembly, &requirements);
                if (result != LAKE_SUCCESS)
                    return result;
                memory_size = requirements.size;
            }
            if (slot->optional_heap != nullptr) {
                /* TODO memory heaps */
            }
            if (report->texture_list != nullptr && out_idx < texture_list_alloc_size) {
                report->texture_list[out_idx] = (moon_memory_texture_size_pair){
                    .texture = id, 
                    .size = memory_size, 
                    .heap_allocated = aliased,
                };
            }
        }
    }
    for (s32 i = 0; i < device->gpu_sr_table.tlas_slots.next_idx; i++) {
        struct tlas_gpu_sr_pool *pool = &device->gpu_sr_table.tlas_slots;
        u64 version = tlas_gpu_sr_pool__version_of_slot(pool, i);

        if ((version & GPU_SR_POOL_VERSION_ZOMBIE_BIT) == 0) {
            moon_tlas_id id = moon_id_make(moon_tlas_id, i, version);
            struct tlas_impl_slot *slot = tlas_gpu_sr_pool__unsafe_get(pool, id);
            if (slot->vk_acceleration_structure == VK_NULL_HANDLE)
                continue;

            u32 out_idx = report->tlas_count++;
            report->total_aliased_tlas_memory_use += slot->assembly.size;

            if (report->tlas_list != nullptr && out_idx < tlas_list_alloc_size) {
                report->tlas_list[out_idx] = (moon_memory_tlas_size_pair){
                    .tlas = id,
                    .size = slot->assembly.size,
                };
            }
        }
    }
    for (s32 i = 0; i < device->gpu_sr_table.blas_slots.next_idx; i++) {
        struct blas_gpu_sr_pool *pool = &device->gpu_sr_table.blas_slots;
        u64 version = blas_gpu_sr_pool__version_of_slot(pool, i);

        if ((version & GPU_SR_POOL_VERSION_ZOMBIE_BIT) == 0) {
            moon_blas_id id = moon_id_make(moon_blas_id, i, version);
            struct blas_impl_slot *slot = blas_gpu_sr_pool__unsafe_get(pool, id);
            if (slot->vk_acceleration_structure == VK_NULL_HANDLE)
                continue;

            u32 out_idx = report->blas_count++;
            report->total_aliased_blas_memory_use += slot->assembly.size;

            if (report->blas_list != nullptr && out_idx < blas_list_alloc_size) {
                report->blas_list[out_idx] = (moon_memory_blas_size_pair){
                    .blas = id,
                    .size = slot->assembly.size,
                };
            }
        }
    }
    /* TODO memory heaps */
    (void)heap_list_alloc_size;

    report->total_memory_use = 
        report->total_buffer_memory_use +
        report->total_texture_memory_use +
        report->total_heap_memory_use;
    return result;
}

FN_MOON_MEMORY_HEAP_ASSEMBLY(vulkan)
{
    struct moon_memory_heap_impl heap = {
        .header = {
            .device.impl = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_memory_heap_zero_refcnt,
        },
    };
    lake_dbg_assert(heap.header.assembly.requirements.type_bitmask != 0, LAKE_INVALID_PARAMETERS, "Memory heap type bitmask must be non-zero.");

    VmaAllocationCreateInfo const vma_alloc_create_info = {
        .flags = heap.header.assembly.flags,
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = nullptr,
        .pUserData = nullptr,
        .priority = 0.5f,
    };
    VkMemoryRequirements const vk_mem_requirements = {
        .size = heap.header.assembly.requirements.size,
        .alignment = heap.header.assembly.requirements.alignment,
        .memoryTypeBits = heap.header.assembly.requirements.type_bitmask,
    };
    VkResult vk_result = vmaAllocateMemory(device->vma_allocator, &vk_mem_requirements, &vma_alloc_create_info, &heap.vma_allocation, &heap.vma_allocation_info);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&heap.header.refcnt);
    *out_heap = __lake_malloc_t(struct moon_memory_heap_impl);
    lake_memcpy(*out_heap, &heap, sizeof(struct moon_memory_heap_impl));
    return LAKE_SUCCESS;
}

FN_MOON_MEMORY_HEAP_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(heap != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&heap->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Memory heap `%s` reference count is %d.", heap->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_device_impl *device = heap->header.device.impl;

    zombie_timeline_memory_heap submit = {
        .first = lake_atomic_read(&device->submit_timeline),
        .second = { .vma_allocation = heap->vma_allocation },
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_memory_heap_idx];
    lake_deque_unshift_v_locked(device->memory_heap_zombies, zombie_timeline_memory_heap, submit, lock);

    moon_device_unref(heap->header.device);
    __lake_free(heap);
}
#endif /* MOON_VULKAN */
