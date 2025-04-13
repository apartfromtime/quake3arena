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
 * name:		be_interface.c // bk010221 - FIXME - DEAD code elimination
 *
 * desc:		bot library interface
 *
 * $Archive: /MissionPack/code/botlib/be_interface.c $
 *
 *****************************************************************************/

#include "../game/q_shared.h"
#include "l_memory.h"
#include "l_log.h"
#include "l_libvar.h"
#include "l_script.h"
#include "l_precomp.h"
#include "l_struct.h"
#include "aasfile.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "be_aas_funcs.h"
#include "be_aas_def.h"
#include "be_interface.h"

#include "../game/be_ea.h"
#include "be_ai_weight.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../game/be_ai_weap.h"
#include "../game/be_ai_chat.h"
#include "../game/be_ai_char.h"
#include "../game/be_ai_gen.h"

void
Bot_DebugInit(void);
int
Export_BotTest(int parm0, char* parm1, vec3_t parm2, vec3_t parm3);

botlib_export_t g_bexport;
botlib_import_t g_bimport;
//
cvar_t* bot_logfile;
cvar_t* sv_maxclients;			// maximum number of clients
cvar_t* sv_maxentities;			// maximum number of entities
bool s_initialized;			// true when the bot library has been setup
int bot_developer;

//===========================================================================
//
// several functions used by the exported functions
//
//===========================================================================

//===========================================================================
// ValidClientNumber
//===========================================================================
bool ValidClientNumber(int num, char *str)
{
	if (num < 0 || num > sv_maxclients->integer)
	{
		// weird: the disabled stuff results in a crash
		g_bimport.Print(PRT_ERROR, "%s: invalid client number %d, [0, %d]\n",
			str, num, sv_maxclients->integer);

		return false;
	}

	return true;
}

//===========================================================================
// ValidEntityNumber
//===========================================================================
bool ValidEntityNumber(int num, char* str)
{
	if (num < 0 || num > sv_maxentities->integer)
	{
		g_bimport.Print(PRT_ERROR, "%s: invalid entity number %d, [0, %d]\n",
			str, num, sv_maxentities->integer);
		return false;
	}

	return true;
}

//===========================================================================
// BotLibSetup
//===========================================================================
bool BotLibSetup(char* str)
{
	if (!s_initialized)
	{
		g_bimport.Print(PRT_ERROR, "%s: bot library used before being setup\n", str);
		return false;
	}

	return true;
}

//===========================================================================
// Export_BotLibSetup
//===========================================================================
int Export_BotLibSetup(void)
{
	int errnum = BLERR_NOERROR;

	bot_developer = Botlib_CvarGetValue("bot_developer");

#ifdef _DEBUG
	Bot_DebugInit();
#endif // #ifdef _DEBUG

	// initialize byte swapping (litte endian etc.)
	// Swap_Init();
	bot_logfile = Botlib_CvarGet("bot_log", "0");
	Bot_LogOpen("botlib.log");

	g_bimport.Print(PRT_MESSAGE, "------- BotLib Initialization -------\n");

	sv_maxclients = Botlib_CvarGet("sv_maxclients", "8");
	sv_maxentities = Botlib_CvarGet("sv_maxentities", "1024");

	errnum = AAS_Setup();           // be_aas_main.c
	if (errnum != BLERR_NOERROR) return errnum;
	errnum = EA_Setup(sv_maxclients->integer);          // be_ea.c
	if (errnum != BLERR_NOERROR) return errnum;
	errnum = BotSetupWeaponAI();            // be_ai_weap.c
	if (errnum != BLERR_NOERROR)return errnum;
	errnum = BotSetupGoalAI();          // be_ai_goal.c
	if (errnum != BLERR_NOERROR) return errnum;
	errnum = BotSetupChatAI();          // be_ai_chat.c
	if (errnum != BLERR_NOERROR) return errnum;
	errnum = BotSetupMoveAI();          // be_ai_move.c
	if (errnum != BLERR_NOERROR) return errnum;

	s_initialized = true;

	return BLERR_NOERROR;
}

