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

#ifndef Q_CIN_TYPES_H
#define Q_CIN_TYPES_H

/*
=============================================================================

VIDEO AND CINEMATIC

=============================================================================
*/

#define CIN_system	1
#define CIN_loop	2
#define	CIN_hold	4
#define CIN_silent	8
#define CIN_shader	16

// cinematic states
typedef enum
{
    FMV_IDLE,
    FMV_PLAY,           // play
    FMV_EOF,            // all other conditions, i.e. stop/EOF/abort
    FMV_ID_BLT,
    FMV_ID_IDLE,
    FMV_LOOPED,
    FMV_ID_WAIT
} e_status;

#endif // #ifndef Q_CIN_TYPES_H