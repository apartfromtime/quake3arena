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

#include "../client/client.h"
#include "../system/sdl_local.h"
#include "../win32/win_local.h"

// when we get a windows message, we store the time off so keyboard processing
// can know the exact time of an event
unsigned int s_sysMsgTime;

/*
=========================================================================

JOYSTICK

=========================================================================
*/

typedef struct
{
	bool	mouseActive;
	bool	mouseInitialized;
} WinMouseVars_t;

static WinMouseVars_t s_wmv;

//
// Joystick definitions
//
#define	JOY_MAX_AXES		6			// X, Y, Z, R, U, V

typedef struct
{
	SDL_Gamepad* gamepad;
	int id;			// joystick number
	int oldbuttonstate;
	int	oldpovstate;
	bool avail;
} joystickInfo_t;

static	joystickInfo_t	joy;

cvar_t* in_joystick;
cvar_t* in_joyBallScale;
cvar_t* in_debugJoystick;
cvar_t* joy_threshold;

bool	in_appactive;

/*
===============
IN_StartupJoystick
===============
*/
void IN_StartupJoystick(void)
{
	int numdevs;

	// assume no joystick
	joy.avail = false;

	if (!in_joystick->integer) {
		Com_Printf("Joystick is not active.\n");
		return;
	}

	if (!SDL_WasInit(SDL_INIT_GAMEPAD)) {
		if (!SDL_Init(SDL_INIT_GAMEPAD)) {
			Com_Printf("Couldn't init SDL gamepad: %s.\n", SDL_GetError());
			return;
		}
	}

	SDL_AddGamepadMappingsFromFile("gamecontrollerdb.txt");

	// verify joystick driver is present
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&numdevs);
	if (numdevs == 0) {
		Com_Printf("joystick not found -- driver not present\n");
		return;
	}

	// cycle through the joystick ids for the first valid one
	joy.gamepad = NULL;
	for (joy.id = 0; joy.id < numdevs; joy.id++)
	{
		if (SDL_IsGamepad(joysticks[joy.id]) == true) {
			joy.gamepad = SDL_OpenGamepad(joysticks[joy.id]);
			break;
		}
	}

	// abort startup if we didn't find a valid joystick
	if (joy.gamepad == NULL) {
		Com_Printf("joystick not found -- no valid joysticks\n");
		return;
	}

	// get the capabilities of the selected joystick
	Com_Printf("Joystick found.\n");
	Com_Printf("Pname: %s\n", SDL_GetGamepadName(joy.gamepad));
	Com_Printf("OemVxD: %d\n", SDL_GetGamepadProductVersion(joy.gamepad));
	Com_Printf("RegKey: %s\n", SDL_GetGamepadSerial(joy.gamepad));

	int numButtons = 0;
	int numAxes = 0;

	for (size_t i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++)
	{
		numButtons = SDL_GamepadHasButton(joy.gamepad, i) == true ? numButtons + 1 : numButtons;
	}

	for (size_t i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++)
	{
		numAxes = SDL_GamepadHasAxis(joy.gamepad, i) == true ? numAxes + 1 : numAxes;
	}

	Com_Printf("Numbuttons: %i / %i\n", numButtons, SDL_GAMEPAD_BUTTON_COUNT);
	Com_Printf("Axis: %i / %i\n", numAxes, SDL_GAMEPAD_AXIS_COUNT);

	// old button and POV states default to no buttons pressed
	joy.oldbuttonstate = 0;
	joy.oldpovstate = 0;

	// mark the joystick as available
	joy.avail = true;
}

/*
===========
JoyToF
===========
*/
float JoyToF(int value)
{
	float fValue;

	if (value > -(int)(joy_threshold->value * 0X7FFF) &&
		value <  (int)(joy_threshold->value * 0X7FFF)) {

		return 0;
	}

	if (value > 0) {
		value++;
	}

	// convert range from -32768..32767 to -1..1 
	fValue = (float)value / 32768.0;

	if (fValue < -1.0f) {
		fValue = -1.0f;
	}
	if (fValue > 1.0f) {
		fValue = 1.0f;
	}

	return fValue;
}

/*
===========
JoyToI
===========
*/
int JoyToI(int value)
{
	if (value > -(int)(joy_threshold->value * 0X7FFF) &&
		value <  (int)(joy_threshold->value * 0X7FFF)) {
		return 0;
	}

	if (value > 0) {
		value++;
	}

	return value;
}

/*
============================================================

  MOUSE CONTROL

============================================================
*/

cvar_t* in_mouse;

/*
===========
IN_ActivateMouse

Called when the window gains focus or changes in some way
===========
*/
void IN_ActivateMouse(void)
{
	if (!s_wmv.mouseInitialized) {
		return;
	}

	if (!in_mouse->integer) {
		s_wmv.mouseActive = false;
		return;
	}

	if (s_wmv.mouseActive) {
		return;
	}

	s_wmv.mouseActive = true;
	SDL_CaptureMouse(true);
	SDL_HideCursor();
	SDL_SetWindowRelativeMouseMode(g_wv.hWnd, true);
}

/*
===========
IN_DeactivateMouse

Called when the window loses focus
===========
*/
void IN_DeactivateMouse(void)
{
	if (!s_wmv.mouseInitialized) {
		return;
	}

	if (!s_wmv.mouseActive) {
		return;
	}

	s_wmv.mouseActive = false;
	SDL_SetWindowRelativeMouseMode(g_wv.hWnd, false);
	SDL_ShowCursor();
	SDL_CaptureMouse(false);
}

/*
===========
IN_StartupMouse
===========
*/
void IN_StartupMouse(void)
{
	s_wmv.mouseInitialized = false;

	if (in_mouse->integer == 0) {
		Com_Printf("Mouse control not active.\n");
		return;
	}

	s_wmv.mouseInitialized = true;
}


/*
=========================================================================

INPUT

=========================================================================
*/


/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown(void)
{
	IN_DeactivateMouse();
}

