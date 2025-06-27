#pragma once

/** @file lake/hadal.h
 *  @brief Display backend interface.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/darray.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The platform-specific display backend. */
LAKE_DECL_INTERFACE(hadal);
/** Represents a display output, maps directly to hardware. A backend without displays is headless. */
LAKE_DECL_HANDLE_INTERFACED(hadal_display, hadal_interface *hadal);
/** Represents a system window with a surface we can draw to. Main context of interaction with the display. */
LAKE_DECL_HANDLE_INTERFACED(hadal_window, hadal_interface *hadal);
/** Represents a keyboard device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_keyboard, hadal_interface *hadal);
/** Represents a mouse or pointer device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_pointer, hadal_interface *hadal);
/** Represents a haptic (force feedback) device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_haptic, hadal_interface *hadal);
/** Represents a low-level joystick raw inputs. */
LAKE_DECL_HANDLE_INTERFACED(hadal_joystick, hadal_interface *hadal);
/** Represents a gamepad, built on top of the joystick API to map console-style gamepads.
 *  The difference between a joystick and a gamepad is the gamepad tells "where" a button 
 *  or axis is on the device, by providing a configuration for the joystick device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_gamepad, hadal_joystick *joystick);
/** Represents a touch device, especially important on mobile platforms. */
LAKE_DECL_HANDLE_INTERFACED(hadal_touch, hadal_interface *hadal);
/** Represents a pen device. */
LAKE_DECL_HANDLE_INTERFACED(hadal_pen, hadal_interface *hadal);

/** Represents an unique key. */
typedef enum hadal_keycode : s16 {
    hadal_keycode_space         = 32,
    hadal_keycode_apostrophe    = 39,  /* ' */
    hadal_keycode_comma         = 44,  /* , */
    hadal_keycode_minus         = 45,  /* - */
    hadal_keycode_period        = 46,  /* . */
    hadal_keycode_slash         = 47,  /* / */
    hadal_keycode_0             = 48,
    hadal_keycode_1             = 49,
    hadal_keycode_2             = 50,
    hadal_keycode_3             = 51,
    hadal_keycode_4             = 52,
    hadal_keycode_5             = 53,
    hadal_keycode_6             = 54,
    hadal_keycode_7             = 55,
    hadal_keycode_8             = 56,
    hadal_keycode_9             = 57,
    hadal_keycode_semicolon     = 59,  /* ; */
    hadal_keycode_equal         = 61,  /* = */
    hadal_keycode_a             = 65,
    hadal_keycode_b             = 66,
    hadal_keycode_c             = 67,
    hadal_keycode_d             = 68,
    hadal_keycode_e             = 69,
    hadal_keycode_f             = 70,
    hadal_keycode_g             = 71,
    hadal_keycode_h             = 72,
    hadal_keycode_i             = 73,
    hadal_keycode_j             = 74,
    hadal_keycode_k             = 75,
    hadal_keycode_l             = 76,
    hadal_keycode_m             = 77,
    hadal_keycode_n             = 78,
    hadal_keycode_o             = 79,
    hadal_keycode_p             = 80,
    hadal_keycode_q             = 81,
    hadal_keycode_r             = 82,
    hadal_keycode_s             = 83,
    hadal_keycode_t             = 84,
    hadal_keycode_u             = 85,
    hadal_keycode_v             = 86,
    hadal_keycode_w             = 87,
    hadal_keycode_x             = 88,
    hadal_keycode_y             = 89,
    hadal_keycode_z             = 90,
    hadal_keycode_left_bracket  = 91,  /* [ */
    hadal_keycode_backslash     = 92,  /* \ */
    hadal_keycode_right_bracket = 93,  /* ] */
    hadal_keycode_grave_accent  = 96,  /* ` */
    hadal_keycode_world_1       = 161, /* non-US #1 */
    hadal_keycode_world_2       = 162, /* non-US #2 */
    hadal_keycode_escape        = 256,
    hadal_keycode_enter         = 257,
    hadal_keycode_tab           = 258,
    hadal_keycode_backspace     = 259,
    hadal_keycode_insert        = 260,
    hadal_keycode_delete        = 261,
    hadal_keycode_right         = 262,
    hadal_keycode_left          = 263,
    hadal_keycode_down          = 264,
    hadal_keycode_up            = 265,
    hadal_keycode_page_up       = 266,
    hadal_keycode_page_down     = 267,
    hadal_keycode_home          = 268,
    hadal_keycode_end           = 269,
    hadal_keycode_caps_lock     = 280,
    hadal_keycode_scroll_lock   = 281,
    hadal_keycode_num_lock      = 282,
    hadal_keycode_print_screen  = 283,
    hadal_keycode_pause         = 284,
    hadal_keycode_f1            = 290,
    hadal_keycode_f2            = 291,
    hadal_keycode_f3            = 292,
    hadal_keycode_f4            = 293,
    hadal_keycode_f5            = 294,
    hadal_keycode_f6            = 295,
    hadal_keycode_f7            = 296,
    hadal_keycode_f8            = 297,
    hadal_keycode_f9            = 298,
    hadal_keycode_f10           = 299,
    hadal_keycode_f11           = 300,
    hadal_keycode_f12           = 301,
    hadal_keycode_f13           = 302,
    hadal_keycode_f14           = 303,
    hadal_keycode_f15           = 304,
    hadal_keycode_f16           = 305,
    hadal_keycode_f17           = 306,
    hadal_keycode_f18           = 307,
    hadal_keycode_f19           = 308,
    hadal_keycode_f20           = 309,
    hadal_keycode_f21           = 310,
    hadal_keycode_f22           = 311,
    hadal_keycode_f23           = 312,
    hadal_keycode_f24           = 313,
    hadal_keycode_f25           = 314,
    hadal_keycode_kp_0          = 320,
    hadal_keycode_kp_1          = 321,
    hadal_keycode_kp_2          = 322,
    hadal_keycode_kp_3          = 323,
    hadal_keycode_kp_4          = 324,
    hadal_keycode_kp_5          = 325,
    hadal_keycode_kp_6          = 326,
    hadal_keycode_kp_7          = 327,
    hadal_keycode_kp_8          = 328,
    hadal_keycode_kp_9          = 329,
    hadal_keycode_kp_decimal    = 330,
    hadal_keycode_kp_divide     = 331,
    hadal_keycode_kp_multiply   = 332,
    hadal_keycode_kp_subtract   = 333,
    hadal_keycode_kp_add        = 334,
    hadal_keycode_kp_enter      = 335,
    hadal_keycode_kp_equal      = 336,
    hadal_keycode_left_shift    = 340,
    hadal_keycode_left_control  = 341,
    hadal_keycode_left_alt      = 342,
    hadal_keycode_left_super    = 343,
    hadal_keycode_right_shift   = 344,
    hadal_keycode_right_control = 345,
    hadal_keycode_right_alt     = 346,
    hadal_keycode_right_super   = 347,
    hadal_keycode_menu          = 348,
    hadal_keycode_invalid       = 0,
} hadal_keycode;
#define hadal_keycode_last hadal_keycode_menu

