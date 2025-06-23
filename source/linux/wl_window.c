#include "wl_hadal.h"
#ifdef HADAL_WAYLAND

#ifdef HADAL_LIBDECOR
static lake_result LAKECALL create_shell_libdecor_frame(struct hadal_window_impl *window)
{
    (void)window;
    return LAKE_ERROR_INITIALIZATION_FAILED;
}
#endif /* HADAL_LIBDECOR */

static void resize_framebuffer(struct hadal_window_impl *window)
{
    /* TODO scale the framebuffer */
    window->header.fb_width = window->header.assembly.width;
    window->header.fb_height = window->header.assembly.height;
}

static bool resize_window(struct hadal_window_impl *window, s32 width, s32 height)
{
    width = lake_max(width, 1);
    height = lake_max(height, 1);

    if (width == window->header.assembly.width && height == window->header.assembly.height)
        return false;

    window->header.assembly.width = width;
    window->header.assembly.height = height;
    resize_framebuffer(window);
    return true;
}

static void handle_xdg_toplevel_close(
    void                *data,
    struct xdg_toplevel *xdg_toplevel)
{
    /* unused */
    (void)xdg_toplevel;

    struct hadal_window_impl *window = (struct hadal_window_impl *)data;
    atomic_fetch_or_explicit(&window->header.flags, hadal_window_flag_should_close, memory_order_release);
}

static void handle_xdg_toplevel_configure(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    s32                  width,
    s32                  height,
    struct wl_array     *states)
{
    struct hadal_window_impl *window = (struct hadal_window_impl *)data;
    u32 *state;

    wl_array_for_each(state, states)
    {
        switch (*state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            window->pending_flags |= hadal_window_flag_maximized;
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            window->pending_flags |= hadal_window_flag_fullscreen;
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            window->pending_flags |= hadal_window_flag_shell_activated;
            break;
        }
    }

    if (width && height) {
        window->pending_width = width;
        window->pending_height = height;
    } else {
        window->pending_width = window->header.assembly.width;
        window->pending_height = window->header.assembly.height;
    }

    /* unused */
    (void)xdg_toplevel;
}

static void handle_xdg_toplevel_configure_bounds(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    s32                  width,
    s32                  height)
{
    (void)data;
    (void)xdg_toplevel;
    (void)width;
    (void)height;
}

static void handle_xdg_toplevel_wm_capabilities(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    struct wl_array     *array)
{
    (void)data;
    (void)xdg_toplevel;
    (void)array;
}

static const struct xdg_toplevel_listener g_xdg_toplevel_listener = {
    .close = handle_xdg_toplevel_close,
    .configure = handle_xdg_toplevel_configure,
    .configure_bounds = handle_xdg_toplevel_configure_bounds,
    .wm_capabilities = handle_xdg_toplevel_wm_capabilities,
};

static void handle_xdg_surface_configure(
    void               *data,
    struct xdg_surface *xdg_surface,
    u32                 serial)
{
    struct hadal_window_impl *window = (struct hadal_window_impl *)data;
    u32 flags = lake_atomic_read(&window->header.flags);
    u32 const pending = window->pending_flags;

    xdg_surface_ack_configure(xdg_surface, serial);

    if ((flags & hadal_window_flag_shell_activated) != (pending & hadal_window_flag_shell_activated)) {
        flags ^= hadal_window_flag_shell_activated;
        if ((flags & (hadal_window_flag_shell_activated | hadal_window_flag_auto_minimize)) == hadal_window_flag_auto_minimize) {
            xdg_toplevel_set_minimized(window->shell_surface.xdg.roleobj.toplevel);
        }
    }
    if ((flags & hadal_window_flag_maximized) != (pending & hadal_window_flag_maximized)) {
        flags ^= hadal_window_flag_maximized;
    }
    if ((flags & hadal_window_flag_fullscreen) != (pending & hadal_window_flag_fullscreen)) {
        flags ^= hadal_window_flag_fullscreen;
    }

    s32 width = window->pending_width;
    s32 height = window->pending_height;

    if (!(flags & (hadal_window_flag_maximized | hadal_window_flag_fullscreen))) {
        if (window->header.assembly.numer > 0 && window->header.assembly.denom > 0) {
            f32 const aspect_ratio = (f32)width / (f32)height;
            f32 const target_ratio = (f32)window->header.assembly.numer / (f32)window->header.assembly.denom;
            if (aspect_ratio < target_ratio)
                height = width / target_ratio;
            else if (aspect_ratio > target_ratio)
                width = height * target_ratio;
        }
    }

    if (resize_window(window, width, height) && (flags & hadal_window_flag_visible))
        window->header.flags |= hadal_window_flag_swapchain_out_of_date;
    lake_atomic_write_explicit(&window->header.flags, flags, lake_memory_model_release);
}

static const struct xdg_surface_listener g_xdg_surface_listener = {
    .configure = handle_xdg_surface_configure,
};