/*
===========
IN_Init
===========
*/
void IN_Init(void)
{
	Com_Printf("\n------- Input Initialization -------\n");

	// mouse variables
	in_mouse = Cvar_Get("in_mouse", "1", CVAR_ARCHIVE | CVAR_LATCH);

	// joystick variables
	in_joystick = Cvar_Get("in_joystick", "1", CVAR_ARCHIVE | CVAR_LATCH);
	in_joyBallScale = Cvar_Get("in_joyBallScale", "0.02", CVAR_ARCHIVE);
	in_debugJoystick = Cvar_Get("in_debugjoystick", "0", CVAR_TEMP);

	joy_threshold = Cvar_Get("joy_threshold", "0.15", CVAR_ARCHIVE);

	IN_StartupMouse();
	IN_StartupJoystick();

	Com_Printf("------------------------------------\n");
}

/*
===========
IN_Activate

Called when the main window gains or loses focus.
The window may have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void IN_Activate(bool active)
{
	in_appactive = active;

	if (!active) {
		IN_DeactivateMouse();
	}
}

/*
==================
IN_Frame

Called every frame, even if not generating commands
==================
*/
void IN_Frame(void)
{
	if (!s_wmv.mouseInitialized) {
		return;
	}

	if (cls.keyCatchers & KEYCATCH_CONSOLE) {
		// temporarily deactivate if not in the game and running on the desktop
		if (Cvar_VariableValue("r_fullscreen") == 0) {
			IN_DeactivateMouse();
			return;
		}
	}

	if (!in_appactive) {
		IN_DeactivateMouse();
		return;
	}

	IN_ActivateMouse();
}

WinVars_t g_wv;

// Console variables that we need to access from this module
cvar_t* r_fullscreen;
cvar_t* vid_xpos;			// X coordinate of window position
cvar_t* vid_ypos;			// Y coordinate of window position

//==========================================================================

static uint32_t s_scantosdlg[SDL_SCANCODE_COUNT] = {
	// gamepad buttons
	// K_UNKNOWN,
	K_GAMEPAD_BUTTON_SOUTH,
	K_GAMEPAD_BUTTON_EAST,
	K_GAMEPAD_BUTTON_WEST,
	K_GAMEPAD_BUTTON_NORTH,
	K_GAMEPAD_BUTTON_BACK,
	K_GAMEPAD_BUTTON_GUIDE,
	K_GAMEPAD_BUTTON_START,
	K_GAMEPAD_BUTTON_LEFT_STICK,
	K_GAMEPAD_BUTTON_RIGHT_STICK,
	K_GAMEPAD_BUTTON_LEFT_SHOULDER,
	K_GAMEPAD_BUTTON_RIGHT_SHOULDER,
	K_GAMEPAD_BUTTON_DPAD_UP,
	K_GAMEPAD_BUTTON_DPAD_DOWN,
	K_GAMEPAD_BUTTON_DPAD_LEFT,
	K_GAMEPAD_BUTTON_DPAD_RIGHT,
	K_GAMEPAD_BUTTON_MISC1,
	K_GAMEPAD_BUTTON_RIGHT_PADDLE1,
	K_GAMEPAD_BUTTON_LEFT_PADDLE1,
	K_GAMEPAD_BUTTON_RIGHT_PADDLE2,
	K_GAMEPAD_BUTTON_LEFT_PADDLE2,
	K_GAMEPAD_BUTTON_TOUCHPAD,
	K_GAMEPAD_BUTTON_MISC2,
	K_GAMEPAD_BUTTON_MISC3,
	K_GAMEPAD_BUTTON_MISC4,
	K_GAMEPAD_BUTTON_MISC5,
	K_GAMEPAD_BUTTON_MISC6,
	K_GAMEPAD_BUTTON_LEFT_TRIGGER,
	K_GAMEPAD_BUTTON_RIGHT_TRIGGER,
};

static uint32_t s_scantosdlb[SDL_SCANCODE_COUNT] = {
	// mouse buttons
	K_UNKNOWN,
	K_MOUSE1,
	K_MOUSE2,
	K_MOUSE3,
	K_MOUSE4,
	K_MOUSE5,
	K_MOUSE_WHEELUP,
	K_MOUSE_WHEELDOWN,
};

