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
 * name:		l_libvar.c
 *
 * desc:		bot library variables
 *
 * $Archive: /MissionPack/code/botlib/l_libvar.c $
 *
 *****************************************************************************/

#include "../game/q_shared.h"
#include "../game/botlib.h"
#include "be_interface.h"
#include "l_libvar.h"

// gets the string of the library variable with the given name
char* Botlib_CvarGetString(char* var_name)
{
	return botimport.Cvar_VariableString(var_name);
}

// gets the value of the library variable with the given name
float Botlib_CvarGetValue(char* var_name)
{
	return botimport.Cvar_VariableValue(var_name);
}

// creates the library variable if not existing already and returns it
cvar_t* Botlib_CvarGet(char* var_name, char* value)
{
	return botimport.Cvar_Get(var_name, value, 0);
}

// sets the library variable
void Botlib_CvarSet(char* var_name, char* value)
{
	botimport.Cvar_Set(var_name, value);
}
