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

#ifndef _MSG_H_
#define _MSG_H_

#include "..\game\q_shared.h"
#include "..\game\bg_public.h"

//
// msg.c
//
typedef struct
{
	qboolean	allowoverflow;	// if false, do a Com_Error
	qboolean	overflowed;		// set to true if the buffer size failed (with allowoverflow set)
	qboolean	oob;			// set to true if the buffer size failed (with allowoverflow set)
	byte*		data;
	int			maxsize;
	int			cursize;
	int			readcount;
	int			bit;			// for bitwise reads and writes
} msg_t;

void MSG_Init(msg_t* buf, byte* data, int length);
void MSG_InitOOB(msg_t* buf, byte* data, int length);
void MSG_Clear(msg_t* buf);
void MSG_WriteData(msg_t* buf, const void* data, int length);
void MSG_Bitstream(msg_t* buf);

// TTimo
// copy a msg_t in case we need to store it as is for a bit
// (as I needed this to keep an msg_t from a static var for later use)
// sets data buffer as MSG_Init does prior to do the copy
void MSG_Copy(msg_t* buf, byte* data, int length, msg_t* src);

void MSG_WriteBits(msg_t* msg, int value, int bits);

void MSG_WriteChar(msg_t* sb, int c);
void MSG_WriteByte(msg_t* sb, int c);
void MSG_WriteShort(msg_t* sb, int c);
void MSG_WriteLong(msg_t* sb, int c);
void MSG_WriteFloat(msg_t* sb, float f);
void MSG_WriteString(msg_t* sb, const char* s);
void MSG_WriteBigString(msg_t* sb, const char* s);
void MSG_WriteAngle16(msg_t* sb, float f);

void MSG_BeginReading(msg_t* sb);
void MSG_BeginReadingOOB(msg_t* sb);

int MSG_ReadBits(msg_t* msg, int bits);

int MSG_ReadChar(msg_t* sb);
int MSG_ReadByte(msg_t* sb);
int MSG_ReadShort(msg_t* sb);
int MSG_ReadLong(msg_t* sb);
float MSG_ReadFloat(msg_t* sb);
char* MSG_ReadString(msg_t* sb);
char* MSG_ReadBigString(msg_t* sb);
char* MSG_ReadStringLine(msg_t* sb);
float MSG_ReadAngle16(msg_t* sb);
void MSG_ReadData(msg_t* sb, void* buffer, int size);


void MSG_WriteDeltaUsercmd(msg_t* msg, usercmd_t* from, usercmd_t* to);
void MSG_ReadDeltaUsercmd(msg_t* msg, usercmd_t* from, usercmd_t* to);

void MSG_WriteDeltaUsercmdKey(msg_t* msg, int key, usercmd_t* from, usercmd_t* to);
void MSG_ReadDeltaUsercmdKey(msg_t* msg, int key, usercmd_t* from, usercmd_t* to);

void MSG_WriteDeltaEntity(msg_t* msg, entityState_t* from, entityState_t* to,
	qboolean force);
void MSG_ReadDeltaEntity(msg_t* msg, entityState_t* from, entityState_t* to,
	int number);

void MSG_WriteDeltaPlayerstate(msg_t* msg, playerState_t* from, playerState_t* to);
void MSG_ReadDeltaPlayerstate(msg_t* msg, playerState_t* from, playerState_t* to);
void MSG_ReportChangeVectors_f(void);

#endif // #ifndef _MSG_H_