static uint32_t s_scantosdlk[SDL_SCANCODE_COUNT] = {
	// keyboard keys
	K_UNKNOWN,
	0,
	0,
	0,
	K_A,
	K_B,
	K_C,
	K_D,
	K_E,
	K_F,
	K_G,
	K_H,
	K_I,
	K_J,
	K_K,
	K_L,
	K_M,
	K_N,
	K_O,
	K_P,
	K_Q,
	K_R,
	K_S,
	K_T,
	K_U,
	K_V,
	K_W,
	K_X,
	K_Y,
	K_Z,
	K_1,
	K_2,
	K_3,
	K_4,
	K_5,
	K_6,
	K_7,
	K_8,
	K_9,
	K_0,
	K_RETURN,
	K_ESCAPE,
	K_BACKSPACE,
	K_TAB,
	K_SPACE,
	K_MINUS,
	K_EQUALS,
	K_LEFTBRACKET,
	K_RIGHTBRACKET,
	K_BACKSLASH,
	0,
	K_SEMICOLON,
	K_APOSTROPHE,
	K_GRAVE,
	K_COMMA,
	K_PERIOD,
	K_SLASH,
	K_CAPSLOCK,
	K_F1,
	K_F2,
	K_F3,
	K_F4,
	K_F5,
	K_F6,
	K_F7,
	K_F8,
	K_F9,
	K_F10,
	K_F11,
	K_F12,
	K_PRINTSCREEN,
	K_SCROLLLOCK,
	K_PAUSE,
	K_INSERT,
	K_HOME,
	K_PAGEUP,
	K_DELETE,
	K_END,
	K_PAGEDOWN,
	K_RIGHT,
	K_LEFT,
	K_DOWN,
	K_UP,
	K_NUMLOCKCLEAR,
	K_KP_DIVIDE,
	K_KP_MULTIPLY,
	K_KP_MINUS,
	K_KP_PLUS,
	K_KP_ENTER,
	K_KP_1,
	K_KP_2,
	K_KP_3,
	K_KP_4,
	K_KP_5,
	K_KP_6,
	K_KP_7,
	K_KP_8,
	K_KP_9,
	K_KP_0,
	K_KP_PERIOD,
	0,
	K_APPLICATION,
	K_POWER,
	K_KP_EQUALS,
	K_F13,
	K_F14,
	K_F15,
	K_F16,
	K_F17,
	K_F18,
	K_F19,
	K_F20,
	K_F21,
	K_F22,
	K_F23,
	K_F24,
	K_EXECUTE,
	K_HELP,
	K_MENU,
	K_SELECT,
	K_STOP,
	K_AGAIN,
	K_UNDO,
	K_CUT,
	K_COPY,
	K_PASTE,
	K_FIND,
	K_MUTE,
	K_VOLUMEUP,
	K_VOLUMEDOWN,
	0,
	0,
	0,
	K_KP_COMMA,
	K_KP_EQUALSAS400,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	K_ALTERASE,
	K_SYSREQ,
	K_CANCEL,
	K_CLEAR,
	K_PRIOR,
	K_RETURN2,
	K_SEPARATOR,
	K_OUT,
	K_OPER,
	K_CLEARAGAIN,
	K_CRSEL,
	K_EXSEL,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	K_KP_00,
	K_KP_000,
	K_THOUSANDSSEPARATOR,
	K_DECIMALSEPARATOR,
	K_CURRENCYUNIT,
	K_CURRENCYSUBUNIT,
	K_KP_LEFTPAREN,
	K_KP_RIGHTPAREN,
	K_KP_LEFTBRACE,
	K_KP_RIGHTBRACE,
	K_KP_TAB,
	K_KP_BACKSPACE,
	K_KP_A,
	K_KP_B,
	K_KP_C,
	K_KP_D,
	K_KP_E,
	K_KP_F,
	K_KP_XOR,
	K_KP_POWER,
	K_KP_PERCENT,
	K_KP_LESS,
	K_KP_GREATER,
	K_KP_AMPERSAND,
	K_KP_DBLAMPERSAND,
	K_KP_VERTICALBAR,
	K_KP_DBLVERTICALBAR,
	K_KP_COLON,
	K_KP_HASH,
	K_KP_SPACE,
	K_KP_AT,
	K_KP_EXCLAM,
	K_KP_MEMSTORE,
	K_KP_MEMRECALL,
	K_KP_MEMCLEAR,
	K_KP_MEMADD,
	K_KP_MEMSUBTRACT,
	K_KP_MEMMULTIPLY,
	K_KP_MEMDIVIDE,
	K_KP_PLUSMINUS,
	K_KP_CLEAR,
	K_KP_CLEARENTRY,
	K_KP_BINARY,
	K_KP_OCTAL,
	K_KP_DECIMAL,
	K_KP_HEXADECIMAL,
	0,
	0,
	K_LCTRL,
	K_LSHIFT,
	K_LALT,
	K_LGUI,
	K_RCTRL,
	K_RSHIFT,
	K_RALT,
	K_RGUI,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	K_MODE,

	// usage keys
	K_SLEEP,
	K_WAKE,
	K_CHANNEL_INCREMENT,
	K_CHANNEL_DECREMENT,
	K_MEDIA_PLAY,
	K_MEDIA_PAUSE,
	K_MEDIA_RECORD,
	K_MEDIA_FAST_FORWARD,
	K_MEDIA_REWIND,
	K_MEDIA_NEXT_TRACK,
	K_MEDIA_PREVIOUS_TRACK,
	K_MEDIA_STOP,
	K_MEDIA_EJECT,
	K_MEDIA_PLAY_PAUSE,
	K_MEDIA_SELECT,
	K_AC_NEW,
	K_AC_OPEN,
	K_AC_CLOSE,
	K_AC_EXIT,
	K_AC_SAVE,
	K_AC_PRINT,
	K_AC_PROPERTIES,
	K_AC_SEARCH,
	K_AC_HOME,
	K_AC_BACK,
	K_AC_FORWARD,
	K_AC_STOP,
	K_AC_REFRESH,
	K_AC_BOOKMARKS,

	// mobile keys
	K_SOFTLEFT,
	K_SOFTRIGHT,
	K_CALL,
	K_ENDCALL
};

/*
=======
MapKey

Map from windows to quake keynums
=======
*/
static uint32_t
MapKey(uint32_t* scantokey, uint32_t scancode)
{
	uint32_t scan = 0;
	uint32_t code = 0;

	if (scantokey == NULL) {
		return SDLK_UNKNOWN;
	}
	scan = scancode & 0x3FF;
	if (scan > SDL_SCANCODE_COUNT) {
		return SDLK_UNKNOWN;
	}
	code = scantokey[scan];
	if ((SDL_SCANCODE_TO_KEYCODE(scancode) & SDLK_EXTENDED_MASK)) {
		switch (SDL_SCANCODE_TO_KEYCODE(scancode))
		{
		case SDLK_LEFT_TAB:
		{
			code = K_TAB;
		} break;
		case SDLK_LMETA:
		{
			code = K_LALT;
		} break;
		case SDLK_RMETA:
		{
			code = K_RALT;
		} break;
		case SDLK_LHYPER:
		{
			code = K_LCTRL;
		} break;
		case SDLK_RHYPER:
		{
			code = K_RCTRL;
		} break;
		default:
		{
			code = K_UNKNOWN;
		}
		}
	}

	return code;
}

