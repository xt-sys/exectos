/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtdebug.h
 * DESCRIPTION:     XT debugging support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTDEBUG_H
#define __XTDK_XTDEBUG_H


/* XTOS platform debugging macros */
#ifdef DBG
    #define DEBUG                       1
    #define LdrPrint(Format, ...)       if(LdrDbgPrint) LdrDbgPrint(Format, __VA_ARGS__);
#else
    #define DEBUG                       0
    #define LdrPrint(Format, ...)       ((VOID)NULL)
#endif

#endif /* __XTDK_XTDEBUG_H */
