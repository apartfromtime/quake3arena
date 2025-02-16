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
// qcommon.h -- definitions common between client and server, but not game.or ref modules
#ifndef _QCOMMON_H_
#define _QCOMMON_H_

#include "../qcommon/cm_public.h"

//#define	PRE_RELEASE_DEMO

/*
===============================================================================

MESSAGE

===============================================================================
*/

#include "msg.h"

/*
===============================================================================

NET

===============================================================================
*/

#include "net.h"

/*
==============================================================

VIRTUAL MACHINE

==============================================================
*/

#include "vm.h"

/*
==============================================================

CMD

Command text buffering and command execution

==============================================================
*/

#include "cmd.h"

/*
==============================================================

CVAR

==============================================================
*/

#include "cvar.h"

/*
==============================================================

FILESYSTEM

No stdio calls should be used by any part of the game, because
we need to deal with all sorts of directory and seperator char
issues.
==============================================================
*/

#include "filesystem.h"



/*
==============================================================

MISC

==============================================================
*/

// declarations for cl_cdkey
extern char cl_cdkey[34];

// the max string you can send to a Com_Printf / Com_DPrintf (above gets truncated)
#define	MAXPRINTMSG	4096

char		*CopyString( const char *in );

void        Com_BeginRedirect (char *buffer, int buffersize, void (*flush)(char *));
void        Com_EndRedirect( void );
void        Q_CDECL Com_Printf( const char *fmt, ... );
void        Q_CDECL Com_DPrintf( const char *fmt, ... );
void        Q_CDECL Com_Error( int code, const char *fmt, ... );
void        Com_Quit_f( void );
int         Com_EventLoop( void );
int         Com_Milliseconds( void );
unsigned    Com_BlockChecksum( const void *buffer, int length );
unsigned    Com_BlockChecksumKey (void *buffer, int length, int key);
int         Com_RealTime(qtime_t *qtime);
bool        Com_SafeMode( void );
void        Com_StartupVariable( const char *match );


extern	cvar_t	*com_developer;
extern	cvar_t	*com_dedicated;
extern	cvar_t	*com_speeds;
extern	cvar_t	*com_timescale;
extern	cvar_t	*com_sv_running;
extern	cvar_t	*com_cl_running;
extern	cvar_t	*com_viewlog;			// 0 = hidden, 1 = visible, 2 = minimized
extern	cvar_t	*com_version;
extern	cvar_t	*com_blood;
extern	cvar_t	*com_buildScript;		// for building release pak files
extern	cvar_t	*com_journal;
extern	cvar_t	*com_cameraMode;

// both client and server must agree to pause
extern	cvar_t	*cl_paused;
extern	cvar_t	*sv_paused;

// com_speeds times
extern	int		time_game;
extern	int		time_frontend;
extern	int		time_backend;		// renderer backend time

extern	int		com_frameTime;
extern	int		com_frameMsec;

extern	bool	com_errorEntered;

extern	qhandle_t	com_journalFile;
extern	qhandle_t	com_journalDataFile;

// commandLine should not include the executable name (argv[0])
void Com_Init(char* commandLine);
void Com_Frame(void);
void Com_Shutdown(void);


/*
==============================================================

MEMORY

==============================================================
*/

#include "q_heap.h"

/*
===============================================================================

PROTOCOL

===============================================================================
*/

// 1.31 - 67
#define	PROTOCOL_VERSION    68

// override on command line, config files etc.
#define	UPDATE_SERVER_NAME      "update.quake3arena.com"
#define MASTER_SERVER_NAME      "master.quake3arena.com"
#define	AUTHORIZE_SERVER_NAME   "authorize.quake3arena.com"

#define	PORT_MASTER         27950
#define	PORT_UPDATE         27951
#define	PORT_AUTHORIZE      27952
#define	PORT_SERVER         27960
#define	NUM_SERVER_PORTS    4       // broadcast scan this many ports after
                                    // PORT_SERVER so a single machine can
                                    // run multiple servers

// the cmd_strings[] array in cl_parse.c should mirror this
//
// client <-> server
//
typedef enum
{
	CMD_BAD,
	CMD_NOP,
	CMD_EOF,
	// Client
	CMD_CL_MOVE,            // [usercmd_t]
	CMD_CL_MOVENODELTA,     // [usercmd_t]
	CMD_CL_COMMAND,         // [string] message
	// Server
	CMD_SV_GAMESTATE,       // [short] [string] only in gamestate messages
	CMD_SV_CONFIGSTRING,    // only in gamestate messages
	CMD_SV_BASELINE,        // [string] to be executed by client game module
	CMD_SV_SERVERCOMMAND,   // [short] size [size bytes]
	CMD_SV_DOWNLOAD,
	CMD_SV_SNAPSHOT
} SVCL_CMD_OPS;

/*
==============================================================

CLIENT / SERVER SYSTEMS

==============================================================
*/

#include "../client/client.h"
#include "../server/server.h"

int
Com_ParseInfo(const char* buf, int max, char* list[MAX_INFO_STRING]);

/*
==============================================================

NON-PORTABLE SYSTEM SERVICES

==============================================================
*/

#include "../system/sys_public.h"

#endif // _QCOMMON_H_