//===========================================================================
// Export_BotLibShutdown
//===========================================================================
int Export_BotLibShutdown(void)
{
	if (!BotLibSetup("BotLibShutdown")) return BLERR_LIBRARYNOTSETUP;

	BotShutdownChatAI();
	BotShutdownMoveAI();
	BotShutdownGoalAI();
	BotShutdownWeaponAI();
	BotShutdownWeights();
	BotShutdownCharacters();
	
	// shut down aas
	AAS_Shutdown();
	// shut down bot elemantary actions
	EA_Shutdown();

	// remove all global defines from the pre compiler
	PC_RemoveAllGlobalDefines();

	// shut down library log file
	Bot_LogShutdown();

	s_initialized = false;

	// print any files still open
	PC_CheckOpenSourceHandles();

	return BLERR_NOERROR;
}

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int Export_BotLibVarSet(char *var_name, char *value)
{
	Botlib_CvarSet(var_name, value);
	return BLERR_NOERROR;
} //end of the function Export_BotLibVarSet
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int Export_BotLibVarGet(char *var_name, char *value, int size)
{
	char *varvalue;

	varvalue = Botlib_CvarGetString(var_name);
	strncpy(value, varvalue, size-1);
	value[size-1] = '\0';
	return BLERR_NOERROR;
} //end of the function Export_BotLibVarGet
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int Export_BotLibStartFrame(float time)
{
	if (!BotLibSetup("BotStartFrame")) return BLERR_LIBRARYNOTSETUP;
	return AAS_StartFrame(time);
} //end of the function Export_BotLibStartFrame

//===========================================================================
// Export_BotLibLoadMap
//===========================================================================
int Export_BotLibLoadMap(const char* mapname)
{
#ifdef DEBUG
	int starttime = g_bimport.Milliseconds();
#endif
	int errnum = BLERR_NOERROR;

	if (!BotLibSetup("BotLoadMap")) return BLERR_LIBRARYNOTSETUP;

	g_bimport.Print(PRT_MESSAGE, "------------ Map Loading ------------\n");

	// startup AAS for the current map, model and sound index
	errnum = AAS_LoadMap(mapname);
	if (errnum != BLERR_NOERROR) return errnum;
	
	// initialize the items in the level
	BotInitLevelItems();
	BotSetBrushModelTypes();

	g_bimport.Print(PRT_MESSAGE, "-------------------------------------\n");
#ifdef DEBUG
	g_bimport.Print(PRT_MESSAGE, "map loaded in %d msec\n",
		g_bimport.Milliseconds() - starttime);
#endif

	return BLERR_NOERROR;
}

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int Export_BotLibUpdateEntity(int ent, bot_entitystate_t *state)
{
	if (!BotLibSetup("BotUpdateEntity")) return BLERR_LIBRARYNOTSETUP;
	if (!ValidEntityNumber(ent, "BotUpdateEntity")) return BLERR_INVALIDENTITYNUMBER;

	return AAS_UpdateEntity(ent, state);
} //end of the function Export_BotLibUpdateEntity

/*
============
Init_AAS_Export
============
*/
static void Init_AAS_Export( aas_export_t *aas ) {
	//--------------------------------------------
	// be_aas_entity.c
	//--------------------------------------------
	aas->AAS_EntityInfo = AAS_EntityInfo;
	//--------------------------------------------
	// be_aas_main.c
	//--------------------------------------------
	aas->AAS_Initialized = AAS_Initialized;
	aas->AAS_PresenceTypeBoundingBox = AAS_PresenceTypeBoundingBox;
	aas->AAS_Time = AAS_Time;
	//--------------------------------------------
	// be_aas_sample.c
	//--------------------------------------------
	aas->AAS_PointAreaNum = AAS_PointAreaNum;
	aas->AAS_PointReachabilityAreaIndex = AAS_PointReachabilityAreaIndex;
	aas->AAS_TraceAreas = AAS_TraceAreas;
	aas->AAS_BBoxAreas = AAS_BBoxAreas;
	aas->AAS_AreaInfo = AAS_AreaInfo;
	//--------------------------------------------
	// be_aas_bspq3.c
	//--------------------------------------------
	aas->AAS_PointContents = AAS_PointContents;
	aas->AAS_NextBSPEntity = AAS_NextBSPEntity;
	aas->AAS_ValueForBSPEpairKey = AAS_ValueForBSPEpairKey;
	aas->AAS_VectorForBSPEpairKey = AAS_VectorForBSPEpairKey;
	aas->AAS_FloatForBSPEpairKey = AAS_FloatForBSPEpairKey;
	aas->AAS_IntForBSPEpairKey = AAS_IntForBSPEpairKey;
	//--------------------------------------------
	// be_aas_reach.c
	//--------------------------------------------
	aas->AAS_AreaReachability = AAS_AreaReachability;
	//--------------------------------------------
	// be_aas_route.c
	//--------------------------------------------
	aas->AAS_AreaTravelTimeToGoalArea = AAS_AreaTravelTimeToGoalArea;
	aas->AAS_EnableRoutingArea = AAS_EnableRoutingArea;
	aas->AAS_PredictRoute = AAS_PredictRoute;
	//--------------------------------------------
	// be_aas_altroute.c
	//--------------------------------------------
	aas->AAS_AlternativeRouteGoals = AAS_AlternativeRouteGoals;
	//--------------------------------------------
	// be_aas_move.c
	//--------------------------------------------
	aas->AAS_Swimming = AAS_Swimming;
	aas->AAS_PredictClientMovement = AAS_PredictClientMovement;
}

  
/*
============
Init_EA_Export
============
*/
static void Init_EA_Export( ea_export_t *ea ) {
	//ClientCommand elementary actions
	ea->EA_Command = EA_Command;
	ea->EA_Say = EA_Say;
	ea->EA_SayTeam = EA_SayTeam;

	ea->EA_Action = EA_Action;
	ea->EA_Gesture = EA_Gesture;
	ea->EA_Talk = EA_Talk;
	ea->EA_Attack = EA_Attack;
	ea->EA_Use = EA_Use;
	ea->EA_Respawn = EA_Respawn;
	ea->EA_Crouch = EA_Crouch;
	ea->EA_MoveUp = EA_MoveUp;
	ea->EA_MoveDown = EA_MoveDown;
	ea->EA_MoveForward = EA_MoveForward;
	ea->EA_MoveBack = EA_MoveBack;
	ea->EA_MoveLeft = EA_MoveLeft;
	ea->EA_MoveRight = EA_MoveRight;

	ea->EA_SelectWeapon = EA_SelectWeapon;
	ea->EA_Jump = EA_Jump;
	ea->EA_DelayedJump = EA_DelayedJump;
	ea->EA_Move = EA_Move;
	ea->EA_View = EA_View;
	ea->EA_GetInput = EA_GetInput;
	ea->EA_EndRegular = EA_EndRegular;
	ea->EA_ResetInput = EA_ResetInput;
}


