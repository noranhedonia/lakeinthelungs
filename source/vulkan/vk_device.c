#include "vk_moon.h"
#ifdef MOON_VULKAN

#include <stdio.h> /* snprintf */

FN_MOON_DEVICE_ASSEMBLY(vulkan)
{
    lake_result result = LAKE_SUCCESS;

    s32 pd_idx = assembly->device_idx;
    if (pd_idx >= moon->physical_devices.da.size)
        return LAKE_ERROR_INVALID_DEVICE_INDEX;

    struct physical_device const *pd = &moon->physical_devices.v[pd_idx];
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

    moon_device device = __lake_malloc_t(struct moon_device_impl);
    lake_zerop(device);

    device->header.moon.adapter = moon;
    device->header.assembly = *assembly;
    device->header.details = details;
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
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
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
            .usage = create_buffer_usage_flags(device),
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
                .objectHandle = (u64)(uptr)device->vk_null_image_view,
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
            .sharingMode = VK_SHARING_MODE_CONCURRENT, /* buffers are always shared */
            .queueFamilyIndexCount = pd->unique_queue_family_count, /* buffers are always shared across all queues */
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
    /* TODO initialize the GPU resource table */

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
    lake_refcnt_inc(&moon->interface.header.refcnt);
    lake_refcnt_inc(&device->header.refcnt);
    *out_device = device;
    return LAKE_SUCCESS;
}

FN_MOON_DEVICE_DESTRUCTOR(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(device != nullptr, LAKE_INVALID_PARAMETERS, nullptr);
    s32 refcnt = lake_atomic_read(&device->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Device %s reference count is %d.", device->physical_device->details.device_name, refcnt);
#endif /* LAKE_NDEBUG */
    moon_adapter moon = device->header.moon.adapter;

    lake_result result = _moon_vulkan_device_wait_idle(device);
    lake_dbg_assert(result == LAKE_SUCCESS, result, "Failed to wait idle.");
    _moon_vulkan_device_destroy_deferred(device);

    for (s32 i = 0; i < moon_queue_type_count; i++) {
        if (device->command_pool_arenas[i].pools_and_buffers.v != nullptr) {
            for (s32 j = 0; j < device->command_pool_arenas[i].pools_and_buffers.da.size; j++)
                device->vkDestroyCommandPool(device->vk_device, device->command_pool_arenas[i].pools_and_buffers.v[j], device->vk_allocator);
            __lake_free(device->command_pool_arenas[i].pools_and_buffers.da.v);
        }
    }
    vmaUnmapMemory(device->vma_allocator, device->device_address_buffer_allocation);
    vmaDestroyBuffer(device->vma_allocator, device->device_address_buffer, device->device_address_buffer_allocation);
    /* TODO cleanup the GPU resource table */

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

    lake_refcnt_dec(&moon->interface.header.refcnt, moon, moon->interface.header.destructor);
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

FN_MOON_DEVICE_SUBMIT_COMMANDS(vulkan)
{
    (void)device;
    (void)submit;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_PRESENT_FRAMES(vulkan)
{
    (void)device;
    (void)present;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_DESTROY_DEFERRED(vulkan)
{
    (void)device;
}

FN_MOON_DEVICE_HEAP_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_device_heap;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_HEAP_DESTRUCTOR(vulkan)
{
    (void)device_heap;
}

FN_MOON_DEVICE_BUFFER_MEMORY_REQUIREMENTS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_requirements;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_TEXTURE_MEMORY_REQUIREMENTS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_requirements;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_DEVICE_MEMORY_REPORT(vulkan)
{
    (void)device;
    (void)out_report;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}
#endif /* MOON_VULKAN */
