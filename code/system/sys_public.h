#ifndef SYS_PUBLIC_H
#define SYS_PUBLIC_H

#ifdef _WIN32
#pragma warning(disable : 4201)			// nameless struct or union
#pragma warning(disable : 4018)			// signed/unsigned mismatch
#pragma warning(disable : 4244)			// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable : 4142)			// benign redefinition
#pragma warning(disable : 4996)			// This function or variable may be unsafe
#endif

// this is the define for determining if we have an asm version of a C function
#if (defined _M_IX86 || defined __i386__) && !defined __sun__  && !defined __LCC__
#define id386	1
#else
#define id386	0
#endif

#if defined(__MACH__) && defined(__APPLE__)

#ifdef __ppc__
#define Q_CPUSTRING	"MacOSXS-ppc"
#elif defined __i386__
#define Q_CPUSTRING	"MacOSXS-i386"
#else
#define Q_CPUSTRING	"MacOSXS-other"
#endif

#endif	// #if defined(__MACH__) && defined(__APPLE__)

#ifdef __MACOS__

#define	Q_CPUSTRING	"MacOS-PPC"

#endif	// #ifdef __MACOS__

#if defined(__FreeBSD__) || defined(__linux__)

#ifdef __i386__
#define	Q_CPUSTRING	"linux-i386"
#elif defined __axp__
#define	Q_CPUSTRING	"linux-alpha"
#else
#define	Q_CPUSTRING	"linux-other"
#endif

#endif	// #if defined(__FreeBSD__) || defined(__linux__)

#ifdef WIN32

// buildstring will be incorporated into the version string
#ifdef NDEBUG
#ifdef _M_IX86
#define	Q_CPUSTRING	"win-x86"
#endif
#else
#ifdef _M_IX86
#define	Q_CPUSTRING	"win-x86-debug"
#endif // #ifdef _M_IX86
#endif // #ifdef NDEBUG

#endif	// #ifdef WIN32

#if defined _M_IX86 && (defined _MSC_VER)
#define Q_INLINE   __inline
#define Q_CDECL    __cdecl
#elif defined _M_IX86 && (defined __GNUC__)
#define Q_INLINE __attribute__((inline))
#define Q_CDECL __attribute__((cdecl))
#else
#  define Q_INLINE
#  define Q_CDECL
#endif // #if defined _M_IX86 && (defined _MSC_VER)

#if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__
#if defined _WIN64
#define Q_OS_STRING "Win64"
#else
#if !defined Q_OS_STRING
#define Q_OS_STRING "Win32"
#endif // #if !defined Q_OS_STRING
#endif // #if defined _WIN64
#endif // #if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

typedef unsigned char 		byte;
typedef int		qhandle_t;

typedef enum
{
	SE_NONE = 0,            // evTime is still valid
	SE_KEY,                 // evValue is a key code, evValue2 is the down flag
	SE_CHAR,                // evValue is an ascii char
	SE_MOUSE,               // evValue and evValue2 are reletive signed x / y moves
	SE_JOYSTICK_AXIS,       // evValue is an axis number and evValue2 is the current state (-127 to 127)
	SE_CONSOLE,             // evPtr is a char*
	SE_PACKET               // evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

typedef struct
{
	int             evTime;
	sysEventType_t  evType;
	int             evValue, evValue2;
	int             evPtrLength;            // bytes of data pointed to by evPtr, for journaling
	void*           evPtr;                  // this must be manually freed if not NULL
} sysEvent_t;

sysEvent_t	Sys_GetEvent(void);

void	Sys_Init(void);

// general development dll loading for virtual machine testing
// fqpath param added 7/20/02 by T.Ray - Sys_LoadDll is only called in vm.c at this time
void* Q_CDECL Sys_LoadDll(const char* name, char* fqpath, int (Q_CDECL** entryPoint)(int, ...),
	int (Q_CDECL* systemcalls)(int, ...));
void	Sys_UnloadDll(void* dllHandle);

void	Q_CDECL Sys_Error(const char* error, ...);
void	Sys_Quit(void);
char* Sys_GetClipboardData(void);	// note that this isn't journaled...

void	Sys_Print(const char* msg);

const char* Sys_DateAndTime(void);

// Sys_Milliseconds should only be used for profiling purposes,
// any game related timing information should come from event timestamps
int		Sys_Milliseconds(void);

int		Sys_GetProcessorId(void);

void	Sys_BeginStreamedFile(qhandle_t f, int readahead);
void	Sys_EndStreamedFile(qhandle_t f);
int		Sys_StreamedRead(void* buffer, int size, int count, qhandle_t f);
void	Sys_StreamSeek(qhandle_t f, int offset, int origin);

void	Sys_ShowConsole(int level, bool quitOnClose);
void	Sys_SetErrorText(const char* text);

#include "..\qcommon\net_types.h"

void	Sys_SendPacket(int length, const void* data, netadr_t to);

// Does NOT parse port numbers, only base addresses.
bool	Sys_StringToAdr(const char* s, netadr_t* a);

bool	Sys_IsLANAddress(netadr_t adr);
void		Sys_ShowIP(void);

bool	Sys_CheckCD(void);

void	Sys_Mkdir(const char* path);
const char* Sys_Cwd(void);
void Sys_SetDefaultCDPath(const char* path);
char* Sys_DefaultCDPath(void);
void Sys_SetDefaultInstallPath(const char* path);
const char* Sys_DefaultInstallPath(void);
void Sys_SetDefaultHomePath(const char* path);
const char* Sys_DefaultHomePath(void);

char** Sys_ListFiles(const char* directory, const char* extension, char* filter, int* numfiles, bool wantsubs);
void Sys_FreeFileList(char** list);

bool Sys_LowPhysicalMemory();
unsigned int Sys_ProcessorCount();


#endif // #ifndef SYS_PUBLIC_H
