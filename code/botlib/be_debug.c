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

#include "../game/q_shared.h"
#include "../game/botlib.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../game/be_aas.h"

#include "l_libvar.h"
#include "aasfile.h"
#include "be_interface.h"
#include "be_aas_def.h"

static vec3_t s_goalorigin;
static int s_goalareanum;

void
Bot_DebugInit(void)
{
	VectorClear(s_goalorigin);
	s_goalareanum = 0;
}

void AAS_TestMovementPrediction(int entnum, vec3_t origin, vec3_t dir);
int BotGetReachabilityToGoal(vec3_t origin, int areanum, int lastgoalareanum,
	int lastareanum, int* avoidreach, float* avoidreachtimes, int* avoidreachtries,
	bot_goal_t* goal, int travelflags, int movetravelflags,
	struct bot_avoidspot_s* avoidspots, int numavoidspots, int* flags);
int BotFuzzyPointReachabilityArea(vec3_t origin);
float BotGapDistance(vec3_t origin, vec3_t hordir, int entnum);
void AAS_FloodAreas(vec3_t origin);

//===========================================================================
// Export_BotTest
//===========================================================================
int
Export_BotTest(int parm0, char* parm1, vec3_t parm2, vec3_t parm3)
{
#ifdef DEBUG
	static int area = -1;
	static int line[2];
	int newarea, i, highlightarea, flood;
	vec3_t eye, forward, right, end, origin;
	// int start_time, end_time;
	vec3_t mins = { -16, -16, -24 };
	vec3_t maxs = { 16, 16, 32 };

	if (!aasworld.loaded) return 0;

	for (i = 0; i < 2; i++) if (!line[i]) line[i] = botimport.DebugLineCreate();

	// AAS_ClearShownDebugLines();
	// if (AAS_AgainstLadder(parm2)) botimport.Print(PRT_MESSAGE, "against ladder\n");
	// BotOnGround(parm2, PRESENCE_NORMAL, 1, &newarea, &newarea);
	// botimport.Print(PRT_MESSAGE, "%f %f %f\n", parm2[0], parm2[1], parm2[2]);

	highlightarea = Botlib_CvarGetValue("bot_highlightarea");
	if (highlightarea > 0)
	{
		newarea = highlightarea;
	}
	else
	{
		VectorCopy(parm2, origin);
		origin[2] += 0.5;
		newarea = BotFuzzyPointReachabilityArea(origin);
	}

	botimport.Print(PRT_MESSAGE, "\rtravel time to goal (%d) = %d  ", s_goalareanum,
		AAS_AreaTravelTimeToGoalArea(newarea, origin, s_goalareanum, TFL_DEFAULT));
	
	if (newarea != area) {

		botimport.Print(PRT_MESSAGE, "origin = %f, %f, %f\n", origin[0], origin[1],
			origin[2]);
		area = newarea;
		botimport.Print(PRT_MESSAGE, "new area %d, cluster %d, presence type %d\n",
			area, AAS_AreaCluster(area), AAS_PointPresenceType(origin));
		
		botimport.Print(PRT_MESSAGE, "area contents: ");
		if (aasworld.areasettings[area].contents & AREACONTENTS_WATER)
		{
			botimport.Print(PRT_MESSAGE, "water &");
		}

		if (aasworld.areasettings[area].contents & AREACONTENTS_LAVA)
		{
			botimport.Print(PRT_MESSAGE, "lava &");
		}

		if (aasworld.areasettings[area].contents & AREACONTENTS_SLIME)
		{
			botimport.Print(PRT_MESSAGE, "slime &");
		}

		if (aasworld.areasettings[area].contents & AREACONTENTS_JUMPPAD)
		{
			botimport.Print(PRT_MESSAGE, "jump pad &");
		}

		if (aasworld.areasettings[area].contents & AREACONTENTS_CLUSTERPORTAL)
		{
			botimport.Print(PRT_MESSAGE, "cluster portal &");
		}

		if (aasworld.areasettings[area].contents & AREACONTENTS_VIEWPORTAL)
		{
			botimport.Print(PRT_MESSAGE, "view portal &");
		}

		if (aasworld.areasettings[area].contents & AREACONTENTS_DONOTENTER)
		{
			botimport.Print(PRT_MESSAGE, "do not enter &");
		}

		if (aasworld.areasettings[area].contents & AREACONTENTS_MOVER)
		{
			botimport.Print(PRT_MESSAGE, "mover &");
		}

		if (!aasworld.areasettings[area].contents)
		{
			botimport.Print(PRT_MESSAGE, "empty");
		}

		botimport.Print(PRT_MESSAGE, "\n");
		botimport.Print(PRT_MESSAGE, "travel time to goal (%d) = %d\n", s_goalareanum,
			AAS_AreaTravelTimeToGoalArea(newarea, origin, s_goalareanum, TFL_DEFAULT |
				TFL_ROCKETJUMP));
	}
	
	flood = Botlib_CvarGetValue("bot_flood");
	if (parm0 & 1)
	{
		if (flood)
		{
			AAS_ClearShownPolygons();
			AAS_ClearShownDebugLines();
			AAS_FloodAreas(parm2);
		}
		else
		{
			s_goalareanum = newarea;
			VectorCopy(parm2, s_goalorigin);
			botimport.Print(PRT_MESSAGE, "new goal %2.1f %2.1f %2.1f area %d\n",
				origin[0], origin[1], origin[2], newarea);
		}
	}

	if (flood)
		return 0;
	
	AAS_ClearShownPolygons();
	AAS_ClearShownDebugLines();
	AAS_ShowAreaPolygons(newarea, 1, parm0 & 4);
	
	if (parm0 & 2) AAS_ShowReachableAreas(area);
	else
	{
		static int lastgoalareanum, lastareanum;
		static int avoidreach[MAX_AVOIDREACH];
		static float avoidreachtimes[MAX_AVOIDREACH];
		static int avoidreachtries[MAX_AVOIDREACH];
		int reachnum, resultFlags;
		bot_goal_t goal;
		aas_reachability_t reach;
		int curarea;
		vec3_t curorigin;

		goal.areanum = s_goalareanum;
		VectorCopy(s_goalorigin, goal.origin);
		VectorCopy(origin, curorigin);
		curarea = newarea;
		
		for (i = 0; i < 100; i++)
		{	
			if (curarea == goal.areanum) {
				break;
			}
			
			reachnum = BotGetReachabilityToGoal(curorigin, curarea,
				lastgoalareanum, lastareanum,
				avoidreach, avoidreachtimes, avoidreachtries,
				&goal, TFL_DEFAULT | TFL_FUNCBOB | TFL_ROCKETJUMP, TFL_DEFAULT |
				TFL_FUNCBOB | TFL_ROCKETJUMP, NULL, 0, &resultFlags);
			AAS_ReachabilityFromNum(reachnum, &reach);
			AAS_ShowReachability(&reach);
			VectorCopy(reach.end, origin);
			lastareanum = curarea;
			curarea = reach.areanum;
		}
	}
	
	VectorClear(forward);
	// BotGapDistance(origin, forward, 0);

	AngleVectors(parm3, forward, right, NULL);
	// get the eye 16 units to the right of the origin
	VectorMA(parm2, 8, right, eye);
	// get the eye 24 units up
	eye[2] += 24;
	// get the end point for the line to be traced
	VectorMA(eye, 800, forward, end);

	// AAS_TestMovementPrediction(1, parm2, forward);

#if 0
	AAS_ClearShownDebugLines();
	bsptrace = AAS_Trace(eye, mins, maxs, end, 1, MASK_PLAYERSOLID);
	
	if (!line[0]) line[0] = botimport.DebugLineCreate();
	
	botimport.DebugLineShow(line[0], eye, bsptrace.endpos, LINECOLOR_YELLOW);
	
	if (bsptrace.fraction < 1.0)
	{
		face = AAS_TraceEndFace(&trace);
		if (face)
		{
			AAS_ShowFace(face - aasworld.faces);
		}

		AAS_DrawPlaneCross(bsptrace.endpos,
			bsptrace.plane.normal,
			bsptrace.plane.dist + bsptrace.exp_dist,
			bsptrace.plane.type, LINECOLOR_GREEN);
		if (trace.ent)
		{
			ent = &aasworld.entities[trace.ent];
			AAS_ShowBoundingBox(ent->origin, ent->mins, ent->maxs);
		}
	}

	bsptrace = AAS_Trace2(eye, mins, maxs, end, 1, MASK_PLAYERSOLID);
	botimport.DebugLineShow(line[1], eye, bsptrace.endpos, LINECOLOR_BLUE);
	
	if (bsptrace.fraction < 1.0)
	{
		AAS_DrawPlaneCross(bsptrace.endpos,
			bsptrace.plane.normal,
			bsptrace.plane.dist,// + bsptrace.exp_dist,
			bsptrace.plane.type, LINECOLOR_RED);
		if (bsptrace.ent)
		{
			ent = &aasworld.entities[bsptrace.ent];
			AAS_ShowBoundingBox(ent->origin, ent->mins, ent->maxs);
		}
	}
#endif
#endif

	return 0;
}