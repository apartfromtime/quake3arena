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
#include "client.h"

/* key up events are sent even if in console mode */
field_t	historyEditLines[COMMAND_HISTORY];

int nextHistoryLine;			// the last line in the history buffer, not masked
int historyLine;			// the line being displayed from history buffer will be <= nextHistoryLine

field_t g_consoleField;
field_t g_chatField;
bool chat_team;
int chat_playerNum;
bool key_overstrikeMode;
bool anykeydown;

qkey_t keys[K_COUNT];

typedef struct
{
	char*	name;
	int		keynum;
} keyname_t;

// names not in this list are '0xnn' hex sequences
keyname_t keynames[] =
{
	{ "A",			K_A },
	{ "B",			K_B },
	{ "C",			K_C },
	{ "D",			K_D },
	{ "E",			K_E },
	{ "F",			K_F },
	{ "G",			K_G },
	{ "H",			K_H },
	{ "I",			K_I },
	{ "J",			K_J },
	{ "K",			K_K },
	{ "L",			K_L },
	{ "M",			K_M },
	{ "N",			K_N },
	{ "O",			K_O },
	{ "P",			K_P },
	{ "Q",			K_Q },
	{ "R",			K_R },
	{ "S",			K_S },
	{ "T",			K_T },
	{ "U",			K_U },
	{ "V",			K_V },
	{ "W",			K_W },
	{ "X",			K_X },
	{ "Y",			K_Y },
	{ "Z",			K_Z },
	{ "1",			K_1 },
	{ "2",			K_2 },
	{ "3",			K_3 },
	{ "4",			K_4 },
	{ "5",			K_5 },
	{ "6",			K_6 },
	{ "7",			K_7 },
	{ "8",			K_8 },
	{ "9",			K_9 },
	{ "0",			K_0 },
	
	{ "ENTER",		K_RETURN },
	{ "ESCAPE",		K_ESCAPE },
	{ "BACKSPACE",	K_BACKSPACE },
	{ "TAB",		K_TAB },
	{ "SPACE",		K_SPACE },

	{ "_",			K_MINUS },
	{ "-",			K_MINUS },
	{ "+",			K_EQUALS },
	{ "=",			K_EQUALS },
	{ "[",			K_LEFTBRACKET },
	{ "]",			K_RIGHTBRACKET },
	{ "\\",			K_BACKSLASH },
	{ "SEMICOLON",	K_SEMICOLON },			// because a raw semicolon seperates commands
	{ "'",			K_APOSTROPHE },
	{ "`",			K_GRAVE },
	{ "~",			K_GRAVE },
	{ ",",			K_COMMA },
	{ ".",			K_PERIOD },
	{ "/",			K_SLASH },

	{ "CAPSLOCK",	K_CAPSLOCK },

	{ "F1",			K_F1 },
	{ "F2",			K_F2 },
	{ "F3",			K_F3 },
	{ "F4",			K_F4 },
	{ "F5",			K_F5 },
	{ "F6",			K_F6 },
	{ "F7",			K_F7 },
	{ "F8",			K_F8 },
	{ "F9",			K_F9 },
	{ "F10",		K_F10 },
	{ "F11",		K_F11 },
	{ "F12",		K_F12 },

	{ "PRINTSCREEN",K_PRINTSCREEN },
	{ "SCROLLLOCK",	K_SCROLLLOCK },
	{ "PAUSE",		K_PAUSE },
	{ "INSERT",		K_INSERT },
 
	{ "HOME",		K_HOME },
	{ "PAGEUP",		K_PAGEUP },
	{ "DELETE",		K_DELETE },
	{ "END",		K_END },
	{ "PAGEDN",		K_PAGEDOWN },
	{ "RIGHT",		K_RIGHT },
	{ "LEFT",		K_LEFT },
	{ "DOWN",		K_DOWN },
	{ "UP",			K_UP },

	{ "KP_DIVIDE",	K_KP_DIVIDE },
	{ "KP_MULTIPLY",K_KP_MULTIPLY },
	{ "KP_MINUS",	K_KP_MINUS },
	{ "KP_PLUS",	K_KP_PLUS },
	{ "KP_ENTER",	K_KP_ENTER },
	{ "KP_1",		K_KP_1 },
	{ "KP_2",		K_KP_2 },
	{ "KP_3",		K_KP_3 },
	{ "KP_4",		K_KP_4 },
	{ "KP_5",		K_KP_5 },
	{ "KP_6",		K_KP_6 },
	{ "KP_7",		K_KP_7 },
	{ "KP_8",		K_KP_8 },
	{ "KP_9",		K_KP_9 },
	{ "KP_0",		K_KP_0 },
	{ "KP_PERIOD",	K_KP_PERIOD },
	{ "KP_EQUALS",	K_KP_EQUALS },

	{ "LCTRL",		K_LCTRL },
	{ "LSHIFT",		K_LSHIFT },
	{ "LALT",		K_LALT },
	{ "LGUI",		K_LGUI },
	{ "RCTRL",		K_RCTRL },
	{ "RSHIFT",		K_RSHIFT },
	{ "RALT",		K_RALT },
	{ "RGUI",		K_RGUI },

	{ "MOUSE1",		K_MOUSE1 },
	{ "MOUSE2",		K_MOUSE2 },
	{ "MOUSE3",		K_MOUSE3 },
	{ "MOUSE4",		K_MOUSE4 },
	{ "MOUSE5",		K_MOUSE5 },

	{ "MWHEELUP",	K_MOUSE_WHEELUP },
	{ "MWHEELDOWN",	K_MOUSE_WHEELDOWN },

	{ "BUTTON_SOUTH",			K_GAMEPAD_BUTTON_SOUTH },
	{ "BUTTON_EAST",			K_GAMEPAD_BUTTON_EAST },
	{ "BUTTON_WEST",			K_GAMEPAD_BUTTON_WEST },
	{ "BUTTON_NORTH",			K_GAMEPAD_BUTTON_NORTH },
	{ "BUTTON_BACK",			K_GAMEPAD_BUTTON_BACK },
	{ "BUTTON_GUIDE",			K_GAMEPAD_BUTTON_GUIDE },
	{ "BUTTON_START",			K_GAMEPAD_BUTTON_START },
	{ "BUTTON_LEFT_STICK",		K_GAMEPAD_BUTTON_LEFT_STICK },
	{ "BUTTON_RIGHT_STICK",		K_GAMEPAD_BUTTON_RIGHT_STICK },
	{ "BUTTON_LEFT_SHOULDER",	K_GAMEPAD_BUTTON_LEFT_SHOULDER },
	{ "BUTTON_RIGHT_SHOULDER",	K_GAMEPAD_BUTTON_RIGHT_SHOULDER },
	{ "BUTTON_DPAD_UP",			K_GAMEPAD_BUTTON_DPAD_UP },
	{ "BUTTON_DPAD_DOWN",		K_GAMEPAD_BUTTON_DPAD_DOWN },
	{ "BUTTON_DPAD_LEFT",		K_GAMEPAD_BUTTON_DPAD_LEFT },
	{ "BUTTON_DPAD_RIGHT",		K_GAMEPAD_BUTTON_DPAD_RIGHT },
	{ "BUTTON_MISC1",			K_GAMEPAD_BUTTON_MISC1 },
	{ "BUTTON_RIGHT_PADDLE1",	K_GAMEPAD_BUTTON_RIGHT_PADDLE1 },
	{ "BUTTON_LEFT_PADDLE1",	K_GAMEPAD_BUTTON_LEFT_PADDLE1 },
	{ "BUTTON_RIGHT_PADDLE2",	K_GAMEPAD_BUTTON_RIGHT_PADDLE2 },
	{ "BUTTON_LEFT_PADDLE2",	K_GAMEPAD_BUTTON_LEFT_PADDLE2 },
	{ "BUTTON_TOUCHPAD",		K_GAMEPAD_BUTTON_TOUCHPAD },
	{ "BUTTON_MISC2",			K_GAMEPAD_BUTTON_MISC2 },
	{ "BUTTON_MISC3",			K_GAMEPAD_BUTTON_MISC3 },
	{ "BUTTON_MISC4",			K_GAMEPAD_BUTTON_MISC4 },
	{ "BUTTON_MISC5",			K_GAMEPAD_BUTTON_MISC5 },
	{ "BUTTON_MISC6",			K_GAMEPAD_BUTTON_MISC6 },
	{ "BUTTON_LEFT_TRIGGER",	K_GAMEPAD_BUTTON_LEFT_TRIGGER },
	{ "BUTTON_RIGHT_TRIGGER",	K_GAMEPAD_BUTTON_RIGHT_TRIGGER },

	{ "JOY1",		K_JOY1 },
	{ "JOY2",		K_JOY2 },
	{ "JOY3",		K_JOY3 },
	{ "JOY4",		K_JOY4 },
	{ "JOY5",		K_JOY5 },
	{ "JOY6",		K_JOY6 },
	{ "JOY7",		K_JOY7 },
	{ "JOY8",		K_JOY8 },
	{ "JOY9",		K_JOY9 },
	{ "JOY10",		K_JOY10 },
	{ "JOY11",		K_JOY11 },
	{ "JOY12",		K_JOY12 },
	{ "JOY13",		K_JOY13 },
	{ "JOY14",		K_JOY14 },
	{ "JOY15",		K_JOY15 },
	{ "JOY16",		K_JOY16 },
	{ "JOY17",		K_JOY17 },
	{ "JOY18",		K_JOY18 },
	{ "JOY19",		K_JOY19 },
	{ "JOY20",		K_JOY20 },
	{ "JOY21",		K_JOY21 },
	{ "JOY22",		K_JOY22 },
	{ "JOY23",		K_JOY23 },
	{ "JOY24",		K_JOY24 },
	{ "JOY25",		K_JOY25 },
	{ "JOY26",		K_JOY26 },

	{NULL,0}
};

