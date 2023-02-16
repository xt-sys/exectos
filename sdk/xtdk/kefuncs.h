/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/kefuncs.h
 * DESCRIPTION:     XTOS kernel services routine definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_KEFUNCS_H
#define __XTDK_KEFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include <ketypes.h>


/* Kernel services routines forward references */
XTAPI
VOID
KeInitializeTimer(OUT PKTIMER Timer);

XTAPI
VOID
KeInitializeTimerEx(OUT PKTIMER Timer,
                    IN KTIMER_TYPE Type);

XTAPI
VOID
KeStartXtSystem(IN PKERNEL_INITIALIZATION_BLOCK Parameters);

#endif /* __XTDK_KEFUNCS_H */
