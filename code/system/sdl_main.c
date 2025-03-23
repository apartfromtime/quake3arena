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
// win_main.c

#include "../client/client.h"
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
	float	fValue;

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
unsigned long s_joystick_lx = 0;
unsigned long s_joystick_ly = 0;
unsigned long s_joystick_rx = 0;
unsigned long s_joystick_ry = 0;

void IN_JoyMove(void)
{
	float fAxisValue[4] = { 0.0f };
	unsigned long povstate = 0;
	float x = 0.0f, y = 0.0f;

	// verify joystick is available and that the user wants to use it
	if (!joy.avail) {
		return;
	}

	// collect the joystick data, if possible
	if (in_debugJoystick->integer) {
	}

	// convert main joystick motion into 6 direction button bits
	fAxisValue[0] = JoyToF(s_joystick_lx);
	fAxisValue[1] = JoyToF(s_joystick_ly);
	fAxisValue[2] = JoyToF(s_joystick_rx);
	fAxisValue[3] = JoyToF(s_joystick_ry);
	
	for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT && i < 4; i++)
	{
		// get the floating point zero-centered, potentially-inverted data for the current axis
		if (fAxisValue[i] < -joy_threshold->value) {
			povstate |= (1 << (i * 2));
		}
		else if (fAxisValue[i] > joy_threshold->value) {
			povstate |= (1 << (i * 2 + 1));
		}
	}

	// determine which bits have changed and key an auxillary event for each change
	for (int i = 0; i < 16; i++) {
		if ((povstate & (1 << i)) && !(joy.oldpovstate & (1 << i))) {
			Sys_QueEvent(s_sysMsgTime, SE_KEY, joyDirectionKeys[i], true, 0, NULL);
		}

		if (!(povstate & (1 << i)) && (joy.oldpovstate & (1 << i))) {
			Sys_QueEvent(s_sysMsgTime, SE_KEY, joyDirectionKeys[i], false, 0, NULL);
		}
	}

	joy.oldpovstate = povstate;

	// if there is a trackball like interface, simulate mouse moves
	if (SDL_GAMEPAD_AXIS_COUNT >= 6) {
		x = JoyToF(s_joystick_rx) * in_joyBallScale->value;
		y = JoyToF(s_joystick_ry) * in_joyBallScale->value;
		if (x != 0 || y != 0) {
			Sys_QueEvent(s_sysMsgTime, SE_MOUSE, x, y, 0, NULL);
		}
	}
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
	in_joystick = Cvar_Get("in_joystick", "0", CVAR_ARCHIVE | CVAR_LATCH);
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
	IN_JoyMove();			// post joystick events

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
	}
	else {
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
		Sys_QueEvent(s_sysMsgTime, SE_KEY, MapKey(msg->key.raw),
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

		Sys_QueEvent(s_sysMsgTime, SE_KEY, key,
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
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MWHEELUP, true, 0, NULL);
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MWHEELUP, false, 0, NULL);
				}
			} else {
				for (int i = 0; i < -(int)(msg->wheel.y); i++)
				{
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MWHEELDOWN, true, 0, NULL);
					Sys_QueEvent(s_sysMsgTime, SE_KEY, K_MWHEELDOWN, false, 0, NULL);
				}
			}

			return;
		}

	} break;
	case SDL_EVENT_GAMEPAD_AXIS_MOTION:
	{
		switch (msg->gaxis.axis)
		{
		case SDL_GAMEPAD_AXIS_LEFTX:
		{
			s_joystick_lx = msg->gaxis.value;
		} break;
		case SDL_GAMEPAD_AXIS_LEFTY:
		{
			s_joystick_ly = msg->gaxis.value;
		} break;
		case SDL_GAMEPAD_AXIS_RIGHTX:
		{
			s_joystick_rx = msg->gaxis.value;
		} break;
		case SDL_GAMEPAD_AXIS_RIGHTY:
		{
			s_joystick_ry = msg->gaxis.value;
		} break;
		case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
		{

		} break;
		case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
		{

		} break;
		}
		return;
	}
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
	{
		if (!joy.avail) {
			return;
		}

		Sys_QueEvent(s_sysMsgTime, SE_KEY, K_JOY1 + msg->gbutton.button,
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

extern char* FS_BuildOSPath(const char* base, const char* game, const char* qpath);

/*
=================
Sys_LoadDll

Used to load a development dll instead of a virtual machine
=================
*/
void* Q_CDECL Sys_LoadDll(const char* name, char* fqpath, int (Q_CDECL** entryPoint)(int, ...),
	int (Q_CDECL* systemcalls)(int, ...))
{
	static int	lastWarning = 0;
	SDL_SharedObject* libHandle;
	void	(Q_CDECL * dllEntry)(int (Q_CDECL * syscallptr)(int, ...));
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

	dllEntry = (void (Q_CDECL*)(int (Q_CDECL*)(int, ...)))SDL_LoadFunction(libHandle, "dllEntry");
	*entryPoint = (int (Q_CDECL*)(int, ...))SDL_LoadFunction(libHandle, "vmMain");

	if (!*entryPoint || !dllEntry) {
		SDL_UnloadObject(libHandle);
		return NULL;
	}

	dllEntry(systemcalls);

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
