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

#ifndef Q_HEAP_H
#define Q_HEAP_H

#include "..\game\q_shared.h"

#define MIN_DEDICATED_COMHUNKMEGS 1
#define MIN_COMHUNKMEGS 56
#define MIN_COMZONEMEGS 20

#ifdef __APPLE__
#define DEF_COMHUNKMEGS "64"
#define DEF_COMZONEMEGS "24"
#else
#define DEF_COMHUNKMEGS "56"
#define DEF_COMZONEMEGS "16"
#endif

typedef enum {
	TAG_FREE,
	TAG_GENERAL,
	TAG_BOTLIB,
	TAG_SOUND,
	TAG_RENDERER,
	TAG_SMALL,
	TAG_STATIC
} memtag_t;

/*

--- low memory ----
server vm
server clipmap
---mark---
renderer initialization (shaders, etc)
UI vm
cgame vm
renderer map
renderer models

---free---

temp file loading
--- high memory ---

*/


#if defined(_DEBUG) && !defined(BSPC)
#define ZONE_DEBUG
#define HUNK_DEBUG
#endif // #if defined(_DEBUG) && !defined(BSPC)


#ifdef ZONE_DEBUG
#define Z_TagMalloc(size, tag)			Z_TagMallocDebug(size, tag, #size, __FILE__, __LINE__)
#define Z_Malloc(size)					Z_MallocDebug(size, #size, __FILE__, __LINE__)
void* Z_TagMallocDebug(int size, int tag, char* label, char* file, int line);
void* Z_MallocDebug(int size, char* label, char* file, int line);
#else
void* Z_TagMalloc(int size, int tag);
void* Z_Malloc(int size);
#endif
bool Zone_InitMemory(int zoneSize);
void Zone_Meminfo(void);
void Z_Free(void* ptr);
int Z_AvailableMemory(void);

#ifdef HUNK_DEBUG
#define Hunk_Alloc(size)			Hunk_AllocDebug(size, #size, __FILE__, __LINE__)
void* Hunk_AllocDebug(int size, char* label, char* file, int line);
#else
void* Hunk_Alloc(int size);
#endif
bool Hunk_InitMemory(int hunkSize);
void Hunk_Meminfo(void);
void Hunk_Free(void);
void Hunk_Clear(void);
void* Hunk_AllocateTempMemory(int size);
void Hunk_FreeTempMemory(void* buf);
int	Hunk_MemoryRemaining(void);

#endif	// Q_HEAP_H