/*
=============================================================================

EDIT FIELDS

=============================================================================
*/


/*
===================
Field_Draw

Handles horizontal scrolling and cursor blinking
x, y, amd width are in pixels
===================
*/
void Field_VariableSizeDraw( field_t *edit, int x, int y, int width, int size, bool showCursor ) {
	int		len;
	int		drawLen;
	int		prestep;
	int		cursorChar;
	char	str[MAX_STRING_CHARS];
	int		i;

	drawLen = edit->widthInChars;
	len = strlen( edit->buffer ) + 1;

	// guarantee that cursor will be visible
	if ( len <= drawLen ) {
		prestep = 0;
	} else {
		if ( edit->scroll + drawLen > len ) {
			edit->scroll = len - drawLen;
			if ( edit->scroll < 0 ) {
				edit->scroll = 0;
			}
		}
		prestep = edit->scroll;

/*
		if ( edit->cursor < len - drawLen ) {
			prestep = edit->cursor;	// cursor at start
		} else {
			prestep = len - drawLen;
		}
*/
	}

	if ( prestep + drawLen > len ) {
		drawLen = len - prestep;
	}

	// extract <drawLen> characters from the field at <prestep>
	if ( drawLen >= MAX_STRING_CHARS ) {
		Com_Error( ERR_DROP, "drawLen >= MAX_STRING_CHARS" );
	}

	Com_Memcpy( str, edit->buffer + prestep, drawLen );
	str[ drawLen ] = 0;

	// draw it
	if ( size == SMALLCHAR_WIDTH ) {
		float	color[4];

		color[0] = color[1] = color[2] = color[3] = 1.0;
		SCR_DrawSmallStringExt( x, y, str, color, false );
	} else {
		// draw big string with drop shadow
		SCR_DrawBigString( x, y, str, 1.0 );
	}

	// draw the cursor
	if ( !showCursor ) {
		return;
	}

	if ( (int)( cls.realtime >> 8 ) & 1 ) {
		return;		// off blink
	}

	if ( key_overstrikeMode ) {
		cursorChar = 11;
	} else {
		cursorChar = 10;
	}

	i = drawLen - ( Q_PrintStrlen( str ) + 1 );

	if ( size == SMALLCHAR_WIDTH ) {
		SCR_DrawSmallChar( x + ( edit->cursor - prestep - i ) * size, y, cursorChar );
	} else {
		str[0] = cursorChar;
		str[1] = 0;
		SCR_DrawBigString( x + ( edit->cursor - prestep - i ) * size, y, str, 1.0 );

	}
}

