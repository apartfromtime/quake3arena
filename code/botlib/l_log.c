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

/*
=============================================================================

BOT LOG

=============================================================================
*/

#include "../game/q_shared.h"
#include "../game/botlib.h"
#include "be_interface.h"
#include "l_log.h"

#define MAX_LOGFILENAMESIZE		1024

typedef struct logfile_s
{
	char filename[MAX_LOGFILENAMESIZE];
	qhandle_t handle;
} logfile_t;

extern cvar_t* bot_logfile;
static logfile_t logfile = { 0 };

/*
=================
Log_Open
=================
*/
void Bot_LogOpen(char* filename)
{
	if (bot_logfile && bot_logfile->integer)
	{
		if (!logfile.handle && filename[0])
		{
			botimport.FS_FOpenFile(filename, &logfile.handle, FS_WRITE);
		}

		if (!logfile.handle)
		{
			botimport.Print(PRT_WARNING, "Couldn't open logfile: %s\n", filename);
			return;
		}
		else
		{
			struct tm* newtime;
			time_t aclock;

			time(&aclock);
			newtime = localtime(&aclock);

			Bot_LogPrintf("Logfile opened on %s\n", asctime(newtime));
			Q_strncpyz(logfile.filename, filename, MAX_LOGFILENAMESIZE);
		}
	}
	else
	{
		botimport.Print(PRT_MESSAGE, "Not logging to disk.\n");
	}
}

/*
=================
Log_Shutdown
=================
*/
void Bot_LogShutdown(void)
{
	if (logfile.handle)
	{
		botimport.FS_FCloseFile(logfile.handle);

		logfile.handle = 0;

		botimport.Print(PRT_MESSAGE, "Closed log %s\n", logfile.filename);
	}
}

/*
=================
Log_Write
=================
*/
void Q_CDECL Bot_LogPrintf(char* fmt, ...)
{
	va_list	argptr;
	char	string[1024];

	if (!logfile.handle)
	{
		return;
	}

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	botimport.FS_Write(string, strlen(string), logfile.handle);
}

/*
=================
Log_WriteTimeStamped
=================
*/
void Q_CDECL Bot_LogPrintfTimeStamped(char* fmt, ...)
{
	va_list	argptr;
	char	string[1024];
	int		min, tens, sec;

	if (!logfile.handle)
	{
		return;
	}

	sec = botlibglobals.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf(string, sizeof(string), "%3i:%i%i ", min, tens, sec);

	va_start(argptr, fmt);
	vsprintf(string + 7, fmt, argptr);
	va_end(argptr);

	botimport.FS_Write(string, strlen(string), logfile.handle);
}

/*
=================
Log_FilePointer
=================
*/
qhandle_t Bot_LogFilePointer(void)
{
	return logfile.handle;
}
