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

#if defined(_DEBUG) && !defined(BSPC)
#define ZONE_DEBUG
#define HUNK_DEBUG
#endif // #if defined(_DEBUG) && !defined(BSPC)

#ifdef ZONE_DEBUG
#define GetZoneMemory(size)			GetZoneMemoryDebug(size, #size, __FILE__, __LINE__);
// allocate a memory block of the given size
void *GetZoneMemoryDebug(unsigned long size, char *label, char *file, int line);
#else
// allocate a memory block of the given size
void* GetZoneMemory(unsigned long size);
#endif // #ifdef ZONE_DEBUG

// free the given memory block
void FreeZoneMemory(void* ptr);

#ifdef HUNK_DEBUG
#define GetHunkMemory(size)			GetHunkMemoryDebug(size, #size, __FILE__, __LINE__);
// allocate a memory block of the given size
void* GetHunkMemoryDebug(unsigned long size, char* label, char* file, int line);
#else
// allocate a memory block of the given size
void* GetHunkMemory(unsigned long size);
#endif // #ifdef HUNK_DEBUG

// returns the amount available memory
int AvailableMemory(void);
