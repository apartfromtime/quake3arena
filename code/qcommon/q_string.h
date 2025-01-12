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

#ifndef Q_STRING_H
#define Q_STRING_H

// TTimo
// vsnprintf is ISO/IEC 9899:1999
// abstracting this to make it portable
#ifdef WIN32
#define Q_vsnprintf _vsnprintf
#else
// TODO: do we need Mac define?
#define Q_vsnprintf vsnprintf
#endif

int Q_isprint(int c);
int Q_islower(int c);
int Q_isupper(int c);
int Q_isalpha(int c);

bool Q_strgtr(const char* s0, const char* s1);

// portable case insensitive compare
int Q_stricmp(const char *s1, const char *s2);
int Q_strncmp(const char *s1, const char *s2, int n);
int Q_stricmpn(const char *s1, const char *s2, int n);
char* Q_strlwr(char *s1);
char* Q_strupr(char *s1);
char* Q_strrchr(const char* string, int c);

// buffer size safe library replacements
void Q_strncpyz(char* dest, const char* src, int destsize);
void Q_strcat(char* dest, int size, const char* src);

void Q_CDECL Com_sprintf(char* dest, int size, const char* fmt, ...);
char* Q_CDECL va(char *format, ...);

// strlen that discounts Quake color sequences
int Q_PrintStrlen(const char* string);
// removes color sequences from string
char* Q_CleanStr(char* string);

int Com_HashString(const char* fname);
int Com_HashKey(char* string, int maxlen);
char* Com_SkipPath(char* pathname);
// it is ok for out == in
void Com_StripExtension(const char* in, char* out);
// "extension" should include the dot: ".map"
void Com_DefaultExtension(char* path, int maxSize, const char* extension);
int Com_Filter(const char* filter, const char* name, int casesensitive);
int Com_FilterPath(char* filter, char* name, int casesensitive);
const char* Com_StringContains(const char* str1, const char* str2, int casesensitive);

#endif // #ifndef Q_STRING_H
