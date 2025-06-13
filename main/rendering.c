#include "a_moonlit_walk.h"

static lake_result record_main_commands(
    moon_interface          moon, 
    moon_command_recorder   cmd, 
    moon_texture_id         image)
{
    LAKE_UNUSED lake_result result = 0;
    moon_texture_view_id image_view = { .handle = image.handle };
    moon_texture_view_assembly image_assembly = {0};
    result |= moon.interface->read_texture_view_assembly(cmd.device->impl, image_view, &image_assembly);

    moon_access src_accesses[1];
    moon_access dst_accesses[1];

    moon_texture_barrier const transition = {
        .dst_accesses = dst_accesses,
        .dst_access_count = 1,
        .dst_layout = moon_layout_optimal,
        .src_accesses = src_accesses,
        .src_access_count = 1,
        .src_layout = moon_layout_optimal,
        .texture_slice = image_assembly.slice,
        .texture = image,
    };
    moon_pipeline_barrier const barriers_and_transitions = {
        .texture_barrier_count = 1,
        .texture_bariers = &transition,
    };

    src_accesses[0] = moon_access_transfer_read;
    dst_accesses[0] = moon_access_transfer_write;
    result |= moon.interface->cmd_pipeline_barrier(cmd.impl, &barriers_and_transitions);

    moon_clear_texture const clear_image = {
        .dst_texture = image,
        .dst_slice = image_assembly.slice,
        .dst_layout = moon_layout_optimal,
        .dst_access = moon_access_color_attachment_write,
        .clear_value = { .color = { .vec = { 1.f, 0.f, 1.f, 1.f }}},
    };
    result |= moon.interface->cmd_clear_texture(cmd.impl, &clear_image);

    src_accesses[0] = moon_access_transfer_write;
    dst_accesses[0] = moon_access_present;
    result |= moon.interface->cmd_pipeline_barrier(cmd.impl, &barriers_and_transitions);
    return result;
}

FN_LAKE_WORK(a_moonlit_walk__rendering, struct pipeline_work *work)
{
    if (work == nullptr) return;

    struct a_moonlit_walk *amw = work->amw;
    moon_device primary = amw->primary_device;
    moon_interface moon = *primary.moon;
    LAKE_UNUSED lake_result result;

    /* for now we'll work with just one command recorder, we'll do multithreading later */
    moon_command_recorder_assembly cmd_assembly = {
        .queue_type = moon_queue_type_main,
        .name = lake_small_string_cstr("gameloop main "),
    };
    cmd_assembly.name.len = snprintf(cmd_assembly.name.str + cmd_assembly.name.len, LAKE_SMALL_STRING_CAPACITY - cmd_assembly.name.len, "%lu", work->timeline);

    moon_command_recorder cmd;
    result = moon.interface->command_recorder_assembly(primary.impl, &cmd_assembly, &cmd.impl);
    if (result != LAKE_SUCCESS) {
        lake_error("error rendering at command_recorder_assembly.");
        lake_atomic_write_explicit(&amw->stage_hint, pipeline_stage_hint_try_recover, lake_memory_model_release);
        return;
    }

    lake_darray_foreach_v(amw->swapchains, moon_swapchain, sc) {
        hadal_window window = lake_impl_v(hadal_window, sc->header->assembly.native_window);
        u32 out_of_date = lake_atomic_and_explicit(&window.header->flags, ~(hadal_window_flag_swapchain_out_of_date), lake_memory_model_acquire);

        /* We handle swapchain resizing by controlling the window flag that is 
         * set whenever the window's framebuffer resizes. */
        if (out_of_date & hadal_window_flag_swapchain_out_of_date)
            result = moon.interface->swapchain_resize(sc->impl);
        /* skip the frame if this fails */
        if (result != LAKE_SUCCESS) continue;

        /* The swapchain image is what we'll be working with here. If swapchain image 
         * acquisition failed, then the result of this acquire function will be an 
         * empty texture handle, and thus we want to explicitly skip this frame. */
        moon_texture_id image;
        result = moon.interface->swapchain_acquire_next_image(sc->impl, &image);
        if (moon_id_is_empty(image))
            continue;
        /* We can record commands now. */
        result = record_main_commands(moon, cmd, image);
        if (result != LAKE_SUCCESS) {
            lake_error("error rendering at record_main_commands for `%s`, forced frame skip.", sc->header->assembly.name.str);
            lake_atomic_write_explicit(&amw->stage_hint, pipeline_stage_hint_try_recover, lake_memory_model_release);
            moon_command_recorder_unref(cmd);
            return;
        }

        moon_staged_command_list_assembly const staged_cmd_list_assembly = {
            .name = sc->header->assembly.name,
        };
        moon_staged_command_list staged_cmd_list;
        /* After doing this, the recorder is empty and can create new staged command lists later. */
        result = moon.interface->staged_command_list_assembly(cmd.impl, &staged_cmd_list_assembly, &staged_cmd_list.impl);
        if (result != LAKE_SUCCESS) {
            lake_error("error rendering at staged_command_list_assembly for `%s`, forced frame skip.", sc->header->assembly.name.str);
            lake_atomic_write_explicit(&amw->stage_hint, pipeline_stage_hint_try_recover, lake_memory_model_release);
            if (staged_cmd_list.impl != nullptr)
                moon_staged_command_list_unref(staged_cmd_list);
            moon_command_recorder_unref(cmd);
            return;
        }

        /* all is fine so let's save the handles for GPU exec */
        lake_darray_append_t(&work->swapchains.da, moon_swapchain, sc);
        lake_darray_append_t(&work->cmd_lists.da, moon_staged_command_list, &staged_cmd_list);
    }
#if 0 /* defined(LAKE_IMGUI) */
    if (amw->imgui.assembly.context != nullptr) {
        result = imgui_record_commands(&amw->imgui, cmd, primary_swapchain_image, igGetDrawData(), width, height);
        if (result != LAKE_SUCCESS) {
            lake_error("error rendering at imgui_record_commands.");
            work->stage_hint = pipeline_stage_hint_try_recover;
        }
    }
#endif /* LAKE_IMGUI */
    /* Recorders CANNOT be keept over multiple frames. They are temporary objects and 
     * need to be destroyed before calling PFN_moon_device_commit_deferred_destructors. */
    moon_command_recorder_unref(cmd);
}
