#pragma once

/** @file lake/platform/window.h
 *  @brief A system window.
 *
 *  TODO docs
 */
#include <lake/platform/hadal_adapter.h>

typedef enum hadal_window_flag_bits : u32 {
    hadal_window_flag_none              = 0u,
    hadal_window_flag_is_valid          = (1u << 0),
    hadal_window_flag_should_close      = (1u << 1),
    hadal_window_flag_visible           = (1u << 2),
    hadal_window_flag_fullscreen        = (1u << 3),
    hadal_window_flag_maximized         = (1u << 4),
    hadal_window_flag_minimized         = (1u << 5),
    hadal_window_flag_resizable         = (1u << 6),
    hadal_window_flag_no_decorations    = (1u << 7),
    hadal_window_flag_input_focus       = (1u << 8),
    hadal_window_flag_mouse_focus       = (1u << 9),
    hadal_window_flag_mouse_captured    = (1u << 11),
    hadal_window_flag_mouse_relative    = (1u << 12),
    hadal_window_flag_cursor_hovered    = (1u << 13),
    hadal_window_flag_modal             = (1u << 14),
    hadal_window_flag_occluded          = (1u << 15),
    hadal_window_flag_always_on_top     = (1u << 16),
    hadal_window_flag_auto_minimize     = (1u << 17),
    hadal_window_flag_shell_activated   = (1u << 18),
    hadal_window_flag_mouse_grabbed     = (1u << 19),
    hadal_window_flag_keyboard_grabbed  = (1u << 20),
    hadal_window_flag_transparent       = (1u << 21),
    hadal_window_flag_vulkan            = (1u << 22),
} hadal_window_flag_bits;

typedef struct hadal_window_assembly {
    s32                 width, height;
    s32                 min_width, min_height;
    s32                 max_width, max_height;
    u32                 flag_hints;
    lake_small_string   name;
} hadal_window_assembly;

typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_hadal_window_assembly)(hadal_adapter hadal, hadal_window_assembly const *assembly, hadal_window *out_window);
#define FN_HADAL_WINDOW_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _hadal_##backend##_window_assembly(hadal_adapter hadal, hadal_window_assembly const *assembly, hadal_window *out_window)

PFN_LAKE_WORK(PFN_hadal_window_zero_refcnt, hadal_window window);
#define FN_HADAL_WINDOW_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_hadal_##backend##_window_zero_refcnt, hadal_window window)

/** Header for `struct hadal_window_impl`. */
typedef struct hadal_window_header {
    hadal_interface         hadal;
    atomic_u32              flags;
    lake_refcnt             refcnt;
    hadal_window_assembly   assembly;
    char const             *title;
    hadal_display           fullscreen;
} hadal_window_header;
