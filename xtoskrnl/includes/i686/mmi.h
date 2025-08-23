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
VOID
MmpClearPte(PHARDWARE_PTE PtePointer);

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

XTAPI
BOOLEAN
MmpPml2PteValid(PHARDWARE_PTE PtePointer);

XTAPI
VOID
MmpSetPml2Pte(PHARDWARE_PTE PtePointer,
              PFN_NUMBER PageFrameNumber,
              BOOLEAN Writable);

XTAPI
VOID
MmpSetPml2PteCaching(PHARDWARE_PTE PtePointer,
                     BOOLEAN CacheDisable,
                     BOOLEAN WriteThrough);

XTAPI
BOOLEAN
MmpPml3PteValid(PHARDWARE_PTE PtePointer);

XTAPI
VOID
MmpSetPml3Pte(PHARDWARE_PTE PtePointer,
              PFN_NUMBER PageFrameNumber,
              BOOLEAN Writable);

XTAPI
VOID
MmpSetPml3PteCaching(PHARDWARE_PTE PtePointer,
                     BOOLEAN CacheDisable,
                     BOOLEAN WriteThrough);

#endif /* __XTOSKRNL_I686_MMI_H */
