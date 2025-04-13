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

/*
=================
Bot_ZoneAlloc
=================
*/
#ifdef ZONE_DEBUG
void* GetZoneMemoryDebug(unsigned long size, char *label, char *file, int line)
#else
void* GetZoneMemory(unsigned long size)
#endif // #ifdef ZONE_DEBUG
{
	void* ptr = NULL;

#ifdef ZONE_DEBUG
	ptr = g_bimport.Zone_AllocDebug(size, label, file, line);
#else
	ptr = botimport.Zone_Alloc(size);
#endif // #ifdef ZONE_DEBUG

	return ptr;
}

/*
=================
Bot_HunkAlloc
=================
*/
#ifdef HUNK_DEBUG
void* GetHunkMemoryDebug(unsigned long size, char* label, char* file, int line)
#else
void* GetHunkMemory(unsigned long size)
#endif // #ifdef HUNK_DEBUG
{
	void* ptr = NULL;

#ifdef HUNK_DEBUG
	ptr = g_bimport.Hunk_AllocDebug(size, label, file, line);
#else
	ptr = botimport.Hunk_Alloc(size);
#endif // #ifdef HUNK_DEBUG

	return ptr;
}

/*
=================
Bot_ZoneFree
=================
*/
void FreeZoneMemory(void* ptr)
{
	if (ptr)
	{
		g_bimport.Zone_Free(ptr);
	}
}

/*
=================
Bot_ZoneAvailableMemory
=================
*/
int AvailableMemory(void)
{
	return g_bimport.Zone_AvailableMemory();
}