/** A scancode is the physical representation of a key on the keyboard,
 *  independent of language and keyboard mapping.
 *
 *  The values in this enumeration are based on the USB usage page standard:
 *  https://usb.org/sites/default/files/hut1_5.pdf */
typedef enum hadal_scancode : s16 {
    hadal_scancode_unknown = 0,

    /* These values are from usage page 0x07 (USB keyboard page) */
    hadal_scancode_a = 4,
    hadal_scancode_b = 5,
    hadal_scancode_c = 6,
    hadal_scancode_d = 7,
    hadal_scancode_e = 8,
    hadal_scancode_f = 9,
    hadal_scancode_g = 10,
    hadal_scancode_h = 11,
    hadal_scancode_i = 12,
    hadal_scancode_j = 13,
    hadal_scancode_k = 14,
    hadal_scancode_l = 15,
    hadal_scancode_m = 16,
    hadal_scancode_n = 17,
    hadal_scancode_o = 18,
    hadal_scancode_p = 19,
    hadal_scancode_q = 20,
    hadal_scancode_r = 21,
    hadal_scancode_s = 22,
    hadal_scancode_t = 23,
    hadal_scancode_u = 24,
    hadal_scancode_v = 25,
    hadal_scancode_w = 26,
    hadal_scancode_x = 27,
    hadal_scancode_y = 28,
    hadal_scancode_z = 29,

    hadal_scancode_1 = 30,
    hadal_scancode_2 = 31,
    hadal_scancode_3 = 32,
    hadal_scancode_4 = 33,
    hadal_scancode_5 = 34,
    hadal_scancode_6 = 35,
    hadal_scancode_7 = 36,
    hadal_scancode_8 = 37,
    hadal_scancode_9 = 38,
    hadal_scancode_0 = 39,
 
    hadal_scancode_return = 40,
    hadal_scancode_escape = 41,
    hadal_scancode_backspace = 42,
    hadal_scancode_tab = 43,
    hadal_scancode_space = 44,
 
    hadal_scancode_minus = 45,
    hadal_scancode_equals = 46,
    hadal_scancode_leftbracket = 47,
    hadal_scancode_rightbracket = 48,
    /** Located at the lower left of the return key on ISO keyboards and 
     *  at the right end of the QWERTY row on ANSI keyboards. Produces 
     *  REVERSE SOLIDUS (backslash) and VERTICAL LINE in a US layout, 
     *  REVERSE SOLIDUS and VERTICAL LINE in a UK Mac layout, NUMBER 
     *  SIGN and TILDE in a UK Windows layout, DOLLAR SIGN and POUND 
     *  SIGN in a Swiss German layout, NUMBER SIGN and APOSTROPHE in 
     *  a German layout, GRAVE ACCENT and POUND SIGN in a French Mac
     *  layout, and ASTERISK and MICRO SIGN in a French Windows layout. */
    hadal_scancode_backslash = 49,
    /** ISO USB keyboards actually use this code instead of 49 for the 
     *  same key, but all OSes I've seen treat the two codes identically. 
     *  So, as an implementor, unless your keyboard generates both of 
     *  those codes and your OS treats them differently, you should 
     *  generate lake_scancode_backlash instead of this code. */
    hadal_scancode_nonushash = 50,
    hadal_scancode_semicolon = 51,
    hadal_scancode_apostrophe = 52,
    /** Located in the top left corner (on both ANSI and ISO keyboards). 
     *  Produces GRAVE ACCENT and TILDE in a US Windows layout and in US 
     *  and UK Mac layouts on ANSI keyboards, GRAVE ACCENT and NOT SIGN 
     *  in a UK Windows layout, SECTION SIGN and PLUS-MINUS SIGN in US 
     *  and UK Mac layouts on ISO keyboards, SECTION SIGN and DEGREE SIGN 
     *  in a Swiss German layout (Mac: only on ISO keyboards), CIRCUMFLEX 
     *  ACCENT and DEGREE SIGN in a German layout (Mac: only on ISO 
     *  keyboards), SUPERSCRIPT TWO and TILDE in a French Windows layout, 
     *  COMMERCIAL AT and NUMBER SIGN in a French Mac layout on ISO 
     *  keyboards, and LESS-THAN SIGN and GREATER-THAN SIGN in a Swiss 
     *  German, German, or French Mac layout on ANSI keyboards. */
    hadal_scancode_grave = 53,
    hadal_scancode_comma = 54,
    hadal_scancode_period = 55,
    hadal_scancode_slash = 56,

    hadal_scancode_capslock = 57,

    hadal_scancode_f1 = 58,
    hadal_scancode_f2 = 59,
    hadal_scancode_f3 = 60,
    hadal_scancode_f4 = 61,
    hadal_scancode_f5 = 62,
    hadal_scancode_f6 = 63,
    hadal_scancode_f7 = 64,
    hadal_scancode_f8 = 65,
    hadal_scancode_f9 = 66,
    hadal_scancode_f10 = 67,
    hadal_scancode_f11 = 68,
    hadal_scancode_f12 = 69,

    hadal_scancode_printscreen = 70,
    hadal_scancode_scrolllock = 71,
    hadal_scancode_pause = 72,
    hadal_scancode_insert = 73, /**< insert on PC, help on some Mac keyboards (but does send code 73, not 117) */
    hadal_scancode_home = 74,
    hadal_scancode_pageup = 75,
    hadal_scancode_delete = 76,
    hadal_scancode_end = 77,
    hadal_scancode_pagedown = 78,
    hadal_scancode_right = 79,
    hadal_scancode_left = 80,
    hadal_scancode_down = 81,
    hadal_scancode_up = 82,

    hadal_scancode_numlockclear = 83, /**< num lock on PC, clear on Mac keyboards */
    hadal_scancode_kp_divide = 84,
    hadal_scancode_kp_multiply = 85,
    hadal_scancode_kp_minus = 86,
    hadal_scancode_kp_plus = 87,
    hadal_scancode_kp_enter = 88,
    hadal_scancode_kp_1 = 89,
    hadal_scancode_kp_2 = 90,
    hadal_scancode_kp_3 = 91,
    hadal_scancode_kp_4 = 92,
    hadal_scancode_kp_5 = 93,
    hadal_scancode_kp_6 = 94,
    hadal_scancode_kp_7 = 95,
    hadal_scancode_kp_8 = 96,
    hadal_scancode_kp_9 = 97,
    hadal_scancode_kp_0 = 98,
    hadal_scancode_kp_period = 99,

    /** This is the additional key that ISO keyboards have over ANSI ones, 
     *  located between left shift and Y. Produces GRAVE ACCENT and TILDE 
     *  in a US or UK Mac layout, REVERSE SOLIDUS (backslash) and VERTICAL 
     *  LINE in a US or UK Windows layout, and LESS-THAN SIGN and GREATER-THAN 
     *  SIGN in a Swiss German, German, or French layout. */
    hadal_scancode_nonusbackslash = 100,
    hadal_scancode_application = 101, /**< windows contextual menu, compose */
    /** The USB document says this is a status flag, not a physical key, 
     *  but some Mac keyboards do have a power key. */
    hadal_scancode_power = 102,
    hadal_scancode_kp_equals = 103,
    hadal_scancode_f13 = 104,
    hadal_scancode_f14 = 105,
    hadal_scancode_f15 = 106,
    hadal_scancode_f16 = 107,
    hadal_scancode_f17 = 108,
    hadal_scancode_f18 = 109,
    hadal_scancode_f19 = 110,
    hadal_scancode_f20 = 111,
    hadal_scancode_f21 = 112,
    hadal_scancode_f22 = 113,
    hadal_scancode_f23 = 114,
    hadal_scancode_f24 = 115,
    hadal_scancode_execute = 116,
    hadal_scancode_help = 117,    /**< AL Integrated Help Center */
    hadal_scancode_menu = 118,    /**< Menu (show menu) */
    hadal_scancode_select = 119,
    hadal_scancode_stop = 120,    /**< AC Stop */
    hadal_scancode_again = 121,   /**< AC Redo/Repeat */
    hadal_scancode_undo = 122,    /**< AC Undo */
    hadal_scancode_cut = 123,     /**< AC Cut */
    hadal_scancode_copy = 124,    /**< AC Copy */
    hadal_scancode_paste = 125,   /**< AC Paste */
    hadal_scancode_find = 126,    /**< AC Find */
    hadal_scancode_mute = 127,
    hadal_scancode_volumeup = 128,
    hadal_scancode_volumedown = 129,
    hadal_scancode_lockingcapslock = 130,
    hadal_scancode_lockingnumlock = 131,
    hadal_scancode_lockingscrolllock = 132,
    hadal_scancode_kp_comma = 133,
    hadal_scancode_kp_equalsas400 = 134,

    hadal_scancode_international1 = 135, /**< used on Asian keyboards, see footnotes in USB doc */
    hadal_scancode_international2 = 136,
    hadal_scancode_international3 = 137, /**< Yen */
    hadal_scancode_international4 = 138,
    hadal_scancode_international5 = 139,
    hadal_scancode_international6 = 140,
    hadal_scancode_international7 = 141,
    hadal_scancode_international8 = 142,
    hadal_scancode_international9 = 143,
    hadal_scancode_lang1 = 144, /**< Hangul/English toggle */
    hadal_scancode_lang2 = 145, /**< Hanja conversion */
    hadal_scancode_lang3 = 146, /**< Katakana */
    hadal_scancode_lang4 = 147, /**< Hiragana */
    hadal_scancode_lang5 = 148, /**< Zenkaku/Hankaku */
    hadal_scancode_lang6 = 149, /**< reserved */
    hadal_scancode_lang7 = 150, /**< reserved */
    hadal_scancode_lang8 = 151, /**< reserved */
    hadal_scancode_lang9 = 152, /**< reserved */

    hadal_scancode_alterase = 153,    /**< Erase-Eaze */
    hadal_scancode_sysreq = 154,
    hadal_scancode_cancel = 155,      /**< AC Cancel */
    hadal_scancode_clear = 156,
    hadal_scancode_prior = 157,
    hadal_scancode_return2 = 158,
    hadal_scancode_separator = 159,
    hadal_scancode_out = 160,
    hadal_scancode_oper = 161,
    hadal_scancode_clearagain = 162,
    hadal_scancode_crsel = 163,
    hadal_scancode_exsel = 164,

    hadal_scancode_kp_00 = 176,
    hadal_scancode_kp_000 = 177,
    hadal_scancode_thousandsseparator = 178,
    hadal_scancode_decimalseparator = 179,
    hadal_scancode_currencyunit = 180,
    hadal_scancode_currencysubunit = 181,
    hadal_scancode_kp_leftparen = 182,
    hadal_scancode_kp_rightparen = 183,
    hadal_scancode_kp_leftbrace = 184,
    hadal_scancode_kp_rightbrace = 185,
    hadal_scancode_kp_tab = 186,
    hadal_scancode_kp_backspace = 187,
    hadal_scancode_kp_a = 188,
    hadal_scancode_kp_b = 189,
    hadal_scancode_kp_c = 190,
    hadal_scancode_kp_d = 191,
    hadal_scancode_kp_e = 192,
    hadal_scancode_kp_f = 193,
    hadal_scancode_kp_xor = 194,
    hadal_scancode_kp_power = 195,
    hadal_scancode_kp_percent = 196,
    hadal_scancode_kp_less = 197,
    hadal_scancode_kp_greater = 198,
    hadal_scancode_kp_ampersand = 199,
    hadal_scancode_kp_dblampersand = 200,
    hadal_scancode_kp_verticalbar = 201,
    hadal_scancode_kp_dblverticalbar = 202,
    hadal_scancode_kp_colon = 203,
    hadal_scancode_kp_hash = 204,
    hadal_scancode_kp_space = 205,
    hadal_scancode_kp_at = 206,
    hadal_scancode_kp_exclam = 207,
    hadal_scancode_kp_memstore = 208,
    hadal_scancode_kp_memrecall = 209,
    hadal_scancode_kp_memclear = 210,
    hadal_scancode_kp_memadd = 211,
    hadal_scancode_kp_memsubtract = 212,
    hadal_scancode_kp_memmultiply = 213,
    hadal_scancode_kp_memdivide = 214,
    hadal_scancode_kp_plusminus = 215,
    hadal_scancode_kp_clear = 216,
    hadal_scancode_kp_clearentry = 217,
    hadal_scancode_kp_binary = 218,
    hadal_scancode_kp_octal = 219,
    hadal_scancode_kp_decimal = 220,
    hadal_scancode_kp_hexadecimal = 221,

    hadal_scancode_lctrl = 224,
    hadal_scancode_lshift = 225,
    hadal_scancode_lalt = 226, /**< alt, option */
    hadal_scancode_lgui = 227, /**< windows, command (apple), meta */
    hadal_scancode_rctrl = 228,
    hadal_scancode_rshift = 229,
    hadal_scancode_ralt = 230, /**< alt gr, option */
    hadal_scancode_rgui = 231, /**< windows, command (apple), meta */
    hadal_scancode_mode = 257,

    /* These values are mapped from usage page 0x0C (USB consumer page). There are 
     * way more keys in the spec than we can represent in the current scancode range, 
     * so pick the ones that commonly come up in real world usage. */

    hadal_scancode_sleep = 258,                 /**< Sleep */
    hadal_scancode_wake = 259,                  /**< Wake */
                                          
    hadal_scancode_channel_increment = 260,     /**< Channel Increment */
    hadal_scancode_channel_decrement = 261,     /**< Channel Decrement */

    hadal_scancode_media_play = 262,            /**< Play */
    hadal_scancode_media_pause = 263,           /**< Pause */
    hadal_scancode_media_record = 264,          /**< Record */
    hadal_scancode_media_fast_forward = 265,    /**< Fast Forward */
    hadal_scancode_media_rewind = 266,          /**< Rewind */
    hadal_scancode_media_next_track = 267,      /**< Next Track */
    hadal_scancode_media_previous_track = 268,  /**< Previous Track */
    hadal_scancode_media_stop = 269,            /**< Stop */
    hadal_scancode_media_eject = 270,           /**< Eject */
    hadal_scancode_media_play_pause = 271,      /**< Play / Pause */
    hadal_scancode_media_select = 272,          /* Media Select */
                                               
    hadal_scancode_ac_new = 273,                /**< AC New */
    hadal_scancode_ac_open = 274,               /**< AC Open */
    hadal_scancode_ac_close = 275,              /**< AC Close */
    hadal_scancode_ac_exit = 276,               /**< AC Exit */
    hadal_scancode_ac_save = 277,               /**< AC Save */
    hadal_scancode_ac_print = 278,              /**< AC Print */
    hadal_scancode_ac_properties = 279,         /**< AC Properties */
                                               
    hadal_scancode_ac_search = 280,             /**< AC Search */
    hadal_scancode_ac_home = 281,               /**< AC Home */
    hadal_scancode_ac_back = 282,               /**< AC Back */
    hadal_scancode_ac_forward = 283,            /**< AC Forward */
    hadal_scancode_ac_stop = 284,               /**< AC Stop */
    hadal_scancode_ac_refresh = 285,            /**< AC Refresh */
    hadal_scancode_ac_bookmarks = 286,          /**< AC Bookmarks */

    /* these are values that are often used on mobile phones */

    /** Usually situated below the display on phones and used as a 
     *  multi-function feature key for selecting a software defined 
     *  function shown on the bottom left of the display. */
    hadal_scancode_softleft = 287,
    /** Usually situated below the display on phones and used as a 
     *  multi-function feature key for selecting a software defined 
     *  function shown on the bottom right of the display. */
    hadal_scancode_softright = 288,
    hadal_scancode_call = 289, /**< used for accepting phone calls. */
    hadal_scancode_endcall = 290, /**< used for rejecting phone calls. */

    /* add any other keys here. */
    hadal_scancode_reserved = 400,    /**< 400-500 reserved for dynamic keycodes */

    hadal_scancode_count = 512 /**< not a key, just marks the number of scancodes for array bounds */
} hadal_scancode;

