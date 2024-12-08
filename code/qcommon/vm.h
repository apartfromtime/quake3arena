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

#ifndef _Q_VM_H_
#define _Q_VM_H_

typedef struct vm_s vm_t;

typedef enum {
	TRAP_MEMSET = 100,
	TRAP_MEMCPY,
	TRAP_STRNCPY,
	TRAP_SIN,
	TRAP_COS,
	TRAP_ATAN2,
	TRAP_SQRT,
	TRAP_MATRIXMULTIPLY,
	TRAP_ANGLEVECTORS,
	TRAP_PERPENDICULARVECTOR,
	TRAP_FLOOR,
	TRAP_CEIL,

	TRAP_TESTPRINTINT,
	TRAP_TESTPRINTFLOAT
} sharedTraps_t;

void VM_Init(void);
vm_t* VM_Create(const char* module, int (*systemCalls)(int*));
// module should be bare: "cgame", not "cgame.dll" or "vm/cgame.qvm"
void VM_Free(vm_t* vm);
void VM_Clear(void);
vm_t* VM_Restart(vm_t* vm);
int Q_CDECL VM_Call(vm_t* vm, int callNum, ...);
void VM_Debug(int level);
void* VM_ArgPtr(int intValue);
void* VM_ExplicitArgPtr(vm_t* vm, int intValue);

#endif // #ifndef _Q_VM_H_
