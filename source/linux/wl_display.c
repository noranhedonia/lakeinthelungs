#include "wl_hadal.h"
#ifdef HADAL_WAYLAND

/** We allow only one Wayland display backend to exist at a time. */
hadal_adapter g_hadal = nullptr;

char const *g_wl_output_tag = "hadal_display";
char const *g_wl_surface_tag = "hadal_window";

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
    hadal_adapter hadal = (hadal_adapter)raw_hadal;

    if (!strcmp(interface, "wl_compositor")) {
        hadal->wl_compositor = wl_registry_bind(registry, name, &wl_compositor_interface, lake_min(3, version));
    } else if (!strcmp(interface, "wl_subcompositor")) {
        hadal->wl_subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    } else if (!strcmp(interface, "wl_shm")) {
        hadal->wl_shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (!strcmp(interface, "wl_output")) {
        lake_dbg_3("TODO add wayland output: %u ver. %u", name, version);
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

static bool load_wayland_symbols(hadal_adapter hadal, char const *name)
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

static bool create_key_tables(hadal_adapter hadal)
{
    lake_memset(hadal->interface.keycodes, -1, sizeof(hadal->interface.keycodes));
    lake_memset(hadal->interface.scancodes, -1, sizeof(hadal->interface.scancodes));

    hadal->interface.keycodes[KEY_GRAVE]      = hadal_keycode_grave_accent;
    hadal->interface.keycodes[KEY_1]          = hadal_keycode_1;
    hadal->interface.keycodes[KEY_2]          = hadal_keycode_2;
    hadal->interface.keycodes[KEY_3]          = hadal_keycode_3;
    hadal->interface.keycodes[KEY_4]          = hadal_keycode_4;
    hadal->interface.keycodes[KEY_5]          = hadal_keycode_5;
    hadal->interface.keycodes[KEY_6]          = hadal_keycode_6;
    hadal->interface.keycodes[KEY_7]          = hadal_keycode_7;
    hadal->interface.keycodes[KEY_8]          = hadal_keycode_8;
    hadal->interface.keycodes[KEY_9]          = hadal_keycode_9;
    hadal->interface.keycodes[KEY_0]          = hadal_keycode_0;
    hadal->interface.keycodes[KEY_SPACE]      = hadal_keycode_space;
    hadal->interface.keycodes[KEY_MINUS]      = hadal_keycode_minus;
    hadal->interface.keycodes[KEY_EQUAL]      = hadal_keycode_equal;
    hadal->interface.keycodes[KEY_Q]          = hadal_keycode_q;
    hadal->interface.keycodes[KEY_W]          = hadal_keycode_w;
    hadal->interface.keycodes[KEY_E]          = hadal_keycode_e;
    hadal->interface.keycodes[KEY_R]          = hadal_keycode_r;
    hadal->interface.keycodes[KEY_T]          = hadal_keycode_t;
    hadal->interface.keycodes[KEY_Y]          = hadal_keycode_y;
    hadal->interface.keycodes[KEY_U]          = hadal_keycode_u;
    hadal->interface.keycodes[KEY_I]          = hadal_keycode_i;
    hadal->interface.keycodes[KEY_O]          = hadal_keycode_o;
    hadal->interface.keycodes[KEY_P]          = hadal_keycode_p;
    hadal->interface.keycodes[KEY_LEFTBRACE]  = hadal_keycode_left_bracket;
    hadal->interface.keycodes[KEY_RIGHTBRACE] = hadal_keycode_right_bracket;
    hadal->interface.keycodes[KEY_A]          = hadal_keycode_a;
    hadal->interface.keycodes[KEY_S]          = hadal_keycode_s;
    hadal->interface.keycodes[KEY_D]          = hadal_keycode_d;
    hadal->interface.keycodes[KEY_F]          = hadal_keycode_f;
    hadal->interface.keycodes[KEY_G]          = hadal_keycode_g;
    hadal->interface.keycodes[KEY_H]          = hadal_keycode_h;
    hadal->interface.keycodes[KEY_J]          = hadal_keycode_j;
    hadal->interface.keycodes[KEY_K]          = hadal_keycode_k;
    hadal->interface.keycodes[KEY_L]          = hadal_keycode_l;
    hadal->interface.keycodes[KEY_SEMICOLON]  = hadal_keycode_semicolon;
    hadal->interface.keycodes[KEY_APOSTROPHE] = hadal_keycode_apostrophe;
    hadal->interface.keycodes[KEY_Z]          = hadal_keycode_z;
    hadal->interface.keycodes[KEY_X]          = hadal_keycode_x;
    hadal->interface.keycodes[KEY_C]          = hadal_keycode_c;
    hadal->interface.keycodes[KEY_V]          = hadal_keycode_v;
    hadal->interface.keycodes[KEY_B]          = hadal_keycode_b;
    hadal->interface.keycodes[KEY_N]          = hadal_keycode_n;
    hadal->interface.keycodes[KEY_M]          = hadal_keycode_m;
    hadal->interface.keycodes[KEY_COMMA]      = hadal_keycode_comma;
    hadal->interface.keycodes[KEY_DOT]        = hadal_keycode_period;
    hadal->interface.keycodes[KEY_SLASH]      = hadal_keycode_slash;
    hadal->interface.keycodes[KEY_BACKSLASH]  = hadal_keycode_backslash;
    hadal->interface.keycodes[KEY_ESC]        = hadal_keycode_escape;
    hadal->interface.keycodes[KEY_TAB]        = hadal_keycode_tab;
    hadal->interface.keycodes[KEY_LEFTSHIFT]  = hadal_keycode_left_shift;
    hadal->interface.keycodes[KEY_RIGHTSHIFT] = hadal_keycode_right_shift;
    hadal->interface.keycodes[KEY_LEFTCTRL]   = hadal_keycode_left_control;
    hadal->interface.keycodes[KEY_RIGHTCTRL]  = hadal_keycode_right_control;
    hadal->interface.keycodes[KEY_LEFTALT]    = hadal_keycode_left_alt;
    hadal->interface.keycodes[KEY_RIGHTALT]   = hadal_keycode_right_alt;
    hadal->interface.keycodes[KEY_LEFTMETA]   = hadal_keycode_left_super;
    hadal->interface.keycodes[KEY_RIGHTMETA]  = hadal_keycode_right_super;
    hadal->interface.keycodes[KEY_COMPOSE]    = hadal_keycode_menu;
    hadal->interface.keycodes[KEY_NUMLOCK]    = hadal_keycode_num_lock;
    hadal->interface.keycodes[KEY_CAPSLOCK]   = hadal_keycode_caps_lock;
    hadal->interface.keycodes[KEY_PRINT]      = hadal_keycode_print_screen;
    hadal->interface.keycodes[KEY_SCROLLLOCK] = hadal_keycode_scroll_lock;
    hadal->interface.keycodes[KEY_PAUSE]      = hadal_keycode_pause;
    hadal->interface.keycodes[KEY_DELETE]     = hadal_keycode_delete;
    hadal->interface.keycodes[KEY_BACKSPACE]  = hadal_keycode_backspace;
    hadal->interface.keycodes[KEY_ENTER]      = hadal_keycode_enter;
    hadal->interface.keycodes[KEY_HOME]       = hadal_keycode_home;
    hadal->interface.keycodes[KEY_END]        = hadal_keycode_end;
    hadal->interface.keycodes[KEY_PAGEUP]     = hadal_keycode_page_up;
    hadal->interface.keycodes[KEY_PAGEDOWN]   = hadal_keycode_page_down;
    hadal->interface.keycodes[KEY_INSERT]     = hadal_keycode_insert;
    hadal->interface.keycodes[KEY_LEFT]       = hadal_keycode_left;
    hadal->interface.keycodes[KEY_RIGHT]      = hadal_keycode_right;
    hadal->interface.keycodes[KEY_DOWN]       = hadal_keycode_down;
    hadal->interface.keycodes[KEY_UP]         = hadal_keycode_up;
    hadal->interface.keycodes[KEY_F1]         = hadal_keycode_f1;
    hadal->interface.keycodes[KEY_F2]         = hadal_keycode_f2;
    hadal->interface.keycodes[KEY_F3]         = hadal_keycode_f3;
    hadal->interface.keycodes[KEY_F4]         = hadal_keycode_f4;
    hadal->interface.keycodes[KEY_F5]         = hadal_keycode_f5;
    hadal->interface.keycodes[KEY_F6]         = hadal_keycode_f6;
    hadal->interface.keycodes[KEY_F7]         = hadal_keycode_f7;
    hadal->interface.keycodes[KEY_F8]         = hadal_keycode_f8;
    hadal->interface.keycodes[KEY_F9]         = hadal_keycode_f9;
    hadal->interface.keycodes[KEY_F10]        = hadal_keycode_f10;
    hadal->interface.keycodes[KEY_F11]        = hadal_keycode_f11;
    hadal->interface.keycodes[KEY_F12]        = hadal_keycode_f12;
    hadal->interface.keycodes[KEY_F13]        = hadal_keycode_f13;
    hadal->interface.keycodes[KEY_F14]        = hadal_keycode_f14;
    hadal->interface.keycodes[KEY_F15]        = hadal_keycode_f15;
    hadal->interface.keycodes[KEY_F16]        = hadal_keycode_f16;
    hadal->interface.keycodes[KEY_F17]        = hadal_keycode_f17;
    hadal->interface.keycodes[KEY_F18]        = hadal_keycode_f18;
    hadal->interface.keycodes[KEY_F19]        = hadal_keycode_f19;
    hadal->interface.keycodes[KEY_F20]        = hadal_keycode_f20;
    hadal->interface.keycodes[KEY_F21]        = hadal_keycode_f21;
    hadal->interface.keycodes[KEY_F22]        = hadal_keycode_f22;
    hadal->interface.keycodes[KEY_F23]        = hadal_keycode_f23;
    hadal->interface.keycodes[KEY_F24]        = hadal_keycode_f24;
    hadal->interface.keycodes[KEY_KPSLASH]    = hadal_keycode_kp_divide;
    hadal->interface.keycodes[KEY_KPASTERISK] = hadal_keycode_kp_multiply;
    hadal->interface.keycodes[KEY_KPMINUS]    = hadal_keycode_kp_subtract;
    hadal->interface.keycodes[KEY_KPPLUS]     = hadal_keycode_kp_add;
    hadal->interface.keycodes[KEY_KP0]        = hadal_keycode_kp_0;
    hadal->interface.keycodes[KEY_KP1]        = hadal_keycode_kp_1;
    hadal->interface.keycodes[KEY_KP2]        = hadal_keycode_kp_2;
    hadal->interface.keycodes[KEY_KP3]        = hadal_keycode_kp_3;
    hadal->interface.keycodes[KEY_KP4]        = hadal_keycode_kp_4;
    hadal->interface.keycodes[KEY_KP5]        = hadal_keycode_kp_5;
    hadal->interface.keycodes[KEY_KP6]        = hadal_keycode_kp_6;
    hadal->interface.keycodes[KEY_KP7]        = hadal_keycode_kp_7;
    hadal->interface.keycodes[KEY_KP8]        = hadal_keycode_kp_8;
    hadal->interface.keycodes[KEY_KP9]        = hadal_keycode_kp_9;
    hadal->interface.keycodes[KEY_KPDOT]      = hadal_keycode_kp_decimal;
    hadal->interface.keycodes[KEY_KPEQUAL]    = hadal_keycode_kp_equal;
    hadal->interface.keycodes[KEY_KPENTER]    = hadal_keycode_kp_enter;
    hadal->interface.keycodes[KEY_102ND]      = hadal_keycode_world_2;

    for (s32 scancode = 0; scancode < 256; scancode++)
        if (hadal->interface.keycodes[scancode] > 0)
            hadal->interface.scancodes[hadal->interface.keycodes[scancode]] = scancode;
    return true;
}

static FN_LAKE_WORK(hadal_interface_destructor, hadal_adapter hadal) 
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

FN_LAKE_WORK(hadal_interface_assembly_wayland, hadal_interface_assembly const *assembly)
{
    char const *name = "hadal/wayland";

    if (lake_unlikely(g_hadal != nullptr)) {
        lake_refcnt_inc(&g_hadal->interface.header.refcnt);
        assembly->out_impl->adapter = g_hadal;
        return;
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
            hadal_interface_destructor(g_hadal);
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
        return;
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

    hadal_adapter hadal = __lake_malloc_t(struct hadal_adapter_impl);
    lake_zerop(hadal);
    g_hadal = hadal;

    hadal->wl_display = wl_display;
    hadal->wl.display_connect = _wl_display_connect;
    hadal->client_library = client_library;
    hadal->cursor_library = cursor_library;
    hadal->xkbcommon_library = xkbcommon_library;
#ifdef HADAL_LIBDECOR
    hadal->libdecor_library = libdecor_library;
#endif /* HADAL_LIBDECOR */

    /* write the interface header */
    hadal->interface.header.framework = assembly->framework;
    hadal->interface.header.destructor = (PFN_lake_work)hadal_interface_destructor;
    hadal->interface.header.name.len = lake_strlen(name) + 1;
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

    /* write the interface */
    hadal->interface.window_assembly = _hadal_wayland_window_assembly;
    hadal->interface.window_destructor = _hadal_wayland_window_destructor;
#ifdef MOON_VULKAN
    hadal->interface.vulkan_connect_instance = _hadal_wayland_vulkan_connect_instance;
    hadal->interface.vulkan_presentation_support = _hadal_wayland_vulkan_presentation_support;
    hadal->interface.vulkan_create_surface = _hadal_wayland_vulkan_create_surface;
#endif /* MOON_VULKAN */

    lake_trace("Connected %s.", name);
    lake_refcnt_inc(&hadal->interface.header.refcnt);
    assembly->out_impl->adapter = hadal;
}

#include "wayland-protocol-code.h"
#include "viewporter-protocol-code.h"
#include "xdg-shell-protocol-code.h"
#endif /* HADAL_WAYLAND */
