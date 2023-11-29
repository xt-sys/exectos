/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/poi.h
 * DESCRIPTION:     Power manager subsystem routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_POI_H
#define __XTOSKRNL_POI_H

#include <xtos.h>


/* Power Manager routines forward references */
XTAPI
VOID
PoInitializeProcessorControlBlock(IN OUT PKPROCESSOR_CONTROL_BLOCK Prcb);

XTFASTCALL
VOID
PopIdle0Function(IN PPROCESSOR_POWER_STATE PowerState);

XTAPI
VOID
PopPerfIdle(PPROCESSOR_POWER_STATE PowerState);

XTAPI
VOID
PopPerfIdleDpc(IN PKDPC Dpc,
               IN PVOID DeferredContext,
               IN PVOID SystemArgument1,
               IN PVOID SystemArgument2);

#endif /* __XTOSKRNL_POI_H */