static void update_xdg_size_limits(struct hadal_window_impl *window)
{
    s32 min_width, min_height, max_width, max_height;
    u32 const flags = lake_atomic_read(&window->header.flags);

    if (flags & hadal_window_flag_resizable) {
        if (window->header.assembly.min_width > 0 || window->header.assembly.min_height > 0) {
            min_width = min_height = 0;
        } else {
            min_width = window->header.assembly.min_width;
            min_height = window->header.assembly.min_height;
        }

        if (window->header.assembly.max_width > 0 || window->header.assembly.max_height > 0) {
            max_width = max_height = 0;
        } else {
            max_width = window->header.assembly.max_width;
            max_height = window->header.assembly.max_height;
        }

        window->header.assembly.min_width = min_width;
        window->header.assembly.min_height = min_height;
        window->header.assembly.max_width = max_width;
        window->header.assembly.max_height = max_height;
    } else {
        min_width = max_width = window->header.assembly.width;
        min_height = max_height = window->header.assembly.height;
    }
    xdg_toplevel_set_min_size(window->shell_surface.xdg.roleobj.toplevel, min_width, min_height);
    xdg_toplevel_set_max_size(window->shell_surface.xdg.roleobj.toplevel, max_width, max_height);
}

static lake_result LAKECALL create_shell_xdg_toplevel(struct hadal_window_impl *window)
{
    struct hadal_impl *hadal = window->header.hadal.impl;
    lake_dbg_assert(hadal->shell.xdg != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);

    window->shell_surface.xdg.surface = xdg_wm_base_get_xdg_surface(hadal->shell.xdg, window->surface);
    if (window->shell_surface.xdg.surface == nullptr) {
        lake_error("Failed to create xdg-surface for window `%s`.", window->header.assembly.name.str);
        return LAKE_ERROR_UNKNOWN;
    }
    xdg_surface_add_listener(window->shell_surface.xdg.surface, &g_xdg_surface_listener, window);

    window->shell_surface.xdg.roleobj.toplevel = xdg_surface_get_toplevel(window->shell_surface.xdg.surface);
    if (window->shell_surface.xdg.roleobj.toplevel == nullptr) {
        lake_error("Failed to obtain xdg-toplevel from window `%s`.", window->header.assembly.name.str);
        return LAKE_ERROR_UNKNOWN;
    }
    xdg_toplevel_add_listener(window->shell_surface.xdg.roleobj.toplevel, &g_xdg_toplevel_listener, window);
    xdg_toplevel_set_title(window->shell_surface.xdg.roleobj.toplevel, window->header.title.v);

    update_xdg_size_limits(window);

    wl_surface_commit(window->surface);
    wl_display_roundtrip(hadal->wl_display);
    return LAKE_SUCCESS;
}

static lake_result LAKECALL create_shell_xdg_popup(struct hadal_window_impl *window)
{
    (void)window;
    return LAKE_ERROR_INITIALIZATION_FAILED;
}

static lake_result LAKECALL create_shell_objects(struct hadal_window_impl *window)
{
    if (window->shell_surface.xdg.roleobj.toplevel || 
#ifdef HADAL_LIBDECOR
        window->shell_surface.libdecor.frame ||
#endif /* HADAL_LIBDECOR */
        window->shell_surface.xdg.roleobj.popup.popup)
        return true;

    switch (window->shell_surface_type) {
#ifdef HADAL_LIBDECOR
        case WAYLAND_SURFACE_LIBDECOR:
            return create_shell_libdecor_frame(window);
#endif /* HADAL_LIBDECOR */
        case WAYLAND_SURFACE_XDG_TOPLEVEL:
            return create_shell_xdg_toplevel(window);
        case WAYLAND_SURFACE_XDG_POPUP:
            return create_shell_xdg_popup(window);
        default: return LAKE_ERROR_SURFACE_LOST;
    }
    LAKE_UNREACHABLE;
}

static void LAKECALL destroy_shell_objects(struct hadal_window_impl *window)
{
#ifdef HADAL_LIBDECOR
    if (window->shell_surface_type == WAYLAND_SURFACE_LIBDECOR && window->shell_surface.libdecor.frame) {
        libdecor_frame_unref(window->shell_surface.libdecor.frame);
        window->shell_surface.libdecor.frame = nullptr;
    } else
#endif
    if (window->shell_surface_type != WAYLAND_SURFACE_UNKNOWN) {
        if (window->shell_surface_type == WAYLAND_SURFACE_XDG_TOPLEVEL && window->shell_surface.xdg.roleobj.toplevel) {
            xdg_toplevel_destroy(window->shell_surface.xdg.roleobj.toplevel);
            window->shell_surface.xdg.roleobj.toplevel = nullptr;
        } else if (window->shell_surface.xdg.roleobj.popup.popup) {
            xdg_popup_destroy(window->shell_surface.xdg.roleobj.popup.popup);
            window->shell_surface.xdg.roleobj.popup.popup = nullptr;
            window->shell_surface.xdg.roleobj.popup.positioner = nullptr;
            window->shell_surface.xdg.roleobj.popup.child.impl = nullptr;
            window->shell_surface.xdg.roleobj.popup.parent_id = 0u;
        }
        if (window->shell_surface.xdg.surface)
            xdg_surface_destroy(window->shell_surface.xdg.surface);
        window->shell_surface.xdg.surface = nullptr;
    }
}

