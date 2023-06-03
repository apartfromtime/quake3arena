#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "posh.h"

// macro definitions to support dynamic link libraries.
// for a DLL library
#define Q3_DLL POSH_IMPORTEXPORT

#if defined NDEBUG
#define BUILD_STRING "Release"
#else
#define BUILD_STRING "Debug"
#endif	// #ifdef NDEBUG

#define OS_STRING	POSH_OS_STRING
#define CPU_STRING	POSH_CPU_STRING

#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS 1			// safe/unsafe deprecation warning
#pragma warning(disable : 4018)     // signed/unsigned mismatch
#pragma warning(disable : 4032)
#pragma warning(disable : 4051)
#pragma warning(disable : 4057)		// slightly different base types
#pragma warning(disable : 4100)		// unreferenced formal parameter
#pragma warning(disable : 4115)
#pragma warning(disable : 4125)		// decimal digit terminates octal escape sequence
#pragma warning(disable : 4127)		// conditional expression is constant
#pragma warning(disable : 4136)
#pragma warning(disable : 4152)		// nonstandard extension, function/data pointer conversion in expression
//#pragma warning(disable : 4201)
//#pragma warning(disable : 4214)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)		// benign redefinition
//#pragma warning(disable : 4305)		// truncation from const double to float
//#pragma warning(disable : 4310)		// cast truncates constant value
//#pragma warning(disable:  4505) 	// unreferenced local function has been removed
#pragma warning(disable : 4514)
#pragma warning(disable : 4702)		// unreachable code
#pragma warning(disable : 4711)		// selected for automatic inline expansion
#pragma warning(disable : 4220)		// varargs matches remaining parameters

#endif	// #ifdef _WIN32
#endif	// #ifndef _SYSTEM_H_