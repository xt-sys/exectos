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
    #define XTCLINK extern "C"
    typedef wchar_t wchar;
#else
    #define XTCLINK
    typedef unsigned short wchar;
#endif

#endif /* __XTDK_XTCOMPAT_H */
