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

// win_local.h: Win32-specific Quake3 header file

#include <SDL3/SDL.h>
#include <windows.h>

void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr );
void Sys_CreateConsole(void);
void Sys_DestroyConsole(void);
char* Sys_ConsoleInput (void);
bool Sys_GetPacket(netadr_t* net_from, msg_t* net_message);

void Conbuf_AppendText(const char* msg);

typedef struct
{
	SDL_SharedObject* hinstOpenGL;	// handle for the OpenGL library
	SDL_GLContext hGLRC;			// handle to GL rendering context
	FILE* log_fp;
	int	desktopBitsPixel;
	int	desktopWidth;
	int desktopHeight;
	bool allowdisplaydepthchange;
	bool pixelFormatSet;
	bool cdsFullscreen;
} glwstate_t;

extern glwstate_t glw_state;

typedef struct
{
	SDL_Window* hWnd;
	bool		isMinimized;
} WinVars_t;


extern WinVars_t g_wv;
