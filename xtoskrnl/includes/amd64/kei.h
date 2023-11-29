/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/kei.h
 * DESCRIPTION:     XTOS kernel services routine definitions specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_KEI_H
#define __XTOSKRNL_AMD64_KEI_H

#include <xtos.h>


/* AMD64 specific Kernel services routines forward references */
XTAPI
PKPROCESSOR_BLOCK
KeGetCurrentProcessorBlock(VOID);

XTAPI
PKPROCESSOR_CONTROL_BLOCK
KeGetCurrentProcessorControlBlock(VOID);

XTAPI
PKTHREAD
KeGetCurrentThread(VOID);

XTAPI
VOID
KepInitializeKernel(VOID);

XTAPI
VOID
KepInitializeMachine(VOID);

XTAPI
VOID
KepInitializeThreadContext(IN PKTHREAD Thread,
                           IN PKSYSTEM_ROUTINE SystemRoutine,
                           IN PKSTART_ROUTINE StartRoutine,
                           IN PVOID StartContext,
                           IN PCONTEXT ContextRecord);

XTAPI
VOID
KepSaveProcessorState(OUT PKPROCESSOR_STATE CpuState);

XTAPI
VOID
KepStartKernel(VOID);

XTAPI
VOID
KepSwitchBootStack(IN ULONG_PTR Stack);

#endif /* __XTOSKRNL_AMD64_KEI_H */
