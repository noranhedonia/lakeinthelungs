#pragma once
#include <lake/bedrock/types.h>

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
    hadal_scancode_backslash = 49, /**< Located at the lower left of the return
                                    *   key on ISO keyboards and at the right end
                                    *   of the QWERTY row on ANSI keyboards.
                                    *   Produces REVERSE SOLIDUS (backslash) and
                                    *   VERTICAL LINE in a US layout, REVERSE
                                    *   SOLIDUS and VERTICAL LINE in a UK Mac
                                    *   layout, NUMBER SIGN and TILDE in a UK
                                    *   Windows layout, DOLLAR SIGN and POUND SIGN
                                    *   in a Swiss German layout, NUMBER SIGN and
                                    *   APOSTROPHE in a German layout, GRAVE
                                    *   ACCENT and POUND SIGN in a French Mac
                                    *   layout, and ASTERISK and MICRO SIGN in a
                                    *   French Windows layout. */
    hadal_scancode_nonushash = 50, /**< ISO USB keyboards actually use this code
                                    *   instead of 49 for the same key, but all
                                    *   OSes I've seen treat the two codes
                                    *   identically. So, as an implementor, unless
                                    *   your keyboard generates both of those
                                    *   codes and your OS treats them differently,
                                    *   you should generate SDL_SCANCODE_BACKSLASH
                                    *   instead of this code. As a user, you
                                    *   should not rely on this code because SDL
                                    *   will never generate it with most (all?)
                                    *   keyboards. */
    hadal_scancode_semicolon = 51,
    hadal_scancode_apostrophe = 52,
    hadal_scancode_grave = 53, /**< Located in the top left corner (on both ANSI
                                *   and ISO keyboards). Produces GRAVE ACCENT and
                                *   TILDE in a US Windows layout and in US and UK
                                *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                                *   and NOT SIGN in a UK Windows layout, SECTION
                                *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                                *   layouts on ISO keyboards, SECTION SIGN and
                                *   DEGREE SIGN in a Swiss German layout (Mac:
                                *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                                *   DEGREE SIGN in a German layout (Mac: only on
                                *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                                *   French Windows layout, COMMERCIAL AT and
                                *   NUMBER SIGN in a French Mac layout on ISO
                                *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                                *   SIGN in a Swiss German, German, or French Mac
                                *   layout on ANSI keyboards. */
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

    hadal_scancode_nonusbackslash = 100, /**< This is the additional key that ISO
                                          *   keyboards have over ANSI ones,
                                          *   located between left shift and Y.
                                          *   Produces GRAVE ACCENT and TILDE in a
                                          *   US or UK Mac layout, REVERSE SOLIDUS
                                          *   (backslash) and VERTICAL LINE in a
                                          *   US or UK Windows layout, and
                                          *   LESS-THAN SIGN and GREATER-THAN SIGN
                                          *   in a Swiss German, German, or French
                                          *   layout. */
    hadal_scancode_application = 101, /**< windows contextual menu, compose */
    hadal_scancode_power = 102, /**< The USB document says this is a status flag,
                                 *   not a physical key - but some Mac keyboards
                                 *   do have a power key. */
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
/* not sure whether there's a reason to enable these */
/*     scancode_lockingcapslock = 130,  */
/*     scancode_lockingnumlock = 131, */
/*     scancode_lockingscrolllock = 132, */
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

    hadal_scancode_mode = 257, /**< I'm not sure if this is really not covered
                          *   by any of the above, but since there's a
                          *   special SDL_KMOD_MODE for it I'm adding it here */

    /** These values are mapped from usage page 0x0C (USB consumer page).
     *
     *  There are way more keys in the spec than we can represent in the
     *  current scancode range, so pick the ones that commonly come up in
     *  real world usage. */

    hadal_scancode_sleep = 258,               /**< Sleep */
    hadal_scancode_wake = 259,                /**< Wake */

    hadal_scancode_channel_increment = 260,   /**< Channel Increment */
    hadal_scancode_channel_decrement = 261,   /**< Channel Decrement */

    hadal_scancode_media_play = 262,          /**< Play */
    hadal_scancode_media_pause = 263,         /**< Pause */
    hadal_scancode_media_record = 264,        /**< Record */
    hadal_scancode_media_fast_forward = 265,  /**< Fast Forward */
    hadal_scancode_media_rewind = 266,        /**< Rewind */
    hadal_scancode_media_next_track = 267,    /**< Next Track */
    hadal_scancode_media_previous_track = 268, /**< Previous Track */
    hadal_scancode_media_stop = 269,          /**< Stop */
    hadal_scancode_media_eject = 270,         /**< Eject */
    hadal_scancode_media_play_pause = 271,    /**< Play / Pause */
    hadal_scancode_media_select = 272,        /* Media Select */

    hadal_scancode_ac_new = 273,              /**< AC New */
    hadal_scancode_ac_open = 274,             /**< AC Open */
    hadal_scancode_ac_close = 275,            /**< AC Close */
    hadal_scancode_ac_exit = 276,             /**< AC Exit */
    hadal_scancode_ac_save = 277,             /**< AC Save */
    hadal_scancode_ac_print = 278,            /**< AC Print */
    hadal_scancode_ac_properties = 279,       /**< AC Properties */

    hadal_scancode_ac_search = 280,           /**< AC Search */
    hadal_scancode_ac_home = 281,             /**< AC Home */
    hadal_scancode_ac_back = 282,             /**< AC Back */
    hadal_scancode_ac_forward = 283,          /**< AC Forward */
    hadal_scancode_ac_stop = 284,             /**< AC Stop */
    hadal_scancode_ac_refresh = 285,          /**< AC Refresh */
    hadal_scancode_ac_bookmarks = 286,        /**< AC Bookmarks */

    /* these are values that are often used on mobile phones */

    hadal_scancode_softleft = 287, /**< usually situated below the display on phones and
                                    *   used as a multi-function feature key for selecting
                                    *   a software defined function shown on the bottom left
                                    *   of the display. */
    hadal_scancode_softright = 288, /**< usually situated below the display on phones and
                                     *   used as a multi-function feature key for selecting
                                     *    a software defined function shown on the bottom right
                                     *    of the display. */
    hadal_scancode_call = 289, /**< used for accepting phone calls. */
    hadal_scancode_endcall = 290, /**< used for rejecting phone calls. */

    /* add any other keys here. */
    hadal_scancode_reserved = 400,    /**< 400-500 reserved for dynamic keycodes */

    hadal_scancode_count = 512 /**< not a key, just marks the number of scancodes for array bounds */
} hadal_scancode;
