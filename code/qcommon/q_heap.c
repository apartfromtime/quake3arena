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

#include "..\..\libs\mimalloc\include\mimalloc.h"
#include "q_heap.h"
#include "qcommon.h"

/*
==============================================================================

ZONE MEMORY ALLOCATION

==============================================================================
*/

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

	if (tag == TAG_SMALL && size <= MI_SMALL_SIZE_MAX) {
		buf = mi_zalloc_small(size);
	}
	else {
		buf = mi_zalloc_aligned(size, 4);
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
	Com_Printf("----- Zone Info ------\n");
}

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

	mi_free(ptr);
	ptr = NULL;
}

/*
========================
Z_AvailableMemory
========================
*/
int Z_AvailableMemory(void)
{
	return s_zoneTotal;
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

static int			s_hunkTotal;
static mi_heap_t*	s_hunk = NULL;

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

	buf = mi_heap_zalloc_aligned(s_hunk, size, 4);

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

	s_hunk = mi_heap_new();

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
Mem_Output_f(const char* msg, void* arg)
{
	Com_Printf(msg, arg);
}

void
Hunk_Meminfo(void)
{
	Com_Printf("----- Hunk Info ------\n");

	mi_stats_print_out(Mem_Output_f, NULL);

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
	Com_Printf("Hunk_Clear: reset the hunk ok\n");
}

/*
==================
Hunk_Free
==================
*/
void
Hunk_Free(void)
{
	mi_heap_destroy(s_hunk);
	s_hunk = NULL;
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

	buf = mi_heap_malloc_aligned(s_hunk, size, 4);

	// don't bother clearing, because we are going to load a file over it
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

	if (mi_heap_check_owned(s_hunk, buf))
	{
		mi_free(buf);
	}
}

/*
====================
Hunk_MemoryRemaining
====================
*/
int	Hunk_MemoryRemaining(void)
{
	return s_hunkTotal;
}