typedef enum hadal_mouse_wheel : s8 {
    hadal_mouse_wheel_normal = 0,
    hadal_mouse_wheel_flipped,
} hadal_mouse_wheel;

typedef enum lake_mouse_button : u16 {
    hadal_mouse_button_1       = 0x0001,
    hadal_mouse_button_2       = 0x0002,
    hadal_mouse_button_3       = 0x0004,
    hadal_mouse_button_4       = 0x0008,
    hadal_mouse_button_5       = 0x0010,
    hadal_mouse_button_6       = 0x0020,
    hadal_mouse_button_7       = 0x0040,
    hadal_mouse_button_8       = 0x0080,
    hadal_mouse_button_left    = hadal_mouse_button_1,
    hadal_mouse_button_right   = hadal_mouse_button_2,
    hadal_mouse_button_middle  = hadal_mouse_button_3,
    hadal_mouse_button_invalid = 0,
} hadal_mouse_button;
#define hadal_mouse_button_last hadal_mouse_button_8

typedef enum hadal_gamepad_button : u16 {
    hadal_gamepad_button_a            = 0x0001,
    hadal_gamepad_button_b            = 0x0002,
    hadal_gamepad_button_x            = 0x0004,
    hadal_gamepad_button_y            = 0x0008,
    hadal_gamepad_button_left_bumper  = 0x0010,
    hadal_gamepad_button_right_bumper = 0x0020,
    hadal_gamepad_button_back         = 0x0040,
    hadal_gamepad_button_start        = 0x0080,
    hadal_gamepad_button_guide        = 0x0100,
    hadal_gamepad_button_left_thumb   = 0x0200,
    hadal_gamepad_button_right_thumb  = 0x0400,
    hadal_gamepad_button_dpad_up      = 0x0800,
    hadal_gamepad_button_dpad_right   = 0x1000,
    hadal_gamepad_button_dpad_down    = 0x2000,
    hadal_gamepad_button_dpad_left    = 0x4000,
    hadal_gamepad_button_cross        = hadal_gamepad_button_a,
    hadal_gamepad_button_circle       = hadal_gamepad_button_b,
    hadal_gamepad_button_square       = hadal_gamepad_button_x,
    hadal_gamepad_button_triangle     = hadal_gamepad_button_y,
    hadal_gamepad_button_invalid      = 0,
} hadal_gamepad_button;
#define hadal_gamepad_button_last hadal_gamepad_button_dpad_left

