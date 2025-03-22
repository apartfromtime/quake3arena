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

// win_input.c -- win32 mouse and joystick code

#include "../client/client.h"
#include "../win32/win_local.h"


typedef struct
{
	bool	mouseActive;
	bool	mouseInitialized;
} WinMouseVars_t;

static WinMouseVars_t s_wmv;

//
// Joystick definitions
//
#define	JOY_MAX_AXES		6				// X, Y, Z, R, U, V

typedef struct
{
	SDL_Gamepad* gamepad;
	int id;			// joystick number
	int oldbuttonstate;
	int	oldpovstate;
	bool avail;
} joystickInfo_t;

static	joystickInfo_t	joy;

cvar_t	*in_mouse;
cvar_t	*in_joystick;
cvar_t	*in_joyBallScale;
cvar_t	*in_debugJoystick;
cvar_t	*joy_threshold;

bool	in_appactive;

/*
============================================================

WIN32 MOUSE CONTROL

============================================================
*/

/*
================
IN_ActivateWin32Mouse
================
*/
void IN_ActivateSDLMouse(void)
{
	SDL_CaptureMouse(true);
	SDL_HideCursor();
	SDL_SetWindowRelativeMouseMode(g_wv.hWnd, true);
}

/*
================
IN_DeactivateWin32Mouse
================
*/
void IN_DeactivateSDLMouse(void)
{
	SDL_SetWindowRelativeMouseMode(g_wv.hWnd, false);
	SDL_ShowCursor();
	SDL_CaptureMouse(false);
}

/*
=========================================================================

JOYSTICK

=========================================================================
*/

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
		numButtons = SDL_GamepadHasButton(joy.gamepad, i) == true ? numButtons+1 : numButtons;
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
	float	fValue;

	if (value > -(int)(joy_threshold->value * 0X7FFF) &&
		value <  (int)(joy_threshold->value * 0X7FFF)) {
		
		return 0;
	}

	// move centerpoint to zero
	if (value < 0)
	{
		value -= 32768;
	}
	else
	{
		value += 32768;
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

int JoyToI(int value)
{
	if (value > -(int)(joy_threshold->value * 0X7FFF) &&
		value <  (int)(joy_threshold->value * 0X7FFF)) {

		return 0;
	}

	// move centerpoint to zero
	if (value < 0) {
		value -= 32768;
	} else {
		value += 32768;
	}

	int iValue = (float)value / 32768.0;

	return iValue;
}

int	joyDirectionKeys[16] =
{
	K_LEFTARROW, K_RIGHTARROW,
	K_UPARROW, K_DOWNARROW,
	K_JOY16, K_JOY17,
	K_JOY18, K_JOY19,
	K_JOY20, K_JOY21,
	K_JOY22, K_JOY23,

	K_JOY24, K_JOY25,
	K_JOY26, K_JOY27
};

/*
===========
IN_JoyMove
===========
*/
void IN_JoyMove(void)
{
	float	fAxisValue;
	unsigned long	buttonstate, povstate;
	int		x, y;

	// verify joystick is available and that the user wants to use it
	if (!joy.avail) {
		return;
	}

	// collect the joystick data, if possible
	if (in_debugJoystick->integer) {

	}

	// loop through the joystick buttons
	// key a joystick event or auxillary event for higher number buttons for each state change
	
	// bitmask of the buttons
	buttonstate = 0;

	for (size_t button = 0; button < SDL_GAMEPAD_BUTTON_COUNT; button++)
	{
		buttonstate |= SDL_GetGamepadButton(joy.gamepad,
			(SDL_GamepadButton)button) << button;
	}

	for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
		if ((buttonstate & (1 << i)) && !(joy.oldbuttonstate & (1 << i))) {
			Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_JOY1 + i, true, 0, NULL);
		}
		if (!(buttonstate & (1 << i)) && (joy.oldbuttonstate & (1 << i))) {
			Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_JOY1 + i, false, 0, NULL);
		}
	}

	joy.oldbuttonstate = buttonstate;

	povstate = 0;

	// convert main joystick motion into 6 direction button bits
	for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT && i < 4; i++)
	{
		// get the floating point zero-centered, potentially-inverted data for the current axis
		fAxisValue = JoyToF(SDL_GetGamepadAxis(joy.gamepad, i));

		if (fAxisValue < -joy_threshold->value) {
			povstate |= (1 << (i * 2));
		} else if (fAxisValue > joy_threshold->value) {
			povstate |= (1 << (i * 2 + 1));
		}
	}

	// determine which bits have changed and key an auxillary event for each change
	for (int i = 0; i < 16; i++) {
		if ((povstate & (1 << i)) && !(joy.oldpovstate & (1 << i))) {
			Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, joyDirectionKeys[i], true, 0, NULL);
		}

		if (!(povstate & (1 << i)) && (joy.oldpovstate & (1 << i))) {
			Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, joyDirectionKeys[i], false, 0, NULL);
		}
	}
	joy.oldpovstate = povstate;

	// if there is a trackball like interface, simulate mouse moves
	if (SDL_GAMEPAD_AXIS_COUNT >= 6) {
		x = JoyToI(SDL_GetGamepadAxis(joy.gamepad, SDL_GAMEPAD_AXIS_RIGHTX)) * in_joyBallScale->value;
		y = JoyToI(SDL_GetGamepadAxis(joy.gamepad, SDL_GAMEPAD_AXIS_RIGHTY)) * in_joyBallScale->value;
		if (x != 0 || y != 0) {
			Sys_QueEvent(g_wv.sysMsgTime, SE_MOUSE, x, y, 0, NULL);
		}
	}
}

/*
============================================================

  MOUSE CONTROL

============================================================
*/

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

	IN_ActivateSDLMouse();
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

	IN_DeactivateSDLMouse();
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
IN_Startup
===========
*/
void IN_Startup(void)
{
	Com_Printf("\n------- Input Initialization -------\n");
	IN_StartupMouse();
	IN_StartupJoystick();
	Com_Printf("------------------------------------\n");

	in_mouse->modified = false;
	in_joystick->modified = false;
}

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
	// mouse variables
	in_mouse = Cvar_Get("in_mouse", "1", CVAR_ARCHIVE | CVAR_LATCH);

	// joystick variables
	in_joystick = Cvar_Get("in_joystick", "0", CVAR_ARCHIVE | CVAR_LATCH);
	in_joyBallScale = Cvar_Get("in_joyBallScale", "0.02", CVAR_ARCHIVE);
	in_debugJoystick = Cvar_Get("in_debugjoystick", "0", CVAR_TEMP);

	joy_threshold = Cvar_Get("joy_threshold", "0.15", CVAR_ARCHIVE);

	IN_Startup();
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
	// post joystick events
	IN_JoyMove();

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

/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates(void)
{
}
