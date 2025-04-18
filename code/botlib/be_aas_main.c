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

/*****************************************************************************
 * name:		be_aas_main.c
 *
 * desc:		AAS
 *
 * $Archive: /MissionPack/code/botlib/be_aas_main.c $
 *
 *****************************************************************************/

#include "../game/q_shared.h"
#include "l_memory.h"
#include "l_libvar.h"
#include "l_script.h"
#include "l_precomp.h"
#include "l_struct.h"
#include "l_log.h"
#include "aasfile.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "be_aas_funcs.h"
#include "be_interface.h"
#include "be_aas_def.h"

aas_t aasworld;

extern cvar_t* sv_maxclients;
extern cvar_t* sv_maxentities;
cvar_t* aas_optimize;
cvar_t* bot_saveroutingcache;

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void Q_CDECL AAS_Error(char *fmt, ...)
{
	char str[1024];
	va_list arglist;

	va_start(arglist, fmt);
	vsprintf(str, fmt, arglist);
	va_end(arglist);
	botimport.Print(PRT_FATAL, str);
} //end of the function AAS_Error
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
char *AAS_StringFromIndex(char *indexname, char *stringindex[], int numindexes, int index)
{
	if (!aasworld.indexessetup)
	{
		botimport.Print(PRT_ERROR, "%s: index %d not setup\n", indexname, index);
		return "";
	} //end if
	if (index < 0 || index >= numindexes)
	{
		botimport.Print(PRT_ERROR, "%s: index %d out of range\n", indexname, index);
		return "";
	} //end if
	if (!stringindex[index])
	{
		if (index)
		{
			botimport.Print(PRT_ERROR, "%s: reference to unused index %d\n", indexname, index);
		} //end if
		return "";
	} //end if
	return stringindex[index];
} //end of the function AAS_StringFromIndex
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_IndexFromString(char *indexname, char *stringindex[], int numindexes, char *string)
{
	int i;
	if (!aasworld.indexessetup)
	{
		botimport.Print(PRT_ERROR, "%s: index not setup \"%s\"\n", indexname, string);
		return 0;
	} //end if
	for (i = 0; i < numindexes; i++)
	{
		if (!stringindex[i]) continue;
		if (!Q_stricmp(stringindex[i], string)) return i;
	} //end for
	return 0;
} //end of the function AAS_IndexFromString
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
char *AAS_ModelFromIndex(int index)
{
	return AAS_StringFromIndex("ModelFromIndex", &aasworld.configstrings[CS_MODELS], MAX_MODELS, index);
} //end of the function AAS_ModelFromIndex
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_IndexFromModel(char *modelname)
{
	return AAS_IndexFromString("IndexFromModel", &aasworld.configstrings[CS_MODELS], MAX_MODELS, modelname);
} //end of the function AAS_IndexFromModel
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_UpdateStringIndexes(int numconfigstrings, char *configstrings[])
{
	int i;
	//set string pointers and copy the strings
	for (i = 0; i < numconfigstrings; i++)
	{
		if (configstrings[i])
		{
			//if (aasworld.configstrings[i]) FreeMemory(aasworld.configstrings[i]);
			aasworld.configstrings[i] = (char *) GetZoneMemory(strlen(configstrings[i]) + 1);
			strcpy(aasworld.configstrings[i], configstrings[i]);
		} //end if
	} //end for
	aasworld.indexessetup = true;
} //end of the function AAS_UpdateStringIndexes
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_Loaded(void)
{
	return aasworld.loaded;
} //end of the function AAS_Loaded
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_Initialized(void)
{
	return aasworld.initialized;
} //end of the function AAS_Initialized
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_SetInitialized(void)
{
	aasworld.initialized = true;
	botimport.Print(PRT_MESSAGE, "AAS initialized.\n");
#ifdef DEBUG
	//create all the routing cache
	//AAS_CreateAllRoutingCache();
	//
	//AAS_RoutingInfo();
#endif
} //end of the function AAS_SetInitialized

//===========================================================================
// AAS_ContinueInit
//===========================================================================
void AAS_ContinueInit(float time)
{
	// if no AAS file loaded
	if (!aasworld.loaded) return;
	// if AAS is already initialized
	if (aasworld.initialized) return;
	// calculate reachability, if not finished return
	if (AAS_ContinueInitReachability(time)) return;
	// initialize clustering for the new map
	AAS_InitClustering();

	// if reachability has been calculated and an AAS file should be written
	// or there is a forced data optimization
	if (aasworld.savefile || ((int)Botlib_CvarGetValue("bot_forcewrite")))
	{
		// optimize the AAS data
		aas_optimize = Botlib_CvarGet("bot_aasoptimize", "0");
		if (aas_optimize->integer) {
			AAS_Optimize();
		}

		// save the AAS file
		if (AAS_WriteAASFile(aasworld.filename)) {
			botimport.Print(PRT_MESSAGE, "%s written succesfully\n",
				aasworld.filename);
		} else {
			botimport.Print(PRT_ERROR, "couldn't write %s\n", aasworld.filename);
		}
	}

	// initialize the routing
	AAS_InitRouting();

	// at this point AAS is initialized
	AAS_SetInitialized();
}

