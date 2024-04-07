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
XTFASTCALL
VOID
MmZeroPages(IN PVOID Address,
            IN ULONG Size);

XTAPI
PMMPTE
MmpGetPdeAddress(PVOID Address);

XTAPI
PMMPTE
MmpGetPpeAddress(PVOID Address);

XTAPI
PMMPTE
MmpGetPteAddress(PVOID Address);

XTAPI
PMMPTE
MmpGetPxeAddress(PVOID Address);

XTAPI
VOID
MmpInitializeArchitecture(VOID);

#endif /* __XTOSKRNL_AMD64_MMI_H */
