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

#ifndef Q_FILESYSTEM_H
#define Q_FILESYSTEM_H

/*
=============================================================================

QUAKE3 FILESYSTEM

=============================================================================
*/

#define	MAX_OSPATH			256		// max length of a filesystem pathname
#define	MAX_QPATH			64		// max length of a quake game pathname

// mode parm for FS_FOpenFile
typedef enum
{
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
} fsMode_t;

typedef enum
{
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

#if defined(__FreeBSD__) || defined(__linux__) || (defined(__MACH__) && defined(__APPLE__))

#define	PATH_SEP	'/'

#endif	// #if defined(__FreeBSD__) || defined(__linux__) || (defined(__MACH__) && defined(__APPLE__))

#ifdef __MACOS__

#define	PATH_SEP ':'

#endif	// #ifdef __MACOS__

#ifdef WIN32

#define	PATH_SEP '\\'

#endif	// #ifdef WIN32

//=============================================================
// 
// referenced flags
// these are in loop specific order so don't change the order
#define FS_GENERAL_REF	0x01
#define FS_UI_REF		0x02
#define FS_CGAME_REF	0x04
#define FS_QAGAME_REF	0x08
// number of id paks that will never be autodownloaded from baseq3
#define NUM_ID_PAKS		9

#define	MAX_FILE_HANDLES	64

#define BASEGAME "baseq3"

bool FS_Initialized();

void	FS_InitFilesystem(void);
void	FS_Shutdown(bool closemfp);

bool	FS_ConditionalRestart(int checksumFeed);
void	FS_Restart(int checksumFeed);
// shutdown and restart the filesystem so changes to fs_gamedir can take effect

char** FS_ListFiles(const char* directory, const char* extension, int* numfiles);
// directory should not have either a leading or trailing /
// if extension is "/", only subdirectories will be returned
// the returned files will not include any directories or /

void	FS_FreeFileList(char** list);

bool FS_FileExists(const char* file);

int		FS_LoadStack();

int		FS_GetFileList(const char* path, const char* extension, char* listbuf, int bufsize);
int		FS_GetModList(char* listbuf, int bufsize);

qhandle_t	FS_FOpenFileWrite(const char* qpath);
// will properly create any needed paths and deal with seperater character issues

int		FS_filelength(qhandle_t f);
char* FS_BuildOSPath(const char* base, const char* game, const char* qpath);
qhandle_t FS_SV_FOpenFileWrite(const char* filename);
int		FS_SV_FOpenFileRead(const char* filename, qhandle_t* fp);
void	FS_SV_Rename(const char* from, const char* to);
int		FS_FOpenFileRead(const char* qpath, qhandle_t* file);
// if uniqueFILE is true, then a new FILE will be fopened even if the file
// is found in an already open pak file.  If uniqueFILE is false, you must call
// FS_FCloseFile instead of fclose, otherwise the pak FILE would be improperly closed
// It is generally safe to always set uniqueFILE to true, because the majority of
// file IO goes through FS_ReadFile, which Does The Right Thing already.

int		FS_FileIsInPAK(const char* filename, int* pChecksum);
// returns 1 if a file is in the PAK file, otherwise -1

int		FS_Write(const void* buffer, int len, qhandle_t f);

int		FS_Read2(void* buffer, int len, qhandle_t f);
int		FS_Read(void* buffer, int len, qhandle_t f);
// properly handles partial reads and reads from other dlls

void	FS_FCloseFile(qhandle_t f);
// note: you can't just fclose from another DLL, due to MS libc issues

int		FS_ReadFile(const char* qpath, void** buffer);
// returns the length of the file
// a null buffer will just return the file length without loading
// as a quick check for existance. -1 length == not present
// A 0 byte will always be appended at the end, so string ops are safe.
// the buffer should be considered read-only, because it may be cached
// for other uses.

void	FS_ForceFlush(qhandle_t f);
// forces flush on files we're writing to.

void	FS_FreeFile(void* buffer);
// frees the memory returned by FS_ReadFile

void	FS_WriteFile(const char* qpath, const void* buffer, int size);
// writes a complete file, creating any subdirectories needed

int		FS_filelength(qhandle_t f);
// doesn't work for files that are opened from a pack file

int		FS_FTell(qhandle_t f);
// where are we?

void	FS_Flush(qhandle_t f);

void 	Q_CDECL FS_Printf(qhandle_t f, const char* fmt, ...);
// like fprintf

int		FS_FOpenFileByMode(const char* qpath, qhandle_t* f, fsMode_t mode, int readAhead);
// opens a file for reading, writing, or appending depending on the value of mode

int		FS_Seek(qhandle_t f, long offset, int origin);
// seek on a file (doesn't work for zip files!!!!!!!!)

bool FS_FilenameCompare(const char* s1, const char* s2);

const char* FS_GamePureChecksum(void);
// Returns the checksum of the pk3 from which the server loaded the qagame.qvm

const char* FS_LoadedPakNames(void);
const char* FS_LoadedPakChecksums(void);
const char* FS_LoadedPakPureChecksums(void);
// Returns a space separated string containing the checksums of all loaded pk3 files.
// Servers with sv_pure set will get this string and pass it to clients.

const char* FS_ReferencedPakNames(void);
const char* FS_ReferencedPakChecksums(void);
const char* FS_ReferencedPakPureChecksums(void);
// Returns a space separated string containing the checksums of all loaded 
// AND referenced pk3 files. Servers with sv_pure set will get this string 
// back from clients for pure validation 

void FS_ClearPakReferences(int flags);
// clears referenced booleans on loaded pk3s

void FS_PureServerSetReferencedPaks(const char* pakSums, const char* pakNames);
void FS_PureServerSetLoadedPaks(const char* pakSums, const char* pakNames);
// If the string is empty, all data sources will be allowed.
// If not empty, only pk3 files that match one of the space
// separated checksums will be checked for files, with the
// sole exception of .cfg files.

bool FS_idPak(char* pak, char* base);
bool FS_ComparePaks(char* neededpaks, int len, bool dlstring);

void FS_Rename(const char* from, const char* to);

#endif // #ifndef Q_FILESYSTEM_H