/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/mmi.h
 * DESCRIPTION:     XT memory manager routines specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_MMI_H
#define __XTOSKRNL_AMD64_MMI_H

#include <xtos.h>


/* AMD64 Memory Manager routines forward references */
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
PMMP5E
MmpGetP5eAddress(PVOID Address);

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
PMMPXE
MmpGetPxeAddress(PVOID Address);

XTAPI
VOID
MmpInitializeArchitecture(VOID);

XTAPI
BOOLEAN
MmpPteValid(PHARDWARE_PTE PtePointer);

XTAPI
VOID
MmpSetPte(PHARDWARE_PTE PtePointer,
          PFN_NUMBER PageFrameNumber,
          BOOLEAN Writable);

XTAPI
VOID
MmpSetPteCaching(PHARDWARE_PTE PtePointer,
                 BOOLEAN CacheDisable,
                 BOOLEAN WriteThrough);

#endif /* __XTOSKRNL_AMD64_MMI_H */
