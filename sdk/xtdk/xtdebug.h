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
#define CHECKPOINT                      LdrPrint(L"Checkpoint reached at %s:%d\n", __FILE__, __LINE__);
#define DEPRECATED                      LdrPrint(L"Called deprecated routine '%s()' at %s:%d\n", \
                                                 __FUNCTION__, __FILE__, __LINE__);
#define UNIMPLEMENTED                   LdrPrint(L"Called unimplemented routine '%s()' at %s:%d\n", \
                                                 __FUNCTION__, __FILE__, __LINE__);

/* XTOS platform debugging macros */
#ifdef DBG
    #define DEBUG                       1
    #define LdrPrint(Format, ...)       if(LdrDbgPrint) LdrDbgPrint(Format, __VA_ARGS__);
#else
    #define DEBUG                       0
    #define LdrPrint(Format, ...)       ((VOID)NULL)
#endif

#endif /* __XTDK_XTDEBUG_H */