void Field_Draw( field_t *edit, int x, int y, int width, bool showCursor ) 
{
	Field_VariableSizeDraw( edit, x, y, width, SMALLCHAR_WIDTH, showCursor );
}

void Field_BigDraw( field_t *edit, int x, int y, int width, bool showCursor ) 
{
	Field_VariableSizeDraw( edit, x, y, width, BIGCHAR_WIDTH, showCursor );
}

/*
================
Field_Paste
================
*/
void Field_Paste( field_t *edit ) {
	char	*cbd;
	int		pasteLen, i;

	cbd = Sys_GetClipboardData();

	if ( !cbd ) {
		return;
	}

	// send as if typed, so insert / overstrike works properly
	pasteLen = strlen( cbd );
	for ( i = 0 ; i < pasteLen ; i++ ) {
		Field_CharEvent( edit, cbd[i] );
	}

	Z_Free( cbd );
}

/*
=================
Field_KeyDownEvent

Performs the basic line editing functions for the console,
in-game talk, and menu fields

Key events are used for non-printable characters, others are gotten from char events.
=================
*/
void Field_KeyDownEvent(field_t* edit, int key)
{
	int		len;

	// shift-insert is paste
	if (((key == K_INSERT) || (key == K_KP_0)) && (keys[K_LSHIFT].down || keys[K_RSHIFT].down)) {
		
		Field_Paste(edit);
		
		return;
	}

	len = strlen(edit->buffer);

	if (key == K_BACKSPACE) {
		if (edit->cursor <= len && len != 0) {
			memmove(edit->buffer + (edit->cursor - 1),
				edit->buffer + edit->cursor, len - (edit->cursor - 1));
			edit->cursor--;
		}

		return;
	}

	if (key == K_DELETE) {
		if (edit->cursor < len) {
			memmove(edit->buffer + edit->cursor,
				edit->buffer + edit->cursor + 1, len - edit->cursor);
		}

		return;
	}

	if (key == K_RIGHT || key == K_KP_6) {
		if (edit->cursor < len) {
			edit->cursor++;
		}

		if (edit->cursor >= edit->scroll + edit->widthInChars && edit->cursor <= len) {
			edit->scroll++;
		}

		return;
	}

	if (key == K_LEFT || key == K_KP_4) {
		if (edit->cursor > 0) {
			edit->cursor--;
		}

		if (edit->cursor < edit->scroll) {
			edit->scroll--;
		}
		return;
	}

	if (key == K_HOME || (tolower(key) == 'a' && (keys[K_LCTRL].down || keys[K_RCTRL].down))) {
		edit->cursor = 0;
		return;
	}

	if (key == K_END || (tolower(key) == 'e' && (keys[K_LCTRL].down || keys[K_RCTRL].down))) {
		edit->cursor = len;
		return;
	}

	if (key == K_INSERT) {
		key_overstrikeMode = !key_overstrikeMode;
		return;
	}
}

