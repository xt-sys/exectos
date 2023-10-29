/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/po.h
 * DESCRIPTION:     Power manager subsystem routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_PO_H
#define __XTOSKRNL_PO_H

#include <xtos.h>


/* Power Manager routines forward references */
XTAPI
VOID
PoInitializeProcessorControlBlock(IN OUT PKPROCESSOR_CONTROL_BLOCK Prcb);

XTFASTCALL
VOID
PopIdle0Function(IN PPROCESSOR_POWER_STATE PowerState);

#endif /* __XTOSKRNL_PO_H */
