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
#include "q_dict.h"


/*
=====================================================================

  INFO STRINGS

=====================================================================
*/


/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
char *Info_ValueForKey(const char* s, const char* key)
{
    char	pkey[BIG_INFO_KEY];
    static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
    // work without stomping on each other
    static int valueindex = 0;
    char* o;

    if (!s || !key) {
        return "";
    }

    if ( strlen( s ) >= BIG_INFO_STRING ) {
        Com_Error( ERR_DROP, "Info_ValueForKey: oversize infostring" );
    }

    valueindex ^= 1;
    if (*s == '\\')
        s++;
    while (1)
    {
        o = pkey;
        while (*s != '\\')
        {
            if (!*s)
                return "";
            *o++ = *s++;
        }
        *o = 0;
        s++;

        o = value[valueindex];

        while (*s != '\\' && *s)
        {
            *o++ = *s++;
        }
        *o = 0;

        if (!Q_stricmp (key, pkey))
            return value[valueindex];

        if (!*s)
            break;
        s++;
    }

    return "";
}


/*
===================
Info_NextPair

Used to itterate through all the key/value pairs in an info string
===================
*/
void Info_NextPair(const char** head, char* key, char* value)
{
    char* o;
    const char* s;

    s = *head;

    if (*s == '\\') {
        s++;
    }

    key[0] = 0;
    value[0] = 0;

    o = key;
    while (*s != '\\') {
        if (!*s) {
            *o = 0;
            *head = s;
            return;
        }

        *o++ = *s++;
    }

    *o = 0;
    s++;

    o = value;
    while (*s != '\\' && *s) {
        *o++ = *s++;
    }

    *o = 0;

    *head = s;
}


/*
===================
Info_RemoveKey
===================
*/
void Info_RemoveKey(char* s, const char* key)
{
    char*   start;
    char	pkey[MAX_INFO_KEY];
    char	value[MAX_INFO_VALUE];
    char*   o;

    if (strlen(s) >= MAX_INFO_STRING) {
        Com_Error(ERR_DROP, "Info_RemoveKey: oversize infostring");
    }

    if (strchr (key, '\\')) {
        return;
    }

    while (1)
    {
        start = s;
        if (*s == '\\')
            s++;
        o = pkey;
        while (*s != '\\')
        {
            if (!*s)
                return;
            *o++ = *s++;
        }
        *o = 0;
        s++;

        o = value;
        while (*s != '\\' && *s)
        {
            if (!*s)
                return;
            *o++ = *s++;
        }
        *o = 0;

        if (!strcmp(key, pkey))
        {
            strcpy(start, s);           // remove this part
            return;
        }

        if (!*s)
            return;
    }

}

/*
===================
Info_RemoveKey_Big
===================
*/
void Info_RemoveKey_Big(char* s, const char* key)
{
    char*   start;
    char	pkey[BIG_INFO_KEY];
    char	value[BIG_INFO_VALUE];
    char*   o;

    if (strlen(s) >= BIG_INFO_STRING) {
        Com_Error(ERR_DROP, "Info_RemoveKey_Big: oversize infostring");
    }

    if (strchr(key, '\\')) {
        return;
    }

    while (1)
    {
        start = s;
        if (*s == '\\')
            s++;
        o = pkey;
        while (*s != '\\')
        {
            if (!*s)
                return;
            *o++ = *s++;
        }
        *o = 0;
        s++;

        o = value;
        while (*s != '\\' && *s)
        {
            if (!*s)
                return;
            *o++ = *s++;
        }
        *o = 0;

        if (!strcmp(key, pkey))
        {
            strcpy (start, s);          // remove this part
            return;
        }

        if (!*s)
            return;
    }

}

/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate(const char *s)
{
    if (strchr(s, '\"')) {
        return qfalse;
    }
    if (strchr(s, ';')) {
        return qfalse;
    }
    return qtrue;
}

/*
==================
Info_SetValueForKey

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey(char* s, const char* key, const char* value)
{
    char	newi[MAX_INFO_STRING];

    if (strlen(s) >= MAX_INFO_STRING) {
        Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
    }

    if (strchr(key, '\\') || strchr(value, '\\'))
    {
        Com_Printf ("Can't use keys or values with a \\\n");
        return;
    }

    if (strchr(key, ';') || strchr(value, ';'))
    {
        Com_Printf ("Can't use keys or values with a semicolon\n");
        return;
    }

    if (strchr(key, '\"') || strchr(value, '\"'))
    {
        Com_Printf ("Can't use keys or values with a \"\n");
        return;
    }

    Info_RemoveKey(s, key);
    if (!value || !strlen(value))
        return;

    Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, value);

    if (strlen(newi) + strlen(s) > MAX_INFO_STRING)
    {
        Com_Printf("Info string length exceeded\n");
        return;
    }

    strcat(newi, s);
    strcpy(s, newi);
}

/*
==================
Info_SetValueForKey_Big

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey_Big(char* s, const char* key, const char* value)
{
    char	newi[BIG_INFO_STRING];

    if (strlen(s) >= BIG_INFO_STRING) {
        Com_Error(ERR_DROP, "Info_SetValueForKey: oversize infostring");
    }

    if (strchr(key, '\\') || strchr(value, '\\'))
    {
        Com_Printf ("Can't use keys or values with a \\\n");
        return;
    }

    if (strchr(key, ';') || strchr(value, ';'))
    {
        Com_Printf ("Can't use keys or values with a semicolon\n");
        return;
    }

    if (strchr(key, '\"') || strchr(value, '\"'))
    {
        Com_Printf ("Can't use keys or values with a \"\n");
        return;
    }

    Info_RemoveKey_Big(s, key);
    if (!value || !strlen(value))
        return;

    Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, value);

    if (strlen(newi) + strlen(s) > BIG_INFO_STRING)
    {
        Com_Printf("BIG Info string length exceeded\n");
        return;
    }

    strcat(s, newi);
}

