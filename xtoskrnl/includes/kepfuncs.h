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
PKPROCESSOR_BLOCK
KeGetCurrentProcessorBlock(VOID);

XTAPI
PKPROCESSOR_CONTROL_BLOCK
KeGetCurrentProcessorControlBlock(VOID);

XTAPI
VOID
KepArchInitialize(VOID);

XTAPI
VOID
KepStartKernel(VOID);

XTAPI
VOID
KepSwitchBootStack(IN ULONG_PTR Stack);

#endif /* __XTOSKRNL_KEPFUNCS_H */