/*
==================
Field_CharEvent
==================
*/
void Field_CharEvent( field_t *edit, int ch ) {
	int		len;

	if ( ch == 'v' - 'a' + 1 ) {	// ctrl-v is paste
		Field_Paste( edit );
		return;
	}

	if ( ch == 'c' - 'a' + 1 ) {	// ctrl-c clears the field
		Field_Clear( edit );
		return;
	}

	len = strlen( edit->buffer );

	if ( ch == 'h' - 'a' + 1 )	{	// ctrl-h is backspace
		if ( edit->cursor > 0 ) {
			memmove( edit->buffer + edit->cursor - 1, 
				edit->buffer + edit->cursor, len + 1 - edit->cursor );
			edit->cursor--;
			if ( edit->cursor < edit->scroll )
			{
				edit->scroll--;
			}
		}
		return;
	}

	if ( ch == 'a' - 'a' + 1 ) {	// ctrl-a is home
		edit->cursor = 0;
		edit->scroll = 0;
		return;
	}

	if ( ch == 'e' - 'a' + 1 ) {	// ctrl-e is end
		edit->cursor = len;
		edit->scroll = edit->cursor - edit->widthInChars;
		return;
	}

	//
	// ignore any other non printable chars
	//
	if ( ch < 32 ) {
		return;
	}

	if ( key_overstrikeMode ) {	
		if ( edit->cursor == MAX_EDIT_LINE - 1 )
			return;
		edit->buffer[edit->cursor] = ch;
		edit->cursor++;
	} else {	// insert mode
		if ( len == MAX_EDIT_LINE - 1 ) {
			return; // all full
		}
		memmove( edit->buffer + edit->cursor + 1, 
			edit->buffer + edit->cursor, len + 1 - edit->cursor );
		edit->buffer[edit->cursor] = ch;
		edit->cursor++;
	}


	if ( edit->cursor >= edit->widthInChars ) {
		edit->scroll++;
	}

	if ( edit->cursor == len + 1) {
		edit->buffer[edit->cursor] = 0;
	}
}

/*
=============================================================================

CONSOLE LINE EDITING

==============================================================================
*/

