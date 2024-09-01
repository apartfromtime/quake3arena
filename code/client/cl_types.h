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

#ifndef Q_CL_TYPES_H
#define Q_CL_TYPES_H

/*
=============================================================================

CLIENT

=============================================================================
*/

// print levels from renderer (FIXME: set up for game / cgame?)
typedef enum
{
    PRINT_ALL,
    PRINT_DEVELOPER,        // only print when "developer 1"
    PRINT_WARNING,
    PRINT_ERROR
} printParm_t;

typedef enum
{
    CA_UNINITIALIZED,
    CA_DISCONNECTED,        // not talking to a server
    CA_AUTHORIZING,         // not used any more, was checking cd key 
    CA_CONNECTING,          // sending request packets to the server
    CA_CHALLENGING,         // sending challenge packets to the server
    CA_CONNECTED,           // netchan_t established, getting gamestate
    CA_LOADING,             // only during cgame initialization, never during main loop
    CA_PRIMED,              // got gamestate, waiting for first frame
    CA_ACTIVE,              // game views should be displayed
    CA_CINEMATIC            // playing a cinematic or a static pic, not connected to a server
} connstate_t;

#endif // #ifndef Q_CL_TYPES_H