//=============================================================================
// Window event callback function
//=============================================================================
static void SDLWndProc(const SDL_Window* hwnd, const SDL_Event* msg)
{
	switch (msg->type)
	{
	case SDL_EVENT_WINDOW_SHOWN:
	{
		r_fullscreen = Cvar_Get("r_fullscreen", "1", CVAR_ARCHIVE | CVAR_LATCH);
		vid_xpos = Cvar_Get("vid_xpos", "3", CVAR_ARCHIVE);
		vid_ypos = Cvar_Get("vid_ypos", "22", CVAR_ARCHIVE);

		return;
	}
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	{
		Cbuf_ExecuteText(EXEC_APPEND, "quit");

		return;
	}
	case SDL_EVENT_WINDOW_FOCUS_GAINED:
	{
		Com_DPrintf("Application Active\n");

		g_wv.isMinimized = false;
		IN_Activate(true);

		return;
	}
	case SDL_EVENT_WINDOW_MINIMIZED:
	case SDL_EVENT_WINDOW_FOCUS_LOST:
	{
		if (msg->type != SDL_EVENT_WINDOW_MINIMIZED) {
			Com_DPrintf("Application Inactive\n");
		}

		g_wv.isMinimized = true;
		IN_Activate(false);

		return;
	}
	case SDL_EVENT_WINDOW_MOVED:
	{
		if (!r_fullscreen->integer) {

			Cvar_SetValue("vid_xpos", msg->window.data1);
			Cvar_SetValue("vid_ypos", msg->window.data2);
		}

		return;
	}
	case SDL_EVENT_CLIPBOARD_UPDATE:
	{
		return;
	}
	case SDL_EVENT_KEY_DOWN:
	case SDL_EVENT_KEY_UP:
	{
		Sys_QueEvent(s_sysMsgTime, SE_KEY, MapKey(s_scantosdlk, msg->key.scancode),
			(msg->type == SDL_EVENT_KEY_DOWN) ? true : false, 0, NULL);
		return;
	}
	case SDL_EVENT_TEXT_INPUT:
	{
		Sys_QueEvent(s_sysMsgTime, SE_CHAR, msg->text.text[0], 0, 0, NULL);
		return;
	}
	case SDL_EVENT_MOUSE_MOTION:
	{
		if ((cls.keyCatchers & KEYCATCH_CONSOLE) != KEYCATCH_CONSOLE) {
			Sys_QueEvent(s_sysMsgTime, SE_MOUSE, msg->motion.xrel, msg->motion.yrel, 0,
				NULL);
		}

		return;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
	{
		Sys_QueEvent(s_sysMsgTime, SE_KEY, MapKey(s_scantosdlb, msg->button.button),
			(msg->type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? true : false, 0, NULL);

		return;
	}
	case SDL_EVENT_MOUSE_WHEEL:
	{
		if (in_mouse->integer == 1 ||
			(!r_fullscreen->integer && (cls.keyCatchers & KEYCATCH_CONSOLE))) {

			if ((int)(msg->wheel.y) > 0) {
				for (int i = 0; i < (int)(msg->wheel.y); i++)
				{
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MOUSE_WHEELUP, true, 0, NULL);
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MOUSE_WHEELUP, false, 0, NULL);
				}
			} else {
				for (int i = 0; i < -(int)(msg->wheel.y); i++)
				{
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MOUSE_WHEELDOWN, true, 0, NULL);
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MOUSE_WHEELDOWN, false, 0, NULL);
				}
			}

			return;
		}

	} break;
	case SDL_EVENT_GAMEPAD_AXIS_MOTION:
	{
		float fPos = 0;
		int16_t iPos = 0;
		int16_t xPos = 0;
		int16_t yPos = 0;
		int16_t zPos = 0;
		int16_t rPos = 0;
		int16_t uPos = 0;
		int16_t vPos = 0;
		int16_t axis = 0;

		switch (msg->gaxis.axis)
		{
		case SDL_GAMEPAD_AXIS_LEFTX:
		{
			xPos = msg->gaxis.value;
			fPos = JoyToF(msg->gaxis.value);
			axis = AXIS_SIDE;
		} break;
		case SDL_GAMEPAD_AXIS_LEFTY:
		{
			yPos = msg->gaxis.value;
			fPos = -JoyToF(msg->gaxis.value);
			axis = AXIS_FORWARD;
		} break;
		case SDL_GAMEPAD_AXIS_RIGHTX:
		{
			zPos = msg->gaxis.value;
			fPos = -JoyToF(msg->gaxis.value);
			axis = AXIS_YAW;
		} break;
		case SDL_GAMEPAD_AXIS_RIGHTY:
		{
			rPos = msg->gaxis.value;
			fPos = JoyToF(msg->gaxis.value);
			axis = AXIS_PITCH;
		} break;
		case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
		{
			uPos = msg->gaxis.value;
			iPos = JoyToI(msg->gaxis.value);
			Sys_QueEvent(s_sysMsgTime, SE_KEY, MapKey(s_scantosdlg, SDL_GAMEPAD_BUTTON_MISC6 + 1),
				(iPos != 0) ? true : false, 0, NULL);
			return;
		} break;
		case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
		{
			vPos = msg->gaxis.value;
			iPos = JoyToI(msg->gaxis.value);
			Sys_QueEvent(s_sysMsgTime, SE_KEY, MapKey(s_scantosdlg, SDL_GAMEPAD_BUTTON_MISC6 + 2),
				(iPos != 0) ? true : false, 0, NULL);
			return;
		} break;
		}

		if (in_debugJoystick->integer) {
			Com_Printf("%5.2f %5.2f %5.2f %5.2f %6i %6i\n",
				JoyToF(xPos), JoyToF(yPos),
				JoyToF(zPos), JoyToF(rPos),
				JoyToI(uPos), JoyToI(vPos));
		}

		if (fPos < 0.0f) {
			iPos = -1;
		}
		if (fPos > 0.0f) {
			iPos = 1;
		}

		Sys_QueEvent(s_sysMsgTime, SE_JOYSTICK_AXIS, axis, iPos, 0, NULL);

		return;
	}
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
	{
		if (!joy.avail) {
			return;
		}

		Sys_QueEvent(s_sysMsgTime, SE_KEY, MapKey(s_scantosdlg, msg->gbutton.button),
			msg->gbutton.down, 0, NULL);
	}
	case SDL_EVENT_GAMEPAD_ADDED:
	case SDL_EVENT_GAMEPAD_REMOVED:
	{
		return;
	}
	case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
	{
		return;
	}
	}
}

//=============================================================================
// Translate virtual key codes
//=============================================================================
void TranslateAndDispatchEvent(const SDL_Event* msg)
{
	if (msg != NULL) {
		s_sysMsgTime = Sys_Milliseconds();
		SDLWndProc(g_wv.hWnd, msg);
	}
}

static char sys_cmdline[MAX_STRING_CHARS] = { 0 };

/*
=============
Sys_Error

Show the early console as an error dialog
=============
*/
void Q_CDECL Sys_Error(const char* error, ...)
{
	char		text[MAXPRINTMSG];
	SDL_Event	msg;
	va_list		argptr;

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	Conbuf_AppendText(text);
	Conbuf_AppendText("\n");

	Sys_SetErrorText(text);
	Sys_ShowConsole(1, true);

	IN_Shutdown();

	// wait for the user to quit
	SDL_PumpEvents();

	while (1) {

		SDL_PeepEvents(&msg, 1, SDL_GETEVENT, SDL_EVENT_FIRST,
			SDL_EVENT_LAST);

		if (msg.type == SDL_EVENT_QUIT) {
			Com_Quit_f();
		}

		TranslateAndDispatchEvent(&msg);
	}

	Sys_DestroyConsole();

	exit(1);
}

