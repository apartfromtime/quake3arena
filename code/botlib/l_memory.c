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
==============================================================================

MEMORY

==============================================================================
*/

#include "../game/q_shared.h"
#include "../game/botlib.h"
#include "be_interface.h"
#include "l_log.h"

#define TAG_BOTLIB		2

int allocatedmemory;
int totalmemorysize;
int numblocks;

/*
=================
Bot_ZoneAlloc
=================
*/
#ifdef ZONE_DEBUG
void *GetMemoryDebug(unsigned long size, char *label, char *file, int line)
#else
void* GetMemory(unsigned long size)
#endif // ZONE_DEBUG
{
	void* ptr = NULL;

#ifdef ZONE_DEBUG
	ptr = botimport.Zone_AllocDebug(size, TAG_BOTLIB, label, __FILE__, __LINE__);
#else
	ptr = botimport.Zone_Alloc(size, TAG_BOTLIB);
#endif

	return ptr;
}

/*
=================
Bot_ZoneMalloc
=================
*/
#ifdef ZONE_DEBUG
void *GetClearedMemoryDebug(unsigned long size, char *label, char *file, int line)
#else
void* GetClearedMemory(unsigned long size)
#endif // ZONE_DEBUG
{
	void* ptr;
#ifdef ZONE_DEBUG
	ptr = GetMemoryDebug(size, label, file, line);
#else
	ptr = GetMemory(size);
#endif // ZONE_DEBUG

	Com_Memset(ptr, 0, size);

	return ptr;
}

/*
=================
Bot_HunkAlloc
=================
*/
#ifdef MEMDEBUG
void *GetHunkMemoryDebug(unsigned long size, char *label, char *file, int line)
#else
void* GetHunkMemory(unsigned long size)
#endif //MEMDEBUG
{
	void* ptr = NULL; 
	
	ptr = botimport.Hunk_Alloc(size);

	return ptr;
}

/*
=================
Bot_HunkMalloc
=================
*/
#ifdef HUNK_DEBUG
void *GetClearedHunkMemoryDebug(unsigned long size, char *label, char *file, int line)
#else
void* GetClearedHunkMemory(unsigned long size)
#endif // HUNK_DEBUG
{
	void* ptr;

#ifdef HUNK_DEBUG
	ptr = GetHunkMemoryDebug(size, label, file, line);
#else
	ptr = GetHunkMemory(size);
#endif // HUNK_DEBUG

	Com_Memset(ptr, 0, size);

	return ptr;
}

/*
=================
Bot_ZoneFree
=================
*/
void FreeMemory(void* ptr)
{
	if (ptr)
	{
		botimport.Zone_Free(ptr);
	}
}

/*
=================
Bot_HunkFree
=================
*/
void Bot_HunkFree(void* ptr)
{
}

/*
=================
Bot_ZoneAvailableMemory
=================
*/
int AvailableMemory(void)
{
	return botimport.Zone_AvailableMemory();
}

/*
=================
Bot_ZoneAvailableMemory
=================
*/
void PrintUsedMemorySize(void)
{
}

/*
=================
Bot_ZoneAvailableMemory
=================
*/
void PrintMemoryLabels(void)
{
	Bot_LogPrintf("============= Botlib memory log ==============\r\n");
	Bot_LogPrintf("\r\n");

	PrintUsedMemorySize();
}