typedef enum hadal_gamepad_axis : u8 {
    hadal_gamepad_axis_left_x         = 0x0001,
    hadal_gamepad_axis_left_y         = 0x0002,
    hadal_gamepad_axis_right_x        = 0x0004,
    hadal_gamepad_axis_right_y        = 0x0008,
    hadal_gamepad_axis_left_trigger   = 0x0010,
    hadal_gamepad_axis_right_trigger  = 0x0020,
} hadal_gamepad_axis;
#define hadal_gamepad_axis_last hadal_gamepad_axis_right_trigger

typedef enum hadal_gamepad_hat : u8 {
    hadal_gamepad_hat_centered    = 0,
    hadal_gamepad_hat_up          = 0x0001,
    hadal_gamepad_hat_right       = 0x0002,
    hadal_gamepad_hat_down        = 0x0004,
    hadal_gamepad_hat_left        = 0x0008,
    hadal_gamepad_hat_right_up    = (hadal_gamepad_hat_right | hadal_gamepad_hat_up),
    hadal_gamepad_hat_right_down  = (hadal_gamepad_hat_right | hadal_gamepad_hat_down),
    hadal_gamepad_hat_left_up     = (hadal_gamepad_hat_left  | hadal_gamepad_hat_up),
    hadal_gamepad_hat_left_down   = (hadal_gamepad_hat_left  | hadal_gamepad_hat_down),
} hadal_gamepad_hat;