/*
==============
Sys_Quit
==============
*/
void Sys_Quit(void)
{
	IN_Shutdown();
	Sys_DestroyConsole();

	exit(0);
}

/*
==============
Sys_Print
==============
*/
void Sys_Print(const char* msg)
{
	Conbuf_AppendText(msg);
}


/*
==============
Sys_Mkdir
==============
*/
void Sys_Mkdir(const char* path)
{
	SDL_CreateDirectory(path);
}

/*
==============
Sys_Cwd
==============
*/
const char* Sys_Cwd(void)
{
	return SDL_GetBasePath();
}

/*
==============
Sys_DefaultCDPath
==============
*/
char* Sys_DefaultCDPath(void)
{
	return "";
}

/*
==============
Sys_DefaultBasePath
==============
*/
const char* Sys_DefaultBasePath(void)
{
	return Sys_Cwd();
}

/*
==============
Sys_DefaultHomePath
==============
*/
const char* Sys_DefaultHomePath(void)
{
	return SDL_GetUserFolder(SDL_FOLDER_HOME);
}

/*
==============
Sys_DefaultInstallPath
==============
*/
const char* Sys_DefaultInstallPath(void)
{
	return Sys_Cwd();
}

/*
==============
Sys_GetCurrentUser
==============
*/
const char* Sys_GetCurrentUser(void)
{
	const char* delim = "/\\";
	const char* save = NULL;
	const char* next = NULL;
	const char* name = NULL;

	next = SDL_strtok_r((char*)SDL_GetUserFolder(SDL_FOLDER_HOME), delim, &save);
	while ((next = SDL_strtok_r(NULL, delim, &save)) != NULL) {
		name = next;
	}

	if (name == NULL) {
		return "player";
	}

	return name;
}

/*
================
Sys_GetClipboardData
================
*/
char* Sys_GetClipboardData(void)
{
	char* data = NULL;
	char* text = NULL;

	if ((text = SDL_GetClipboardText()) != 0) {
		data = Z_Malloc(SDL_strlen(text) + 1);
		SDL_strlcpy(data, text, SDL_strlen(text) + 1);
		SDL_free(text);
	}

	return data;
}

/*
==================
Sys_LowPhysicalMemory()
==================
*/
bool Sys_LowPhysicalMemory(void)
{
	const int memThreshold = 96*1024*1024;
	return ((SDL_GetSystemRAM()*1024*1024) <= memThreshold) ? true : false;
}

/*
==============
Sys_ProcessorCount
==============
*/
unsigned int Sys_ProcessorCount(void)
{
	return SDL_GetNumLogicalCPUCores();
}

/*
================
Sys_CheckCD

Return true if the proper CD is in the drive
================
*/
bool Sys_CheckCD(void)
{
	// FIXME: mission pack
	return true; // return Sys_ScanForCD();
}

/*
================
Sys_DateAndTime

Returns current system date and time
================
*/
const char*
Sys_DateAndTime(void)
{
	static char buffer[MAX_STRING_CHARS] = { 0 };
	char* pstr = NULL;
	struct tm* newtime = NULL;
	time_t aclock;

	time(&aclock);
	newtime = localtime(&aclock);
	pstr = asctime(newtime);

	if (pstr != NULL) {
		Q_strncpyz(buffer, pstr, strlen(pstr));
	}

	return &buffer[0];
}

/*
================
Sys_Milliseconds
================
*/
int sys_timeBase = 0;
int Sys_Milliseconds(void)
{
	int			sys_curtime;
	static bool	initialized = false;

	if (!initialized) {
		sys_timeBase = SDL_GetTicks();
		initialized = true;
	}

	sys_curtime = SDL_GetTicks();

	return sys_curtime;
}

/*
========================================================================

BACKGROUND FILE STREAMING

========================================================================
*/

typedef struct
{
	qhandle_t   file;
	byte*       buffer;
	bool        eof;
	bool        active;
	int         bufferSize;
	int         streamPosition;			// next byte to be returned by Sys_StreamRead
	int         threadPosition;			// next byte to be read from file
} streamsIO_t;

typedef struct
{
	SDL_Thread* threadHandle;
	int         threadId;
	SDL_Mutex*  crit;
	streamsIO_t sIO[MAX_FILE_HANDLES];
} streamState_t;

static streamState_t s_stream;

/*
===============
Sys_StreamThread

A thread will be sitting in this loop forever
================
*/
void Sys_StreamThread(void)
{
	int buffer;
	int count;
	int readCount;
	int bufferPoint;
	int r, i;

	while (1)
	{
		SDL_Delay(10);
		SDL_LockMutex(s_stream.crit);
		for (i = 1; i < MAX_FILE_HANDLES; i++)
		{
			// if there is any space left in the buffer, fill it up
			if (s_stream.sIO[i].active && !s_stream.sIO[i].eof) {
				count = s_stream.sIO[i].bufferSize - (s_stream.sIO[i].threadPosition -
					s_stream.sIO[i].streamPosition);
				if (!count) {
					continue;
				}

				bufferPoint = s_stream.sIO[i].threadPosition %
					s_stream.sIO[i].bufferSize;
				buffer = s_stream.sIO[i].bufferSize - bufferPoint;
				readCount = buffer < count ? buffer : count;

				r = FS_Read(s_stream.sIO[i].buffer + bufferPoint, readCount,
					s_stream.sIO[i].file);
				s_stream.sIO[i].threadPosition += r;

				if (r != readCount) {
					s_stream.sIO[i].eof = true;
				}
			}
		}
		SDL_UnlockMutex(s_stream.crit);
	}
}

void Sys_InitStreamThread(void)
{
	int i;

	s_stream.crit = SDL_CreateMutex();
	s_stream.threadHandle = SDL_CreateThread((SDL_ThreadFunction)Sys_StreamThread,
		"Sys_StreamThread", NULL);
	if (!s_stream.threadHandle) {
		return;
	}
	s_stream.threadId = SDL_GetThreadID(s_stream.threadHandle);

	for (i = 0; i < MAX_FILE_HANDLES; i++)
	{
		s_stream.sIO[i].active = false;
	}
}

void Sys_ShutdownStreamThread(void)
{
	SDL_DetachThread(s_stream.threadHandle);
}

