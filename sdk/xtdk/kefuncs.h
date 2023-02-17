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
KeInitializeApc(IN PKAPC Apc,
                IN PKTHREAD Thread,
                IN KAPC_ENVIRONMENT Environment,
                IN PKKERNEL_ROUTINE KernelRoutine,
                IN PKRUNDOWN_ROUTINE RundownRoutine,
                IN PKNORMAL_ROUTINE NormalRoutine,
                IN KPROCESSOR_MODE ApcMode,
                IN PVOID Context);

XTAPI
VOID
KeInitializeSemaphore(IN PKSEMAPHORE Semaphore,
                      IN LONG Count,
                      IN LONG Limit);

XTAPI
VOID
KeInitializeSpinLock(IN PKSPIN_LOCK SpinLock);

XTAPI
VOID
KeInitializeTimer(OUT PKTIMER Timer);

XTAPI
VOID
KeInitializeTimerEx(OUT PKTIMER Timer,
                    IN KTIMER_TYPE Type);

XTAPI
LONG
KeReadSemaphoreState(IN PKSEMAPHORE Semaphore);

XTAPI
LONG
KeReleaseSemaphore(IN PKSEMAPHORE Semaphore,
                   IN KPRIORITY Increment,
                   IN LONG Adjustment,
                   IN BOOLEAN Wait);

XTAPI
VOID
KeStartXtSystem(IN PKERNEL_INITIALIZATION_BLOCK Parameters);

#endif /* __XTDK_KEFUNCS_H */