typedef struct hadal_gamepad_state {
    u8  buttons[15];
    f32 axes[6];
} hadal_gamepad_state;

typedef enum : s8 {
    hadal_touch_device_invalid = -1,
    hadal_touch_device_direct,            /**< Touch screen with window-relative coordinates. */
    hadal_touch_device_indirect_absolute, /**< Trackpad with absolute device coordinates. */
    hadal_touch_device_indirect_relative, /**< Trackpad with screen cursor-relative coordinates. */
} hadal_touch_device;

/** Data about a single finger in a multitouch event. */
typedef struct hadal_finger {
    u64 id;
    f32 x, y;
    f32 pressure;
} hadal_finger;

typedef u8 hadal_pen_input;
typedef enum hadal_pen_input_bits : hadal_pen_input {
    hadal_pen_input_down       = (1u << 0), /**< Pen is pressed down. */
    hadal_pen_input_button_1   = (1u << 1), /**< Button 1 is pressed. */
    hadal_pen_input_button_2   = (1u << 2), /**< Button 2 is pressed. */
    hadal_pen_input_button_3   = (1u << 3), /**< Button 3 is pressed. */
    hadal_pen_input_button_4   = (1u << 4), /**< Button 4 is pressed. */
    hadal_pen_input_button_5   = (1u << 5), /**< Button 5 is pressed. */
    hadal_pen_input_eraser_tip = (1u << 6), /**< Eraser tip is used. */
} hadal_pen_input_bits;

