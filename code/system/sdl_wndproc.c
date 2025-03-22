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
#include "../win32/win_local.h"

WinVars_t g_wv;

// Console variables that we need to access from this module
cvar_t* vid_xpos;			// X coordinate of window position
cvar_t* vid_ypos;			// Y coordinate of window position
cvar_t* r_fullscreen;

#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[0] ) )

/*
==================
VID_AppActivate
==================
*/
static void
VID_AppActivate(bool active, bool minimize)
{
	g_wv.isMinimized = minimize;

	Com_DPrintf("VID_AppActivate: %i\n", active);

	Key_ClearStates();			// FIXME!!!

	// we don't want to act like we're active if we're minimized
	if (active && !g_wv.isMinimized) {
		g_wv.activeApp = true;
	} else {
		g_wv.activeApp = false;
	}

	// minimize/restore mouse-capture on demand
	if (!g_wv.activeApp) {
		IN_Activate(false);
	} else {
		IN_Activate(true);
	}
}

//==========================================================================

static byte s_scantokey[128] = 
					{ 
//  0			1			2			3			4			5			6			7 
//  8           9			A			B			C			D			E			F 
	0,			27,			'1',		'2',		'3',		'4',		'5',		'6', 
	'7',		'8',		'9',		'0',		'-',		'=',		K_BACKSPACE,  9,		// 0
	'q',		'w',		'e',		'r',		't',		'y',		'u',		'i', 
	'o',		'p',		'[',		']',		13 ,		K_CTRL,		'a',		's',		// 1
	'd',		'f',		'g',		'h',		'j',		'k',		'l',		';', 
	'\'',		'`',		K_SHIFT,	'\\',		'z',		'x',		'c',		'v',		// 2
	'b',		'n',		'm',		',',		'.',		'/',		K_SHIFT,	'*', 
	K_ALT,		' ',		K_CAPSLOCK,	K_F1,		K_F2,		K_F3,		K_F4,		K_F5,		// 3
	K_F6,		K_F7,		K_F8,		K_F9,		K_F10,		K_PAUSE,	0,			K_HOME, 
	K_UPARROW,	K_PGUP,		K_KP_MINUS,	K_LEFTARROW,K_KP_5,		K_RIGHTARROW,K_KP_PLUS,	K_END,		// 4
	K_DOWNARROW,K_PGDN,		K_INS,		K_DEL,		0,			0,			0,			K_F11, 
	K_F12,		0,			0,			0,			0,			0,			0,			0,			// 5
	0,			0,			0,			0,			0,			0,			0,			0, 
	0,			0,			0,			0,			0,			0,			0,			0,			// 6
	0,			0,			0,			0,			0,			0,			0,			0, 
	0,			0,			0,			0,			0,			0,			0,			0			// 7
}; 

/*
=======
MapKey

Map from windows to quake keynums
=======
*/
static int MapKey(int key)
{
	int result;
	int modified;
	bool is_extended;

	//	Com_Printf( "0x%x\n", key);

	modified = key & 255;

	if (modified > 127) {
		return 0;
	}

	if (key & (1 << 24)) {
		is_extended = true;
	} else {
		is_extended = false;
	}

	result = s_scantokey[modified];

	if (!is_extended)
	{
		switch (result)
		{
		case K_HOME:
			return K_KP_HOME;
		case K_UPARROW:
			return K_KP_UPARROW;
		case K_PGUP:
			return K_KP_PGUP;
		case K_LEFTARROW:
			return K_KP_LEFTARROW;
		case K_RIGHTARROW:
			return K_KP_RIGHTARROW;
		case K_END:
			return K_KP_END;
		case K_DOWNARROW:
			return K_KP_DOWNARROW;
		case K_PGDN:
			return K_KP_PGDN;
		case K_INS:
			return K_KP_INS;
		case K_DEL:
			return K_KP_DEL;
		default:
			return result;
		}
	}
	else
	{
		switch (result)
		{
		case K_PAUSE:
			return K_KP_NUMLOCK;
		case 0x0D:
			return K_KP_ENTER;
		case 0x2F:
			return K_KP_SLASH;
		case 0xAF:
			return K_KP_PLUS;
		}
		return result;
	}
}

