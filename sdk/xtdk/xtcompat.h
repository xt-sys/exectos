/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtcompat.h
 * DESCRIPTION:     C/C++ compatibility macros
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_XTCOMPAT_H
#define __XTDK_XTCOMPAT_H


#ifdef __cplusplus
    /* C++ definitions */
    #define XTCLINK         extern "C"
    #define NULLPTR         nullptr

    /* C++ types */
    typedef wchar_t wchar;
#else
    /* C definitions */
    #define XTCLINK
    #define NULLPTR         ((void *)0)

    /* C types */
    typedef unsigned short wchar;
#endif

#endif /* __XTDK_XTCOMPAT_H */