typedef enum hadal_pen_axis : u8 {
    hadal_pen_axis_pressure = 0,        /**< Pen pressure. Unidirectional: 0.0 to 1.0. */
    hadal_pen_axis_xtilt,               /**< Pen horizontal tilt angle. Bidirectional: -90.0 to 90.0 (left-to-right). */
    hadal_pen_axis_ytilt,               /**< Pen vertical tilt angle. Bidirectional: -90.0 to 90.0 (top-to-down). */
    hadal_pen_axis_distance,            /**< Pen distance to drawing surface. Unidirectional: 0.0 to 1.0. */
    hadal_pen_axis_rotation,            /**< Pen barrel rotation. Bidirectional: -180.0 to 179.9 (clockwise, 0 is facing up, -180 is facing down). */
    hadal_pen_axis_slider,              /**< Pen finger wheel or slider. Unidirectional: 0.0 to 1.0. */
    hadal_pen_axis_tangential_pressure, /**< Pressure from squeezing the pen ("barrel pressure"). */
    hadal_pen_axis_count,               /**< Total known pen axis types. */
} hadal_pen_axis;

typedef enum hadal_window_flag_bits : u32 {
    hadal_window_flag_none                  = 0u,
    hadal_window_flag_is_valid              = (1u << 0),
    hadal_window_flag_should_close          = (1u << 1),
    hadal_window_flag_visible               = (1u << 2),
    hadal_window_flag_fullscreen            = (1u << 3),
    hadal_window_flag_maximized             = (1u << 4),
    hadal_window_flag_minimized             = (1u << 5),
    hadal_window_flag_resizable             = (1u << 6),
    hadal_window_flag_no_decorations        = (1u << 7),
    hadal_window_flag_input_focus           = (1u << 8),
    hadal_window_flag_mouse_focus           = (1u << 9),
    hadal_window_flag_mouse_captured        = (1u << 11),
    hadal_window_flag_mouse_relative        = (1u << 12),
    hadal_window_flag_cursor_hovered        = (1u << 13),
    hadal_window_flag_modal                 = (1u << 14),
    hadal_window_flag_occluded              = (1u << 15),
    hadal_window_flag_always_on_top         = (1u << 16),
    hadal_window_flag_auto_minimize         = (1u << 17),
    hadal_window_flag_shell_activated       = (1u << 18),
    hadal_window_flag_mouse_grabbed         = (1u << 19),
    hadal_window_flag_keyboard_grabbed      = (1u << 20),
    hadal_window_flag_transparent           = (1u << 21),
    hadal_window_flag_vulkan                = (1u << 22),
    hadal_window_flag_swapchain_out_of_date = (1u << 23),
} hadal_window_flag_bits;

