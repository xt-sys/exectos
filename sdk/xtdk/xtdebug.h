/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtdebug.h
 * DESCRIPTION:     XT debugging support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTDEBUG_H
#define __XTDK_XTDEBUG_H


/* Debugging macros */
#define CHECKPOINT                      DebugPrint(L"Checkpoint reached at %s:%d\n", __RELFILE__, __LINE__);
#define DEPRECATED                      DebugPrint(L"Called deprecated routine '%s()' at %s:%d\n", \
                                                   __FUNCTION__, __RELFILE__, __LINE__);
#define UNIMPLEMENTED                   DebugPrint(L"Called unimplemented routine '%s()' at %s:%d\n", \
                                                   __FUNCTION__, __RELFILE__, __LINE__);

/* XTOS platform debugging macros */
#ifdef DBG
    #define DEBUG                       1
    #define DebugPrint(Format, ...)     DbgPrint(Format, __VA_ARGS__);
#else
    #define DEBUG                       0
    #define DebugPrint(Format, ...)     ((VOID)NULL)
#endif

#endif /* __XTDK_XTDEBUG_H */
