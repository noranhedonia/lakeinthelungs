#include "a_moonlit_walk.h"

FN_LAKE_WORK(a_moonlit_walk__gpuexec, struct pipeline_work *work)
{
    if (work == nullptr) return;

    struct a_moonlit_walk *amw = work->amw;
    moon_device primary = amw->primary_device;
    moon_interface moon = *primary.moon;
    LAKE_UNUSED lake_result result;

    lake_darray acquire_semaphores;
    lake_darray present_semaphores;
    lake_darray timeline_pairs;
    if (lake_darray_empty(&work->cmd_lists.da)) return;

    u32 const swapchain_count = lake_darray_size(&work->swapchains.da);
    lake_dbg_assert(swapchain_count != 0, LAKE_PANIC, nullptr);

    /* TODO implement deferred allocator */
    lake_darray_init_t(&acquire_semaphores, moon_binary_semaphore const, swapchain_count);
    lake_darray_init_t(&present_semaphores, moon_binary_semaphore const, swapchain_count);
    lake_darray_init_t(&timeline_pairs, moon_timeline_pair, swapchain_count);

    /* collect semaphores for the current frame */
    lake_darray_foreach_v(work->swapchains, moon_swapchain, sc) {
        moon_timeline_pair timeline_pair;
        result = moon.interface->swapchain_current_timeline_pair(sc->impl, &timeline_pair);
        lake_darray_append_t(&acquire_semaphores, moon_binary_semaphore const, moon.interface->swapchain_current_acquire_semaphore(sc->impl));
        lake_darray_append_t(&present_semaphores, moon_binary_semaphore const, moon.interface->swapchain_current_present_semaphore(sc->impl));
        lake_darray_append_t(&timeline_pairs, moon_timeline_pair, &timeline_pair);
    }
    moon_device_submit const submit = {
        .queue = MOON_QUEUE_MAIN,
        .staged_command_list_count = lake_darray_size(&work->cmd_lists.da),
        .staged_command_lists = lake_darray_first_t(&work->cmd_lists.da, struct moon_staged_command_list_impl const *),
        .wait_binary_semaphore_count = swapchain_count,
        .wait_binary_semaphores = lake_darray_first_t(&acquire_semaphores, struct moon_binary_semaphore_impl const *),
        .wait_timeline_semaphore_count = 0,
        .wait_timeline_semaphores = nullptr,
        .signal_binary_semaphore_count = swapchain_count,
        .signal_binary_semaphores = lake_darray_first_t(&present_semaphores, struct moon_binary_semaphore_impl const *),
        .signal_timeline_semaphore_count = swapchain_count,
        .signal_timeline_semaphores = lake_darray_first_t(&timeline_pairs, moon_timeline_pair),
        .wait_stages = moon_access_none,
    };
    result = moon.interface->device_submit_commands(primary.impl, &submit);
    if (result != LAKE_SUCCESS) {
        lake_error("error gpuexec at device_submit_commands");
        lake_atomic_write_explicit(&amw->stage_hint, pipeline_stage_hint_try_recover, lake_memory_model_release);
    }

    if (!lake_darray_empty(&work->swapchains.da)) {
        moon_device_present const present = {
            .queue = MOON_QUEUE_MAIN,
            .swapchain_count = swapchain_count,
            .swapchains = lake_darray_first_t(&work->swapchains.da, struct moon_swapchain_impl const *),
            .wait_binary_semaphore_count = swapchain_count,
            .wait_binary_semaphores = lake_darray_first_t(&acquire_semaphores, struct moon_binary_semaphore_impl const *),
        };
        result = moon.interface->device_present_frames(primary.impl, &present);
        if (result != LAKE_SUCCESS) {
            lake_error("error gpuexec at device_present_frames");
            lake_atomic_write_explicit(&amw->stage_hint, pipeline_stage_hint_try_recover, lake_memory_model_release);
        }
    }
    result = moon.interface->device_commit_deferred_destructors(primary.impl);
    if (result != LAKE_SUCCESS) {
        lake_error("error gpuexec at device_commit_deferred_destructors");
        lake_atomic_write_explicit(&amw->stage_hint, pipeline_stage_hint_try_recover, lake_memory_model_release);
    }
    /* TODO implement deferred allocator */
    lake_darray_fini(&acquire_semaphores);
    lake_darray_fini(&present_semaphores);
    lake_darray_fini(&timeline_pairs);
}
