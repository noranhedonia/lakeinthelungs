#include "wl_hadal.h"

#ifdef HADAL_WAYLAND

/** We allow only one Wayland display backend to exist at a time. */
struct hadal_impl *g_hadal = nullptr;

char const *g_wl_output_tag = "hadal_display";
char const *g_wl_surface_tag = "hadal_window";

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
    xdg_toplevel_set_title(window->shell_surface.xdg.roleobj.toplevel, window->header.title);

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
    window->header.title = "Lake in the Lungs"; /* TODO */

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
            window->header.assembly.name.str, window->header.title, refcnt);
#endif /* LAKE_NDEBUG */
    destroy_shell_objects(window);
    if (window->surface)
        wl_surface_destroy(window->surface);

    lake_dbg_2("Destroyed Hadal window `%s`.", window->header.assembly.name.str);
    hadal_interface_unref(window->header.hadal);
    __lake_free(window);
}

#if defined(HADAL_WAYLAND) && defined(MOON_VULKAN) 
FN_HADAL_VULKAN_CONNECT_INSTANCE(wayland)
{
    lake_dbg_assert(hadal && vk_instance && vkGetInstanceProcAddr, LAKE_INVALID_PARAMETERS, nullptr);

    hadal->vulkan.vk_instance = vk_instance;
    hadal->vulkan.vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)
        (void *)vkGetInstanceProcAddr(vk_instance, "vkCreateWaylandSurfaceKHR");
    hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)
        (void *)vkGetInstanceProcAddr(vk_instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    if (!hadal->vulkan.vkCreateWaylandSurfaceKHR || !hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR)
    {
        hadal->vulkan.vk_instance = nullptr;
        hadal->vulkan.vkCreateWaylandSurfaceKHR = nullptr;
        hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
        return LAKE_ERROR_EXTENSION_NOT_PRESENT;
    }
    return LAKE_SUCCESS;
}

FN_HADAL_VULKAN_PRESENTATION_SUPPORT(wayland)
{
    lake_dbg_assert(hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR != nullptr, LAKE_ERROR_FEATURE_NOT_PRESENT, nullptr);
    return (bool)hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR(vk_physical_device, queue_family, hadal->wl_display);
}

FN_HADAL_VULKAN_CREATE_SURFACE(wayland)
{
    struct hadal_impl *hadal = window->header.hadal.impl;
    lake_dbg_assert(hadal->vulkan.vkCreateWaylandSurfaceKHR != nullptr, LAKE_ERROR_FEATURE_NOT_PRESENT, nullptr);

    struct VkWaylandSurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .display = hadal->wl_display,
        .surface = window->surface,
    };
    s32 result = hadal->vulkan.vkCreateWaylandSurfaceKHR(hadal->vulkan.vk_instance, &surface_info, callbacks, out_surface);

    if (result == -1)
        return LAKE_ERROR_OUT_OF_HOST_MEMORY;
    else if (result == -2)
        return LAKE_ERROR_OUT_OF_DEVICE_MEMORY;
    return LAKE_SUCCESS;
}
#endif /* HADAL_WAYLAND && MOON_VULKAN */

