/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/kei.h
 * DESCRIPTION:     XTOS kernel services routine definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_KEI_H
#define __XTOSKRNL_KEI_H

#include <xtos.h>


/* Kernel services routines forward references */
XTAPI
VOID
KeClearEvent(IN PKEVENT Event);

XTAPI
VOID
KeClearTimer(IN PKTIMER Timer);

XTAPI
VOID
KeHaltSystem(VOID);

XTAPI
VOID
KeInitializeEvent(OUT PKEVENT Event,
                  IN KEVENT_TYPE EventType,
                  IN BOOLEAN InitialState);

XTAPI
VOID
KeInitializeProcess(IN OUT PKPROCESS Process,
                    IN KPRIORITY Priority,
                    IN KAFFINITY Affinity,
                    IN PULONG_PTR DirectoryTable,
                    IN BOOLEAN Alignment);

XTAPI
XTSTATUS
KeInitializeThread(IN PKPROCESS Process,
                   IN OUT PKTHREAD Thread,
                   IN PKSYSTEM_ROUTINE SystemRoutine,
                   IN PKSTART_ROUTINE StartRoutine,
                   IN PVOID StartContext,
                   IN PCONTEXT Context,
                   IN PVOID EnvironmentBlock,
                   IN PVOID Stack,
                   IN BOOLEAN StartThread);

XTAPI
VOID
KePanic(IN ULONG Code);

XTAPI
VOID
KePanicEx(IN ULONG Code,
          IN ULONG_PTR Parameter1,
          IN ULONG_PTR Parameter2,
          IN ULONG_PTR Parameter3,
          IN ULONG_PTR Parameter4);

XTAPI
ULONGLONG
KeQueryTimer(IN PKTIMER Timer);

XTAPI
LONG
KeSetEvent(IN PKEVENT Event,
           IN KPRIORITY Increment,
           IN BOOLEAN Wait);

XTAPI
VOID
KeSetInterruptHandler(IN ULONG Vector,
                      IN PVOID Handler);

XTAPI
VOID
KeStartThread(IN PKTHREAD Thread);

XTAPI
VOID
KeStartXtSystem(IN PKERNEL_INITIALIZATION_BLOCK Parameters);

XTFASTCALL
VOID
KepExitDispatcher(IN KRUNLEVEL OldRunLevel);

XTAPI
VOID
KepRemoveTimer(IN OUT PKTIMER Timer);

XTFASTCALL
VOID
KepRetireDpcList(IN PKPROCESSOR_CONTROL_BLOCK Prcb);

XTAPI
VOID
KepSuspendNop(IN PKAPC Apc,
              IN OUT PKNORMAL_ROUTINE *NormalRoutine,
              IN OUT PVOID *NormalContext,
              IN OUT PVOID *SystemArgument1,
              IN OUT PVOID *SystemArgument2);

XTAPI
VOID
KepSuspendRundown(IN PKAPC Apc);

XTAPI
VOID
KepSuspendThread(IN PVOID NormalContext,
                 IN PVOID SystemArgument1,
                 IN PVOID SystemArgument2);

#endif /* __XTOSKRNL_KEI_H */
