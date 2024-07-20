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

#ifndef Q_DICT_H
#define Q_DICT_H

//
// key / value info strings
//
char *Info_ValueForKey(const char* s, const char* key);
void Info_RemoveKey(char* s, const char* key);
void Info_RemoveKey_big(char* s, const char* key);
void Info_SetValueForKey(char* s, const char* key, const char* value);
void Info_SetValueForKey_Big(char* s, const char* key, const char* value);
qboolean Info_Validate(const char* s);
void Info_NextPair(const char** s, char* key, char* value);

#endif // #ifndef Q_DICT_H