static void handle_xdg_wm_base_ping(
    void               *raw_hadal,
    struct xdg_wm_base *xdg_wm_base,
    u32                 serial)
{
    (void)raw_hadal;
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static struct xdg_wm_base_listener const g_wm_base_listener = {
    .ping = handle_xdg_wm_base_ping,
};

static void handle_wl_registry_global(
    void               *raw_hadal,
    struct wl_registry *registry,
    u32                 name,
    char const         *interface,
    u32                 version)
{
    struct hadal_impl *hadal = (struct hadal_impl *)raw_hadal;

    if (!strcmp(interface, "wl_compositor")) {
        hadal->wl_compositor = wl_registry_bind(registry, name, &wl_compositor_interface, lake_min(3, version));
    } else if (!strcmp(interface, "wl_subcompositor")) {
        hadal->wl_subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    } else if (!strcmp(interface, "wl_shm")) {
        hadal->wl_shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (!strcmp(interface, "wl_output")) {
        lake_dbg_3("TODO add wayland output: %u ver. %u", name, version);
        hadal->interface.displays.len++; /* TODO */
    } else if (!strcmp(interface, "xdg_wm_base")) {
        hadal->shell.xdg = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(hadal->shell.xdg, &g_wm_base_listener, hadal);
    }
}

static void handle_wl_registry_global_remove(
    void               *data,
    struct wl_registry *registry,
    u32                 name)
{
    (void)registry;
    (void)data;
    (void)name;
    /* TODO delete outputs */
}

static struct wl_registry_listener const g_wl_registry_listener = {
    .global = handle_wl_registry_global,
    .global_remove = handle_wl_registry_global_remove,
};

static bool load_wayland_symbols(struct hadal_impl *hadal, char const *name)
{
    lake_dbg_assert(hadal->wl.display_connect, LAKE_ERROR_INITIALIZATION_FAILED, "Assumed Wayland procedure is missing.");

    void *module = hadal->client_library;
    hadal->wl.proxy_marshal = (PFN_wl_proxy_marshal)
        lake_get_proc_address(module, "wl_proxy_marshal");
    hadal->wl.proxy_create = (PFN_wl_proxy_create)
        lake_get_proc_address(module, "wl_proxy_create");
    hadal->wl.proxy_destroy = (PFN_wl_proxy_destroy)
        lake_get_proc_address(module, "wl_proxy_destroy");
    hadal->wl.proxy_add_listener = (PFN_wl_proxy_add_listener)
        lake_get_proc_address(module, "wl_proxy_add_listener");
    hadal->wl.proxy_set_user_data = (PFN_wl_proxy_set_user_data)
        lake_get_proc_address(module, "wl_proxy_set_user_data");
    hadal->wl.proxy_get_user_data = (PFN_wl_proxy_get_user_data)
        lake_get_proc_address(module, "wl_proxy_get_user_data");
    hadal->wl.proxy_get_version = (PFN_wl_proxy_get_version)
        lake_get_proc_address(module, "wl_proxy_get_version");
    hadal->wl.proxy_get_id = (PFN_wl_proxy_get_id)
        lake_get_proc_address(module, "wl_proxy_get_id");
    hadal->wl.proxy_get_class = (PFN_wl_proxy_get_class)
        lake_get_proc_address(module, "wl_proxy_get_class");
    hadal->wl.proxy_set_queue = (PFN_wl_proxy_set_queue)
        lake_get_proc_address(module, "wl_proxy_set_queue");
    hadal->wl.proxy_create_wrapper = (PFN_wl_proxy_create_wrapper)
        lake_get_proc_address(module, "wl_proxy_create_wrapper");
    hadal->wl.proxy_wrapper_destroy = (PFN_wl_proxy_wrapper_destroy)
        lake_get_proc_address(module, "wl_proxy_wrapper_destroy");
    hadal->wl.proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)
        lake_get_proc_address(module, "wl_proxy_marshal_constructor");
    hadal->wl.proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)
        lake_get_proc_address(module, "wl_proxy_marshal_constructor_versioned");
    hadal->wl.proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)
        lake_get_proc_address(module, "wl_proxy_marshal_flags");
    hadal->wl.proxy_marshal_array_flags = (PFN_wl_proxy_marshal_array_flags)
        lake_get_proc_address(module, "wl_proxy_marshal_array_flags");
    hadal->wl.proxy_set_tag = (PFN_wl_proxy_set_tag)
        lake_get_proc_address(module, "wl_proxy_set_tag");
    hadal->wl.proxy_get_tag = (PFN_wl_proxy_get_tag)
        lake_get_proc_address(module, "wl_proxy_get_tag");
    hadal->wl.display_connect_to_fd = (PFN_wl_display_connect_to_fd)
        lake_get_proc_address(module, "wl_display_connect_to_fd");
    hadal->wl.display_disconnect = (PFN_wl_display_disconnect)
        lake_get_proc_address(module, "wl_display_disconnect");
    hadal->wl.display_get_fd = (PFN_wl_display_get_fd)
        lake_get_proc_address(module, "wl_display_get_fd");
    hadal->wl.display_dispatch = (PFN_wl_display_dispatch)
        lake_get_proc_address(module, "wl_display_dispatch");
    hadal->wl.display_dispatch_queue = (PFN_wl_display_dispatch_queue)
        lake_get_proc_address(module, "wl_display_dispatch_queue");
    hadal->wl.display_dispatch_queue_pending = (PFN_wl_display_dispatch_queue_pending)
        lake_get_proc_address(module, "wl_display_dispatch_queue_pending");
    hadal->wl.display_dispatch_pending = (PFN_wl_display_dispatch_pending)
        lake_get_proc_address(module, "wl_display_dispatch_pending");
    hadal->wl.display_prepare_read = (PFN_wl_display_prepare_read)
        lake_get_proc_address(module, "wl_display_prepare_read");
    hadal->wl.display_prepare_read_queue = (PFN_wl_display_prepare_read_queue)
        lake_get_proc_address(module, "wl_display_prepare_read_queue");
    hadal->wl.display_read_events = (PFN_wl_display_read_events)
        lake_get_proc_address(module, "wl_display_read_events");
    hadal->wl.display_cancel_read = (PFN_wl_display_cancel_read)
        lake_get_proc_address(module, "wl_display_cancel_read");
    hadal->wl.display_get_error = (PFN_wl_display_get_error)
        lake_get_proc_address(module, "wl_display_get_error");
    hadal->wl.display_flush = (PFN_wl_display_flush)
        lake_get_proc_address(module, "wl_display_flush");
    hadal->wl.display_roundtrip = (PFN_wl_display_roundtrip)
        lake_get_proc_address(module, "wl_display_roundtrip");
    hadal->wl.display_create_queue = (PFN_wl_display_create_queue)
        lake_get_proc_address(module, "wl_display_create_queue");
    hadal->wl.event_queue_destroy = (PFN_wl_event_queue_destroy)
        lake_get_proc_address(module, "wl_event_queue_destroy");
    hadal->wl.log_set_handler_client = (PFN_wl_log_set_handler_client)
        lake_get_proc_address(module, "wl_log_set_handler_client");
    hadal->wl.list_init = (PFN_wl_list_init)
        lake_get_proc_address(module, "wl_list_init");
    hadal->wl.list_insert = (PFN_wl_list_insert)
        lake_get_proc_address(module, "wl_list_insert");
    hadal->wl.list_remove = (PFN_wl_list_remove)
        lake_get_proc_address(module, "wl_list_remove");
    hadal->wl.list_length = (PFN_wl_list_length)
        lake_get_proc_address(module, "wl_list_length");
    hadal->wl.list_empty = (PFN_wl_list_empty)
        lake_get_proc_address(module, "wl_list_empty");
    hadal->wl.list_insert_list = (PFN_wl_list_insert_list)
        lake_get_proc_address(module, "wl_list_insert_list");

    if (!hadal->wl.proxy_marshal ||
        !hadal->wl.proxy_create ||
        !hadal->wl.proxy_destroy ||
        !hadal->wl.proxy_add_listener ||
        !hadal->wl.proxy_set_user_data ||
        !hadal->wl.proxy_get_user_data ||
        !hadal->wl.proxy_get_version ||
        !hadal->wl.proxy_get_id ||
        !hadal->wl.proxy_get_class ||
        !hadal->wl.proxy_set_queue ||
        !hadal->wl.proxy_create_wrapper ||
        !hadal->wl.proxy_wrapper_destroy ||
        !hadal->wl.proxy_marshal_constructor ||
        !hadal->wl.proxy_marshal_constructor_versioned ||
        !hadal->wl.proxy_marshal_flags ||
        !hadal->wl.proxy_marshal_array_flags ||
        !hadal->wl.proxy_set_tag ||
        !hadal->wl.proxy_get_tag ||
        !hadal->wl.display_connect_to_fd ||
        !hadal->wl.display_disconnect ||
        !hadal->wl.display_get_fd ||
        !hadal->wl.display_dispatch ||
        !hadal->wl.display_dispatch_queue ||
        !hadal->wl.display_dispatch_queue_pending ||
        !hadal->wl.display_dispatch_pending ||
        !hadal->wl.display_prepare_read ||
        !hadal->wl.display_prepare_read_queue ||
        !hadal->wl.display_read_events ||
        !hadal->wl.display_cancel_read ||
        !hadal->wl.display_get_error ||
        !hadal->wl.display_flush ||
        !hadal->wl.display_roundtrip ||
        !hadal->wl.display_create_queue ||
        !hadal->wl.event_queue_destroy ||
        !hadal->wl.log_set_handler_client ||
        !hadal->wl.list_init ||
        !hadal->wl.list_insert ||
        !hadal->wl.list_remove ||
        !hadal->wl.list_length ||
        !hadal->wl.list_empty ||
        !hadal->wl.list_insert_list)
    {
        lake_dbg_1("'%s' can't load libwayland-client procedures.", name);
        return false;
    }

    module = hadal->cursor_library;
    hadal->wl.cursor_theme_load = (PFN_wl_cursor_theme_load)
        lake_get_proc_address(module, "wl_cursor_theme_load");
    hadal->wl.cursor_theme_destroy = (PFN_wl_cursor_theme_destroy)
        lake_get_proc_address(module, "wl_cursor_theme_destroy");
    hadal->wl.cursor_theme_get_cursor = (PFN_wl_cursor_theme_get_cursor)
        lake_get_proc_address(module, "wl_cursor_theme_get_cursor");
    hadal->wl.cursor_image_get_buffer = (PFN_wl_cursor_image_get_buffer)
        lake_get_proc_address(module, "wl_cursor_image_get_buffer");
    hadal->wl.cursor_frame = (PFN_wl_cursor_frame)
        lake_get_proc_address(module, "wl_cursor_frame");

    if (!hadal->wl.cursor_theme_load ||
        !hadal->wl.cursor_theme_destroy ||
        !hadal->wl.cursor_theme_get_cursor ||
        !hadal->wl.cursor_image_get_buffer ||
        !hadal->wl.cursor_frame)
    {
        lake_dbg_1("%s: can't load libwayland-cursor procedures.", name);
        return false;
    }

    module = hadal->xkbcommon_library;
    hadal->xkb.context_new = (PFN_xkb_context_new)
        lake_get_proc_address(module, "xkb_context_new");
    hadal->xkb.context_unref = (PFN_xkb_context_unref)
        lake_get_proc_address(module, "xkb_context_unref");
    hadal->xkb.keymap_new_from_string = (PFN_xkb_keymap_new_from_string)
        lake_get_proc_address(module, "xkb_keymap_new_from_string");
    hadal->xkb.keymap_unref = (PFN_xkb_keymap_unref)
        lake_get_proc_address(module, "xkb_keymap_unref");
    hadal->xkb.keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)
        lake_get_proc_address(module, "xkb_keymap_mod_get_index");
    hadal->xkb.keymap_key_repeats = (PFN_xkb_keymap_key_repeats)
        lake_get_proc_address(module, "xkb_keymap_key_repeats");
    hadal->xkb.keymap_key_for_each = (PFN_xkb_keymap_key_for_each)
        lake_get_proc_address(module, "xkb_keymap_key_for_each");
    hadal->xkb.keymap_key_get_syms_by_level = (PFN_xkb_keymap_key_get_syms_by_level)
        lake_get_proc_address(module, "xkb_keymap_key_get_syms_by_level");
    hadal->xkb.keymap_layout_get_name = (PFN_xkb_keymap_layout_get_name)
        lake_get_proc_address(module, "xkb_keymap_layout_get_name");
    hadal->xkb.keysym_to_utf8 = (PFN_xkb_keysym_to_utf8)
        lake_get_proc_address(module, "xkb_keysym_to_utf8");
    hadal->xkb.keysym_to_utf32 = (PFN_xkb_keysym_to_utf32)
        lake_get_proc_address(module, "xkb_keysym_to_utf32");
    hadal->xkb.state_new = (PFN_xkb_state_new)
        lake_get_proc_address(module, "xkb_state_new");
    hadal->xkb.state_unref = (PFN_xkb_state_unref)
        lake_get_proc_address(module, "xkb_state_unref");
    hadal->xkb.state_key_get_syms = (PFN_xkb_state_key_get_syms)
        lake_get_proc_address(module, "xkb_state_key_get_syms");
    hadal->xkb.state_key_get_layout = (PFN_xkb_state_key_get_layout)
        lake_get_proc_address(module, "xkb_state_key_get_layout");
    hadal->xkb.state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)
        lake_get_proc_address(module, "xkb_state_mod_index_is_active");
    hadal->xkb.state_update_mask = (PFN_xkb_state_update_mask)
        lake_get_proc_address(module, "xkb_state_update_mask");
    hadal->xkb.compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)
        lake_get_proc_address(module, "xkb_compose_table_new_from_locale");
    hadal->xkb.compose_table_unref = (PFN_xkb_compose_table_unref)
        lake_get_proc_address(module, "xkb_compose_table_unref");
    hadal->xkb.compose_state_new = (PFN_xkb_compose_state_new)
        lake_get_proc_address(module, "xkb_compose_state_new");
    hadal->xkb.compose_state_reset = (PFN_xkb_compose_state_reset)
        lake_get_proc_address(module, "xkb_compose_state_reset");
    hadal->xkb.compose_state_feed = (PFN_xkb_compose_state_feed)
        lake_get_proc_address(module, "xkb_compose_state_feed");
    hadal->xkb.compose_state_get_status = (PFN_xkb_compose_state_get_status)
        lake_get_proc_address(module, "xkb_compose_state_get_status");
    hadal->xkb.compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)
        lake_get_proc_address(module, "xkb_compose_state_get_one_sym");

    if (!hadal->xkb.context_new ||
        !hadal->xkb.context_unref ||
        !hadal->xkb.keymap_new_from_string ||
        !hadal->xkb.keymap_unref ||
        !hadal->xkb.keymap_mod_get_index ||
        !hadal->xkb.keymap_key_repeats ||
        !hadal->xkb.keymap_key_for_each ||
        !hadal->xkb.keymap_key_get_syms_by_level ||
        !hadal->xkb.keymap_layout_get_name ||
        !hadal->xkb.keysym_to_utf8 ||
        !hadal->xkb.keysym_to_utf32 ||
        !hadal->xkb.state_new ||
        !hadal->xkb.state_unref ||
        !hadal->xkb.state_key_get_syms ||
        !hadal->xkb.state_key_get_layout ||
        !hadal->xkb.state_mod_index_is_active ||
        !hadal->xkb.state_update_mask ||
        !hadal->xkb.compose_table_new_from_locale ||
        !hadal->xkb.compose_table_unref ||
        !hadal->xkb.compose_state_new ||
        !hadal->xkb.compose_state_reset ||
        !hadal->xkb.compose_state_feed ||
        !hadal->xkb.compose_state_get_status ||
        !hadal->xkb.compose_state_get_one_sym)
    {
        lake_dbg_1("%s: can't load libxkbcommon procedures.", name);
        return false;
    }

