/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/mmi.h
 * DESCRIPTION:     XT memory manager routines specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_MMI_H
#define __XTOSKRNL_I686_MMI_H

#include <xtos.h>


/* i686 Memory Manager routines forward references */
XTAPI
VOID
MmInitializePageMapSupport(VOID);

XTFASTCALL
VOID
MmZeroPages(IN PVOID Address,
            IN ULONG Size);

XTAPI
BOOLEAN
MmpGetExtendedPhysicalAddressingStatus(VOID);

XTAPI
PMMPDE
MmpGetPdeAddress(PVOID Address);

XTAPI
PMMPPE
MmpGetPpeAddress(PVOID Address);

XTAPI
PMMPTE
MmpGetPteAddress(PVOID Address);

XTAPI
VOID
MmpInitializeArchitecture(VOID);

#endif /* __XTOSKRNL_I686_MMI_H */
