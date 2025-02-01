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

MEMORY

==============================================================================
*/

extern qhandle_t logfile;

/*
==============================================================================

ZONE MEMORY ALLOCATION

==============================================================================
*/

static int s_zoneTotal;
static int s_smallZoneTotal;

/*
=================
Zone_InitSmallZoneMemory
=================
*/
bool Zone_InitSmallZoneMemory(int smallSize)
{
	s_smallZoneTotal = 1024 * smallSize;

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
Zone_TouchMemory
========================
*/
void Zone_TouchMemory(void)
{
}

void Zone_Meminfo(void)
{
	int			zoneBytes, zoneBlocks;
	int			smallZoneBytes, smallZoneBlocks;
	int			botlibBytes, rendererBytes;

	zoneBytes = 0;
	botlibBytes = 0;
	rendererBytes = 0;
	zoneBlocks = 0;

	Com_Printf("----- Zone Info ------\n");

	smallZoneBytes = 0;
	smallZoneBlocks = 0;

	Com_Printf("%8i bytes total zone\n", s_zoneTotal);
	Com_Printf("\n");
	Com_Printf("%8i bytes in %i zone blocks\n", zoneBytes, zoneBlocks);
	Com_Printf("        %8i bytes in dynamic botlib\n", botlibBytes);
	Com_Printf("        %8i bytes in dynamic renderer\n", rendererBytes);
	Com_Printf("        %8i bytes in dynamic other\n", zoneBytes - (botlibBytes + rendererBytes));
	Com_Printf("        %8i bytes in small Zone memory\n", smallZoneBytes);
	Com_Printf("----------------------\n");
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
================
Z_FreeTags
================
*/
void Z_FreeTags(int tag)
{

}

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
Z_CheckHeap
========================
*/
void Z_CheckHeap(void)
{
}

/*
========================
Z_LogZoneHeap
========================
*/
void Z_LogZoneHeap(char* name)
{
	char		buf[4096];
	int size, allocSize, numBlocks;

	if (!logfile || !FS_Initialized())
	{
		return;
	}

	size = allocSize = numBlocks = 0;
	Com_sprintf(buf, sizeof(buf), "\r\n================\r\n%s log\r\n================\r\n", name);
	FS_Write(buf, strlen(buf), logfile);

	Com_sprintf(buf, sizeof(buf), "%d %s memory in %d blocks\r\n", size, name, numBlocks);
	FS_Write(buf, strlen(buf), logfile);
	Com_sprintf(buf, sizeof(buf), "%d %s memory overhead\r\n", size - allocSize, name);
	FS_Write(buf, strlen(buf), logfile);
}

/*
========================
Z_LogHeap
========================
*/
void Z_LogHeap(void)
{
	Z_LogZoneHeap("MAIN");
	Z_LogZoneHeap("SMALL");
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
	out = S_Malloc(strlen(in) + 1);
	strcpy(out, in);
	return out;
}

/*
==============================================================================

HUNK MEMORY ALLOCATION

==============================================================================
*/

static mi_heap_t* s_hunk = NULL;

static	int		s_hunkTotal;

/*
=================
Com_InitHunkMemory
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

	Hunk_Clear();

	return true;
}

void Mem_Output_f(const char* msg, void* arg)
{
	Com_Printf(msg, arg);
}

void Hunk_Meminfo(void)
{
	int unused = 0;

	Com_Printf("----- Hunk Info ------\n");

	mi_stats_print_out(Mem_Output_f, NULL);

	Com_Printf("----------------------\n");
}

void Hunk_TouchMemory(void)
{
}

/*
=================
Hunk_Log
=================
*/
void Hunk_Log(void)
{
	char		buf[4096];
	int size, numBlocks;

	if (!logfile || !FS_Initialized())
	{
		return;
	}

	size = 0;
	numBlocks = 0;
	Com_sprintf(buf, sizeof(buf), "\r\n================\r\nHunk log\r\n================\r\n");
	FS_Write(buf, strlen(buf), logfile);

	Com_sprintf(buf, sizeof(buf), "%d Hunk memory\r\n", size);
	FS_Write(buf, strlen(buf), logfile);
	Com_sprintf(buf, sizeof(buf), "%d hunk blocks\r\n", numBlocks);
	FS_Write(buf, strlen(buf), logfile);
}

/*
=================
Hunk_SmallLog
=================
*/
void Hunk_SmallLog(void)
{
	char		buf[4096];
	int size, numBlocks;

	if (!logfile || !FS_Initialized())
	{
		return;
	}

	size = 0;
	numBlocks = 0;
	Com_sprintf(buf, sizeof(buf), "\r\n================\r\nHunk Small log\r\n================\r\n");
	FS_Write(buf, strlen(buf), logfile);

	Com_sprintf(buf, sizeof(buf), "%d Hunk memory\r\n", size);
	FS_Write(buf, strlen(buf), logfile);
	Com_sprintf(buf, sizeof(buf), "%d hunk blocks\r\n", numBlocks);
	FS_Write(buf, strlen(buf), logfile);
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

/*
===================
Hunk_SetMark

The server calls this after the level and game VM have been loaded
===================
*/
void Hunk_SetMark(void)
{
}

/*
=================
Hunk_ClearToMark

The client calls this before starting a vid_restart or snd_restart
=================
*/
void Hunk_ClearToMark(void)
{
}

/*
=================
Hunk_CheckMark
=================
*/
bool Hunk_CheckMark(void)
{
	return true;
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

static void Hunk_SwapBanks(void)
{
}

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
=================
Hunk_ClearTempMemory

The temp space is no longer needed.  If we have left more
touched but unused memory on this side, have future
permanent allocs use this side.
=================
*/
void Hunk_ClearTempMemory(void)
{
}

/*
=================
Hunk_Trash
=================
*/
void Hunk_Trash(void)
{
}