#ifdef HADAL_LIBDECOR
    if (!hadal->libdecor_library) 
        return true;

    module = hadal->libdecor_library;
    hadal->libdecor._new = (PFN_libdecor_new)
        lake_get_proc_address(module, "libdecor_new");
    hadal->libdecor.unref = (PFN_libdecor_unref)
        lake_get_proc_address(module, "libdecor_unref");
    hadal->libdecor.get_fd = (PFN_libdecor_get_fd)
        lake_get_proc_address(module, "libdecor_get_fd");
    hadal->libdecor.decorate = (PFN_libdecor_decorate)
        lake_get_proc_address(module, "libdecor_decorate");
    hadal->libdecor.frame_unref = (PFN_libdecor_frame_unref)
        lake_get_proc_address(module, "libdecor_frame_unref");
    hadal->libdecor.frame_set_title = (PFN_libdecor_frame_set_title)
        lake_get_proc_address(module, "libdecor_frame_set_title");
    hadal->libdecor.frame_set_app_id = (PFN_libdecor_frame_set_app_id)
        lake_get_proc_address(module, "libdecor_frame_set_app_id");
    hadal->libdecor.frame_set_max_content_size = (PFN_libdecor_frame_set_max_content_size)
        lake_get_proc_address(module, "libdecor_frame_set_max_content_size");
    hadal->libdecor.frame_get_max_content_size = (PFN_libdecor_frame_get_max_content_size)
        lake_get_proc_address(module, "libdecor_frame_get_max_content_size");
    hadal->libdecor.frame_set_min_content_size = (PFN_libdecor_frame_set_min_content_size)
        lake_get_proc_address(module, "libdecor_frame_set_min_content_size");
    hadal->libdecor.frame_get_min_content_size = (PFN_libdecor_frame_get_min_content_size)
        lake_get_proc_address(module, "libdecor_frame_get_min_content_size");
    hadal->libdecor.frame_resize = (PFN_libdecor_frame_resize)
        lake_get_proc_address(module, "libdecor_frame_resize");
    hadal->libdecor.frame_move = (PFN_libdecor_frame_move)
        lake_get_proc_address(module, "libdecor_frame_move");
    hadal->libdecor.frame_commit = (PFN_libdecor_frame_commit)
        lake_get_proc_address(module, "libdecor_frame_commit");
    hadal->libdecor.frame_set_minimized = (PFN_libdecor_frame_set_minimized)
        lake_get_proc_address(module, "libdecor_frame_set_minimized");
    hadal->libdecor.frame_set_maximized = (PFN_libdecor_frame_set_maximized)
        lake_get_proc_address(module, "libdecor_frame_set_maximized");
    hadal->libdecor.frame_unset_maximized = (PFN_libdecor_frame_unset_maximized)
        lake_get_proc_address(module, "libdecor_frame_unset_maximized");
    hadal->libdecor.frame_set_fullscreen = (PFN_libdecor_frame_set_fullscreen)
        lake_get_proc_address(module, "libdecor_frame_set_fullscreen");
    hadal->libdecor.frame_unset_fullscreen = (PFN_libdecor_frame_unset_fullscreen)
        lake_get_proc_address(module, "libdecor_frame_unset_fullscreen");
    hadal->libdecor.frame_set_capabilities = (PFN_libdecor_frame_set_capabilities)
        lake_get_proc_address(module, "libdecor_frame_set_capabilities");
    hadal->libdecor.frame_unset_capabilities = (PFN_libdecor_frame_unset_capabilities)
        lake_get_proc_address(module, "libdecor_frame_unset_capabilities");
    hadal->libdecor.frame_has_capability = (PFN_libdecor_frame_has_capability)
        lake_get_proc_address(module, "libdecor_frame_has_capability");
    hadal->libdecor.frame_set_visibility = (PFN_libdecor_frame_set_visibility)
        lake_get_proc_address(module, "libdecor_frame_set_visibility");
    hadal->libdecor.frame_is_visible = (PFN_libdecor_frame_is_visible)
        lake_get_proc_address(module, "libdecor_frame_is_visible");
    hadal->libdecor.frame_is_floating = (PFN_libdecor_frame_is_floating)
        lake_get_proc_address(module, "libdecor_frame_is_floating");
    hadal->libdecor.frame_set_parent = (PFN_libdecor_frame_set_parent)
        lake_get_proc_address(module, "libdecor_frame_set_parent");
    hadal->libdecor.frame_get_xdg_surface = (PFN_libdecor_frame_get_xdg_surface)
        lake_get_proc_address(module, "libdecor_frame_get_xdg_surface");
    hadal->libdecor.frame_get_xdg_toplevel = (PFN_libdecor_frame_get_xdg_toplevel)
        lake_get_proc_address(module, "libdecor_frame_get_xdg_toplevel");
    hadal->libdecor.frame_map = (PFN_libdecor_frame_map)
        lake_get_proc_address(module, "libdecor_frame_map");
    hadal->libdecor.state_new = (PFN_libdecor_state_new)
        lake_get_proc_address(module, "libdecor_state_new");
    hadal->libdecor.state_free = (PFN_libdecor_state_free)
        lake_get_proc_address(module, "libdecor_state_free");
    hadal->libdecor.configuration_get_content_size = (PFN_libdecor_configuration_get_content_size)
        lake_get_proc_address(module, "libdecor_configuration_get_content_size");
    hadal->libdecor.configuration_get_window_state = (PFN_libdecor_configuration_get_window_state)
        lake_get_proc_address(module, "libdecor_configuration_get_window_state");
    hadal->libdecor.dispatch = (PFN_libdecor_dispatch)
        lake_get_proc_address(module, "libdecor_dispatch");

    if (!hadal->libdecor._new ||
        !hadal->libdecor.unref ||
        !hadal->libdecor.get_fd ||
        !hadal->libdecor.decorate ||
        !hadal->libdecor.frame_unref ||
        !hadal->libdecor.frame_set_title ||
        !hadal->libdecor.frame_set_app_id ||
        !hadal->libdecor.frame_set_max_content_size ||
        !hadal->libdecor.frame_get_max_content_size ||
        !hadal->libdecor.frame_set_min_content_size ||
        !hadal->libdecor.frame_get_min_content_size ||
        !hadal->libdecor.frame_resize ||
        !hadal->libdecor.frame_move ||
        !hadal->libdecor.frame_commit ||
        !hadal->libdecor.frame_set_minimized ||
        !hadal->libdecor.frame_set_maximized ||
        !hadal->libdecor.frame_unset_maximized ||
        !hadal->libdecor.frame_set_fullscreen ||
        !hadal->libdecor.frame_unset_fullscreen ||
        !hadal->libdecor.frame_set_capabilities ||
        !hadal->libdecor.frame_unset_capabilities ||
        !hadal->libdecor.frame_has_capability ||
        !hadal->libdecor.frame_set_visibility ||
        !hadal->libdecor.frame_is_visible ||
        !hadal->libdecor.frame_is_floating ||
        !hadal->libdecor.frame_set_parent ||
        !hadal->libdecor.frame_get_xdg_surface ||
        !hadal->libdecor.frame_get_xdg_toplevel ||
        !hadal->libdecor.frame_map ||
        !hadal->libdecor.state_new ||
        !hadal->libdecor.state_free ||
        !hadal->libdecor.configuration_get_content_size ||
        !hadal->libdecor.configuration_get_window_state ||
        !hadal->libdecor.dispatch) 
    {
        /* disable libdecor */
        lake_dbg_1("%s: can't load libdecor procedures, the XDG protocl will be used instead.", name);
        lake_close_library(hadal->libdecor_library);
        hadal->libdecor_library = nullptr;
    }
