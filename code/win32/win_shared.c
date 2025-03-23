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
bool Sys_ScanForCD(void) {
	static char	cddir[MAX_OSPATH];
	char		drive[4];
	FILE* f;
	char		test[MAX_OSPATH];
#if 0
	// don't override a cdpath on the command line
	if (strstr(sys_cmdline, "cdpath")) {
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

/*
========================================================================

BACKGROUND FILE STREAMING

========================================================================
*/

#if 1

void Sys_InitStreamThread(void)
{
}

void Sys_ShutdownStreamThread(void)
{
}

void Sys_BeginStreamedFile(qhandle_t f, int readAhead)
{
}

void Sys_EndStreamedFile(qhandle_t f)
{
}

int Sys_StreamedRead(void* buffer, int size, int count, qhandle_t f)
{
	return FS_Read(buffer, size * count, f);
}

void Sys_StreamSeek(qhandle_t f, int offset, int origin)
{
	FS_Seek(f, offset, origin);
}


#else

typedef struct {
	qhandle_t	file;
	byte* buffer;
	bool	eof;
	bool	active;
	int		bufferSize;
	int		streamPosition;	// next byte to be returned by Sys_StreamRead
	int		threadPosition;	// next byte to be read from file
} streamsIO_t;

typedef struct {
	HANDLE				threadHandle;
	int					threadId;
	CRITICAL_SECTION	crit;
	streamsIO_t			sIO[MAX_FILE_HANDLES];
} streamState_t;

streamState_t	stream;

/*
===============
Sys_StreamThread

A thread will be sitting in this loop forever
================
*/
void Sys_StreamThread(void) {
	int		buffer;
	int		count;
	int		readCount;
	int		bufferPoint;
	int		r, i;

	while (1) {
		Sleep(10);
		//		EnterCriticalSection (&stream.crit);

		for (i = 1; i < MAX_FILE_HANDLES; i++) {
			// if there is any space left in the buffer, fill it up
			if (stream.sIO[i].active && !stream.sIO[i].eof) {
				count = stream.sIO[i].bufferSize - (stream.sIO[i].threadPosition - stream.sIO[i].streamPosition);
				if (!count) {
					continue;
				}

				bufferPoint = stream.sIO[i].threadPosition % stream.sIO[i].bufferSize;
				buffer = stream.sIO[i].bufferSize - bufferPoint;
				readCount = buffer < count ? buffer : count;

				r = FS_Read(stream.sIO[i].buffer + bufferPoint, readCount, stream.sIO[i].file);
				stream.sIO[i].threadPosition += r;

				if (r != readCount) {
					stream.sIO[i].eof = true;
				}
			}
		}
		//		LeaveCriticalSection (&stream.crit);
	}
}

/*
===============
Sys_InitStreamThread

================
*/
void Sys_InitStreamThread(void) {
	int i;

	InitializeCriticalSection(&stream.crit);

	// don't leave the critical section until there is a
	// valid file to stream, which will cause the StreamThread
	// to sleep without any overhead
//	EnterCriticalSection( &stream.crit );

	stream.threadHandle = CreateThread(
		NULL,	// LPSECURITY_ATTRIBUTES lpsa,
		0,		// DWORD cbStack,
		(LPTHREAD_START_ROUTINE)Sys_StreamThread,	// LPTHREAD_START_ROUTINE lpStartAddr,
		0,			// LPVOID lpvThreadParm,
		0,			//   DWORD fdwCreate,
		&stream.threadId);
	for (i = 0; i < MAX_FILE_HANDLES; i++) {
		stream.sIO[i].active = false;
	}
}

/*
===============
Sys_ShutdownStreamThread

================
*/
void Sys_ShutdownStreamThread(void) {
}


/*
===============
Sys_BeginStreamedFile

================
*/
void Sys_BeginStreamedFile(qhandle_t f, int readAhead) {
	if (stream.sIO[f].file) {
		Sys_EndStreamedFile(stream.sIO[f].file);
	}

	stream.sIO[f].file = f;
	stream.sIO[f].buffer = Z_Malloc(readAhead);
	stream.sIO[f].bufferSize = readAhead;
	stream.sIO[f].streamPosition = 0;
	stream.sIO[f].threadPosition = 0;
	stream.sIO[f].eof = false;
	stream.sIO[f].active = true;

	// let the thread start running
//	LeaveCriticalSection( &stream.crit );
}

/*
===============
Sys_EndStreamedFile

================
*/
void Sys_EndStreamedFile(qhandle_t f) {
	if (f != stream.sIO[f].file) {
		Com_Error(ERR_FATAL, "Sys_EndStreamedFile: wrong file");
	}
	// don't leave critical section until another stream is started
	EnterCriticalSection(&stream.crit);

	stream.sIO[f].file = 0;
	stream.sIO[f].active = false;

	Z_Free(stream.sIO[f].buffer);

	LeaveCriticalSection(&stream.crit);
}


/*
===============
Sys_StreamedRead

================
*/
int Sys_StreamedRead(void* buffer, int size, int count, qhandle_t f) {
	int		available;
	int		remaining;
	int		sleepCount;
	int		copy;
	int		bufferCount;
	int		bufferPoint;
	byte* dest;

	if (stream.sIO[f].active == false) {
		Com_Error(ERR_FATAL, "Streamed read with non-streaming file");
	}

	dest = (byte*)buffer;
	remaining = size * count;

	if (remaining <= 0) {
		Com_Error(ERR_FATAL, "Streamed read with non-positive size");
	}

	sleepCount = 0;
	while (remaining > 0) {
		available = stream.sIO[f].threadPosition - stream.sIO[f].streamPosition;
		if (!available) {
			if (stream.sIO[f].eof) {
				break;
			}
			if (sleepCount == 1) {
				Com_DPrintf("Sys_StreamedRead: waiting\n");
			}
			if (++sleepCount > 100) {
				Com_Error(ERR_FATAL, "Sys_StreamedRead: thread has died");
			}
			Sleep(10);
			continue;
		}

		EnterCriticalSection(&stream.crit);

		bufferPoint = stream.sIO[f].streamPosition % stream.sIO[f].bufferSize;
		bufferCount = stream.sIO[f].bufferSize - bufferPoint;

		copy = available < bufferCount ? available : bufferCount;
		if (copy > remaining) {
			copy = remaining;
		}
		Com_Memcpy(dest, stream.sIO[f].buffer + bufferPoint, copy);
		stream.sIO[f].streamPosition += copy;
		dest += copy;
		remaining -= copy;

		LeaveCriticalSection(&stream.crit);
	}

	return (count * size - remaining) / size;
}

/*
===============
Sys_StreamSeek

================
*/
void Sys_StreamSeek(qhandle_t f, int offset, int origin) {

	// halt the thread
	EnterCriticalSection(&stream.crit);

	// clear to that point
	FS_Seek(f, offset, origin);
	stream.sIO[f].streamPosition = 0;
	stream.sIO[f].threadPosition = 0;
	stream.sIO[f].eof = false;

	// let the thread start running at the new position
	LeaveCriticalSection(&stream.crit);
}

#endif

/*
==============================================================

DIRECTORY SCANNING

==============================================================
*/

#define	MAX_FOUND_FILES	0x1000

void Sys_ListFilteredFiles(const char* basedir, char* subdirs, char* filter, char** list, int* numfiles)
{
	char		search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	char		filename[MAX_OSPATH];
	int			findhandle;
	struct _finddata_t findinfo;

	if (*numfiles >= MAX_FOUND_FILES - 1) {
		return;
	}

	if (strlen(subdirs)) {
		Com_sprintf(search, sizeof(search), "%s\\%s\\*", basedir, subdirs);
	} else {
		Com_sprintf(search, sizeof(search), "%s\\*", basedir);
	}

	findhandle = _findfirst(search, &findinfo);
	if (findhandle == -1) {
		return;
	}

	do {
		if (findinfo.attrib & _A_SUBDIR) {
			if (Q_stricmp(findinfo.name, ".") && Q_stricmp(findinfo.name, "..")) {
				if (strlen(subdirs)) {
					Com_sprintf(newsubdirs, sizeof(newsubdirs), "%s\\%s", subdirs, findinfo.name);
				} else {
					Com_sprintf(newsubdirs, sizeof(newsubdirs), "%s", findinfo.name);
				}

				Sys_ListFilteredFiles(basedir, newsubdirs, filter, list, numfiles);
			}
		}

		if (*numfiles >= MAX_FOUND_FILES - 1) {
			break;
		}
		
		Com_sprintf(filename, sizeof(filename), "%s\\%s", subdirs, findinfo.name);
		
		if (!Com_FilterPath(filter, filename, false))
			continue;
		
		list[*numfiles] = Z_TagMalloc(strlen(filename) + 1, TAG_SMALL);
		Q_strncpyz(list[*numfiles], filename, strlen(filename) + 1);
		(*numfiles)++;

	} while (_findnext(findhandle, &findinfo) != -1);

	_findclose(findhandle);
}

char** Sys_ListFiles(const char* directory, const char* extension, char* filter, int* numfiles, bool wantsubs)
{
	char		search[MAX_OSPATH];
	int			nfiles;
	char**		listCopy;
	char*		list[MAX_FOUND_FILES];
	struct _finddata_t findinfo;
	int			findhandle;
	int			flag;
	int			i;

	if (filter) {

		nfiles = 0;
		Sys_ListFilteredFiles(directory, "", filter, list, &nfiles);

		list[nfiles] = 0;
		*numfiles = nfiles;

		if (!nfiles)
			return NULL;

		listCopy = Z_Malloc((nfiles + 1) * sizeof(*listCopy));
		for (i = 0; i < nfiles; i++) {
			listCopy[i] = list[i];
		}

		listCopy[i] = NULL;

		return listCopy;
	}

	if (!extension) {
		extension = "";
	}

	// passing a slash as extension will find directories
	if (extension[0] == '/' && extension[1] == 0) {
		extension = "";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	Com_sprintf(search, sizeof(search), "%s\\*%s", directory, extension);

	// search
	nfiles = 0;

	findhandle = _findfirst(search, &findinfo);
	if (findhandle == -1) {
		*numfiles = 0;
		return NULL;
	}

	do {
		if ((!wantsubs && flag ^ (findinfo.attrib & _A_SUBDIR)) ||
			(wantsubs && findinfo.attrib & _A_SUBDIR)) {
			if (nfiles == MAX_FOUND_FILES - 1) {
				break;
			}

			list[nfiles] = Z_TagMalloc(strlen(findinfo.name) + 1, TAG_SMALL);
			Q_strncpyz(list[nfiles], findinfo.name, strlen(findinfo.name) + 1);
			nfiles++;
		}
	} while (_findnext(findhandle, &findinfo) != -1);

	list[nfiles] = 0;

	_findclose(findhandle);

	// return a copy of the list
	*numfiles = nfiles;

	if (!nfiles) {
		return NULL;
	}

	listCopy = Z_Malloc((nfiles + 1) * sizeof(*listCopy));
	for (i = 0; i < nfiles; i++) {
		listCopy[i] = list[i];
	}

	listCopy[i] = NULL;

	do {
		flag = 0;
		for (i = 1; i < nfiles; i++) {
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
	int		i;

	if (!list) {
		return;
	}

	for (i = 0; list[i]; i++) {
		Z_Free(list[i]);
	}

	Z_Free(list);
}

//========================================================

/*
================
Sys_SnapVector
================
*/
long fastftol(float f)
{
	static int tmp;
	__asm fld f
	__asm fistp tmp
	__asm mov eax, tmp
}

void Sys_SnapVector(float* v)
{
	int i;
	float f;

	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	v++;
	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	v++;
	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	/*
	*v = fastftol(*v);
	v++;
	*v = fastftol(*v);
	v++;
	*v = fastftol(*v);
	*/
}

//============================================
