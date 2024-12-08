#ifndef SYS_PUBLIC_H
#define SYS_PUBLIC_H

#ifdef _WIN32
#pragma warning(disable : 4018)			// signed/unsigned mismatch
#pragma warning(disable : 4244)			// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable : 4142)			// benign redefinition
#pragma warning(disable : 4996)			// This function or variable may be unsafe
#endif

// returnbed by Sys_GetProcessorId
#define CPUID_GENERIC			0			// any unrecognized processor

#define CPUID_AXP				0x10

#define CPUID_INTEL_UNSUPPORTED	0x20			// Intel 386/486
#define CPUID_INTEL_PENTIUM		0x21			// Intel Pentium or PPro
#define CPUID_INTEL_MMX			0x22			// Intel Pentium/MMX or P2/MMX
#define CPUID_INTEL_KATMAI		0x23			// Intel Katmai

#define CPUID_AMD_3DNOW			0x30			// AMD K6 3DNOW!

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

#endif // #ifndef SYS_PUBLIC_H