void Sys_BeginStreamedFile(qhandle_t f, int readAhead)
{
	if (!s_stream.threadHandle) {
		return;
	}

	if (s_stream.sIO[f].file) {
		Sys_EndStreamedFile(s_stream.sIO[f].file);
	}

	s_stream.sIO[f].file = f;
	s_stream.sIO[f].buffer = Z_Malloc(readAhead);
	s_stream.sIO[f].bufferSize = readAhead;
	s_stream.sIO[f].streamPosition = 0;
	s_stream.sIO[f].threadPosition = 0;
	s_stream.sIO[f].eof = false;
	s_stream.sIO[f].active = true;
}

void Sys_EndStreamedFile(qhandle_t f)
{
	if (!s_stream.threadHandle) {
		return;
	}

	if (f != s_stream.sIO[f].file) {
		Com_Error(ERR_FATAL, "Sys_EndStreamedFile: wrong file");
	}
	// don't leave critical section until another stream is started
	SDL_LockMutex(s_stream.crit);
	s_stream.sIO[f].file = 0;
	s_stream.sIO[f].active = false;
	Z_Free(s_stream.sIO[f].buffer);
	SDL_UnlockMutex(s_stream.crit);
}

int Sys_StreamedRead(void* buffer, int size, int count, qhandle_t f)
{
	if (!s_stream.threadHandle) {
		return FS_Read(buffer, size * count, f);
	}

	int available;
	int remaining;
	int sleepCount;
	int copy;
	int bufferCount;
	int bufferPoint;
	byte* dest;

	if (s_stream.sIO[f].active == false) {
		Com_Error(ERR_FATAL, "Streamed read with non-streaming file");
	}

	dest = (byte*)buffer;
	remaining = size * count;

	if (remaining <= 0) {
		Com_Error(ERR_FATAL, "Streamed read with non-positive size");
	}

	sleepCount = 0;
	while (remaining > 0)
	{
		available = s_stream.sIO[f].threadPosition - s_stream.sIO[f].streamPosition;
		if (!available) {
			if (s_stream.sIO[f].eof) {
				break;
			}
			if (sleepCount == 1) {
				Com_DPrintf("Sys_StreamedRead: waiting\n");
			}
			if (++sleepCount > 100) {
				Com_Error(ERR_FATAL, "Sys_StreamedRead: thread has died");
			}
			SDL_Delay(10);
			continue;
		}
		SDL_LockMutex(s_stream.crit);

		bufferPoint = s_stream.sIO[f].streamPosition % s_stream.sIO[f].bufferSize;
		bufferCount = s_stream.sIO[f].bufferSize - bufferPoint;

		copy = available < bufferCount ? available : bufferCount;
		if (copy > remaining) {
			copy = remaining;
		}

		Com_Memcpy(dest, s_stream.sIO[f].buffer + bufferPoint, copy);
		s_stream.sIO[f].streamPosition += copy;
		dest += copy;
		remaining -= copy;

		SDL_UnlockMutex(s_stream.crit);
	}

	return (count * size - remaining) / size;
}

void Sys_StreamSeek(qhandle_t f, int offset, int origin)
{
	if (!s_stream.threadHandle) {
		FS_Seek(f, offset, origin);
	}

	// halt the thread
	SDL_LockMutex(s_stream.crit);
	// clear to that point
	FS_Seek(f, offset, origin);
	s_stream.sIO[f].streamPosition = 0;
	s_stream.sIO[f].threadPosition = 0;
	s_stream.sIO[f].eof = false;
	// let the thread start running at the new position
	SDL_UnlockMutex(s_stream.crit);
}

/*
==============================================================

DIRECTORY SCANNING

==============================================================
*/

#define	MAX_FOUND_FILES	0x1000

void Sys_ListFilteredFiles(const char* basedir, char* subdirs, char* filter,
	char** list, int* numfiles)
{
	char    newsubdirs[MAX_OSPATH];
	char    filename[MAX_OSPATH];
	char    search[MAX_OSPATH];
	char**  findinfo;
	int     findhandle;
	int     nfiles;

	if (*numfiles >= MAX_FOUND_FILES - 1) {
		return;
	}

	if (strlen(subdirs)) {
		Com_sprintf(search, sizeof(search), "%s\\%s", basedir, subdirs);
	} else {
		Com_sprintf(search, sizeof(search), "%s", basedir);
	}

	findinfo = SDL_GlobDirectory(search, "*", 0, &findhandle);
	if (findhandle == -1) {
		return;
	}

	nfiles = 0;
	do {
		if (Q_stricmp(findinfo[nfiles], ".") && Q_stricmp(findinfo[nfiles], "..")) {
			if (strlen(subdirs)) {
				Com_sprintf(newsubdirs, sizeof(newsubdirs), "%s\\%s", subdirs,
					findinfo[nfiles]);
			} else {
				Com_sprintf(newsubdirs, sizeof(newsubdirs), "%s",
					findinfo[nfiles]);
			}

			Sys_ListFilteredFiles(basedir, newsubdirs, filter, list, numfiles);
		}

		if (*numfiles >= MAX_FOUND_FILES - 1) {
			break;
		}

		Com_sprintf(filename, sizeof(filename), "%s\\%s", subdirs,
			findinfo[nfiles]);

		if (!Com_FilterPath(filter, filename, false)) {
			continue;
		}

		list[*numfiles] = Z_Malloc(strlen(filename) + 1);
		Q_strncpyz(list[*numfiles], filename, strlen(filename) + 1);
		(*numfiles)++;
		nfiles++;

	} while (findinfo[nfiles] != NULL);

	SDL_free(findinfo);
}

