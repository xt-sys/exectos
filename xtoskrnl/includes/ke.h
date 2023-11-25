/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke.h
 * DESCRIPTION:     XTOS kernel services routine definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_KE_H
#define __XTOSKRNL_KE_H

#include <xtos.h>


/* Kernel services routines forward references */
XTAPI
VOID
KeClearEvent(IN PKEVENT Event);

XTFASTCALL
KRUNLEVEL
KeGetCurrentRunLevel(VOID);

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

XTFASTCALL
VOID
KeLowerRunLevel(IN KRUNLEVEL RunLevel);

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

XTFASTCALL
KRUNLEVEL
KeRaiseRunLevel(IN KRUNLEVEL RunLevel);

XTAPI
LONG
KeSetEvent(IN PKEVENT Event,
           IN KPRIORITY Increment,
           IN BOOLEAN Wait);

XTAPI
VOID
KeStartThread(IN PKTHREAD Thread);

XTAPI
VOID
KeStartXtSystem(IN PKERNEL_INITIALIZATION_BLOCK Parameters);

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

#endif /* __XTOSKRNL_KE_H */
