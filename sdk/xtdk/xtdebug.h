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
    #define LDRPRINT(Format, ...)       if(LdrPrint) LdrPrint(Format, __VA_ARGS__);
#else
    #define LDRPRINT(Format, ...)       ((VOID)NULL)
#endif

#endif /* __XTDK_XTDEBUG_H */
