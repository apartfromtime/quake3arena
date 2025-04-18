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
#include "../ui/keycodes.h"

typedef struct
{
	char    binding[MAX_STRING_CHARS];
	int     repeats;			// if > 1, it is autorepeating
	bool    down;
} qkey_t;

extern	bool	key_overstrikeMode;
extern	qkey_t		keys[K_COUNT];

/*
==============================================================

Edit fields and command line history/completion

==============================================================
*/

#define	MAX_EDIT_LINE	256
typedef struct {
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
} field_t;

void Field_Clear(field_t* edit);
void Field_CompleteCommand(field_t* edit);
void Field_KeyDownEvent( field_t *edit, int key );
void Field_CharEvent( field_t *edit, int ch );
void Field_Draw( field_t *edit, int x, int y, int width, bool showCursor );
void Field_BigDraw( field_t *edit, int x, int y, int width, bool showCursor );

#define		COMMAND_HISTORY		32
extern	field_t	historyEditLines[COMMAND_HISTORY];

extern	field_t	g_consoleField;
extern	field_t	g_chatField;
extern	bool	anykeydown;
extern	bool	chat_team;
extern	int			chat_playerNum;

void Key_WriteBindings( qhandle_t f );
void Key_SetBinding( int keynum, const char *binding );
char *Key_GetBinding( int keynum );
bool Key_IsDown( int keynum );
bool Key_GetOverstrikeMode( void );
void Key_SetOverstrikeMode( bool state );
void Key_ClearStates( void );
int Key_GetKey(const char *binding);
