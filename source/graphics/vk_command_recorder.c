#include "vk_moon.h"
#ifdef MOON_VULKAN

#ifndef LAKE_NDEBUG
LAKE_CONST_FN
static lake_result LAKECALL validate_queue_family(
        moon_queue_type recorder_queue, 
        moon_queue_type command_queue)
{
    lake_result result = LAKE_SUCCESS;
    if (command_queue == moon_queue_type_none) 
        return result;

    bool const main_on_transfer = command_queue == moon_queue_type_main && recorder_queue == moon_queue_type_transfer;
    bool const compute_on_transfer = command_queue == moon_queue_type_compute && recorder_queue == moon_queue_type_transfer;
    bool const main_on_compute = command_queue == moon_queue_type_main && recorder_queue == moon_queue_type_compute;
    result = main_on_transfer ? LAKE_ERROR_MAIN_QUEUE_CMD_ON_TRANSFER_QUEUE_RECORDER : result;
    result = compute_on_transfer ? LAKE_ERROR_COMPUTE_QUEUE_CMD_ON_TRANSFER_QUEUE_RECORDER : result;
    result = main_on_compute ? LAKE_ERROR_MAIN_QUEUE_CMD_ON_COMPUTE_QUEUE_RECORDER : result;
    return result;
}
#else
#define validate_queue_family(a,b) LAKE_SUCCESS
#endif /* LAKE_NDEBUG */

static void LAKECALL execute_deferred_destructors(
        struct moon_device_impl *device, 
        struct staged_command_list_data *data)
{
    LAKE_UNUSED lake_result __ignore = 0;
    lake_darray_foreach_t(&data->deferred_destructors, staged_deferred_destructor_pair, pair) {
        switch (pair->second) {
            case DEFERRED_DESTRUCTOR_BUFFER_IDX: __ignore = _moon_vulkan_destroy_buffer(device, (moon_buffer_id){ pair->first }); break;
            case DEFERRED_DESTRUCTOR_TEXTURE_IDX: __ignore = _moon_vulkan_destroy_texture(device, (moon_texture_id){ pair->first }); break;
            case DEFERRED_DESTRUCTOR_TEXTURE_VIEW_IDX: __ignore = _moon_vulkan_destroy_texture_view(device, (moon_texture_view_id){ pair->first }); break;
            case DEFERRED_DESTRUCTOR_SAMPLER_IDX: __ignore = _moon_vulkan_destroy_sampler(device, (moon_sampler_id){ pair->first }); break;
            default:
                LAKE_UNREACHABLE;
        }
    }
    lake_darray_fini(&data->deferred_destructors, lake_machina);
}

static VkCommandPool LAKECALL acquire_command_pool(
        struct command_pool_arena *arena,
        struct moon_device_impl *device)
{
    VkCommandPool pool = VK_NULL_HANDLE;
    if (lake_darray_empty(&arena->pools_and_buffers)) {
        VkCommandPoolCreateInfo const vk_command_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .queueFamilyIndex = arena->queue_family_idx,
        };
        device->vkCreateCommandPool(device->vk_device, &vk_command_pool_create_info, device->vk_allocator, &pool);
    } else {
        pool = *lake_darray_last_t(&arena->pools_and_buffers, VkCommandPool);
        lake_darray_pop(&arena->pools_and_buffers);
    }
    return pool;
}

#define remember_id(RECORDER, T, ID) \
    lake_darray_append_t(&(RECORDER)->current_command_data.used_##T##s, moon_##T##_id, 1, ID, lake_machina)
#define check_id(RECORDER, T, NAME, ID) \
    __ID_CHECK_RESULT = (_moon_vulkan_is_##T##_valid((RECORDER)->header.device.impl, ID)) \
        ? LAKE_SUCCESS : LAKE_ERROR_INVALID_##NAME##_ID

#define begin_cmd_validation(family) \
    lake_result __ID_CHECK_RESULT = validate_queue_family(cmd->queue_type, family)
#define end_cmd_validation() \
    if (__ID_CHECK_RESULT != LAKE_SUCCESS) return __ID_CHECK_RESULT

static lake_result LAKECALL prepare_cmd_data(
    struct moon_device_impl const *device,
    struct moon_command_recorder_impl *cmd)
{
    VkCommandBufferAllocateInfo const vk_cmd_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = cmd->vk_cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkResult vk_result = device->vkAllocateCommandBuffers(device->vk_device, &vk_cmd_buffer_allocate_info, &cmd->current_command_data.vk_cmd_buffer);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    VkCommandBufferBeginInfo const vk_cmd_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    vk_result = device->vkBeginCommandBuffer(cmd->current_command_data.vk_cmd_buffer, &vk_cmd_buffer_begin_info);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    lake_darray_append_t(&cmd->allocated_command_buffers, VkCommandBuffer, 1, &cmd->current_command_data.vk_cmd_buffer, lake_machina);
    lake_darray_init_t(&cmd->current_command_data.used_buffers, moon_buffer_id, 16, lake_machina);
    lake_darray_init_t(&cmd->current_command_data.used_textures, moon_texture_id, 16, lake_machina);
    lake_darray_init_t(&cmd->current_command_data.used_texture_views, moon_texture_view_id, 16, lake_machina);
    lake_darray_init_t(&cmd->current_command_data.used_samplers, moon_sampler_id, 16, lake_machina);
    return LAKE_SUCCESS;
}

FN_MOON_COMMAND_RECORDER_ASSEMBLY(vulkan) 
{
    struct moon_command_recorder_impl cmd = {
        .header = {
            .device.impl = device,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_command_recorder_zero_refcnt,
        },
        .queue_type = assembly->queue_type,
    };
    struct command_pool_arena *arena = &device->command_pool_arenas[assembly->queue_type];

    lake_spinlock_acquire(&arena->spinlock);
    cmd.vk_cmd_pool = acquire_command_pool(arena, device);
    lake_spinlock_release(&arena->spinlock);

    lake_result result = prepare_cmd_data(device, &cmd);
    if (result != LAKE_SUCCESS) {
        lake_spinlock_acquire(&arena->spinlock);
        lake_darray_append_t(&arena->pools_and_buffers, VkCommandPool, 1, &cmd.vk_cmd_pool, lake_machina);
        lake_spinlock_release(&arena->spinlock);
        return result;
    }
#ifndef LAKE_NDEBUG
    if (device->vkSetDebugUtilsObjectNameEXT != nullptr) {
        VkDebugUtilsObjectNameInfoEXT const name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .pNext = nullptr,
            .objectType = VK_OBJECT_TYPE_COMMAND_POOL,
            .objectHandle = (u64)(uptr)cmd.vk_cmd_pool,
            .pObjectName = cmd.header.assembly.name.str,
        };
        device->vkSetDebugUtilsObjectNameEXT(device->vk_device, &name_info);
    }
#endif /* LAKE_NDEBUG */
    lake_inc_refcnt(&device->header.refcnt);
    lake_inc_refcnt(&cmd.header.refcnt);
    *out_cmd = __lake_malloc_t(struct moon_command_recorder_impl);
    lake_memcpy(*out_cmd, &cmd, sizeof(struct moon_command_recorder_impl));
    return LAKE_SUCCESS;
}

FN_MOON_COMMAND_RECORDER_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(cmd != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&cmd->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Command recorder `%s` reference count is %d.", cmd->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_device_impl *device = cmd->header.device.impl;
    execute_deferred_destructors(cmd->header.device.impl, &cmd->current_command_data);

    zombie_timeline_command_recorder submit = {
        .first = lake_atomic_read(&device->submit_timeline),
        .second = { 
            .queue_type = cmd->queue_type,
            .allocated_command_buffers = cmd->allocated_command_buffers,
            .vk_cmd_pool = cmd->vk_cmd_pool,
        },
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_command_recorder_idx];
    lake_deque_unshift_w_spinlock(&device->command_recorder_zombies, zombie_timeline_command_recorder, &submit, lock, lake_machina);
    
    moon_device_unref(cmd->header.device);
    __lake_free(cmd);
}

static void flush_barriers(struct moon_command_recorder_impl *cmd)
{
    u32 const global_barrier_count = cmd->memory_barrier_batch_count;
    u32 const buffer_barrier_count = cmd->buffer_barrier_batch_count;
    u32 const image_barrier_count = cmd->image_barrier_batch_count;

    if (global_barrier_count || buffer_barrier_count || image_barrier_count) {
        VkDependencyInfo const vk_dependency_info = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = 0,
            .memoryBarrierCount = global_barrier_count,
            .pMemoryBarriers = global_barrier_count ? cmd->memory_barrier_batch : nullptr,
            .bufferMemoryBarrierCount = buffer_barrier_count,
            .pBufferMemoryBarriers = buffer_barrier_count ? cmd->buffer_barrier_batch : nullptr,
            .imageMemoryBarrierCount = image_barrier_count,
            .pImageMemoryBarriers = image_barrier_count ? cmd->image_barrier_batch : nullptr,
        };
        cmd->header.device.impl->vkCmdPipelineBarrier2(cmd->current_command_data.vk_cmd_buffer, &vk_dependency_info);
        cmd->memory_barrier_batch_count = 0;
        cmd->buffer_barrier_batch_count = 0;
        cmd->image_barrier_batch_count = 0;
    }
}

FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(vulkan)
{ 
    struct moon_staged_command_list_impl impl = {
        .header = {
            .cmd.impl = cmd,
            .assembly = *assembly,
            .zero_refcnt = (PFN_lake_work)_moon_vulkan_staged_command_list_zero_refcnt,
        },
    };
    struct moon_device_impl const *device = cmd->header.device.impl;
    flush_barriers(cmd);
    VkResult vk_result = device->vkEndCommandBuffer(cmd->current_command_data.vk_cmd_buffer);
    if (vk_result != VK_SUCCESS)
        return vk_result_translate(vk_result);

    lake_memcpy(&impl.data, &cmd->current_command_data, sizeof(struct staged_command_list_data));
    lake_result result = prepare_cmd_data(device, cmd);
    if (result != LAKE_SUCCESS) {
        lake_memcpy(&cmd->current_command_data, &impl.data, sizeof(struct staged_command_list_data));
        return result;
    }
    lake_inc_refcnt(&cmd->header.refcnt);
    lake_inc_refcnt(&impl.header.refcnt);
    *out_cmd_list = __lake_malloc_t(struct moon_staged_command_list_impl);
    lake_memcpy(*out_cmd_list, &impl, sizeof(struct moon_staged_command_list_impl));
    return LAKE_SUCCESS;
}

FN_MOON_STAGED_COMMAND_LIST_ZERO_REFCNT(vulkan)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(cmd_list != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&cmd_list->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, "Staged command list `%s` reference count is %d.", cmd_list->header.assembly.name.str, refcnt);
#endif /* LAKE_NDEBUG */
    struct moon_command_recorder_impl *cmd = cmd_list->header.cmd.impl;
    execute_deferred_destructors(cmd->header.device.impl, &cmd_list->data);

    moon_command_recorder_unref(cmd_list->header.cmd);
    __lake_free(cmd_list);
}