typedef struct hadal_window_assembly {
    s32                 width, height;
    s32                 min_width, min_height;
    s32                 max_width, max_height;
    s32                 numer, denom;
    u32                 flag_hints;
    hadal_display       fullscreen;
    lake_small_string   name;
} hadal_window_assembly;

typedef struct hadal_window_impl *(LAKECALL *PFN_hadal_window_assembly)(struct hadal_impl *hadal, hadal_window_assembly const *assembly);
#define FN_HADAL_WINDOW_ASSEMBLY(backend) \
    struct hadal_window_impl *LAKECALL _hadal_##backend##_window_assembly(struct hadal_impl *hadal, hadal_window_assembly const *assembly)

PFN_LAKE_WORK(PFN_hadal_window_zero_refcnt, struct hadal_window_impl *window);
#define FN_HADAL_WINDOW_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_hadal_##backend##_window_zero_refcnt, struct hadal_window_impl *window)

#ifdef MOON_VULKAN
/* to avoid including the Vulkan header */
struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
typedef void (*(*PFN_vkGetInstanceProcAddr)(struct VkInstance_T *, char const *))(void);

/** Allows the display backend to query procedures it needs to support Vulkan. */
typedef lake_result (LAKECALL *PFN_hadal_vulkan_connect_instance)(struct hadal_impl *hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr);
#define FN_HADAL_VULKAN_CONNECT_INSTANCE(backend) \
    lake_result LAKECALL _hadal_##backend##_vulkan_connect_instance(struct hadal_impl *hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr)