//===========================================================================
// AAS_StartFrame
// called at the start of every frame
//===========================================================================
int AAS_StartFrame(float time)
{
	aasworld.time = time;
	// unlink all entities that were not updated last frame
	AAS_UnlinkInvalidEntities();
	// invalidate the entities
	AAS_InvalidateEntities();
	// initialize AAS
	AAS_ContinueInit(time);
	aasworld.frameroutingupdates = 0;

	if (bot_developer)
	{
		if (Botlib_CvarGetValue("showcacheupdates"))
		{
			AAS_RoutingInfo();
			Botlib_CvarSet("showcacheupdates", "0");
		}
#if 0
		if (Botlib_CvarGetValue("showmemoryusage"))
		{
			PrintUsedMemorySize();
			Botlib_CvarSet("showmemoryusage", "0");
		}

		if (Botlib_CvarGetValue("memorydump"))
		{
			PrintMemoryLabels();
			Botlib_CvarSet("memorydump", "0");
		}
#endif // #if 0
	}

	if (bot_saveroutingcache->value)
	{
		AAS_WriteRouteCache();
	}

	aasworld.numframes++;

	return BLERR_NOERROR;
}

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
float AAS_Time(void)
{
	return aasworld.time;
} //end of the function AAS_Time
//===========================================================================
//
// Parameter:			-
// Returns:				-
// Changes Globals:		-
//===========================================================================
void AAS_ProjectPointOntoVector( vec3_t point, vec3_t vStart, vec3_t vEnd, vec3_t vProj )
{
	vec3_t pVec, vec;

	VectorSubtract( point, vStart, pVec );
	VectorSubtract( vEnd, vStart, vec );
	VectorNormalize( vec );
	// project onto the directional vector for this segment
	VectorMA( vStart, DotProduct( pVec, vec ), vec, vProj );
} //end of the function AAS_ProjectPointOntoVector
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_LoadFiles(const char *mapname)
{
	int errnum;
	char aasfile[MAX_PATH];
//	char bspfile[MAX_PATH];

	strcpy(aasworld.mapname, mapname);
	//NOTE: first reset the entity links into the AAS areas and BSP leaves
	// the AAS link heap and BSP link heap are reset after respectively the
	// AAS file and BSP file are loaded
	AAS_ResetEntityLinks();
	// load bsp info
	AAS_LoadBSPFile();

	//load the aas file
	Com_sprintf(aasfile, MAX_PATH, "maps/%s.aas", mapname);
	errnum = AAS_LoadAASFile(aasfile);
	if (errnum != BLERR_NOERROR)
		return errnum;

	botimport.Print(PRT_MESSAGE, "loaded %s\n", aasfile);
	strncpy(aasworld.filename, aasfile, MAX_PATH);
	return BLERR_NOERROR;
} //end of the function AAS_LoadFiles
//===========================================================================
// called everytime a map changes
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_LoadMap(const char *mapname)
{
	int	errnum;

	//if no mapname is provided then the string indexes are updated
	if (!mapname)
	{
		return 0;
	} //end if
	//
	aasworld.initialized = false;
	//NOTE: free the routing caches before loading a new map because
	// to free the caches the old number of areas, number of clusters
	// and number of areas in a clusters must be available
	AAS_FreeRoutingCaches();
	//load the map
	errnum = AAS_LoadFiles(mapname);
	if (errnum != BLERR_NOERROR)
	{
		aasworld.loaded = false;
		return errnum;
	} //end if
	//
	AAS_InitSettings();
	//initialize the AAS link heap for the new map
	AAS_InitAASLinkHeap();
	//initialize the AAS linked entities for the new map
	AAS_InitAASLinkedEntities();
	//initialize reachability for the new map
	AAS_InitReachability();
	//initialize the alternative routing
	AAS_InitAlternativeRouting();
	//everything went ok
	return 0;
} //end of the function AAS_LoadMap

//===========================================================================
// AAS_Setup
// called when the library is first loaded
//===========================================================================
int AAS_Setup(void)
{
	sv_maxclients = Botlib_CvarGet("sv_maxclients", "8");
	sv_maxentities = Botlib_CvarGet("sv_maxentities", "1024");

	aasworld.maxclients = sv_maxclients->integer;
	aasworld.maxentities = sv_maxentities->integer;

	// as soon as it's set to 1 the routing cache will be saved
	bot_saveroutingcache = Botlib_CvarGet("bot_saveroutingcache", "0");

	// allocate memory for the entities
	if (aasworld.entities) FreeZoneMemory(aasworld.entities);
	aasworld.entities = (aas_entity_t*)GetHunkMemory(aasworld.maxentities *
		sizeof(aas_entity_t));

	// invalidate all the entities
	AAS_InvalidateEntities();
	
	// force clustering and reachability calculations
	// Botlib_CvarSetValue("bot_forceclustering", "1");
	// Botlib_CvarSetValue("bot_forcereachability", "1");
	aasworld.numframes = 0;

	return BLERR_NOERROR;
}

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_Shutdown(void)
{
	AAS_ShutdownAlternativeRouting();
	//
	AAS_DumpBSPData();
	//free routing caches
	AAS_FreeRoutingCaches();
	//free aas link heap
	AAS_FreeAASLinkHeap();
	//free aas linked entities
	AAS_FreeAASLinkedEntities();
	//free the aas data
	AAS_DumpAASData();
	//free the entities
	//if (aasworld.entities) FreeMemory(aasworld.entities);
	//clear the aasworld structure
	Com_Memset(&aasworld, 0, sizeof(aas_t));
	//aas has not been initialized
	aasworld.initialized = false;
	//NOTE: as soon as a new .bsp file is loaded the .bsp file memory is
	// freed an reallocated, so there's no need to free that memory here
	//print shutdown
	botimport.Print(PRT_MESSAGE, "AAS shutdown.\n");
} //end of the function AAS_Shutdown