FN_MOON_CMD_COPY_BUFFER(vulkan)
{
    begin_cmd_validation(moon_queue_type_transfer);
    check_id(cmd, buffer, BUFFER, work->src_buffer);
    check_id(cmd, buffer, BUFFER, work->dst_buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->src_buffer);
    remember_id(cmd, buffer, &work->dst_buffer);

    flush_barriers(cmd);
    if (work->region_count == 0)
        return LAKE_INVALID_PARAMETERS;

    struct buffer_impl_slot const *src_slot = acquire_buffer_slot(cmd->header.device.impl, work->src_buffer);
    struct buffer_impl_slot const *dst_slot = acquire_buffer_slot(cmd->header.device.impl, work->dst_buffer);
    if (src_slot == nullptr || dst_slot == nullptr) 
        return LAKE_ERROR_INVALID_BUFFER_ID;

    VkBufferCopy *vk_regions = lake_drift_alias(sizeof(VkBufferCopy) * work->region_count, alignof(VkBufferCopy));
    for (u32 i = 0; i < work->region_count; i++) {
        moon_buffer_copy_region const *region = &work->regions[i];

        vk_regions[i] = (VkBufferCopy){
            .srcOffset = (VkDeviceSize)region->src_offset,
            .dstOffset = (VkDeviceSize)region->dst_offset,
            .size = (VkDeviceSize)region->size,
        };
    }
    cmd->header.device.impl->vkCmdCopyBuffer(
            cmd->current_command_data.vk_cmd_buffer,
            src_slot->vk_buffer,
            dst_slot->vk_buffer,
            work->region_count,
            vk_regions);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(vulkan)
{
    begin_cmd_validation(moon_queue_type_transfer);
    check_id(cmd, buffer, BUFFER, work->buffer);
    check_id(cmd, texture, TEXTURE, work->texture);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->buffer);
    remember_id(cmd, texture, &work->texture);

    flush_barriers(cmd);
    if (work->region_count == 0)
        return LAKE_INVALID_PARAMETERS;

    struct buffer_impl_slot const *src_slot = acquire_buffer_slot(cmd->header.device.impl, work->buffer);
    if (src_slot == nullptr) 
        return LAKE_ERROR_INVALID_BUFFER_ID;

    struct texture_impl_slot const *dst_slot = acquire_texture_slot(cmd->header.device.impl, work->texture);
    if (dst_slot == nullptr) 
        return LAKE_ERROR_INVALID_TEXTURE_ID;

    VkImageLayout vk_layout;
    populate_vk_access_info(1, &work->texture_access, nullptr, nullptr, &vk_layout, nullptr);

    VkBufferImageCopy *vk_regions = lake_drift_alias(sizeof(VkBufferImageCopy) * work->region_count, alignof(VkBufferImageCopy));
    for (u32 i = 0; i < work->region_count; i++) {
        moon_buffer_and_texture_copy_region const *region = &work->regions[i];

        vk_regions[i] = (VkBufferImageCopy){
            .imageSubresource = make_subresource_layers(&region->texture_slice, dst_slot->aspect_flags),
            .imageOffset = {
                .x = region->texture_offset.x,
                .y = region->texture_offset.y,
                .z = region->texture_offset.z,
            },
            .imageExtent = {
                .width = region->texture_extent.width,
                .height = region->texture_extent.height,
                .depth = region->texture_extent.depth,
            },
            .bufferImageHeight = region->buffer_texture_height,
            .bufferRowLength = region->buffer_row_length,
            .bufferOffset = region->buffer_offset,
        };
    }
    cmd->header.device.impl->vkCmdCopyBufferToImage(
            cmd->current_command_data.vk_cmd_buffer,
            src_slot->vk_buffer,
            dst_slot->vk_image,
            vk_layout,
            work->region_count,
            vk_regions);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(vulkan)
{
    begin_cmd_validation(moon_queue_type_transfer);
    check_id(cmd, buffer, BUFFER, work->buffer);
    check_id(cmd, texture, TEXTURE, work->texture);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->buffer);
    remember_id(cmd, texture, &work->texture);

    flush_barriers(cmd);
    if (work->region_count == 0)
        return LAKE_INVALID_PARAMETERS;

    struct texture_impl_slot const *src_slot = acquire_texture_slot(cmd->header.device.impl, work->texture);
    if (src_slot == nullptr) 
        return LAKE_ERROR_INVALID_TEXTURE_ID;

    struct buffer_impl_slot const *dst_slot = acquire_buffer_slot(cmd->header.device.impl, work->buffer);
    if (dst_slot == nullptr) 
        return LAKE_ERROR_INVALID_BUFFER_ID;

    VkImageLayout vk_layout;
    populate_vk_access_info(1, &work->texture_access, nullptr, nullptr, &vk_layout, nullptr);

    VkBufferImageCopy *vk_regions = lake_drift_alias(sizeof(VkBufferImageCopy) * work->region_count, alignof(VkBufferImageCopy));
    for (u32 i = 0; i < work->region_count; i++) {
        moon_buffer_and_texture_copy_region const *region = &work->regions[i];

        vk_regions[i] = (VkBufferImageCopy){
            .imageSubresource = make_subresource_layers(&region->texture_slice, src_slot->aspect_flags),
            .imageOffset = {
                .x = region->texture_offset.x,
                .y = region->texture_offset.y,
                .z = region->texture_offset.z,
            },
            .imageExtent = {
                .width = region->texture_extent.width,
                .height = region->texture_extent.height,
                .depth = region->texture_extent.depth,
            },
            .bufferImageHeight = region->buffer_texture_height,
            .bufferRowLength = region->buffer_row_length,
            .bufferOffset = region->buffer_offset,
        };
    }
    cmd->header.device.impl->vkCmdCopyImageToBuffer(
            cmd->current_command_data.vk_cmd_buffer,
            src_slot->vk_image,
            vk_layout,
            dst_slot->vk_buffer,
            work->region_count,
            vk_regions);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_COPY_TEXTURE(vulkan)
{
    begin_cmd_validation(moon_queue_type_transfer);
    check_id(cmd, texture, TEXTURE, work->src_texture);
    check_id(cmd, texture, TEXTURE, work->dst_texture);
    end_cmd_validation();

    remember_id(cmd, texture, &work->src_texture);
    remember_id(cmd, texture, &work->dst_texture);

    flush_barriers(cmd);
    if (work->region_count == 0)
        return LAKE_INVALID_PARAMETERS;

    VkImageLayout src_vk_layout;
    VkImageLayout dst_vk_layout;
    struct texture_impl_slot const *src_slot = acquire_texture_slot(cmd->header.device.impl, work->src_texture);
    struct texture_impl_slot const *dst_slot = acquire_texture_slot(cmd->header.device.impl, work->dst_texture);
    if (src_slot == nullptr || dst_slot == nullptr) 
        return LAKE_ERROR_INVALID_TEXTURE_ID;

    populate_vk_access_info(1, &work->dst_access, nullptr, nullptr, &dst_vk_layout, nullptr);
    populate_vk_access_info(1, &work->src_access, nullptr, nullptr, &src_vk_layout, nullptr);

    VkImageCopy *vk_regions = lake_drift_alias(sizeof(VkImageCopy) * work->region_count, alignof(VkImageCopy));
    for (u32 i = 0; i < work->region_count; i++) {
        moon_texture_copy_region const *region = &work->regions[i];

        vk_regions[i] = (VkImageCopy){
            .srcSubresource = make_subresource_layers(&region->src_slice, src_slot->aspect_flags),
            .srcOffset = {
                .x = region->src_offset.x,
                .y = region->src_offset.y,
                .z = region->src_offset.z,
            },
            .dstSubresource = make_subresource_layers(&region->dst_slice, dst_slot->aspect_flags),
            .dstOffset = {
                .x = region->dst_offset.x,
                .y = region->dst_offset.y,
                .z = region->dst_offset.z,
            },
            .extent = {
                .width = region->extent.width,
                .height = region->extent.height,
                .depth = region->extent.depth,
            },
        };
    }
    cmd->header.device.impl->vkCmdCopyImage(
            cmd->current_command_data.vk_cmd_buffer,
            src_slot->vk_image,
            src_vk_layout,
            dst_slot->vk_image,
            dst_vk_layout,
            work->region_count,
            vk_regions);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_BLIT_TEXTURE(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    check_id(cmd, texture, TEXTURE, work->src_texture);
    check_id(cmd, texture, TEXTURE, work->dst_texture);
    end_cmd_validation();

    remember_id(cmd, texture, &work->src_texture);
    remember_id(cmd, texture, &work->dst_texture);

    flush_barriers(cmd);
    if (work->region_count == 0)
        return LAKE_INVALID_PARAMETERS;

    VkImageLayout src_vk_layout;
    VkImageLayout dst_vk_layout;
    struct texture_impl_slot const *src_slot = acquire_texture_slot(cmd->header.device.impl, work->src_texture);
    struct texture_impl_slot const *dst_slot = acquire_texture_slot(cmd->header.device.impl, work->dst_texture);
    if (src_slot == nullptr || dst_slot == nullptr) 
        return LAKE_ERROR_INVALID_TEXTURE_ID;

    populate_vk_access_info(1, &work->dst_access, nullptr, nullptr, &dst_vk_layout, nullptr);
    populate_vk_access_info(1, &work->src_access, nullptr, nullptr, &src_vk_layout, nullptr);
    
    VkImageBlit *vk_regions = lake_drift_alias(sizeof(VkImageBlit) * work->region_count, alignof(VkImageBlit));
    for (u32 i = 0; i < work->region_count; i++) {
        moon_texture_blit_region const *region = &work->regions[i];

        vk_regions[i] = (VkImageBlit){
            .srcSubresource = make_subresource_layers(&region->src_slice, src_slot->aspect_flags),
            .srcOffsets[0] = {
                .x = region->src_offsets[0].x,
                .y = region->src_offsets[0].y,
                .z = region->src_offsets[0].z,
            },
            .srcOffsets[1] = {
                .x = region->src_offsets[1].x,
                .y = region->src_offsets[1].y,
                .z = region->src_offsets[1].z,
            },
            .dstSubresource = make_subresource_layers(&region->dst_slice, dst_slot->aspect_flags),
            .dstOffsets[0] = {
                .x = region->dst_offsets[0].x,
                .y = region->dst_offsets[0].y,
                .z = region->dst_offsets[0].z,
            },
            .dstOffsets[1] = {
                .x = region->dst_offsets[1].x,
                .y = region->dst_offsets[1].y,
                .z = region->dst_offsets[1].z,
            },
        };
    }
    cmd->header.device.impl->vkCmdBlitImage(
            cmd->current_command_data.vk_cmd_buffer,
            src_slot->vk_image,
            src_vk_layout,
            dst_slot->vk_image,
            dst_vk_layout,
            work->region_count,
            vk_regions,
            (VkFilter)work->filter);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_RESOLVE_TEXTURE(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    check_id(cmd, texture, TEXTURE, work->src_texture);
    check_id(cmd, texture, TEXTURE, work->dst_texture);
    end_cmd_validation();

    remember_id(cmd, texture, &work->src_texture);
    remember_id(cmd, texture, &work->dst_texture);

    flush_barriers(cmd);
    if (work->region_count == 0)
        return LAKE_INVALID_PARAMETERS;

    VkImageLayout src_vk_layout;
    VkImageLayout dst_vk_layout;
    struct texture_impl_slot const *src_slot = acquire_texture_slot(cmd->header.device.impl, work->src_texture);
    struct texture_impl_slot const *dst_slot = acquire_texture_slot(cmd->header.device.impl, work->dst_texture);
    if (src_slot == nullptr || dst_slot == nullptr) 
        return LAKE_ERROR_INVALID_TEXTURE_ID;

    populate_vk_access_info(1, &work->dst_access, nullptr, nullptr, &dst_vk_layout, nullptr);
    populate_vk_access_info(1, &work->src_access, nullptr, nullptr, &src_vk_layout, nullptr);

    VkImageResolve *vk_regions = lake_drift_alias(sizeof(VkImageResolve) * work->region_count, alignof(VkImageResolve));
    for (u32 i = 0; i < work->region_count; i++) {
        moon_texture_resolve_region const *region = &work->regions[i];

        vk_regions[i] = (VkImageResolve){
            .srcSubresource = make_subresource_layers(&region->src_slice, src_slot->aspect_flags),
            .srcOffset = {
                .x = region->src_offset.x,
                .y = region->src_offset.y,
                .z = region->src_offset.z,
            },
            .dstSubresource = make_subresource_layers(&region->dst_slice, dst_slot->aspect_flags),
            .dstOffset = {
                .x = region->dst_offset.x,
                .y = region->dst_offset.y,
                .z = region->dst_offset.z,
            },
            .extent = {
                .width = region->extent.width,
                .height = region->extent.height,
                .depth = region->extent.depth,
            },
        };
    }
    cmd->header.device.impl->vkCmdResolveImage(
            cmd->current_command_data.vk_cmd_buffer,
            src_slot->vk_image,
            src_vk_layout,
            dst_slot->vk_image,
            dst_vk_layout,
            work->region_count,
            vk_regions);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_CLEAR_BUFFER(vulkan)
{
    begin_cmd_validation(moon_queue_type_transfer);
    check_id(cmd, buffer, BUFFER, work->dst_buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->dst_buffer);
    flush_barriers(cmd);

    struct buffer_impl_slot const *slot = acquire_buffer_slot(cmd->header.device.impl, work->dst_buffer);
    if (slot == nullptr) 
        return LAKE_ERROR_INVALID_BUFFER_ID;

    bool const in_bounds = ((work->dst_offset) + (work->size)) <= slot->assembly.size;
    if (!in_bounds) 
        return LAKE_ERROR_OUT_OF_RANGE;

    cmd->header.device.impl->vkCmdFillBuffer(
            cmd->current_command_data.vk_cmd_buffer, 
            slot->vk_buffer,
            (VkDeviceSize)work->dst_offset,
            (VkDeviceSize)work->size,
            work->clear_value);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_CLEAR_TEXTURE(vulkan) 
{
    begin_cmd_validation(moon_queue_type_transfer);
    check_id(cmd, texture, TEXTURE, work->dst_texture);
    end_cmd_validation();

    remember_id(cmd, texture, &work->dst_texture);
    flush_barriers(cmd);

    struct texture_impl_slot const *slot = acquire_texture_slot(cmd->header.device.impl, work->dst_texture);
    if (slot == nullptr) 
        return LAKE_ERROR_INVALID_TEXTURE_ID;

    bool const is_image_depth_stencil = is_depth_format(slot->assembly.format) || is_stencil_format(slot->assembly.format);
    VkImageSubresourceRange const sub_range = make_subresource_range(&work->dst_slice, slot->aspect_flags);
    VkImageLayout vk_layout;
    populate_vk_access_info(1, &work->dst_access, nullptr, nullptr, &vk_layout, nullptr);

    if (work->is_clear_depth_stencil) {
        if (!is_image_depth_stencil)
            return LAKE_INVALID_PARAMETERS;

        cmd->header.device.impl->vkCmdClearDepthStencilImage(
                cmd->current_command_data.vk_cmd_buffer,
                slot->vk_image,
                vk_layout,
                (VkClearDepthStencilValue const *)&work->clear_value.depth_stencil,
                1, &sub_range);
    } else {
        if (is_image_depth_stencil)
            return LAKE_INVALID_PARAMETERS;
        
        cmd->header.device.impl->vkCmdClearColorImage(
                cmd->current_command_data.vk_cmd_buffer,
                slot->vk_image,
                vk_layout,
                (VkClearColorValue const *)&work->clear_value.color, 
                1, &sub_range);
    }
    return LAKE_SUCCESS;
}

FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan)
{
    flush_barriers(cmd);
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

#define IMPL_CMD_DESTROY_DEFERRED(T, NAME) \
    FN_MOON_CMD_DESTROY_##NAME##_DEFERRED(vulkan) \
    { \
        begin_cmd_validation(moon_queue_type_none); \
        check_id(cmd, T, NAME, T); \
        end_cmd_validation(); \
        remember_id(cmd, T, &T); \
        lake_darray_append_t(&cmd->current_command_data.deferred_destructors, moon_##T##_id, 1, &T, lake_machina); \
        return LAKE_SUCCESS; \
    }
IMPL_CMD_DESTROY_DEFERRED(buffer, BUFFER)
IMPL_CMD_DESTROY_DEFERRED(texture, TEXTURE)
IMPL_CMD_DESTROY_DEFERRED(texture_view, TEXTURE_VIEW)
IMPL_CMD_DESTROY_DEFERRED(sampler, SAMPLER)

FN_MOON_CMD_PUSH_CONSTANTS(vulkan)
{
    VkPipelineLayout vk_pipeline_layout = VK_NULL_HANDLE;
    u32 current_push_constant_size = 0;

    flush_barriers(cmd);
    switch (cmd->current_pipeline_variant) {
        case COMMAND_RECORDER_COMPUTE_PIPELINE:
            current_push_constant_size = cmd->current_pipeline.compute->header.assembly.push_constant_size;
            vk_pipeline_layout = cmd->current_pipeline.compute->vk_pipeline_layout;
            break;
        case COMMAND_RECORDER_WORK_GRAPH_PIPELINE:
            current_push_constant_size = cmd->current_pipeline.work_graph->header.assembly.push_constant_size;
            vk_pipeline_layout = cmd->current_pipeline.work_graph->vk_pipeline_layout;
            break;
        case COMMAND_RECORDER_RAY_TRACING_PIPELINE:
            current_push_constant_size = cmd->current_pipeline.ray_tracing->header.assembly.push_constant_size;
            vk_pipeline_layout = cmd->current_pipeline.ray_tracing->vk_pipeline_layout;
            break;
        case COMMAND_RECORDER_RASTER_PIPELINE:
            current_push_constant_size = cmd->current_pipeline.raster->header.assembly.push_constant_size;
            vk_pipeline_layout = cmd->current_pipeline.raster->vk_pipeline_layout;
            break;
        default: return LAKE_ERROR_INVALID_SHADER;
    }
    if (current_push_constant_size < work->size)
        return LAKE_ERROR_EXCEEDED_PUSH_CONSTANT_RANGE;

    /* Always write the whole range, fill with 0xff to the size of the push constant.
     * This makes validation and renderdoc happy, as well as help debug uninitialized push constant data. */
    u8 const_data[MOON_MAX_PUSH_CONSTANT_BYTE_SIZE];
    lake_memset(const_data, 0xff, MOON_MAX_PUSH_CONSTANT_BYTE_SIZE);
    lake_memcpy(const_data, work->data, work->size);
    cmd->header.device.impl->vkCmdPushConstants(
            cmd->current_command_data.vk_cmd_buffer,
            vk_pipeline_layout,
            VK_SHADER_STAGE_ALL,
            0, current_push_constant_size, 
            const_data);
    return LAKE_SUCCESS;
}

#define IMPL_CMD_SET_PIPELINE(T, NAME, vk_bind_point) \
    FN_MOON_CMD_SET_##NAME##_PIPELINE(vulkan) \
    { \
        flush_barriers(cmd); \
        bool const prev_pipeline_is_##T = cmd->current_pipeline_variant == COMMAND_RECORDER_##NAME##_PIPELINE; \
        bool const same_type_same_layout_as_prev_pipeline = prev_pipeline_is_##T \
            && cmd->current_pipeline.T->vk_pipeline_layout == pipeline->vk_pipeline_layout; \
        if (!same_type_same_layout_as_prev_pipeline) { \
            cmd->header.device.impl->vkCmdBindDescriptorSets( \
                    cmd->current_command_data.vk_cmd_buffer, \
                    vk_bind_point, \
                    pipeline->vk_pipeline_layout, \
                    0, 1, &cmd->header.device.impl->gpu_sr_table.vk_descriptor_set, \
                    0, nullptr); \
        } \
        cmd->current_pipeline.T = pipeline; \
        cmd->current_pipeline_variant = COMMAND_RECORDER_##NAME##_PIPELINE; \
        cmd->header.device.impl->vkCmdBindPipeline( \
                cmd->current_command_data.vk_cmd_buffer, \
                vk_bind_point, \
                pipeline->vk_pipeline); \
        return LAKE_SUCCESS; \
    }
IMPL_CMD_SET_PIPELINE(compute, COMPUTE, VK_PIPELINE_BIND_POINT_COMPUTE)
IMPL_CMD_SET_PIPELINE(work_graph, WORK_GRAPH, VK_PIPELINE_BIND_POINT_EXECUTION_GRAPH_AMDX)
IMPL_CMD_SET_PIPELINE(ray_tracing, RAY_TRACING, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR)
IMPL_CMD_SET_PIPELINE(raster, RASTER, VK_PIPELINE_BIND_POINT_GRAPHICS)

FN_MOON_CMD_SET_VIEWPORT(vulkan)
{
    flush_barriers(cmd);
    cmd->header.device.impl->vkCmdSetViewport(
            cmd->current_command_data.vk_cmd_buffer,
            work->first_viewport,
            work->viewport_count,
            (VkViewport const *)work->viewports);
}

FN_MOON_CMD_SET_SCISSOR(vulkan)
{
    flush_barriers(cmd);
    cmd->header.device.impl->vkCmdSetScissor(
            cmd->current_command_data.vk_cmd_buffer, 
            work->first_scissor, 
            work->scissor_count, 
            (VkRect2D const *)work->scissors);
}

FN_MOON_CMD_SET_DEPTH_BIAS(vulkan)
{
    flush_barriers(cmd);
    cmd->header.device.impl->vkCmdSetDepthBias(
            cmd->current_command_data.vk_cmd_buffer,
            work->constant_factor,
            work->clamp,
            work->slope_factor);
}

FN_MOON_CMD_SET_INDEX_BUFFER(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    check_id(cmd, buffer, BUFFER, work->buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->buffer);

    cmd->header.device.impl->vkCmdBindIndexBuffer(
            cmd->current_command_data.vk_cmd_buffer,
            acquire_buffer_slot(cmd->header.device.impl, work->buffer)->vk_buffer,
            work->offset,
            (VkIndexType)work->index_format);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(vulkan)
{
    (void)cmd;
    (void)sample_count;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_BEGIN_RENDERPASS(vulkan)
{
    lake_dbg_assert(cmd->in_renderpass == false, LAKE_ERROR_NOT_PERMITTED, "Must be outside of a renderpass.");
    /* TODO */
    (void)work;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_MOON_CMD_END_RENDERPASS(vulkan)
{
    lake_dbg_assert(cmd->in_renderpass == true, LAKE_ERROR_NOT_PERMITTED, "Must be in renderpass.");
    flush_barriers(cmd);
    cmd->header.device.impl->vkCmdEndRendering(cmd->current_command_data.vk_cmd_buffer);
    cmd->in_renderpass = false;
}

FN_MOON_CMD_WRITE_TIMESTAMPS(vulkan)
{
    flush_barriers(cmd);
    cmd->header.device.impl->vkCmdWriteTimestamp2(
        cmd->current_command_data.vk_cmd_buffer,
        work->stage,
        work->timeline_query_pool->vk_query_pool,
        work->query_index);
}

FN_MOON_CMD_RESOLVE_TIMESTAMPS(vulkan)
{
    flush_barriers(cmd);
    cmd->header.device.impl->vkCmdResetQueryPool(
        cmd->current_command_data.vk_cmd_buffer,
        work->timeline_query_pool->vk_query_pool,
        work->start_index,
        work->count);
}

FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(vulkan)
{
    begin_cmd_validation(moon_queue_type_none);
    for (u32 i = 0; i < work->buffer_barrier_count; i++) {
        moon_buffer_id id = work->buffer_bariers[i].buffer;
        check_id(cmd, buffer, BUFFER, id);
        remember_id(cmd, buffer, &id);
    }
    for (u32 i = 0; i < work->texture_barrier_count; i++) {
        moon_texture_id id = work->texture_bariers[i].texture;
        check_id(cmd, texture, TEXTURE, id);
        remember_id(cmd, texture, &id);
    }
    end_cmd_validation();

    if (work->global_barrier) {
        if (cmd->memory_barrier_batch_count == COMMAND_LIST_BARRIER_MAX_BATCH_SIZE)
            flush_barriers(cmd);
        populate_vk_memory_barrier(work->global_barrier, &cmd->memory_barrier_batch[cmd->memory_barrier_batch_count++]);
    }
    for (u32 i = 0; i < work->buffer_barrier_count; i++) {
        if (cmd->buffer_barrier_batch_count == COMMAND_LIST_BARRIER_MAX_BATCH_SIZE)
            flush_barriers(cmd);
        populate_vk_buffer_memory_barrier(cmd->header.device.impl, &work->buffer_bariers[i], &cmd->buffer_barrier_batch[cmd->buffer_barrier_batch_count++]);
    }
    for (u32 i = 0; i < work->texture_barrier_count; i++) {
        if (cmd->image_barrier_batch_count == COMMAND_LIST_BARRIER_MAX_BATCH_SIZE)
            flush_barriers(cmd);
        populate_vk_image_memory_barrier(cmd->header.device.impl, &work->texture_bariers[i], &cmd->image_barrier_batch[cmd->image_barrier_batch_count++]);
    }
    return LAKE_SUCCESS;
}

FN_MOON_CMD_SIGNAL_EVENT(vulkan)
{
    flush_barriers(cmd);
    /* TODO */
    (void)work;
}

FN_MOON_CMD_RESET_EVENT(vulkan)
{
    flush_barriers(cmd);

    VkPipelineStageFlags2 stage_mask = 0;
    populate_vk_access_info(work->src_access_count, work->src_accesses, &stage_mask, nullptr, nullptr, nullptr);

    cmd->header.device.impl->vkCmdResetEvent2(
            cmd->current_command_data.vk_cmd_buffer,
            work->event->vk_event,
            stage_mask);
}

FN_MOON_CMD_WAIT_EVENTS(vulkan)
{
    flush_barriers(cmd);
    /* TODO */
    (void)work;
}

FN_MOON_CMD_BEGIN_LABEL(vulkan)
{
    flush_barriers(cmd);
    VkDebugUtilsLabelEXT const vk_debug_label_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = nullptr,
        .pLabelName = work->label_name,
        .color = {
            work->label_color[0],
            work->label_color[1],
            work->label_color[2],
            work->label_color[3],
        },
    };
    if ((cmd->header.device.impl->vkCmdBeginDebugUtilsLabelEXT != nullptr))
        cmd->header.device.impl->vkCmdBeginDebugUtilsLabelEXT(cmd->current_command_data.vk_cmd_buffer, &vk_debug_label_info);
}

FN_MOON_CMD_END_LABEL(vulkan)
{
    flush_barriers(cmd);
    if ((cmd->header.device.impl->vkCmdEndDebugUtilsLabelEXT != nullptr))
        cmd->header.device.impl->vkCmdEndDebugUtilsLabelEXT(cmd->current_command_data.vk_cmd_buffer);
}

FN_MOON_CMD_DISPATCH(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    end_cmd_validation();

    if (cmd->current_pipeline_variant != COMMAND_RECORDER_COMPUTE_PIPELINE)
        return LAKE_ERROR_INVALID_SHADER;

    cmd->header.device.impl->vkCmdDispatch(
            cmd->current_command_data.vk_cmd_buffer,
            work->group_x,
            work->group_y,
            work->group_z);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DISPATCH_INDIRECT(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    check_id(cmd, buffer, BUFFER, work->indirect_buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->indirect_buffer);

    if (cmd->current_pipeline_variant != COMMAND_RECORDER_COMPUTE_PIPELINE)
        return LAKE_ERROR_INVALID_SHADER;

    cmd->header.device.impl->vkCmdDispatchIndirect(
            cmd->current_command_data.vk_cmd_buffer,
            acquire_buffer_slot(cmd->header.device.impl, work->indirect_buffer)->vk_buffer,
            work->offset);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DISPATCH_GRAPH(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    end_cmd_validation();

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_work_graph) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (cmd->current_pipeline_variant != COMMAND_RECORDER_WORK_GRAPH_PIPELINE)
        return LAKE_ERROR_INVALID_SHADER;

    VkDispatchGraphCountInfoAMDX const vk_dispatch_graph_count_info = {
        .count = work->graph.node_count,
        .infos = (VkDeviceOrHostAddressConstAMDX){ 
            .hostAddress = work->graph.nodes.host_address,
        },
        .stride = work->graph.node_stride,
    };
    cmd->header.device.impl->vkCmdDispatchGraphAMDX(
            cmd->current_command_data.vk_cmd_buffer,
            (VkDeviceAddress)work->scratch,
            (VkDeviceSize)work->scratch_size,
            &vk_dispatch_graph_count_info);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    end_cmd_validation();

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_work_graph) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (cmd->current_pipeline_variant != COMMAND_RECORDER_WORK_GRAPH_PIPELINE)
        return LAKE_ERROR_INVALID_SHADER;

    VkDispatchGraphCountInfoAMDX const vk_dispatch_graph_count_info = {
        .count = work->graph.node_count,
        .infos = (VkDeviceOrHostAddressConstAMDX){ 
            .deviceAddress = (VkDeviceAddress)work->graph.nodes.device_address,
        },
        .stride = work->graph.node_stride,
    };
    cmd->header.device.impl->vkCmdDispatchGraphIndirectAMDX(
            cmd->current_command_data.vk_cmd_buffer,
            (VkDeviceAddress)work->scratch,
            (VkDeviceSize)work->scratch_size, 
            &vk_dispatch_graph_count_info);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    end_cmd_validation();

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_work_graph) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (cmd->current_pipeline_variant != COMMAND_RECORDER_WORK_GRAPH_PIPELINE)
        return LAKE_ERROR_INVALID_SHADER;

    cmd->header.device.impl->vkCmdDispatchGraphIndirectCountAMDX(
            cmd->current_command_data.vk_cmd_buffer,
            (VkDeviceAddress)work->scratch,
            (VkDeviceSize)work->scratch_size,
            (VkDeviceAddress)work->graph);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DISPATCH_GRAPH_SCRATCH_MEMORY(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    end_cmd_validation();

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_work_graph) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;

    cmd->header.device.impl->vkCmdInitializeGraphScratchMemoryAMDX(
            cmd->current_command_data.vk_cmd_buffer,
            work->work_graph->vk_pipeline,
            (VkDeviceAddress)work->scratch,
            (VkDeviceSize)work->scratch_size);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_TRACE_RAYS(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    end_cmd_validation();

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_ray_tracing_pipeline) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (cmd->current_pipeline_variant != COMMAND_RECORDER_RAY_TRACING_PIPELINE)
        return LAKE_ERROR_INVALID_SHADER;

    moon_shader_binding_table const *sbt = &work->shader_binding_table;
    VkStridedDeviceAddressRegionKHR raygen = { 
        .deviceAddress = sbt->raygen_region.device_address,
        .size = sbt->raygen_region.size,
        .stride = sbt->raygen_region.stride,
    };
    raygen.deviceAddress += sbt->raygen_region.stride * work->raygen_shader_binding_table_offset;
    VkStridedDeviceAddressRegionKHR miss = {
        .deviceAddress = sbt->miss_region.device_address,
        .size = sbt->miss_region.size,
        .stride = sbt->miss_region.stride,
    };
    miss.deviceAddress += sbt->miss_region.stride * work->miss_shader_binding_table_offset;
    VkStridedDeviceAddressRegionKHR hit = {
        .deviceAddress = sbt->hit_region.device_address,
        .size = sbt->hit_region.size,
        .stride = sbt->hit_region.stride,
    };
    hit.deviceAddress += sbt->hit_region.stride * work->hit_shader_binding_table_offset;
    VkStridedDeviceAddressRegionKHR callable = {
        .deviceAddress = sbt->callable_region.device_address,
        .size = sbt->callable_region.size,
        .stride = sbt->callable_region.stride,
    };
    callable.deviceAddress += sbt->callable_region.stride * work->callable_shader_binding_table_offset;

    cmd->header.device.impl->vkCmdTraceRaysKHR(
            cmd->current_command_data.vk_cmd_buffer,
            &raygen, &miss, &hit, &callable,
            work->width, work->height, work->depth);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_TRACE_RAYS_INDIRECT(vulkan)
{
    begin_cmd_validation(moon_queue_type_compute);
    end_cmd_validation();

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_ray_tracing_pipeline) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;
    if (cmd->current_pipeline_variant != COMMAND_RECORDER_RAY_TRACING_PIPELINE)
        return LAKE_ERROR_INVALID_SHADER;

    moon_shader_binding_table const *sbt = &work->shader_binding_table;
    VkStridedDeviceAddressRegionKHR raygen = { 
        .deviceAddress = sbt->raygen_region.device_address,
        .size = sbt->raygen_region.size,
        .stride = sbt->raygen_region.stride,
    };
    raygen.deviceAddress += sbt->raygen_region.stride * work->raygen_shader_binding_table_offset;
    VkStridedDeviceAddressRegionKHR miss = {
        .deviceAddress = sbt->miss_region.device_address,
        .size = sbt->miss_region.size,
        .stride = sbt->miss_region.stride,
    };
    miss.deviceAddress += sbt->miss_region.stride * work->miss_shader_binding_table_offset;
    VkStridedDeviceAddressRegionKHR hit = {
        .deviceAddress = sbt->hit_region.device_address,
        .size = sbt->hit_region.size,
        .stride = sbt->hit_region.stride,
    };
    hit.deviceAddress += sbt->hit_region.stride * work->hit_shader_binding_table_offset;
    VkStridedDeviceAddressRegionKHR callable = {
        .deviceAddress = sbt->callable_region.device_address,
        .size = sbt->callable_region.size,
        .stride = sbt->callable_region.stride,
    };
    callable.deviceAddress += sbt->callable_region.stride * work->callable_shader_binding_table_offset;

    cmd->header.device.impl->vkCmdTraceRaysIndirectKHR(
            cmd->current_command_data.vk_cmd_buffer,
            &raygen, &miss, &hit, &callable,
            work->indirect_buffer_address);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DRAW(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    lake_dbg_assert(__ID_CHECK_RESULT == LAKE_SUCCESS, LAKE_ERROR_INVALID_QUEUE, nullptr);

    cmd->header.device.impl->vkCmdDraw(
            cmd->current_command_data.vk_cmd_buffer,
            work->vertex_count,
            work->instance_count,
            work->first_vertex,
            work->first_instance);
}

FN_MOON_CMD_DRAW_INDEXED(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    lake_dbg_assert(__ID_CHECK_RESULT == LAKE_SUCCESS, LAKE_ERROR_INVALID_QUEUE, nullptr);

    cmd->header.device.impl->vkCmdDrawIndexed(
            cmd->current_command_data.vk_cmd_buffer,
            work->index_count,
            work->instance_count,
            work->first_index,
            work->vertex_offset,
            work->first_instance);
}

FN_MOON_CMD_DRAW_INDIRECT(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    check_id(cmd, buffer, BUFFER, work->indirect_buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->indirect_buffer);

    if (work->is_indexed) {
        cmd->header.device.impl->vkCmdDrawIndexedIndirect(
                cmd->current_command_data.vk_cmd_buffer,
                acquire_buffer_slot(cmd->header.device.impl, work->indirect_buffer)->vk_buffer,
                work->indirect_buffer_offset,
                work->draw_count,
                work->draw_stride);
    } else {
        cmd->header.device.impl->vkCmdDrawIndirect(
                cmd->current_command_data.vk_cmd_buffer,
                acquire_buffer_slot(cmd->header.device.impl, work->indirect_buffer)->vk_buffer,
                work->indirect_buffer_offset,
                work->draw_count,
                work->draw_stride);
    }
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DRAW_INDIRECT_COUNT(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    check_id(cmd, buffer, BUFFER, work->indirect_buffer);
    check_id(cmd, buffer, BUFFER, work->count_buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->indirect_buffer);
    remember_id(cmd, buffer, &work->count_buffer);

    if (work->is_indexed) {
        cmd->header.device.impl->vkCmdDrawIndexedIndirectCount(
                cmd->current_command_data.vk_cmd_buffer,
                acquire_buffer_slot(cmd->header.device.impl, work->indirect_buffer)->vk_buffer,
                work->indirect_buffer_offset,
                acquire_buffer_slot(cmd->header.device.impl, work->count_buffer)->vk_buffer,
                work->count_buffer_offset,
                work->max_draw_count,
                work->draw_stride);
    } else {
        cmd->header.device.impl->vkCmdDrawIndirectCount(
                cmd->current_command_data.vk_cmd_buffer,
                acquire_buffer_slot(cmd->header.device.impl, work->indirect_buffer)->vk_buffer,
                work->indirect_buffer_offset,
                acquire_buffer_slot(cmd->header.device.impl, work->count_buffer)->vk_buffer,
                work->count_buffer_offset,
                work->max_draw_count,
                work->draw_stride);
    }
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DRAW_MESH_TASKS(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    lake_dbg_assert(__ID_CHECK_RESULT == LAKE_SUCCESS, LAKE_ERROR_INVALID_QUEUE, nullptr);

    if (cmd->header.device.header->details->implicit_features & moon_implicit_feature_mesh_shader)
        cmd->header.device.impl->vkCmdDrawMeshTasksEXT(cmd->current_command_data.vk_cmd_buffer, work->group_x, work->group_y, work->group_z);
}

FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    check_id(cmd, buffer, BUFFER, work->indirect_buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->indirect_buffer);

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_mesh_shader) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;

    cmd->header.device.impl->vkCmdDrawMeshTasksIndirectEXT(
            cmd->current_command_data.vk_cmd_buffer,
            acquire_buffer_slot(cmd->header.device.impl, work->indirect_buffer)->vk_buffer,
            work->indirect_buffer_offset,
            work->draw_count,
            work->draw_stride);
    return LAKE_SUCCESS;
}

FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(vulkan)
{
    begin_cmd_validation(moon_queue_type_main);
    check_id(cmd, buffer, BUFFER, work->indirect_buffer);
    check_id(cmd, buffer, BUFFER, work->count_buffer);
    end_cmd_validation();

    remember_id(cmd, buffer, &work->indirect_buffer);
    remember_id(cmd, buffer, &work->count_buffer);

    if ((cmd->header.device.header->details->implicit_features & moon_implicit_feature_mesh_shader) == moon_implicit_feature_none)
        return LAKE_ERROR_FEATURE_NOT_PRESENT;

    cmd->header.device.impl->vkCmdDrawMeshTasksIndirectCountEXT(
            cmd->current_command_data.vk_cmd_buffer,
            acquire_buffer_slot(cmd->header.device.impl, work->indirect_buffer)->vk_buffer,
            work->indirect_buffer_offset,
            acquire_buffer_slot(cmd->header.device.impl, work->count_buffer)->vk_buffer,
            work->count_buffer_offset,
            work->max_draw_count,
            work->draw_stride);
    return LAKE_SUCCESS;
}
#endif /* MOON_VULKAN */
