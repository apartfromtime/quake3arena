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
#ifndef __Q_SHARED_H
#define __Q_SHARED_H

#include "..\system\sys_public.h"

// q_shared.h -- included first by ALL program modules.
// A user mod should never modify this file

#define	Q3_VERSION		"Q3 1.32b"
// 1.32 released 7-10-2002

/**********************************************************************
  VM Considerations

  The VM can not use the standard system headers because we aren't really
  using the compiler they were meant for.  We use bg_lib.h which contains
  prototypes for the functions we define for our own use in bg_lib.c.

  When writing mods, please add needed headers HERE, do not start including
  stuff like <stdio.h> in the various .c files that make up each of the VMs
  since you will be including system headers files can will have issues.

  Remember, if you use a C library function that is not defined in bg_lib.c,
  you will have to add your own version for support in the VM.

 **********************************************************************/



#ifndef NULL
#define NULL ((void *)0)
#endif

#define CDKEY_LEN 16
#define CDCHKSUM_LEN 2

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192	// used for system info key only
#define	BIG_INFO_KEY		8192
#define	BIG_INFO_VALUE		8192

#define	MAX_NAME_LENGTH		32		// max length of a client name

#define	MAX_SAY_TEXT		150

#ifdef ERR_FATAL
#undef ERR_FATAL			// this is be defined in malloc.h
#endif

// parameters to the main Error routine
typedef enum
{
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

//=============================================================================

void* Com_Allocate(int bytes);
void Com_Dealloc(void* ptr);
void Com_Memset(void* dest, const int val, const size_t count);
void Com_Memcpy(void* dest, const void* src, const size_t count);

/*
===============================================================================

MATHLIB

===============================================================================
*/

#include "..\game\q_math.h"

/*
===============================================================================

SCRIPT PARSING

===============================================================================
*/

#include "..\game\q_parse.h"

/*
===============================================================================

LIBRARY REPLACEMENT FUNCTIONS

===============================================================================
*/

#include "..\qcommon\q_string.h"

/*
===============================================================================

GROWLISTS

===============================================================================
*/

#include "..\qcommon\q_list.h"

/*
===============================================================================

  INFO STRINGS

===============================================================================
*/

#include "..\qcommon\q_dict.h"


/*
===============================================================================

  FILE FORMATS

===============================================================================
*/


#include "..\qcommon\qfiles.h"


/*
===============================================================================

REAL TIME

===============================================================================
*/

typedef struct qtime_s
{
	int tm_sec;     /* seconds after the minute - [0,59] */
	int tm_min;     /* minutes after the hour - [0,59] */
	int tm_hour;    /* hours since midnight - [0,23] */
	int tm_mday;    /* day of the month - [1,31] */
	int tm_mon;     /* months since January - [0,11] */
	int tm_year;    /* years since 1900 */
	int tm_wday;    /* days since Sunday - [0,6] */
	int tm_yday;    /* days since January 1 - [0,365] */
	int tm_isdst;   /* daylight savings time flag */
} qtime_t;

#ifdef __cplusplus
extern "C" {
#endif


/*
===============================================================================

ENDIAN

===============================================================================
*/

// 64-bit integers for global rankings interface
// implemented as a struct for qvm compatibility
typedef struct
{
	byte	b0;
	byte	b1;
	byte	b2;
	byte	b3;
	byte	b4;
	byte	b5;
	byte	b6;
	byte	b7;
} qint64;

short	BigShort(short l);
short	LittleShort(short l);
int		BigLong(int l);
int		LittleLong(int l);
qint64  BigLong64(qint64 l);
qint64  LittleLong64(qint64 l);
float	BigFloat(float l);
float	LittleFloat(float l);

int ReadIntLE(const byte* src);
float ReadFltLE(const byte* src);

void	Swap_Init(void);

#ifdef __cplusplus
	}
#endif


/*
===============================================================================

QUAKE3 FILESYSTEM

===============================================================================
*/

#include "..\qcommon\filesystem.h"


//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

	// this is only here so the functions in q_shared.c and bg_*.c can link
	void	Q_CDECL Com_Error(int level, const char* error, ...);
	void	Q_CDECL Com_Printf(const char* msg, ...);
	void	Q_CDECL Com_DPrintf(const char* msg, ...);

#ifdef __cplusplus
}
#endif

#ifndef Q3RADIANT

/*
===============================================================================

CVAR SYSTEM

===============================================================================
*/

#include "..\qcommon\cvar.h"

/*
===============================================================================

COMMAND BUFFER

===============================================================================
*/

#include "..\qcommon\cmd.h"

/*
===============================================================================

COLLISION DETECTION

===============================================================================
*/

#include "..\qcommon\cm_public.h"

/*
===============================================================================

USER INTERFACE

===============================================================================
*/

#include "..\q3_ui\ui_public.h"

/*
=============================================================================

GAME SHARED DEFINITIONS

=============================================================================
*/

#include "..\game\bg_public.h"

/*
===============================================================================

RENDERER

===============================================================================
*/

#include "..\renderer\tr_types.h"

/*
===============================================================================

CLIENT

===============================================================================
*/

#include "..\client\cl_types.h"

/*
===============================================================================

KEYS

===============================================================================
*/

#include "..\client\key_types.h"

/*
===============================================================================

SOUND

===============================================================================
*/

#include "..\client\snd_types.h"

/*
===============================================================================

CINEMATIC

===============================================================================
*/

#include "..\client\cin_types.h"

/*
===============================================================================

GAME

===============================================================================
*/

#include "..\game\g_types.h"

/*
===============================================================================

USER INTERFACE

===============================================================================
*/

#include "..\ui\ui_types.h"

//=============================================================================

#endif	// #ifndef Q3RADIANT

#endif	// __Q_SHARED_H
