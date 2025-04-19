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

#include <windows.h>
#include "../game/q_shared.h"
#include "../qcommon/qcommon.h"
#include "win_local.h"
#include <io.h>

#define	CD_BASEDIR	"quake3"
#define	CD_EXE		"quake3.exe"
#define	CD_BASEDIR_LINUX	"bin\\x86\\glibc-2.1"
#define	CD_EXE_LINUX "quake3"

/*
================
Sys_ScanForCD

Search all the drives to see if there is a valid CD to grab
the cddir from
================
*/
bool Sys_ScanForCD(char* commandline)
{
	static char	cddir[MAX_OSPATH];
	char		test[MAX_OSPATH];
	char		drive[4];
	FILE*		f;

#if 0
	// don't override a cdpath on the command line
	if (strstr(commandline, "cdpath")) {
		return;
	}
#endif

	drive[0] = 'c';
	drive[1] = ':';
	drive[2] = '\\';
	drive[3] = 0;

	// scan the drives
	for (drive[0] = 'c'; drive[0] <= 'z'; drive[0]++) {
		if (GetDriveType(drive) != DRIVE_CDROM) {
			continue;
		}

		sprintf(cddir, "%s%s", drive, CD_BASEDIR);
		sprintf(test, "%s\\%s", cddir, CD_EXE);
		f = fopen(test, "r");
		if (f) {
			fclose(f);
			return true;
		}
		else {
			sprintf(cddir, "%s%s", drive, CD_BASEDIR_LINUX);
			sprintf(test, "%s\\%s", cddir, CD_EXE_LINUX);
			f = fopen(test, "r");
			if (f) {
				fclose(f);
				return true;
			}
		}
	}

	return false;
}