/*
====================
Console_Key

Handles history and console scrollback
====================
*/
void Console_Key(int key) {
	// ctrl-L clears screen
	if (key == 'l' && (keys[K_LCTRL].down || keys[K_RCTRL].down)) {
		Cbuf_AddText("clear\n");
		return;
	}

	// enter finishes the line
	if (key == K_RETURN || key == K_KP_ENTER) {
		// if not in the game explicitly prepent a slash if needed
		if (cls.state != CA_ACTIVE && g_consoleField.buffer[0] != '\\'
			&& g_consoleField.buffer[0] != '/') {
			char	temp[MAX_STRING_CHARS];

			Q_strncpyz(temp, g_consoleField.buffer, sizeof(temp));
			Com_sprintf(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\\%s", temp);
			g_consoleField.cursor++;
		}

		Com_Printf("]%s\n", g_consoleField.buffer);

		// leading slash is an explicit command
		if (g_consoleField.buffer[0] == '\\' || g_consoleField.buffer[0] == '/') {
			Cbuf_AddText(g_consoleField.buffer + 1);	// valid command
			Cbuf_AddText("\n");
		}
		else {
			// other text will be chat messages
			if (!g_consoleField.buffer[0]) {
				return;	// empty lines just scroll the console without adding to history
			}
			else {
				Cbuf_AddText("cmd say ");
				Cbuf_AddText(g_consoleField.buffer);
				Cbuf_AddText("\n");
			}
		}

		// copy line to history buffer
		historyEditLines[nextHistoryLine % COMMAND_HISTORY] = g_consoleField;
		nextHistoryLine++;
		historyLine = nextHistoryLine;

		Field_Clear(&g_consoleField);

		g_consoleField.widthInChars = g_console_field_width;

		if (cls.state == CA_DISCONNECTED) {
			SCR_UpdateScreen();	// force an update, because the command
		}							// may take some time
		return;
	}

	// command completion

	if (key == K_TAB) {
		Field_CompleteCommand(&g_consoleField);
		return;
	}

	// command history (ctrl-p ctrl-n for unix style)
	if ((key == K_MOUSE_WHEELUP && (keys[K_LSHIFT].down || keys[K_RSHIFT].down)) ||
		(key == K_UP) || (key == K_KP_8) ||
		((tolower(key) == 'p') && (keys[K_LCTRL].down || keys[K_RCTRL].down))) {
		if (nextHistoryLine - historyLine < COMMAND_HISTORY
			&& historyLine > 0) {
			historyLine--;
		}
		g_consoleField = historyEditLines[historyLine % COMMAND_HISTORY];
		return;
	}

	if ((key == K_MOUSE_WHEELDOWN && (keys[K_LSHIFT].down || keys[K_RSHIFT].down)) ||
		(key == K_DOWN) || (key == K_KP_3) ||
		((tolower(key) == 'n') && (keys[K_LCTRL].down || keys[K_RCTRL].down))) {
		if (historyLine == nextHistoryLine)
			return;
		historyLine++;
		g_consoleField = historyEditLines[historyLine % COMMAND_HISTORY];
		return;
	}

	// console scrolling
	if (key == K_PAGEUP || key == K_KP_9) {
		Con_PageUp();
		return;
	}

	if (key == K_PAGEDOWN || key == K_KP_3) {
		Con_PageDown();
		return;
	}

	if (key == K_MOUSE_WHEELUP) {	//----(SA)	added some mousewheel functionality to the console
		Con_PageUp();
		if ((keys[K_LCTRL].down || keys[K_RCTRL].down)) {	// hold <ctrl> to accelerate scrolling
			Con_PageUp();
			Con_PageUp();
		}
		return;
	}

	if (key == K_MOUSE_WHEELDOWN) {	//----(SA)	added some mousewheel functionality to the console
		Con_PageDown();
		if ((keys[K_LCTRL].down || keys[K_RCTRL].down)) {	// hold <ctrl> to accelerate scrolling
			Con_PageDown();
			Con_PageDown();
		}
		return;
	}

	// ctrl-home = top of console
	if ((key == K_HOME || key == K_KP_7) && (keys[K_LCTRL].down || keys[K_RCTRL].down)) {
		Con_Top();
		return;
	}

	// ctrl-end = bottom of console
	if ((key == K_END || key == K_KP_1) && (keys[K_LCTRL].down || keys[K_RCTRL].down)) {
		Con_Bottom();
		return;
	}

	// pass to the normal editline routine
	Field_KeyDownEvent(&g_consoleField, key);
}

//============================================================================


/*
================
Message_Key

In game talk message
================
*/
void Message_Key( int key ) {

	char	buffer[MAX_STRING_CHARS];


	if (key == K_ESCAPE) {
		cls.keyCatchers &= ~KEYCATCH_MESSAGE;
		Field_Clear( &g_chatField );
		return;
	}

	if ( key == K_RETURN || key == K_KP_ENTER )
	{
		if ( g_chatField.buffer[0] && cls.state == CA_ACTIVE ) {
			if (chat_playerNum != -1 )

				Com_sprintf( buffer, sizeof( buffer ), "tell %i \"%s\"\n", chat_playerNum, g_chatField.buffer );

			else if (chat_team)

				Com_sprintf( buffer, sizeof( buffer ), "say_team \"%s\"\n", g_chatField.buffer );
			else
				Com_sprintf( buffer, sizeof( buffer ), "say \"%s\"\n", g_chatField.buffer );



			CL_AddReliableCommand( buffer );
		}
		cls.keyCatchers &= ~KEYCATCH_MESSAGE;
		Field_Clear( &g_chatField );
		return;
	}

	Field_KeyDownEvent( &g_chatField, key );
}

//============================================================================


bool Key_GetOverstrikeMode( void ) {
	return key_overstrikeMode;
}


void Key_SetOverstrikeMode( bool state ) {
	key_overstrikeMode = state;
}


/*
===================
Key_IsDown
===================
*/
bool Key_IsDown(int keynum)
{
	if (keynum < 0 || keynum >= K_COUNT) {
		return false;
	}

	return keys[keynum].down;
}

/*
===================
Key_StringToKeynum

Returns a key number to be used to index keys[] by looking at
the given string.  Single ascii characters return themselves, while
the K_* names are matched up.

0x11 will be interpreted as raw hex, which will allow new controlers

to be configured even if they don't have defined names.
===================
*/
int Key_StringToKeynum(char* str)
{
	keyname_t* kn;

	if (!str || !str[0]) {
		return -1;
	}

	// scan for a text match
	for (kn = keynames; kn->name; kn++)
	{
		if (!Q_stricmp(str, kn->name)) {
			return kn->keynum;
		}
	}

	// check for hex code
	if (str[0] == '0' && str[1] == 'x' && strlen(str) == 4) {
		
		int n1 = 0, n2 = 0;

		n1 = str[2];
		if (n1 >= '0' && n1 <= '9') {
			n1 -= '0';
		} else if (n1 >= 'a' && n1 <= 'f') {
			n1 = n1 - 'a' + 10;
		} else {
			n1 = 0;
		}

		n2 = str[3];
		if (n2 >= '0' && n2 <= '9') {
			n2 -= '0';
		} else if (n2 >= 'a' && n2 <= 'f') {
			n2 = n2 - 'a' + 10;
		} else {
			n2 = 0;
		}

		return n1 * 16 + n2;
	}

	return -1;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, a K_* name, or a 0x11 hex string) for the
given keynum.
===================
*/
char* Key_KeynumToString(int keynum)
{
	keyname_t* kn;
	static char	tinystr[5];
	int			i, j;

	if (keynum == -1) {
		return "<KEY NOT FOUND>";
	}

	if (keynum < 0 || keynum >= K_COUNT) {
		return "<OUT OF RANGE>";
	}

	// check for a key string
	for (kn = keynames; kn->name; kn++)
	{
		if (keynum == kn->keynum) {
			return kn->name;
		}
	}

	// make a hex string
	i = keynum >> 4;
	j = keynum & 15;

	tinystr[0] = '0';
	tinystr[1] = 'x';
	tinystr[2] = i > 9 ? i - 10 + 'a' : i + '0';
	tinystr[3] = j > 9 ? j - 10 + 'a' : j + '0';
	tinystr[4] = 0;

	return tinystr;
}

/*
===================
Key_SetBinding
===================
*/
void Key_SetBinding(int keynum, const char* binding)
{
	if (keynum == -1) {
		return;
	}

	// free old bindings
	if (keys[keynum].binding) {
		Com_Memset(keys[keynum].binding, 0, MAX_STRING_CHARS);
	}

	// allocate memory for new binding
	Q_strncpyz(keys[keynum].binding, binding, MAX_STRING_CHARS);

	// consider this like modifying an archived cvar, so the
	// file write will be triggered at the next oportunity
	cvar_modifiedFlags |= CVAR_ARCHIVE;
}

/*
===================
Key_GetBinding
===================
*/
char* Key_GetBinding(int keynum)
{
	if (keynum == -1) {
		return "";
	}

	return keys[keynum].binding;
}

/* 
===================
Key_GetKey
===================
*/

int Key_GetKey(const char* binding)
{
	if (binding) {
		for (int i = 0; i < K_COUNT; i++)
		{
			if (keys[i].binding && Q_stricmp(binding, keys[i].binding) == 0) {
				return i;
			}
		}
	}

	return -1;
}

/*
===================
Key_Unbind_f
===================
*/
void Key_Unbind_f(void)
{
	int b = 0;

	if (Cmd_Argc() != 2) {
		Com_Printf("unbind <key> : remove commands from a key\n");
		return;
	}

	b = Key_StringToKeynum(Cmd_Argv(1));
	if (b == -1) {
		Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding(b, "");
}

/*
===================
Key_Unbindall_f
===================
*/
void Key_Unbindall_f(void)
{
	for (int i = 0; i < K_COUNT; i++)
	{
		if (keys[i].binding) {
			Key_SetBinding(i, "");
		}
	}
}


/*
===================
Key_Bind_f
===================
*/
void Key_Bind_f(void)
{
	char	cmd[MAX_STRING_CHARS];
	int		i, c, b;

	c = Cmd_Argc();

	if (c < 2) {
		Com_Printf("bind <key> [command] : attach a command to a key\n");
		return;
	}

	b = Key_StringToKeynum(Cmd_Argv(1));
	if (b == -1) {
		Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if (c == 2) {
		if (keys[b].binding) {
			Com_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), keys[b].binding);
		} else {
			Com_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
		}
		return;
	}

	// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for (i = 2; i < c; i++)
	{
		strcat(cmd, Cmd_Argv(i));
		if (i != (c - 1)) {
			strcat(cmd, " ");
		}
	}

	Key_SetBinding(b, cmd);
}

/*
============
Key_WriteBindings

Writes lines containing "bind key value"
============
*/
void Key_WriteBindings(qhandle_t f)
{
	FS_Printf(f, "unbindall\n");

	for (int i = 0; i < K_COUNT; i++)
	{
		if (keys[i].binding && keys[i].binding[0]) {
			FS_Printf(f, "bind %s \"%s\"\n",
				Key_KeynumToString(i), keys[i].binding);
		}
	}
}


/*
============
Key_Bindlist_f

============
*/
void Key_Bindlist_f(void)
{
	for (int i = 0; i < K_COUNT; i++)
	{
		if (keys[i].binding && keys[i].binding[0]) {
			Com_Printf("%s \"%s\"\n",
				Key_KeynumToString(i), keys[i].binding);
		}
	}
}

/*
===================
CL_InitKeyCommands
===================
*/
void CL_InitKeyCommands(void)
{
	// register our functions
	Cmd_AddCommand("bind", Key_Bind_f);
	Cmd_AddCommand("unbind", Key_Unbind_f);
	Cmd_AddCommand("unbindall", Key_Unbindall_f);
	Cmd_AddCommand("bindlist", Key_Bindlist_f);
}

/*
===================
CL_AddKeyUpCommands
===================
*/
void CL_AddKeyUpCommands(int key, char* kb)
{
	char button[MAX_STRING_CHARS];
	char	cmd[MAX_STRING_CHARS];
	char* buttonPtr;
	bool keyevent;

	if (!kb) {
		return;
	}
	keyevent = false;
	buttonPtr = button;
	for (int i = 0; ; i++) {
		if (kb[i] == ';' || !kb[i]) {
			*buttonPtr = '\0';
			if (button[0] == '+') {
				// button commands add keynum and time as parms so that multiple
				// sources can be discriminated and subframe corrected
				Com_sprintf(cmd, sizeof(cmd), "-%s %i %i\n", button + 1, key, time);
				Cbuf_AddText(cmd);
				keyevent = true;
			} else {
				if (keyevent) {
					// down-only command
					Cbuf_AddText(button);
					Cbuf_AddText("\n");
				}
			}
			buttonPtr = button;
			while ((kb[i] <= ' ' || kb[i] == ';') && kb[i] != 0) {
				i++;
			}
		}
		*buttonPtr++ = kb[i];
		if (!kb[i]) {
			break;
		}
	}
}

/*
===================
CL_KeyEvent

Called by the system for both key up and key down events
===================
*/
void CL_KeyEvent(int key, bool down, unsigned time)
{
	char	cmd[MAX_STRING_CHARS];
	char*	kb;

	// update auto-repeat status and BUTTON_ANY status
	keys[key].down = down;

	if (down) {
		keys[key].repeats++;
		if (keys[key].repeats == 1) {
			anykeydown++;
		}
	} else {
		keys[key].repeats = 0;
		anykeydown--;
		if (anykeydown < 0) {
			anykeydown = 0;
		}
	}

#ifdef __linux__
	if (key == K_ENTER)
	{
		if (down)
		{
			if (keys[K_ALT].down)
			{
				Key_ClearStates();
				if (Cvar_VariableValue("r_fullscreen") == 0)
				{
					Com_Printf("Switching to fullscreen rendering\n");
					Cvar_Set("r_fullscreen", "1");
				}
				else
				{
					Com_Printf("Switching to windowed rendering\n");
					Cvar_Set("r_fullscreen", "0");
				}
				Cbuf_ExecuteText(EXEC_APPEND, "vid_restart\n");
				return;
			}
		}
	}
#endif

	// console key is hardcoded, so the user can never unbind it
	if (key == K_GRAVE || key == '~') {
		if (!down) {
			return;
		}
		Con_ToggleConsole_f();
		return;
	}

	// keys can still be used for bound actions
	if (down && (key < 128 || key == K_MOUSE1) &&
		(clc.demoplaying || cls.state == CA_CINEMATIC) && !cls.keyCatchers) {
		if (Cvar_VariableValue("com_cameraMode") == 0) {
			Cvar_Set("nextdemo", "");
			key = K_ESCAPE;
		}
	}

	// escape is always handled special
	if (key == K_ESCAPE && down) {
		if (cls.keyCatchers & KEYCATCH_MESSAGE) {
			// clear message mode
			Message_Key(key);
			return;
		}

		// escape always gets out of CGAME stuff
		if (cls.keyCatchers & KEYCATCH_CGAME) {
			cls.keyCatchers &= ~KEYCATCH_CGAME;
			VM_Call(cgvm, CG_EVENT_HANDLING, CGAME_EVENT_NONE);
			return;
		}

		if (!(cls.keyCatchers & KEYCATCH_UI)) {
			if (cls.state == CA_ACTIVE && !clc.demoplaying) {
				VM_Call(uivm, UI_SET_ACTIVE_MENU, UIMENU_INGAME);
			} else {
				CL_Disconnect_f();
				S_StopAllSounds();
				VM_Call(uivm, UI_SET_ACTIVE_MENU, UIMENU_MAIN);
			}
			return;
		}

		VM_Call(uivm, UI_KEY_EVENT, key, down);
		return;
	}

	// key up events only perform actions if the game key binding is
	// a button command (leading + sign).  These will be processed even in
	// console mode and menu mode, to keep the character from continuing 
	// an action started before a mode switch.
	if (!down) {
		kb = keys[key].binding;

		CL_AddKeyUpCommands(key, kb);

		if (cls.keyCatchers & KEYCATCH_UI && uivm) {
			VM_Call(uivm, UI_KEY_EVENT, key, down);
		} else if (cls.keyCatchers & KEYCATCH_CGAME && cgvm) {
			VM_Call(cgvm, CG_KEY_EVENT, key, down);
		}

		return;
	}


	// distribute the key down event to the apropriate handler
	if (cls.keyCatchers & KEYCATCH_CONSOLE) {
		Console_Key(key);
	} else if (cls.keyCatchers & KEYCATCH_UI) {
		if (uivm) {
			VM_Call(uivm, UI_KEY_EVENT, key, down);
		}
	} else if (cls.keyCatchers & KEYCATCH_CGAME) {
		if (cgvm) {
			VM_Call(cgvm, CG_KEY_EVENT, key, down);
		}
	} else if (cls.keyCatchers & KEYCATCH_MESSAGE) {
		Message_Key(key);
	} else if (cls.state == CA_DISCONNECTED) {
		Console_Key(key);
	} else {
		// send the bound action
		kb = keys[key].binding;
		if (!kb) {
			if (key >= 200) {
				Com_Printf("%s is unbound, use controls menu to set.\n"
					, Key_KeynumToString(key));
			}
		} else if (kb[0] == '+') {
			char	button[MAX_STRING_CHARS];
			char *	buttonPtr;

			buttonPtr = button;
			for (int i = 0; ; i++) {
				if (kb[i] == ';' || !kb[i]) {
					*buttonPtr = '\0';
					if (button[0] == '+') {
						// button commands add keynum and time as parms so that multiple
						// sources can be discriminated and subframe corrected
						Com_sprintf(cmd, sizeof(cmd), "%s %i %i\n", button, key, time);
						Cbuf_AddText(cmd);
					} else {
						// down-only command
						Cbuf_AddText(button);
						Cbuf_AddText("\n");
					}
					buttonPtr = button;
					while ((kb[i] <= ' ' || kb[i] == ';') && kb[i] != 0) {
						i++;
					}
				}
				*buttonPtr++ = kb[i];
				if (!kb[i]) {
					break;
				}
			}
		} else {
			// down-only command
			Cbuf_AddText(kb);
			Cbuf_AddText("\n");
		}
	}
}


/*
===================
CL_CharEvent

Normal keyboard characters, already shifted / capslocked / etc
===================
*/
void CL_CharEvent(int key)
{
	// the console key should never be used as a char
	if (key == '`' || key == '~') {
		return;
	}

	// distribute the key down event to the apropriate handler
	if (cls.keyCatchers & KEYCATCH_CONSOLE) {
		Field_CharEvent(&g_consoleField, key);
	} else if (cls.keyCatchers & KEYCATCH_UI) {
		VM_Call(uivm, UI_KEY_EVENT, key | K_CHAR_FLAG, true);
	} else if (cls.keyCatchers & KEYCATCH_MESSAGE) {
		Field_CharEvent(&g_chatField, key);
	} else if (cls.state == CA_DISCONNECTED) {
		Field_CharEvent(&g_consoleField, key);
	}
}


/*
===================
Key_ClearStates
===================
*/
void Key_ClearStates(void)
{
	anykeydown = false;

	for (int i = 0; i < K_COUNT; i++)
	{
		if (keys[i].down) {
			CL_KeyEvent(i, false, 0);

		}
		keys[i].down = 0;
		keys[i].repeats = 0;
	}
}
