/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/kepfuncs.h
 * DESCRIPTION:     Private routine definitions for kernel services
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_KEPFUNCS_H
#define __XTOSKRNL_KEPFUNCS_H

#include <xtos.h>


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
                   IN PVOID Stack);

XTAPI
VOID
KeStartThread(IN PKTHREAD Thread);

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

#endif /* __XTOSKRNL_KEPFUNCS_H */
