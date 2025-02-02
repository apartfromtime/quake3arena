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

#include "..\..\libs\rpmalloc\rpmalloc\rpmalloc.h"
#include "q_heap.h"
#include "qcommon.h"

void
Mem_Initialize(void)
{
	rpmalloc_initialize();
}

void Mem_Shutdown(void)
{
	rpmalloc_finalize();
}

/*
==============================================================================

ZONE MEMORY ALLOCATION

==============================================================================
*/

static int s_zoneAlloc;
static int s_zoneTotal;

/*
================
Z_TagMalloc
================
*/
#ifdef ZONE_DEBUG
void* Z_TagMallocDebug(int size, int tag, char* label, char* file, int line)
{
#else
void* Z_TagMalloc(int size, int tag)
{
#endif
	void* buf = NULL;

	if (!tag) {
		Com_Error(ERR_FATAL, "Z_TagMalloc: tried to use a 0 tag");
	}
	
	if ((s_zoneAlloc + size) < s_zoneTotal)
	{
		buf = rpaligned_alloc(4, size);
		s_zoneAlloc += rpmalloc_usable_size(buf);
		memset(buf, 0, size);
	}

	return buf;
}

/*
========================
Z_Malloc
========================
*/
#ifdef ZONE_DEBUG
void* Z_MallocDebug(int size, char* label, char* file, int line)
{
#else
void* Z_Malloc(int size)
{
#endif
	void* buf = NULL;

#ifdef ZONE_DEBUG
	buf = Z_TagMallocDebug(size, TAG_GENERAL, label, file, line);
#else
	buf = Z_TagMalloc(size, TAG_GENERAL);
#endif

	return buf;
}

/*
========================
Zone_InitMemory
========================
*/
bool Zone_InitSmallZoneMemory(int zoneSize)
{
	s_zoneTotal = 1024 * zoneSize;

	return true;
}

/*
========================
Zone_InitMemory
========================
*/
bool Zone_InitMemory(int zoneSize)
{
	s_zoneTotal = 1024 * 1024 * zoneSize;

	return true;
}

/*
========================
Zone_Meminfo
========================
*/
void Zone_Meminfo(void)
{
	rpmalloc_global_statistics_t stat;

	Com_Printf("----- Zone Info ------\n");
	Com_Printf("%8i bytes total\n", s_zoneTotal);
	Com_Printf("%8i bytes alloc\n", s_zoneAlloc);
	Com_Printf("----------------------\n");

	rpmalloc_global_statistics(&stat);

	Com_Printf("----- rpmalloc Info ------\n");
	Com_Printf("%8i bytes total mapped\n", stat.mapped_total);
	Com_Printf("%8i bytes total unmapped\n", stat.unmapped_total);
	Com_Printf("%8i bytes mapped\n", stat.mapped);
	Com_Printf("%8i bytes mapped peak\n", stat.mapped_peak);
	Com_Printf("%8i bytes cached\n", stat.cached);
	Com_Printf("----------------------\n");
}

/*
========================
Z_AvailableMemory
========================
*/
int Z_AvailableMemory(void)
{
	return (s_zoneTotal - s_zoneAlloc);
}

// static mem blocks to reduce a lot of small zone overhead
typedef struct memstatic_s
{
	byte mem[2];
} memstatic_t;

// bk001204 - initializer brackets
memstatic_t emptystring =
	{ {'\0', '\0'} };
memstatic_t numberstring[] = {
	{ {'0', '\0'} },
	{ {'1', '\0'} },
	{ {'2', '\0'} },
	{ {'3', '\0'} },
	{ {'4', '\0'} },
	{ {'5', '\0'} },
	{ {'6', '\0'} },
	{ {'7', '\0'} },
	{ {'8', '\0'} },
	{ {'9', '\0'} }
};

/*
========================
Z_Free
========================
*/
void Z_Free(void* ptr)
{
	if (!ptr) {
		Com_Error(ERR_DROP, "Z_Free: NULL pointer");
	}

	if (ptr != (char*)&emptystring && ptr != (char*)&numberstring[((const char*)ptr)[0] - '0'])
	{
		s_zoneAlloc -= rpmalloc_usable_size(ptr);
		rpfree(ptr);
		ptr = NULL;
	}
}

/*
========================
CopyString

 NOTE:	never write over the memory CopyString returns because
		memory from a memstatic_t might be returned
========================
*/
char* CopyString(const char* in) {
	char* out;

	if (!in[0]) {
		return ((char*)&emptystring);
	}
	else if (!in[1]) {
		if (in[0] >= '0' && in[0] <= '9') {
			return ((char*)&numberstring[in[0] - '0']);
		}
	}
	out = Z_TagMalloc(strlen(in) + 1, TAG_SMALL);
	strcpy(out, in);
	return out;
}

/*
==============================================================================

HUNK MEMORY ALLOCATION

==============================================================================
*/

static int s_hunkAlloc;
static int s_hunkTotal;
static rpmalloc_heap_t*	s_hunk = NULL;

/*
=================
Hunk_Alloc

Allocate permanent (until the hunk is cleared) memory
=================
*/
#ifdef HUNK_DEBUG
void* Hunk_AllocDebug(int size, char* label, char* file, int line)
{
#else
void* Hunk_Alloc(int size)
{
#endif
	void* buf;

	if (s_hunk == NULL)
	{
		Com_Error(ERR_FATAL, "Hunk_Alloc: Hunk memory system not initialized");
	}

	if ((s_hunkAlloc + size) < s_hunkTotal)
	{
		buf = rpmalloc_heap_aligned_alloc(s_hunk, 4, size);
		s_hunkAlloc += rpmalloc_usable_size(buf);
		memset(buf, 0, size);
	}

	return buf;
}

/*
=================
Hunk_InitMemory
=================
*/
bool Hunk_InitMemory(int hunkSize)
{
	s_hunkTotal = 1024 * 1024 * hunkSize;
	s_hunk = rpmalloc_heap_acquire();

	if (!s_hunk)
	{
		return false;
	}

	return true;
}

/*
=================
Hunk_Meminfo
=================
*/

void
Hunk_Meminfo(void)
{
	Com_Printf("----- Hunk Info ------\n");
	Com_Printf("%8i bytes total\n", s_hunkTotal);
	Com_Printf("%8i bytes alloc\n", s_hunkAlloc);
	Com_Printf("----------------------\n");
}

/*
=================
Hunk_Clear

The server calls this before shutting down or loading a new map
=================
*/
void Hunk_Clear(void)
{
	s_hunkAlloc = 0;
	rpmalloc_heap_free_all(s_hunk);
	Com_Printf("Hunk_Clear: reset the hunk ok\n");
}

/*
==================
Hunk_Free
==================
*/
void
Hunk_Free(void* buf)
{
	rpmalloc_heap_free(s_hunk, buf);
}

/*
=================
Hunk_AllocateTempMemory

This is used by the file loading system.
Multiple files can be loaded in temporary memory.
When the files-in-use count reaches zero, all temp memory will be deleted
=================
*/
void* Hunk_AllocateTempMemory(int size)
{
	void* buf;

	// return a Z_Malloc'd block if the hunk has not been initialized
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different 
	// memory systems
	if (s_hunk == NULL)
	{
		return Z_Malloc(size);
	}

	buf = rpmalloc_heap_aligned_alloc(s_hunk, 4, size);
	s_hunkAlloc += rpmalloc_usable_size(buf);
	memset(buf, 0, size);

	return buf;
}

/*
==================
Hunk_FreeTempMemory
==================
*/
void Hunk_FreeTempMemory(void* buf)
{
	// free with Z_Free if the hunk has not been initialized
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different 
	// memory systems
	if (s_hunk == NULL)
	{
		Z_Free(buf);

		return;
	}
	
	s_hunkAlloc -= rpmalloc_usable_size(buf);
	rpmalloc_heap_free(s_hunk, buf);
}

/*
====================
Hunk_MemoryRemaining
====================
*/
int	Hunk_MemoryRemaining(void)
{
	return (s_hunkTotal - s_hunkAlloc);
}