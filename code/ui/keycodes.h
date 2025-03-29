/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
#ifndef __KEYCODES_H__
#define __KEYCODES_H__

// these are the key numbers that should be passed to KeyEvent

// A copy of SDL scancodes + quake keycodes for mouse and gamepad
typedef enum
{
    // keyboard keys
    K_UNKNOWN = 0,
    K_A = 4,
    K_B = 5,
    K_C = 6,
    K_D = 7,
    K_E = 8,
    K_F = 9,
    K_G = 10,
    K_H = 11,
    K_I = 12,
    K_J = 13,
    K_K = 14,
    K_L = 15,
    K_M = 16,
    K_N = 17,
    K_O = 18,
    K_P = 19,
    K_Q = 20,
    K_R = 21,
    K_S = 22,
    K_T = 23,
    K_U = 24,
    K_V = 25,
    K_W = 26,
    K_X = 27,
    K_Y = 28,
    K_Z = 29,

    K_1 = 30,
    K_2 = 31,
    K_3 = 32,
    K_4 = 33,
    K_5 = 34,
    K_6 = 35,
    K_7 = 36,
    K_8 = 37,
    K_9 = 38,
    K_0 = 39,

    K_RETURN = 40,
    K_ESCAPE = 41,
    K_BACKSPACE = 42,
    K_TAB = 43,
    K_SPACE = 44,

    K_MINUS = 45,
    K_EQUALS = 46,
    K_LEFTBRACKET = 47,
    K_RIGHTBRACKET = 48,
    K_BACKSLASH = 49,
    K_NONUSHASH = 50, /** As a user, you should not rely on this code because SDL
                        * will never generate it with most (all?) keyboards. */
    K_SEMICOLON = 51,
    K_APOSTROPHE = 52,
    K_GRAVE = 53,
    K_COMMA = 54,
    K_PERIOD = 55,
    K_SLASH = 56,

    K_CAPSLOCK = 57,

    K_F1 = 58,
    K_F2 = 59,
    K_F3 = 60,
    K_F4 = 61,
    K_F5 = 62,
    K_F6 = 63,
    K_F7 = 64,
    K_F8 = 65,
    K_F9 = 66,
    K_F10 = 67,
    K_F11 = 68,
    K_F12 = 69,

    K_PRINTSCREEN = 70,
    K_SCROLLLOCK = 71,
    K_PAUSE = 72,
    K_INSERT = 73,
    K_HOME = 74,
    K_PAGEUP = 75,
    K_DELETE = 76,
    K_END = 77,
    K_PAGEDOWN = 78,
    K_RIGHT = 79,
    K_LEFT = 80,
    K_DOWN = 81,
    K_UP = 82,

    K_NUMLOCKCLEAR = 83,
    K_KP_DIVIDE = 84,
    K_KP_MULTIPLY = 85,
    K_KP_MINUS = 86,
    K_KP_PLUS = 87,
    K_KP_ENTER = 88,
    K_KP_1 = 89,
    K_KP_2 = 90,
    K_KP_3 = 91,
    K_KP_4 = 92,
    K_KP_5 = 93,
    K_KP_6 = 94,
    K_KP_7 = 95,
    K_KP_8 = 96,
    K_KP_9 = 97,
    K_KP_0 = 98,
    K_KP_PERIOD = 99,

    K_NONUSBACKSLASH = 100,
    K_APPLICATION = 101,
    K_POWER = 102,
    K_KP_EQUALS = 103,
    K_F13 = 104,
    K_F14 = 105,
    K_F15 = 106,
    K_F16 = 107,
    K_F17 = 108,
    K_F18 = 109,
    K_F19 = 110,
    K_F20 = 111,
    K_F21 = 112,
    K_F22 = 113,
    K_F23 = 114,
    K_F24 = 115,
    K_EXECUTE = 116,
    K_HELP = 117,
    K_MENU = 118,
    K_SELECT = 119,
    K_STOP = 120,
    K_AGAIN = 121,
    K_UNDO = 122,
    K_CUT = 123,
    K_COPY = 124,
    K_PASTE = 125,
    K_FIND = 126,
    K_MUTE = 127,
    K_VOLUMEUP = 128,
    K_VOLUMEDOWN = 129,

    K_KP_COMMA = 133,
    K_KP_EQUALSAS400 = 134,

    K_INTERNATIONAL1 = 135,
    K_INTERNATIONAL2 = 136,
    K_INTERNATIONAL3 = 137,
    K_INTERNATIONAL4 = 138,
    K_INTERNATIONAL5 = 139,
    K_INTERNATIONAL6 = 140,
    K_INTERNATIONAL7 = 141,
    K_INTERNATIONAL8 = 142,
    K_INTERNATIONAL9 = 143,
    K_LANG1 = 144,
    K_LANG2 = 145,
    K_LANG3 = 146,
    K_LANG4 = 147,
    K_LANG5 = 148,
    K_LANG6 = 149,
    K_LANG7 = 150,
    K_LANG8 = 151,
    K_LANG9 = 152,

    K_ALTERASE = 153,
    K_SYSREQ = 154,
    K_CANCEL = 155,
    K_CLEAR = 156,
    K_PRIOR = 157,
    K_RETURN2 = 158,
    K_SEPARATOR = 159,
    K_OUT = 160,
    K_OPER = 161,
    K_CLEARAGAIN = 162,
    K_CRSEL = 163,
    K_EXSEL = 164,

    K_KP_00 = 176,
    K_KP_000 = 177,
    K_THOUSANDSSEPARATOR = 178,
    K_DECIMALSEPARATOR = 179,
    K_CURRENCYUNIT = 180,
    K_CURRENCYSUBUNIT = 181,
    K_KP_LEFTPAREN = 182,
    K_KP_RIGHTPAREN = 183,
    K_KP_LEFTBRACE = 184,
    K_KP_RIGHTBRACE = 185,
    K_KP_TAB = 186,
    K_KP_BACKSPACE = 187,
    K_KP_A = 188,
    K_KP_B = 189,
    K_KP_C = 190,
    K_KP_D = 191,
    K_KP_E = 192,
    K_KP_F = 193,
    K_KP_XOR = 194,
    K_KP_POWER = 195,
    K_KP_PERCENT = 196,
    K_KP_LESS = 197,
    K_KP_GREATER = 198,
    K_KP_AMPERSAND = 199,
    K_KP_DBLAMPERSAND = 200,
    K_KP_VERTICALBAR = 201,
    K_KP_DBLVERTICALBAR = 202,
    K_KP_COLON = 203,
    K_KP_HASH = 204,
    K_KP_SPACE = 205,
    K_KP_AT = 206,
    K_KP_EXCLAM = 207,
    K_KP_MEMSTORE = 208,
    K_KP_MEMRECALL = 209,
    K_KP_MEMCLEAR = 210,
    K_KP_MEMADD = 211,
    K_KP_MEMSUBTRACT = 212,
    K_KP_MEMMULTIPLY = 213,
    K_KP_MEMDIVIDE = 214,
    K_KP_PLUSMINUS = 215,
    K_KP_CLEAR = 216,
    K_KP_CLEARENTRY = 217,
    K_KP_BINARY = 218,
    K_KP_OCTAL = 219,
    K_KP_DECIMAL = 220,
    K_KP_HEXADECIMAL = 221,

    K_LCTRL = 224,
    K_LSHIFT = 225,
    K_LALT = 226,
    K_LGUI = 227,
    K_RCTRL = 228,
    K_RSHIFT = 229,
    K_RALT = 230,
    K_RGUI = 231,

    K_MODE = 257,

    // usage keys
    K_SLEEP = 258,
    K_WAKE = 259,

    K_CHANNEL_INCREMENT = 260,
    K_CHANNEL_DECREMENT = 261,

    K_MEDIA_PLAY = 262,
    K_MEDIA_PAUSE = 263,
    K_MEDIA_RECORD = 264,
    K_MEDIA_FAST_FORWARD = 265,
    K_MEDIA_REWIND = 266,
    K_MEDIA_NEXT_TRACK = 267,
    K_MEDIA_PREVIOUS_TRACK = 268,
    K_MEDIA_STOP = 269,
    K_MEDIA_EJECT = 270,
    K_MEDIA_PLAY_PAUSE = 271,
    K_MEDIA_SELECT = 272,

    K_AC_NEW = 273,
    K_AC_OPEN = 274,
    K_AC_CLOSE = 275,
    K_AC_EXIT = 276,
    K_AC_SAVE = 277,
    K_AC_PRINT = 278,
    K_AC_PROPERTIES = 279,

    K_AC_SEARCH = 280,
    K_AC_HOME = 281,
    K_AC_BACK = 282,
    K_AC_FORWARD = 283,
    K_AC_STOP = 284,
    K_AC_REFRESH = 285,
    K_AC_BOOKMARKS = 286,

    // mobile keys
    K_SOFTLEFT = 287,
    K_SOFTRIGHT = 288,
    K_CALL = 289,
    K_ENDCALL = 290,

    /* Add any other keys here. */

    K_RESERVED = 400,    /**< 400-500 reserved for dynamic keycodes */

    K_MOUSE1,
    K_MOUSE2,
    K_MOUSE3,
    K_MOUSE4,
    K_MOUSE5,

    K_MOUSE_WHEELUP,
    K_MOUSE_WHEELDOWN,

    K_JOY1,
    K_JOY2,
    K_JOY3,
    K_JOY4,
    K_JOY5,
    K_JOY6,
    K_JOY7,
    K_JOY8,
    K_JOY9,
    K_JOY10,
    K_JOY11,
    K_JOY12,
    K_JOY13,
    K_JOY14,
    K_JOY15,
    K_JOY16,
    K_JOY17,
    K_JOY18,
    K_JOY19,
    K_JOY20,
    K_JOY21,
    K_JOY22,
    K_JOY23,
    K_JOY24,
    K_JOY25,
    K_JOY26,
    K_JOY27,
    K_JOY28,
    K_JOY29,
    K_JOY30,
    K_JOY31,
    K_JOY32,

    K_COUNT = 512
} keyNum_t;


// The menu code needs to get both key and char events, but
// to avoid duplicating the paths, the char events are just
// distinguished by or'ing in K_CHAR_FLAG (ugly)
#define	K_CHAR_FLAG		1024

#endif