#endif /* HADAL_LIBDECOR */
    return true;
}

static bool create_key_tables(struct hadal_impl *hadal)
{
    lake_memset(hadal->keycodes, -1, sizeof(hadal->keycodes));
    lake_memset(hadal->scancodes, -1, sizeof(hadal->scancodes));

    hadal->keycodes[KEY_GRAVE]      = hadal_keycode_grave_accent;
    hadal->keycodes[KEY_1]          = hadal_keycode_1;
    hadal->keycodes[KEY_2]          = hadal_keycode_2;
    hadal->keycodes[KEY_3]          = hadal_keycode_3;
    hadal->keycodes[KEY_4]          = hadal_keycode_4;
    hadal->keycodes[KEY_5]          = hadal_keycode_5;
    hadal->keycodes[KEY_6]          = hadal_keycode_6;
    hadal->keycodes[KEY_7]          = hadal_keycode_7;
    hadal->keycodes[KEY_8]          = hadal_keycode_8;
    hadal->keycodes[KEY_9]          = hadal_keycode_9;
    hadal->keycodes[KEY_0]          = hadal_keycode_0;
    hadal->keycodes[KEY_SPACE]      = hadal_keycode_space;
    hadal->keycodes[KEY_MINUS]      = hadal_keycode_minus;
    hadal->keycodes[KEY_EQUAL]      = hadal_keycode_equal;
    hadal->keycodes[KEY_Q]          = hadal_keycode_q;
    hadal->keycodes[KEY_W]          = hadal_keycode_w;
    hadal->keycodes[KEY_E]          = hadal_keycode_e;
    hadal->keycodes[KEY_R]          = hadal_keycode_r;
    hadal->keycodes[KEY_T]          = hadal_keycode_t;
    hadal->keycodes[KEY_Y]          = hadal_keycode_y;
    hadal->keycodes[KEY_U]          = hadal_keycode_u;
    hadal->keycodes[KEY_I]          = hadal_keycode_i;
    hadal->keycodes[KEY_O]          = hadal_keycode_o;
    hadal->keycodes[KEY_P]          = hadal_keycode_p;
    hadal->keycodes[KEY_LEFTBRACE]  = hadal_keycode_left_bracket;
    hadal->keycodes[KEY_RIGHTBRACE] = hadal_keycode_right_bracket;
    hadal->keycodes[KEY_A]          = hadal_keycode_a;
    hadal->keycodes[KEY_S]          = hadal_keycode_s;
    hadal->keycodes[KEY_D]          = hadal_keycode_d;
    hadal->keycodes[KEY_F]          = hadal_keycode_f;
    hadal->keycodes[KEY_G]          = hadal_keycode_g;
    hadal->keycodes[KEY_H]          = hadal_keycode_h;
    hadal->keycodes[KEY_J]          = hadal_keycode_j;
    hadal->keycodes[KEY_K]          = hadal_keycode_k;
    hadal->keycodes[KEY_L]          = hadal_keycode_l;
    hadal->keycodes[KEY_SEMICOLON]  = hadal_keycode_semicolon;
    hadal->keycodes[KEY_APOSTROPHE] = hadal_keycode_apostrophe;
    hadal->keycodes[KEY_Z]          = hadal_keycode_z;
    hadal->keycodes[KEY_X]          = hadal_keycode_x;
    hadal->keycodes[KEY_C]          = hadal_keycode_c;
    hadal->keycodes[KEY_V]          = hadal_keycode_v;
    hadal->keycodes[KEY_B]          = hadal_keycode_b;
    hadal->keycodes[KEY_N]          = hadal_keycode_n;
    hadal->keycodes[KEY_M]          = hadal_keycode_m;
    hadal->keycodes[KEY_COMMA]      = hadal_keycode_comma;
    hadal->keycodes[KEY_DOT]        = hadal_keycode_period;
    hadal->keycodes[KEY_SLASH]      = hadal_keycode_slash;
    hadal->keycodes[KEY_BACKSLASH]  = hadal_keycode_backslash;
    hadal->keycodes[KEY_ESC]        = hadal_keycode_escape;
    hadal->keycodes[KEY_TAB]        = hadal_keycode_tab;
    hadal->keycodes[KEY_LEFTSHIFT]  = hadal_keycode_left_shift;
    hadal->keycodes[KEY_RIGHTSHIFT] = hadal_keycode_right_shift;
    hadal->keycodes[KEY_LEFTCTRL]   = hadal_keycode_left_control;
    hadal->keycodes[KEY_RIGHTCTRL]  = hadal_keycode_right_control;
    hadal->keycodes[KEY_LEFTALT]    = hadal_keycode_left_alt;
    hadal->keycodes[KEY_RIGHTALT]   = hadal_keycode_right_alt;
    hadal->keycodes[KEY_LEFTMETA]   = hadal_keycode_left_super;
    hadal->keycodes[KEY_RIGHTMETA]  = hadal_keycode_right_super;
    hadal->keycodes[KEY_COMPOSE]    = hadal_keycode_menu;
    hadal->keycodes[KEY_NUMLOCK]    = hadal_keycode_num_lock;
    hadal->keycodes[KEY_CAPSLOCK]   = hadal_keycode_caps_lock;
    hadal->keycodes[KEY_PRINT]      = hadal_keycode_print_screen;
    hadal->keycodes[KEY_SCROLLLOCK] = hadal_keycode_scroll_lock;
    hadal->keycodes[KEY_PAUSE]      = hadal_keycode_pause;
    hadal->keycodes[KEY_DELETE]     = hadal_keycode_delete;
    hadal->keycodes[KEY_BACKSPACE]  = hadal_keycode_backspace;
    hadal->keycodes[KEY_ENTER]      = hadal_keycode_enter;
    hadal->keycodes[KEY_HOME]       = hadal_keycode_home;
    hadal->keycodes[KEY_END]        = hadal_keycode_end;
    hadal->keycodes[KEY_PAGEUP]     = hadal_keycode_page_up;
    hadal->keycodes[KEY_PAGEDOWN]   = hadal_keycode_page_down;
    hadal->keycodes[KEY_INSERT]     = hadal_keycode_insert;
    hadal->keycodes[KEY_LEFT]       = hadal_keycode_left;
    hadal->keycodes[KEY_RIGHT]      = hadal_keycode_right;
    hadal->keycodes[KEY_DOWN]       = hadal_keycode_down;
    hadal->keycodes[KEY_UP]         = hadal_keycode_up;
    hadal->keycodes[KEY_F1]         = hadal_keycode_f1;
    hadal->keycodes[KEY_F2]         = hadal_keycode_f2;
    hadal->keycodes[KEY_F3]         = hadal_keycode_f3;
    hadal->keycodes[KEY_F4]         = hadal_keycode_f4;
    hadal->keycodes[KEY_F5]         = hadal_keycode_f5;
    hadal->keycodes[KEY_F6]         = hadal_keycode_f6;
    hadal->keycodes[KEY_F7]         = hadal_keycode_f7;
    hadal->keycodes[KEY_F8]         = hadal_keycode_f8;
    hadal->keycodes[KEY_F9]         = hadal_keycode_f9;
    hadal->keycodes[KEY_F10]        = hadal_keycode_f10;
    hadal->keycodes[KEY_F11]        = hadal_keycode_f11;
    hadal->keycodes[KEY_F12]        = hadal_keycode_f12;
    hadal->keycodes[KEY_F13]        = hadal_keycode_f13;
    hadal->keycodes[KEY_F14]        = hadal_keycode_f14;
    hadal->keycodes[KEY_F15]        = hadal_keycode_f15;
    hadal->keycodes[KEY_F16]        = hadal_keycode_f16;
    hadal->keycodes[KEY_F17]        = hadal_keycode_f17;
    hadal->keycodes[KEY_F18]        = hadal_keycode_f18;
    hadal->keycodes[KEY_F19]        = hadal_keycode_f19;
    hadal->keycodes[KEY_F20]        = hadal_keycode_f20;
    hadal->keycodes[KEY_F21]        = hadal_keycode_f21;
    hadal->keycodes[KEY_F22]        = hadal_keycode_f22;
    hadal->keycodes[KEY_F23]        = hadal_keycode_f23;
    hadal->keycodes[KEY_F24]        = hadal_keycode_f24;
    hadal->keycodes[KEY_KPSLASH]    = hadal_keycode_kp_divide;
    hadal->keycodes[KEY_KPASTERISK] = hadal_keycode_kp_multiply;
    hadal->keycodes[KEY_KPMINUS]    = hadal_keycode_kp_subtract;
    hadal->keycodes[KEY_KPPLUS]     = hadal_keycode_kp_add;
    hadal->keycodes[KEY_KP0]        = hadal_keycode_kp_0;
    hadal->keycodes[KEY_KP1]        = hadal_keycode_kp_1;
    hadal->keycodes[KEY_KP2]        = hadal_keycode_kp_2;
    hadal->keycodes[KEY_KP3]        = hadal_keycode_kp_3;
    hadal->keycodes[KEY_KP4]        = hadal_keycode_kp_4;
    hadal->keycodes[KEY_KP5]        = hadal_keycode_kp_5;
    hadal->keycodes[KEY_KP6]        = hadal_keycode_kp_6;
    hadal->keycodes[KEY_KP7]        = hadal_keycode_kp_7;
    hadal->keycodes[KEY_KP8]        = hadal_keycode_kp_8;
    hadal->keycodes[KEY_KP9]        = hadal_keycode_kp_9;
    hadal->keycodes[KEY_KPDOT]      = hadal_keycode_kp_decimal;
    hadal->keycodes[KEY_KPEQUAL]    = hadal_keycode_kp_equal;
    hadal->keycodes[KEY_KPENTER]    = hadal_keycode_kp_enter;
    hadal->keycodes[KEY_102ND]      = hadal_keycode_world_2;

    for (s32 scancode = 0; scancode < 256; scancode++)
        if (hadal->keycodes[scancode] > 0)
            hadal->scancodes[hadal->keycodes[scancode]] = scancode;
    return true;
}