/*
============
Init_AI_Export
============
*/
static void Init_AI_Export( ai_export_t *ai ) {
	//-----------------------------------
	// be_ai_char.h
	//-----------------------------------
	ai->BotLoadCharacter = BotLoadCharacter;
	ai->BotFreeCharacter = BotFreeCharacter;
	ai->Characteristic_Float = Characteristic_Float;
	ai->Characteristic_BFloat = Characteristic_BFloat;
	ai->Characteristic_Integer = Characteristic_Integer;
	ai->Characteristic_BInteger = Characteristic_BInteger;
	ai->Characteristic_String = Characteristic_String;
	//-----------------------------------
	// be_ai_chat.h
	//-----------------------------------
	ai->BotAllocChatState = BotAllocChatState;
	ai->BotFreeChatState = BotFreeChatState;
	ai->BotQueueConsoleMessage = BotQueueConsoleMessage;
	ai->BotRemoveConsoleMessage = BotRemoveConsoleMessage;
	ai->BotNextConsoleMessage = BotNextConsoleMessage;
	ai->BotNumConsoleMessages = BotNumConsoleMessages;
	ai->BotInitialChat = BotInitialChat;
	ai->BotNumInitialChats = BotNumInitialChats;
	ai->BotReplyChat = BotReplyChat;
	ai->BotChatLength = BotChatLength;
	ai->BotEnterChat = BotEnterChat;
	ai->BotGetChatMessage = BotGetChatMessage;
	ai->StringContains = StringContains;
	ai->BotFindMatch = BotFindMatch;
	ai->BotMatchVariable = BotMatchVariable;
	ai->UnifyWhiteSpaces = UnifyWhiteSpaces;
	ai->BotReplaceSynonyms = BotReplaceSynonyms;
	ai->BotLoadChatFile = BotLoadChatFile;
	ai->BotSetChatGender = BotSetChatGender;
	ai->BotSetChatName = BotSetChatName;
	//-----------------------------------
	// be_ai_goal.h
	//-----------------------------------
	ai->BotResetGoalState = BotResetGoalState;
	ai->BotResetAvoidGoals = BotResetAvoidGoals;
	ai->BotRemoveFromAvoidGoals = BotRemoveFromAvoidGoals;
	ai->BotPushGoal = BotPushGoal;
	ai->BotPopGoal = BotPopGoal;
	ai->BotEmptyGoalStack = BotEmptyGoalStack;
	ai->BotDumpAvoidGoals = BotDumpAvoidGoals;
	ai->BotDumpGoalStack = BotDumpGoalStack;
	ai->BotGoalName = BotGoalName;
	ai->BotGetTopGoal = BotGetTopGoal;
	ai->BotGetSecondGoal = BotGetSecondGoal;
	ai->BotChooseLTGItem = BotChooseLTGItem;
	ai->BotChooseNBGItem = BotChooseNBGItem;
	ai->BotTouchingGoal = BotTouchingGoal;
	ai->BotItemGoalInVisButNotVisible = BotItemGoalInVisButNotVisible;
	ai->BotGetLevelItemGoal = BotGetLevelItemGoal;
	ai->BotGetNextCampSpotGoal = BotGetNextCampSpotGoal;
	ai->BotGetMapLocationGoal = BotGetMapLocationGoal;
	ai->BotAvoidGoalTime = BotAvoidGoalTime;
	ai->BotSetAvoidGoalTime = BotSetAvoidGoalTime;
	ai->BotInitLevelItems = BotInitLevelItems;
	ai->BotUpdateEntityItems = BotUpdateEntityItems;
	ai->BotLoadItemWeights = BotLoadItemWeights;
	ai->BotFreeItemWeights = BotFreeItemWeights;
	ai->BotInterbreedGoalFuzzyLogic = BotInterbreedGoalFuzzyLogic;
	ai->BotSaveGoalFuzzyLogic = BotSaveGoalFuzzyLogic;
	ai->BotMutateGoalFuzzyLogic = BotMutateGoalFuzzyLogic;
	ai->BotAllocGoalState = BotAllocGoalState;
	ai->BotFreeGoalState = BotFreeGoalState;
	//-----------------------------------
	// be_ai_move.h
	//-----------------------------------
	ai->BotResetMoveState = BotResetMoveState;
	ai->BotMoveToGoal = BotMoveToGoal;
	ai->BotMoveInDirection = BotMoveInDirection;
	ai->BotResetAvoidReach = BotResetAvoidReach;
	ai->BotResetLastAvoidReach = BotResetLastAvoidReach;
	ai->BotReachabilityArea = BotReachabilityArea;
	ai->BotMovementViewTarget = BotMovementViewTarget;
	ai->BotPredictVisiblePosition = BotPredictVisiblePosition;
	ai->BotAllocMoveState = BotAllocMoveState;
	ai->BotFreeMoveState = BotFreeMoveState;
	ai->BotInitMoveState = BotInitMoveState;
	ai->BotAddAvoidSpot = BotAddAvoidSpot;
	//-----------------------------------
	// be_ai_weap.h
	//-----------------------------------
	ai->BotChooseBestFightWeapon = BotChooseBestFightWeapon;
	ai->BotGetWeaponInfo = BotGetWeaponInfo;
	ai->BotLoadWeaponWeights = BotLoadWeaponWeights;
	ai->BotAllocWeaponState = BotAllocWeaponState;
	ai->BotFreeWeaponState = BotFreeWeaponState;
	ai->BotResetWeaponState = BotResetWeaponState;
	//-----------------------------------
	// be_ai_gen.h
	//-----------------------------------
	ai->GeneticParentsAndChildSelection = GeneticParentsAndChildSelection;
}


