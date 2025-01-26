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
//
//
// gameinfo.c
//

#include "ui_local.h"


//
// arena and bot info
//

static char*    s_gameinfo_list[GAMEINFO_COUNT][MAX_GAMEINFO_LIST];
static int      s_gameinfo_size[GAMEINFO_COUNT];

#ifndef MISSIONPACK // bk001206
static int		ui_numSinglePlayerArenas;
static int		ui_numSpecialSinglePlayerArenas;
#endif

/*
===============
UI_LoadArenasFromFile
===============
*/
static
void UI_LoadArenasFromFile(char* arenaFile)
{
	char        buf[MAX_GAMEINFO_TEXT];
	int         len;
	qhandle_t   file;

	len = trap_FS_FOpenFile(arenaFile, &file, FS_READ);
	if (!file)
	{
		trap_Print(va(S_COLOR_RED "arena file not found: %s\n", arenaFile));
		return;
	}
	
	if (len >= MAX_GAMEINFO_TEXT)
	{
		trap_Print(va(S_COLOR_RED "arena file too large: %s is %i, max allowed is %i",
			arenaFile, len, MAX_GAMEINFO_TEXT));
		trap_FS_FCloseFile(file);
		
		return;
	}

	trap_FS_Read(buf, len, file);
	buf[len] = 0;
	trap_FS_FCloseFile(file);

	int count = s_gameinfo_size[GAMEINFO_ARENA];
	s_gameinfo_size[GAMEINFO_ARENA] += trap_ParseInfo(buf, (MAX_GAMEINFO_LIST - count),
		&s_gameinfo_list[GAMEINFO_ARENA][count]);
}

/*
===============
UI_LoadArenas
===============
*/
void UI_LoadArenas( void ) {
	int			numdirs;
	vmCvar_t	arenasFile;
	char		filename[128];
	char		dirlist[1024];
	char*		dirptr;
	int			i, n;
	int			dirlen;
	char		*type;

	s_gameinfo_size[GAMEINFO_ARENA] = 0;
	uiInfo.mapCount = 0;

	trap_Cvar_Register( &arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM );
	if( *arenasFile.string ) {
		UI_LoadArenasFromFile(arenasFile.string);
	}
	else {
		UI_LoadArenasFromFile("scripts/arenas.txt");
	}

	// get all arenas from .arena files
	numdirs = trap_FS_GetFileList("scripts", ".arena", dirlist, 1024 );
	dirptr  = dirlist;
	for (i = 0; i < numdirs; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		UI_LoadArenasFromFile(filename);
	}
	trap_Print( va( "%i arenas parsed\n", s_gameinfo_size[GAMEINFO_ARENA]) );
	if (UI_OutOfMemory()) {
		trap_Print(S_COLOR_YELLOW"WARNING: not anough memory in pool to load all arenas\n");
	}

	for( n = 0; n < s_gameinfo_size[GAMEINFO_ARENA]; n++ ) {
		// determine type

		uiInfo.mapList[uiInfo.mapCount].cinematic = -1;
		uiInfo.mapList[uiInfo.mapCount].mapLoadName = String_Alloc(Info_ValueForKey(s_gameinfo_list[GAMEINFO_ARENA][n], "map"));
		uiInfo.mapList[uiInfo.mapCount].mapName = String_Alloc(Info_ValueForKey(s_gameinfo_list[GAMEINFO_ARENA][n], "longname"));
		uiInfo.mapList[uiInfo.mapCount].levelShot = -1;
		uiInfo.mapList[uiInfo.mapCount].imageName = String_Alloc(va("levelshots/%s", uiInfo.mapList[uiInfo.mapCount].mapLoadName));
		uiInfo.mapList[uiInfo.mapCount].typeBits = 0;

		type = Info_ValueForKey(s_gameinfo_list[GAMEINFO_ARENA][n], "type" );
		// if no type specified, it will be treated as "ffa"
		if( *type ) {
			if( strstr( type, "ffa" ) ) {
				uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << GT_FFA);
			}
			if( strstr( type, "tourney" ) ) {
				uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << GT_TOURNAMENT);
			}
			if( strstr( type, "ctf" ) ) {
				uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << GT_CTF);
			}
			if( strstr( type, "oneflag" ) ) {
				uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << GT_1FCTF);
			}
			if( strstr( type, "overload" ) ) {
				uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << GT_OBELISK);
			}
			if( strstr( type, "harvester" ) ) {
				uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << GT_HARVESTER);
			}
		} else {
			uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << GT_FFA);
		}

		uiInfo.mapCount++;
		if (uiInfo.mapCount >= MAX_MAPS) {
			break;
		}
	}
}


