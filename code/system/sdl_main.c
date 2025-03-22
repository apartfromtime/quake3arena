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

static char		sys_cmdline[MAX_STRING_CHARS];

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
bool	Sys_CheckCD( void ) {
  // FIXME: mission pack
  return true;
	//return Sys_ScanForCD();
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

	sys_curtime = SDL_GetTicks() - sys_timeBase;

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

		// save the msg time, because wndprocs don't have access to the timestamp
		g_wv.sysMsgTime = Sys_Milliseconds();

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