char** Sys_ListFiles(const char* directory, const char* extension, char* filter,
	int* numfiles, bool wantsubs)
{
	char    pattern[MAX_OSPATH];
	char    search[MAX_OSPATH];
	char*   list[MAX_FOUND_FILES];
	char**  listCopy;
	char**  findinfo;
	int     nfiles;
	int     findhandle;
	int     flag;
	int     i;

	if (filter) {

		nfiles = 0;
		Sys_ListFilteredFiles(directory, "", filter, list, &nfiles);

		list[nfiles] = 0;
		*numfiles = nfiles;

		if (!nfiles) {
			return NULL;
		}

		listCopy = Z_Malloc((nfiles + 1) * sizeof(*listCopy));
		for (i = 0; i < nfiles; i++)
		{
			listCopy[i] = list[i];
		}

		listCopy[i] = NULL;

		return listCopy;
	}

	if (!extension) {
		extension = "";
	}

	Com_sprintf(pattern, sizeof(pattern), "*%s", extension);
	Com_sprintf(search, sizeof(search), "%s", directory);

	// search
	findinfo = SDL_GlobDirectory(search, pattern, 0, &findhandle);

	if (findhandle == 0) {
		*numfiles = 0;
		return NULL;
	}

	nfiles = 0;
	do {

		list[nfiles] = Z_Malloc(strlen(findinfo[nfiles]) + 1);
		Q_strncpyz(list[nfiles], findinfo[nfiles], strlen(findinfo[nfiles]) + 1);
		nfiles++;
	} while (findinfo[nfiles] != NULL);

	list[nfiles] = 0;

	SDL_free(findinfo);

	// return a copy of the list
	*numfiles = nfiles;

	if (!nfiles) {
		return NULL;
	}

	listCopy = Z_Malloc((nfiles + 1) * sizeof(*listCopy));
	for (i = 0; i < nfiles; i++)
	{
		listCopy[i] = list[i];
	}

	listCopy[i] = NULL;

	do {
		flag = 0;
		for (i = 1; i < nfiles; i++)
		{
			if (Q_strgtr(listCopy[i - 1], listCopy[i])) {
				char* temp = listCopy[i];
				listCopy[i] = listCopy[i - 1];
				listCopy[i - 1] = temp;
				flag = 1;
			}
		}
	} while (flag);

	return listCopy;
}

void Sys_FreeFileList(char** list)
{
	if (!list) {
		return;
	}

	for (int i = 0; list[i]; i++)
	{
		Z_Free(list[i]);
	}

	Z_Free(list);
}

/*
========================================================================

LOAD/UNLOAD DLL

========================================================================
*/

/*
=================
Sys_UnloadDll

=================
*/
void Sys_UnloadDll(void* dllHandle)
{
	if (dllHandle != NULL) {
		SDL_UnloadObject((SDL_SharedObject*)dllHandle);
	}
}

/*
=================
Sys_LoadDll

Used to load a development dll instead of a virtual machine
=================
*/
typedef int (Q_CDECL* EntryPoint)(int num, ...);
typedef int (Q_CDECL* SystemCalls)(int arg, ...);
typedef void (Q_CDECL* DLLEntry)(SystemCalls syscallptr);

