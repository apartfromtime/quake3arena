#ifndef SYS_PUBLIC_H
#define SYS_PUBLIC_H

#ifdef _WIN32

#pragma warning(disable : 4996)			// This function or variable may be unsafe
#pragma warning(disable : 4018)			// signed/unsigned mismatch
#pragma warning(disable : 4032)
#pragma warning(disable : 4051)
#pragma warning(disable : 4057)			// slightly different base types
#pragma warning(disable : 4100)			// unreferenced formal parameter
#pragma warning(disable : 4115)
#pragma warning(disable : 4125)			// decimal digit terminates octal escape sequence
#pragma warning(disable : 4127)			// conditional expression is constant
#pragma warning(disable : 4136)
#pragma warning(disable : 4152)			// nonstandard extension, function/data pointer conversion in expression
#pragma warning(disable : 4201)
#pragma warning(disable : 4214)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)			// benign redefinition
#pragma warning(disable : 4305)			// truncation from const double to float
#pragma warning(disable : 4310)			// cast truncates constant value
#pragma warning(disable:  4505)			// unreferenced local function has been removed
#pragma warning(disable : 4514)
#pragma warning(disable : 4702)			// unreachable code
#pragma warning(disable : 4711)			// selected for automatic inline expansion
#pragma warning(disable : 4220)			// varargs matches remaining parameters
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

#endif // #ifndef SYS_PUBLIC_H