static FN_LAKE_WORK(_hadal_wayland_zero_refcnt, struct hadal_impl *hadal) 
{
    if (hadal == nullptr) return;

    s32 refcnt = lake_atomic_read(&hadal->interface.header.refcnt);
    lake_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, nullptr);

#ifdef HADAL_LIBDECOR
    if (hadal->shell.libdecor)
        libdecor_unref(hadal->shell.libdecor);
    if (hadal->libdecor_library)
        lake_close_library(hadal->libdecor_library);
#endif /* HADAL_LIBDECOR */

    if (hadal->xkb_compose_state)
        xkb_compose_state_unref(hadal->xkb_compose_state);
    if (hadal->xkb_keymap)
        xkb_keymap_unref(hadal->xkb_keymap);
    if (hadal->xkb_state)
        xkb_state_unref(hadal->xkb_state);
    if (hadal->xkb_context)
        xkb_context_unref(hadal->xkb_context);
    if (hadal->xkbcommon_library)
        lake_close_library(hadal->xkbcommon_library);

    if (hadal->cursor_library)
        lake_close_library(hadal->cursor_library);

    if (hadal->wl_subcompositor)
        wl_subcompositor_destroy(hadal->wl_subcompositor);
    if (hadal->wl_compositor)
        wl_compositor_destroy(hadal->wl_compositor);
    if (hadal->wl_shm)
        wl_shm_destroy(hadal->wl_shm);
    if (hadal->shell.xdg)
        xdg_wm_base_destroy(hadal->shell.xdg);
    if (hadal->wl_seat)
        wl_seat_destroy(hadal->wl_seat);
    if (hadal->viewporter)
        wp_viewporter_destroy(hadal->viewporter);
    if (hadal->data_device_manager)
        wl_data_device_manager_destroy(hadal->data_device_manager);
    if (hadal->wl_registry)
        wl_registry_destroy(hadal->wl_registry);
    if (hadal->wl_display) {
        wl_display_flush(hadal->wl_display);
        wl_display_disconnect(hadal->wl_display);
    }
    if (hadal->client_library)
        lake_close_library(hadal->client_library);

    __lake_free(hadal);
    g_hadal = nullptr;
}