void* Q_CDECL Sys_LoadDll(const char* name, char* fqpath, EntryPoint* entryPoint,
	SystemCalls systemCalls)
{
	static int	lastWarning = 0;
	SDL_SharedObject* libHandle;
	DLLEntry dllEntry;
	char* basepath;
	char* cdpath;
	char* gamedir;
	char* fn;
#ifdef NDEBUG
	int		timestamp;
	int   ret;
#endif
	char	filename[MAX_QPATH];

	*fqpath = 0;		// added 7/20/02 by T.Ray

	Com_sprintf(filename, sizeof(filename), "%sx86.dll", name);

#ifdef NDEBUG
	timestamp = Sys_Milliseconds();
	if (((timestamp - lastWarning) > (5 * 60000)) && !Cvar_VariableIntegerValue("dedicated")
		&& !Cvar_VariableIntegerValue("com_blindlyLoadDLLs")) {
		if (FS_FileExists(filename)) {

			lastWarning = timestamp;

			const SDL_MessageBoxButtonData buttons[] = {
				{ /* .flags, .buttonid, .text */        0, 0, "no" },
				{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "ok" },
				{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel" },
			};

			const SDL_MessageBoxColorScheme colorScheme = {
			{ /* .colors (.r, .g, .b) */
				/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
				{ 255,   0,   0 },
				/* [SDL_MESSAGEBOX_COLOR_TEXT] */
				{   0, 255,   0 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
				{ 255, 255,   0 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
				{   0,   0, 255 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
				{ 255,   0, 255 }
			}
			};

			const SDL_MessageBoxData messageboxdata = {
				SDL_MESSAGEBOX_INFORMATION,			/* .flags */
				NULL,			/* .window */
				"Security Warning",			/* .title */
				"You are about to load a .DLL executable that\n"
				"has not been verified for use with Quake III Arena.\n"
				"This type of file can compromise the security of\n"
				"your computer.\n\n"
				"Select 'OK' if you choose to load it anyway.",			/* .message */
				SDL_arraysize(buttons),			/* .numbuttons */
				buttons,			/* .buttons */
				&colorScheme			/* .colorScheme */
			};

			SDL_ShowMessageBox(&messageboxdata, &ret);
			if (ret != 1) {
				return NULL;
			}
		}
	}
#endif

#ifndef NDEBUG
	libHandle = SDL_LoadObject(filename);
	if (libHandle) {
		Com_Printf("LoadLibrary '%s' ok\n", filename);
	} else {
		Com_Printf("LoadLibrary '%s' failed\n", filename);
	}

	if (!libHandle) {
#endif
		basepath = Cvar_VariableString("fs_basepath");
		cdpath = Cvar_VariableString("fs_cdpath");
		gamedir = Cvar_VariableString("fs_game");

		fn = FS_BuildOSPath(basepath, gamedir, filename);
		libHandle = SDL_LoadObject(fn);
#ifndef NDEBUG
		if (libHandle) {
			Com_Printf("LoadLibrary '%s' ok\n", fn);
		} else {
			Com_Printf("LoadLibrary '%s' failed\n", fn);
		}
#endif

		if (!libHandle) {
			if (cdpath[0]) {
				fn = FS_BuildOSPath(cdpath, gamedir, filename);
				libHandle = SDL_LoadObject(fn);
#ifndef NDEBUG
				if (libHandle) {
					Com_Printf("LoadLibrary '%s' ok\n", fn);
				} else {
					Com_Printf("LoadLibrary '%s' failed\n", fn);
				}
#endif
			}

			if (!libHandle) {
				return NULL;
			}
		}
#ifndef NDEBUG
	}
#endif

	dllEntry    = (DLLEntry)SDL_LoadFunction(libHandle, "dllEntry");
	*entryPoint = (EntryPoint)SDL_LoadFunction(libHandle, "vmMain");

	if (!entryPoint || !dllEntry) {
		SDL_UnloadObject(libHandle);
		return NULL;
	}

	dllEntry(systemCalls);

	if (libHandle) {
		Q_strncpyz(fqpath, filename, MAX_QPATH);
	}
	
	return libHandle;
}

/*
========================================================================

EVENT LOOP

========================================================================
*/

#define	MAX_QUED_EVENTS		256
#define	MASK_QUED_EVENTS	( MAX_QUED_EVENTS - 1 )

sysEvent_t	eventQue[MAX_QUED_EVENTS];
int			eventHead, eventTail;
byte		sys_packetReceived[MAX_MSGLEN];

/*
================
Sys_QueEvent

A time of 0 will get the current time
Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Sys_QueEvent(int time, sysEventType_t type, int value, int value2, int ptrLength,
	void* ptr)
{
	sysEvent_t* ev;

	ev = &eventQue[eventHead & MASK_QUED_EVENTS];
	if (eventHead - eventTail >= MAX_QUED_EVENTS) {
		Com_Printf("Sys_QueEvent: overflow\n");
		// we are discarding an event, but don't leak memory
		if (ev->evPtr) {
			Z_Free(ev->evPtr);
		}
		eventTail++;
	}

	eventHead++;

	if (time == 0) {
		time = Sys_Milliseconds();
	}

	ev->evTime = time;
	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}

/*
================
Sys_GetEvent
================
*/
sysEvent_t Sys_GetEvent(void)
{
	SDL_Event	msg = { 0 };
	sysEvent_t	ev;
	char*		s;
	msg_t		netmsg;
	netadr_t	adr;

	// return if we have data
	if (eventHead > eventTail) {
		eventTail++;
		return eventQue[(eventTail - 1) & MASK_QUED_EVENTS];
	}

	// pump the message loop
	SDL_PumpEvents();

	while (SDL_PeepEvents(&msg, 1, SDL_GETEVENT, SDL_EVENT_FIRST,
		SDL_EVENT_LAST) > 0) {

		if (msg.type == SDL_EVENT_QUIT) {
			Com_Quit_f();
		}

		TranslateAndDispatchEvent(&msg);
	}

	// check for console commands
	s = Sys_ConsoleInput();
	if (s) {

		char* b;
		int		len;

		len = strlen(s) + 1;
		b = Z_Malloc(len);
		Q_strncpyz(b, s, len - 1);
		Sys_QueEvent(0, SE_CONSOLE, 0, 0, len, b);
	}

	// check for network packets
	MSG_Init(&netmsg, sys_packetReceived, sizeof(sys_packetReceived));

	if (Sys_GetPacket(&adr, &netmsg)) {

		netadr_t* buf = NULL;
		int len = 0;

		// copy out to a seperate buffer for qeueing
		// the readcount stepahead is for SOCKS support
		len = sizeof(netadr_t) + netmsg.cursize - netmsg.readcount;
		buf = Z_Malloc(len);
		*buf = adr;
		Com_Memcpy(buf + 1, &netmsg.data[netmsg.readcount], netmsg.cursize -
			netmsg.readcount);
		Sys_QueEvent(0, SE_PACKET, 0, 0, len, buf);
	}

	// return if we have data
	if (eventHead > eventTail) {
		eventTail++;
		return eventQue[(eventTail - 1) & MASK_QUED_EVENTS];
	}

	// create an empty event to return
	Com_Memset(&ev, 0, sizeof(ev));
	ev.evTime = Sys_Milliseconds();

	return ev;
}

//================================================================

/*
=================
Sys_In_Restart_f

Restart the input subsystem
=================
*/
void Sys_In_Restart_f(void)
{
	IN_Shutdown();
	IN_Init();
}

/*
=================
Sys_Net_Restart_f

Restart the network subsystem
=================
*/
void Sys_Net_Restart_f(void)
{
	NET_Restart();
}

/*
================
Sys_Init

Called after the common systems (cvars, files, etc)
are initialized
================
*/
void Sys_Init(void)
{
	Cmd_AddCommand("in_restart", Sys_In_Restart_f);
	Cmd_AddCommand("net_restart", Sys_Net_Restart_f);

	// operating system
	Cvar_Set("sys_arch", SDL_GetPlatform());

	// cpu
#if 0
	Cvar_Set("sys_cpustring", SDL_GetCPUName());
#endif

	Cvar_Set("username", Sys_GetCurrentUser());

	IN_Init();		// FIXME: not in dedicated?
}


//=======================================================================

void Sys_InitStreamThread(void);

/*
==================
main
==================
*/
int main(int argc, const char** argv)
{
	int totalMsec = 0, countMsec = 0;
	int startTime = 0, endTime = 0;

	// concatenate the command line
	for (int i = 1; i < argc; i++)
	{
		if (i > 1) {
			Q_strcat(sys_cmdline, 1, " ");
		}

		Q_strcat(sys_cmdline, strlen(argv[i]), argv[i]);
	}

	// done before Com/Sys_Init since we need this for error output
	Sys_CreateConsole();

	// get the initial time base
	Sys_Milliseconds();

#if 0
	// if we find the CD, add a +set cddir xxx command line
	Sys_ScanForCD();
#endif

	Sys_InitStreamThread();

	Com_Init(sys_cmdline);
	NET_Init();

	Com_Printf("Working directory: %s\n", Sys_Cwd());

	// hide the early console since we've reached the point where we
	// have a working graphics subsystems
	if (!com_dedicated->integer && !com_viewlog->integer) {
		Sys_ShowConsole(0, false);
	}

	// main game loop
	while (1) {

		// if not running as a game client, sleep a bit
		if (g_wv.isMinimized || (com_dedicated && com_dedicated->integer)) {
			SDL_Delay(5);
		}

		// set low precision every frame, because some system calls reset it arbitrarily
		// _controlfp( _PC_24, _MCW_PC );
		// no exceptions, even if some crappy syscall turns them back on!
		// _controlfp( -1, _MCW_EM  );

		startTime = Sys_Milliseconds();

		// make sure mouse and joystick are only called once a frame
		IN_Frame();

		Com_Frame();			// run the game

		endTime = Sys_Milliseconds();
		totalMsec += endTime - startTime;
		countMsec++;
	}

	// never gets here
}
