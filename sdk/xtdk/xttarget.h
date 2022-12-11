/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xttarget.h
 * DESCRIPTION:     XT target architecture specific definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTTARGET_H
#define __XTDK_XTTARGET_H


/* Preprocessor macros for including arch-specific headers */
#define _ARCH_STRINGIZE(x)             _INCL_STRINGIZE(x)
#define _INCL_STRINGIZE(x)             #x
#define ARCH_HEADER(header)            _ARCH_STRINGIZE(_ARCH/header)

/* Architecture specific definitions */
#if defined(__i386__) || defined(__i686__)
    #define _ARCH                      i686
    #define _ARCH_I686                 1
    #define _XT32                      1
    #define EFI_ERROR_MASK             0x80000000
#elif defined(__amd64__) || defined(__x86_64__)
    #define _ARCH                      amd64
    #define _ARCH_AMD64                1
    #define _XT64                      1
    #define EFI_ERROR_MASK             0x8000000000000000
#else
    #error Unknown architecture
#endif

#endif /* __XTDK_XTTARGET_H */
