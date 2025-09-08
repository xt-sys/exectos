/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/kei.h
 * DESCRIPTION:     XTOS kernel services routine definitions specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_KEI_H
#define __XTOSKRNL_I686_KEI_H

#include <xtos.h>


/* I686 specific Kernel services routines forward references */
XTAPI
PKPROCESSOR_BLOCK
KeGetCurrentProcessorBlock(VOID);

XTAPI
PKPROCESSOR_CONTROL_BLOCK
KeGetCurrentProcessorControlBlock(VOID);

XTAPI
ULONG
KeGetCurrentProcessorNumber(VOID);

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
KepSwitchBootStack();

#endif /* __XTOSKRNL_I686_KEI_H */
