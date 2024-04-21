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
XTFASTCALL
VOID
KeAcquireQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);

XTFASTCALL
VOID
KeAcquireSpinLock(IN OUT PKSPIN_LOCK SpinLock);

XTAPI
BOOLEAN
KeCancelTimer(IN PKTIMER Timer);

XTFASTCALL
KRUNLEVEL
KeGetCurrentRunLevel(VOID);

XTAPI
BOOLEAN
KeGetTimerState(IN PKTIMER Timer);

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
KeInitializeDpc(IN PKDPC Dpc,
                IN PKDEFERRED_ROUTINE DpcRoutine,
                IN PVOID DpcContext);

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
KeInitializeThreadedDpc(IN PKDPC Dpc,
                        IN PKDEFERRED_ROUTINE DpcRoutine,
                        IN PVOID DpcContext);

XTAPI
VOID
KeInitializeTimer(OUT PKTIMER Timer,
                  IN KTIMER_TYPE Type);

XTFASTCALL
VOID
KeLowerRunLevel(IN KRUNLEVEL RunLevel);

XTFASTCALL
KRUNLEVEL
KeRaiseRunLevel(IN KRUNLEVEL RunLevel);

XTAPI
LONG
KeReadSemaphoreState(IN PKSEMAPHORE Semaphore);

XTAPI
LONG
KeReleaseSemaphore(IN PKSEMAPHORE Semaphore,
                   IN KPRIORITY Increment,
                   IN LONG Adjustment,
                   IN BOOLEAN Wait);

XTFASTCALL
VOID
KeReleaseQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);

XTFASTCALL
VOID
KeReleaseSpinLock(IN OUT PKSPIN_LOCK SpinLock);

XTAPI
VOID
KeSetTargetProcessorDpc(IN PKDPC Dpc,
                        IN CCHAR Number);

XTAPI
VOID
KeSetTimer(IN PKTIMER Timer,
           IN LARGE_INTEGER DueTime,
           IN LONG Period,
           IN PKDPC Dpc);

XTAPI
VOID
KeSignalCallDpcDone(IN PVOID SystemArgument);

XTAPI
BOOLEAN
KeSignalCallDpcSynchronize(IN PVOID SystemArgument);

#endif /* __XTDK_KEFUNCS_H */