static void handle_wl_surface_enter(
    void              *data, 
    struct wl_surface *surface, 
    struct wl_output  *output)
{
    struct hadal_window_impl *window = (struct hadal_window_impl *)data;

    if (!wayland_own_output(output))
        return;

    /* TODO output scales ? */

    /* unused */
    (void)surface;
    (void)window;
}

static void handle_wl_surface_leave(
    void              *data,
    struct wl_surface *surface,
    struct wl_output  *output)
{
    struct hadal_window_impl *window = (struct hadal_window_impl *)data;

    if (!wayland_own_output(output))
        return;

    /* TODO output scales ? */

    /* unused */
    (void)surface;
    (void)window;
}

static const struct wl_surface_listener g_wl_surface_listener = {
    .enter = handle_wl_surface_enter,
    .leave = handle_wl_surface_leave,
};

FN_HADAL_WINDOW_ASSEMBLY(wayland)
{
    struct hadal_window_impl *window = __lake_malloc_t(struct hadal_window_impl); 
    lake_zerop(window);

    window->header.hadal.impl = hadal;
    window->header.assembly = *assembly;
    window->header.zero_refcnt = (PFN_lake_work)_hadal_wayland_window_zero_refcnt;
    window->header.title.v = "Lake in the Lungs"; /* TODO */

    u32 flags = hadal_window_flag_is_valid;
    if (assembly->flag_hints & hadal_window_flag_modal) {
        window->shell_surface_type = WAYLAND_SURFACE_XDG_POPUP;
        flags |= hadal_window_flag_modal;
    } else {
#ifdef HADAL_LIBDECOR
        window->shell_surface_type = hadal->shell.libdecor 
            ? WAYLAND_SURFACE_LIBDECOR : WAYLAND_SURFACE_XDG_TOPLEVEL;
#else
        window->shell_surface_type = WAYLAND_SURFACE_XDG_TOPLEVEL;
#endif /* HADAL_LIBDECOR */
    }

    window->surface = wl_compositor_create_surface(hadal->wl_compositor);
    if (window->surface == nullptr) {
        __lake_free(window);
        return nullptr;
    }
    wayland_register_surface(window->surface);
    wl_surface_add_listener(window->surface, &g_wl_surface_listener, window);

    if ((assembly->flag_hints & hadal_window_flag_fullscreen) || assembly->fullscreen.impl != nullptr)
        flags |= hadal_window_flag_fullscreen;
    else if ((assembly->flag_hints & hadal_window_flag_maximized))
        flags |= hadal_window_flag_maximized;

    if ((assembly->flag_hints & hadal_window_flag_shell_activated))
        flags |= hadal_window_flag_shell_activated;
#ifdef MOON_VULKAN
    if (hadal->vulkan.vk_instance != nullptr)
        flags |= hadal_window_flag_vulkan;
#endif /* MOON_VULKAN */
    lake_atomic_write(&window->header.flags, flags);

    resize_framebuffer(window);

    if ((assembly->flag_hints & hadal_window_flag_visible)) {
        lake_atomic_or(&window->header.flags, hadal_window_flag_visible);
        
        lake_result result = create_shell_objects(window);
        if (result != LAKE_SUCCESS) {
            destroy_shell_objects(window);
            wl_surface_destroy(window->surface);
            __lake_free(window);
            return nullptr;
        }
    }
    lake_dbg_2("Created Hadal window `%s`.", window->header.assembly.name.str);
    lake_inc_refcnt(&hadal->interface.header.refcnt);
    lake_inc_refcnt(&window->header.refcnt);
    return window;
}

FN_HADAL_WINDOW_ZERO_REFCNT(wayland)
{
#ifndef LAKE_NDEBUG
    lake_dbg_assert(window != nullptr, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
    s32 refcnt = lake_atomic_read(&window->header.refcnt);
    lake_dbg_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, 
            "Window `%s` titled `%s` reference count is %d.", 
            window->header.assembly.name.str, window->header.title.v, refcnt);
#endif /* LAKE_NDEBUG */
    destroy_shell_objects(window);
    if (window->surface)
        wl_surface_destroy(window->surface);

    lake_dbg_2("Destroyed Hadal window `%s`.", window->header.assembly.name.str);
    hadal_interface_unref(window->header.hadal);
    __lake_free(window);
}
#endif /* HADAL_WAYLAND */