FN_LAKE_INTERFACE_IMPL(hadal, wayland)
{
    char const *name = "wayland";

    if (lake_unlikely(g_hadal != nullptr)) {
        lake_inc_refcnt(&g_hadal->interface.header.refcnt);
        return g_hadal;
    }

    char const *client_library_name = "libwayland-client.so.0";
    void *client_library = lake_open_library(client_library_name);
    if (client_library == nullptr) 
        lake_dbg_1("%s: %s is missing.", name, client_library_name);

    char const *cursor_library_name = "libwayland-cursor.so.0";
    void *cursor_library = lake_open_library(cursor_library_name);
    if (cursor_library == nullptr) 
        lake_dbg_1("%s: %s is missing.", name, cursor_library_name);
    
    char const *xkbcommon_library_name = "libxkbcommon.so.0";
    void *xkbcommon_library = lake_open_library(xkbcommon_library_name);
    if (xkbcommon_library == nullptr) 
        lake_dbg_1("%s: %s is missing.", name, xkbcommon_library_name);

#ifdef HADAL_LIBDECOR
    /* libdecor is optional */
    char const *libdecor_library_name = "libdecor-0.so.0";
    void *libdecor_library = lake_open_library(libdecor_library_name);
#endif /* HADAL_LIBDECOR */

    if (!client_library || !cursor_library || !xkbcommon_library) {
disconnect:
        if (g_hadal != nullptr) {
            _hadal_wayland_zero_refcnt(g_hadal);
        } else {
#ifdef HADAL_LIBDECOR
            if (libdecor_library)
                lake_close_library(libdecor_library);
#endif /* HADAL_LIBDECOR */
            if (xkbcommon_library)
                lake_close_library(xkbcommon_library);
            if (cursor_library)
                lake_close_library(cursor_library);
            if (client_library)
                lake_close_library(client_library);
        }
        return nullptr;
    }

    PFN_wl_display_connect _wl_display_connect = (PFN_wl_display_connect)
        lake_get_proc_address(client_library, "wl_display_connect");
    if (!_wl_display_connect) {
        lake_dbg_1("%s can't load `wl_display_connect`.", name);
        goto disconnect;
    }

    struct wl_display *wl_display = _wl_display_connect(nullptr);
    if (wl_display == nullptr) {
        lake_dbg_1("%s can't connect to a Wayland display." 
                "Make sure you're running a Wayland compositor on a XDG session.", name);
        goto disconnect;
    }

    struct hadal_impl *hadal = __lake_malloc_t(struct hadal_impl);
    lake_zerop(hadal);
    g_hadal = hadal;

    hadal->wl_display = wl_display;
    hadal->wl.display_connect = _wl_display_connect;
    hadal->client_library = client_library;
    hadal->cursor_library = cursor_library;
    hadal->xkbcommon_library = xkbcommon_library;

    /* write the interface header */
    hadal->interface.header.bedrock = bedrock;
    hadal->interface.header.zero_refcnt = (PFN_lake_work)_hadal_wayland_zero_refcnt;
    hadal->interface.header.name.len = lake_strlen(name);
    lake_memcpy(hadal->interface.header.name.str, name, hadal->interface.header.name.len);

    if (lake_unlikely(!load_wayland_symbols(hadal, name)))
        goto disconnect;

    /* TODO posix poll events */
    hadal->key_repeat_timerfd = -1;
    hadal->cursor_timerfd = -1;

    hadal->wl_registry = wl_display_get_registry(hadal->wl_display);
    wl_registry_add_listener(hadal->wl_registry, &g_wl_registry_listener, hadal);
    create_key_tables(hadal);

    hadal->xkb_context = xkb_context_new(0);
    if (hadal->xkb_context == nullptr) {
        lake_dbg_1("%s: failed to create xkb_context.", name);
        goto disconnect;
    }

    /* sync to get all registry objects */
    wl_display_roundtrip(hadal->wl_display);

    /* sync to get initial output events */
    wl_display_roundtrip(hadal->wl_display);

#ifdef HADAL_LIBDECOR
    if (libdecor_library) {
        hadal->libdecor_library = libdecor_library;
        //hadal->shell.libdecor = libdecor_new(hadal->wl_display, &libdecor_interface);
        if (hadal->shell.libdecor) {
            /* TODO create callbacks? */
        }
    }
#endif /* HADAL_LIBDECOR */

    /* write the interface */
    hadal->interface.window_assembly = _hadal_wayland_window_assembly;
    hadal->interface.window_zero_refcnt = _hadal_wayland_window_zero_refcnt;
#ifdef MOON_VULKAN
    hadal->interface.vulkan_connect_instance = _hadal_wayland_vulkan_connect_instance;
    hadal->interface.vulkan_presentation_support = _hadal_wayland_vulkan_presentation_support;
    hadal->interface.vulkan_create_surface = _hadal_wayland_vulkan_create_surface;
#endif /* MOON_VULKAN */

    lake_trace("Connected to hadal::%s, %d displays available.", name, lake_darray_len(&hadal->interface.displays));
    lake_inc_refcnt(&hadal->interface.header.refcnt);
    return hadal;
}

#include "wayland-protocol-code.h"
#include "fractional-scale-v1-protocol.h"
#include "idle-inhibit-unstable-v1-protocol.h"
#include "tablet-unstable-v2-protocol.h"
#include "viewporter-protocol-code.h"
#include "xdg-activation-v1-protocol.h"
#include "xdg-shell-protocol-code.h"
#endif /* HADAL_WAYLAND */