/** Returns whether a queue family in the physical device is capable of presenting to a display. */
typedef bool (LAKECALL *PFN_hadal_vulkan_presentation_support)(struct hadal_impl *hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family);
#define FN_HADAL_VULKAN_PRESENTATION_SUPPORT(backend) \
    bool LAKECALL _hadal_##backend##_vulkan_presentation_support(struct hadal_impl *hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family)

/** Create a surface that a Vulkan renderer may draw to. */
typedef lake_result (LAKECALL *PFN_hadal_vulkan_create_surface)(struct hadal_window_impl *window, struct VkAllocationCallbacks const *callbacks, struct VkSurfaceKHR_T **out_surface);
#define FN_HADAL_VULKAN_CREATE_SURFACE(backend) \
    lake_result LAKECALL _hadal_##backend##_vulkan_create_surface(struct hadal_window_impl *window, struct VkAllocationCallbacks const *callbacks, struct VkSurfaceKHR_T **out_surface)
#endif /* MOON_VULKAN */

/** Interface of the display backend. */
struct hadal_interface_impl {
    lake_interface_header                   header;
    lake_darray_t(hadal_display)            displays;
#ifdef MOON_VULKAN
    PFN_hadal_vulkan_connect_instance       vulkan_connect_instance;
    PFN_hadal_vulkan_presentation_support   vulkan_presentation_support;
    PFN_hadal_vulkan_create_surface         vulkan_create_surface;
#endif /* MOON_VULKAN */
    PFN_hadal_window_assembly               window_assembly;
    PFN_hadal_window_zero_refcnt            window_zero_refcnt;
};

/* I guess only the window is what the application creates directly. */
LAKE_IMPL_HANDLE_INTERFACED(hadal_window, hadal_interface hadal,  s32 fb_width, fb_height; char const *title; )

typedef lake_bedrock hadal_interface_assembly;
#ifdef HADAL_WIN32
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, win32, hadal_interface_assembly);
#endif /* HADAL_WIN32 */
#ifdef HADAL_COCOA
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, cocoa, hadal_interface_assembly);
#endif /* HADAL_COCOA */
#ifdef HADAL_UIKIT
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, uikit, hadal_interface_assembly);
#endif /* HADAL_UIKIT */
#ifdef HADAL_ANDROID
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, android, hadal_interface_assembly);
#endif /* HADAL_ANDROID */
#ifdef HADAL_HAIKU
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, haiku, hadal_interface_assembly);
#endif /* HADAL_HAIKU */
#ifdef HADAL_HTML5
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, html5, hadal_interface_assembly);
#endif /* HADAL_HTML5 */
#ifdef HADAL_WAYLAND
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, wayland, hadal_interface_assembly);
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, xcb, hadal_interface_assembly);
#endif /* HADAL_XCB */
#ifdef HADAL_KMS
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, kms, hadal_interface_assembly);
#endif /* HADAL_KMS */

/** Headless display backend, used for validation and rendering without a swapchain. */
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadal, headless, hadal_interface_assembly);

#ifdef __cplusplus
}
#endif /* __cplusplus */