/*
===============
UI_LoadBotsFromFile
===============
*/
static void
UI_LoadBotsFromFile(char* botFile)
{
	char        buf[MAX_GAMEINFO_TEXT];
	int         len;
	qhandle_t   file;

	len = trap_FS_FOpenFile(botFile, &file, FS_READ);
	if (!file)
	{
		trap_Print(va(S_COLOR_RED "bot file not found: %s\n", botFile));
		return;
	}

	if (len >= MAX_GAMEINFO_TEXT)
	{
		trap_Print(va(S_COLOR_RED "bot file too large: %s is %i, max allowed is %i",
			botFile, len, MAX_GAMEINFO_TEXT));
		trap_FS_FCloseFile(file);
		return;
	}

	trap_FS_Read(buf, len, file);
	buf[len] = 0;
	trap_FS_FCloseFile(file);

	Com_Compress(buf);

	int count = s_gameinfo_size[GAMEINFO_BOT];
	s_gameinfo_size[GAMEINFO_BOT] += trap_ParseInfo(buf, (MAX_GAMEINFO_LIST - count),
		&s_gameinfo_list[GAMEINFO_BOT][count]);
}

/*
===============
UI_LoadBots
===============
*/
void UI_LoadBots( void ) {
	vmCvar_t	botsFile;
	int			numdirs;
	char		filename[128];
	char		dirlist[1024];
	char*		dirptr;
	int			i;
	int			dirlen;

	s_gameinfo_size[GAMEINFO_BOT] = 0;

	trap_Cvar_Register( &botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM );
	if( *botsFile.string ) {
		UI_LoadBotsFromFile(botsFile.string);
	}
	else {
		UI_LoadBotsFromFile("scripts/bots.txt");
	}

	// get all bots from .bot files
	numdirs = trap_FS_GetFileList("scripts", ".bot", dirlist, 1024 );
	dirptr  = dirlist;
	for (i = 0; i < numdirs; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		UI_LoadBotsFromFile(filename);
	}
	trap_Print( va( "%i bots parsed\n", s_gameinfo_size[GAMEINFO_BOT] ) );
}


/*
===============
UI_GetBotInfoByNumber
===============
*/
char *UI_GetBotInfoByNumber( int num ) {
	if( num < 0 || num >= s_gameinfo_size[GAMEINFO_BOT] ) {
		trap_Print( va( S_COLOR_RED "Invalid bot number: %i\n", num ) );
		return NULL;
	}
	return s_gameinfo_list[GAMEINFO_BOT][num];
}


/*
===============
UI_GetBotInfoByName
===============
*/
char *UI_GetBotInfoByName( const char *name ) {
	int		n;
	char	*value;

	for ( n = 0; n < s_gameinfo_size[GAMEINFO_BOT]; n++ ) {
		value = Info_ValueForKey( s_gameinfo_list[GAMEINFO_BOT][n], "name" );
		if ( !Q_stricmp( value, name ) ) {
			return s_gameinfo_list[GAMEINFO_BOT][n];
		}
	}

	return NULL;
}

int UI_GetNumBots() {
	return s_gameinfo_size[GAMEINFO_BOT];
}


char *UI_GetBotNameByNumber( int num ) {
	char *info = UI_GetBotInfoByNumber(num);
	if (info) {
		return Info_ValueForKey( info, "name" );
	}
	return "Sarge";
}