extern cvar_t* in_mouse;

//=============================================================================
// Window event callback function
//=============================================================================
static void SDLWndProc(const SDL_Window* hwnd, const SDL_Event* msg)
{
	switch (msg->type)
	{
	case SDL_EVENT_WINDOW_SHOWN:
	{
		vid_xpos = Cvar_Get("vid_xpos", "3", CVAR_ARCHIVE);
		vid_ypos = Cvar_Get("vid_ypos", "22", CVAR_ARCHIVE);
		r_fullscreen = Cvar_Get("r_fullscreen", "1", CVAR_ARCHIVE | CVAR_LATCH);

		return;
	}
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	{
		Cbuf_ExecuteText(EXEC_APPEND, "quit");

		return;
	}
	case SDL_EVENT_WINDOW_FOCUS_GAINED:
	{
		VID_AppActivate(true, false);

		return;
	}
	case SDL_EVENT_WINDOW_MINIMIZED:
	case SDL_EVENT_WINDOW_FOCUS_LOST:
	{
		VID_AppActivate(false, true);
		return;
	}
	case SDL_EVENT_WINDOW_MOVED:
	{
		if (!r_fullscreen->integer) {

			Cvar_SetValue("vid_xpos", msg->window.data1);
			Cvar_SetValue("vid_ypos", msg->window.data2);
			
			if (g_wv.activeApp) {
				IN_Activate(true);
			}
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
		Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, MapKey(msg->key.raw),
			(msg->type == SDL_EVENT_KEY_DOWN) ? true : false, 0, NULL);
		return;
	}
	case SDL_EVENT_TEXT_INPUT:
	{
		Sys_QueEvent(g_wv.sysMsgTime, SE_CHAR, msg->text.text[0], 0, 0, NULL);
		return;
	}
	case SDL_EVENT_MOUSE_MOTION:
	{
		if ((cls.keyCatchers & KEYCATCH_CONSOLE) != KEYCATCH_CONSOLE) {
			Sys_QueEvent(g_wv.sysMsgTime, SE_MOUSE, msg->motion.xrel, msg->motion.yrel, 0,
				NULL);
		}

		return;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
	{
		int	key = 0;

		switch (msg->button.button)
		{
		case SDL_BUTTON_LEFT:
		{
			key = K_MOUSE1;
		} break;
		case SDL_BUTTON_MIDDLE:
		{
			key = K_MOUSE3;
		} break;
		case SDL_BUTTON_RIGHT:
		{
			key = K_MOUSE2;
		} break;
		case SDL_BUTTON_X1:
		{
			key = K_MOUSE4;
		} break;
		case SDL_BUTTON_X2:
		{
			key = K_MOUSE5;
		} break;
		}

		Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, key,
			(msg->type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? true : false, 0, NULL);

		return;
	}
	case SDL_EVENT_MOUSE_WHEEL:
	{
		if (in_mouse->integer != 1) {
			if (((int)(msg->wheel.y)) > 0) {
				Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELUP,  true, 0, NULL);
				Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, false, 0, NULL);
			} else {
				Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN,  true, 0, NULL);
				Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, false, 0, NULL);

				return;
			}
		}

		if (in_mouse->integer != 1 ||
			(!r_fullscreen->integer && (cls.keyCatchers & KEYCATCH_CONSOLE))) {

			if ((int)(msg->wheel.y) > 0) {
				for (int i = 0; i < (int)(msg->wheel.y); i++)
				{
					Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELUP,  true, 0, NULL);
					Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, false, 0, NULL);
				}
			} else {
				for (int i = 0; i < -(int)(msg->wheel.y); i++)
				{
					Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN,  true, 0, NULL);
					Sys_QueEvent(g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, false, 0, NULL);
				}
			}

			return;
		}

	} break;
	case SDL_EVENT_GAMEPAD_AXIS_MOTION:
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
	case SDL_EVENT_GAMEPAD_ADDED:
	case SDL_EVENT_GAMEPAD_REMOVED:
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
		SDLWndProc(g_wv.hWnd, msg);
	}
}