/*
============
GetBotLibAPI
============
*/
botlib_export_t *GetBotLibAPI(int apiVersion, botlib_import_t *import)
{
	assert(import);   // bk001129 - this wasn't set for baseq3/
	g_bimport = *import;
	assert(g_bimport.Print);   // bk001129 - pars pro toto

	Com_Memset( &g_bexport, 0, sizeof( g_bexport ) );

	if ( apiVersion != BOTLIB_API_VERSION ) {
		g_bimport.Print( PRT_ERROR, "Mismatched BOTLIB_API_VERSION: expected %i, got %i\n", BOTLIB_API_VERSION, apiVersion );
		return NULL;
	}

	Init_AAS_Export(&g_bexport.aas);
	Init_EA_Export(&g_bexport.ea);
	Init_AI_Export(&g_bexport.ai);

	g_bexport.BotLibSetup = Export_BotLibSetup;
	g_bexport.BotLibShutdown = Export_BotLibShutdown;

	g_bexport.PC_AddGlobalDefine = PC_AddGlobalDefine;
	g_bexport.PC_LoadSourceHandle = PC_LoadSourceHandle;
	g_bexport.PC_FreeSourceHandle = PC_FreeSourceHandle;
	g_bexport.PC_ReadTokenHandle = PC_ReadTokenHandle;
	g_bexport.PC_SourceFileAndLine = PC_SourceFileAndLine;

	g_bexport.BotLibStartFrame = Export_BotLibStartFrame;
	g_bexport.BotLibLoadMap = Export_BotLibLoadMap;
	g_bexport.BotLibUpdateEntity = Export_BotLibUpdateEntity;
	g_bexport.Test = Export_BotTest;

	return &g_bexport;
}
