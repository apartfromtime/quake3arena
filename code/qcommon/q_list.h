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

#ifndef Q_LIST_H
#define Q_LIST_H

typedef struct
{
    qboolean    frameMemory;
    int         currentElements;
    int         maxElements;            // will reallocate and move when exceeded
    void**      elements;
} growList_t;

// you don't need to init the growlist if you don't mind it growing and moving
// the list as it expands
void GrowList_Init(growList_t* list, int maxElements);
int GrowList_AddTo(growList_t* list, void* data);
void* GrowList_Element(const growList_t* list, int index);
int GrowList_IndexForElement(const growList_t* list, const void* element);

#endif // #ifndef Q_LIST_H