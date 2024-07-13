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
#include "q_list.h"


/*
============================================================================

GROWLISTS

============================================================================
*/


void GrowList_Init(growList_t* list, int maxElements)
{
    list->maxElements = maxElements;
    list->currentElements = 0;
    list->elements = (void**)Com_Allocate(list->maxElements * sizeof(void*));
}

int GrowList_AddTo(growList_t* list, void* data)
{
    void** old;

    if (list->currentElements != list->maxElements) {
        list->elements[list->currentElements] = data;
        return list->currentElements++;
    }

    // grow, reallocate and move
    old = list->elements;

    if (list->maxElements < 0) {
        Com_Error(ERR_FATAL, "Com_AddToGrowList: maxElements = %i", list->maxElements);
    }

    if (list->maxElements == 0) {
        // initialize the list to hold 100 elements
        GrowList_Init(list, 100);
        return GrowList_AddTo(list, data);
    }

    list->maxElements *= 2;

    Com_DPrintf("Resizing growlist to %i maxElements\n", list->maxElements);

    list->elements = (void**)Com_Allocate(list->maxElements * sizeof(void*));

    if (!list->elements) {
        Com_Error(ERR_DROP, "Growlist alloc failed");
    }

    Com_Memcpy(list->elements, old, list->currentElements * sizeof(void*));

    Com_Dealloc(old);

    return GrowList_AddTo(list, data);
}

void* GrowList_Element(const growList_t* list, int index)
{
    if (index < 0 || index >= list->currentElements) {
        Com_Error(ERR_DROP, "Com_GrowListElement: %i out of range of %i",
            index, list->currentElements);
    }

    return list->elements[index];
}

int GrowList_IndexForElement(const growList_t* list, const void* element)
{
    int i;

    for (i = 0; i < list->currentElements; i++) {
        if (list->elements[i] == element) {
            return i;
        }
    }

    return -1;
}
