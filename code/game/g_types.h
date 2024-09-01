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

#ifndef Q_GAME_TYPES_H
#define Q_GAME_TYPES_H

/*
=============================================================================

GAME

=============================================================================
*/

typedef enum _flag_status
{
    FLAG_ATBASE = 0,
    FLAG_TAKEN,             // CTF
    FLAG_TAKEN_RED,         // One Flag CTF
    FLAG_TAKEN_BLUE,        // One Flag CTF
    FLAG_DROPPED
} flagStatus_t;

#define SAY_ALL		0
#define SAY_TEAM	1
#define SAY_TELL	2

#endif // #ifndef Q_GAME_TYPES_H