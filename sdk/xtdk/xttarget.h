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
#define ARCH_COMMON(header)                         STRINGIFY(../_ARCH_COMMON/header)
#define ARCH_HEADER(header)                         STRINGIFY(_ARCH/header)

/* Architecture specific definitions */
#if defined(__i386__) || defined(__i686__)
    #define _ARCH                                   i686
    #define _ARCH_I686                              1
    #define _ARCH_COMMON                            x86
    #define _ARCH_NAME                              "32-bit x86"
    #define _ARCH_IMAGE_MACHINE_TYPE                0x014C
    #define _XT32                                   1
    #define BITS_PER_LONG                           32
    #define CACHE_ALIGNMENT                         64
    #define EFI_ERROR_MASK                          0x80000000
    #define MAXIMUM_PROCESSORS                      32
    #define MEMORY_ALIGNMENT                        8
    #define MM_USERPAGE_TABLES                      1536
    #define MM_VIRTUAL_PAGESIZE                     20
#elif defined(__amd64__) || defined(__x86_64__)
    #define _ARCH                                   amd64
    #define _ARCH_AMD64                             1
    #define _ARCH_COMMON                            x86
    #define _ARCH_NAME                              "64-bit x86"
    #define _ARCH_IMAGE_MACHINE_TYPE                0x8664
    #define _XT64                                   1
    #define BITS_PER_LONG                           64
    #define CACHE_ALIGNMENT                         64
    #define EFI_ERROR_MASK                          0x8000000000000000
    #define MAXIMUM_PROCESSORS                      256
    #define MEMORY_ALIGNMENT                        16
    #define MM_USERPAGE_TABLES                      4194304
    #define MM_VIRTUAL_PAGESIZE                     52
#else
    #error Unknown architecture
#endif

#endif /* __XTDK_XTTARGET_H */
