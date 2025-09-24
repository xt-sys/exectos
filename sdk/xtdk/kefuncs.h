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
XTCLINK
XTFASTCALL
VOID
KeAcquireQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);

XTCLINK
XTFASTCALL
VOID
KeAcquireSpinLock(IN OUT PKSPIN_LOCK SpinLock);

XTCLINK
XTAPI
XTSTATUS
KeAcquireSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                        OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader);

XTCLINK
XTAPI
BOOLEAN
KeCancelTimer(IN PKTIMER Timer);

XTCLINK
XTFASTCALL
KRUNLEVEL
KeGetCurrentRunLevel(VOID);

XTCLINK
XTAPI
XTSTATUS
KeGetSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                    OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader);

XTCLINK
XTAPI
BOOLEAN
KeGetTimerState(IN PKTIMER Timer);

XTCLINK
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

XTCLINK
XTAPI
VOID
KeInitializeDpc(IN PKDPC Dpc,
                IN PKDEFERRED_ROUTINE DpcRoutine,
                IN PVOID DpcContext);

XTCLINK
XTAPI
VOID
KeInitializeSemaphore(IN PKSEMAPHORE Semaphore,
                      IN LONG Count,
                      IN LONG Limit);

XTCLINK
XTAPI
VOID
KeInitializeSpinLock(IN PKSPIN_LOCK SpinLock);

XTCLINK
XTAPI
VOID
KeInitializeThreadedDpc(IN PKDPC Dpc,
                        IN PKDEFERRED_ROUTINE DpcRoutine,
                        IN PVOID DpcContext);

XTCLINK
XTAPI
VOID
KeInitializeTimer(OUT PKTIMER Timer,
                  IN KTIMER_TYPE Type);

XTCLINK
XTFASTCALL
VOID
KeLowerRunLevel(IN KRUNLEVEL RunLevel);

XTCLINK
XTFASTCALL
KRUNLEVEL
KeRaiseRunLevel(IN KRUNLEVEL RunLevel);

XTCLINK
XTAPI
LONG
KeReadSemaphoreState(IN PKSEMAPHORE Semaphore);

XTCLINK
XTAPI
LONG
KeReleaseSemaphore(IN PKSEMAPHORE Semaphore,
                   IN KPRIORITY Increment,
                   IN LONG Adjustment,
                   IN BOOLEAN Wait);

XTCLINK
XTFASTCALL
VOID
KeReleaseQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);

XTCLINK
XTFASTCALL
VOID
KeReleaseSpinLock(IN OUT PKSPIN_LOCK SpinLock);

XTCLINK
XTAPI
VOID
KeReleaseSystemResource(IN PSYSTEM_RESOURCE_HEADER ResourceHeader);

XTCLINK
XTAPI
VOID
KeSetTargetProcessorDpc(IN PKDPC Dpc,
                        IN CCHAR Number);

XTCLINK
XTAPI
VOID
KeSetTimer(IN PKTIMER Timer,
           IN LARGE_INTEGER DueTime,
           IN LONG Period,
           IN PKDPC Dpc);

XTCLINK
XTAPI
VOID
KeSignalCallDpcDone(IN PVOID SystemArgument);

XTCLINK
XTAPI
BOOLEAN
KeSignalCallDpcSynchronize(IN PVOID SystemArgument);

#endif /* __XTDK_KEFUNCS_H */
