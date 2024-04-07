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


/* I686 Memory Manager routines forward references */
XTFASTCALL
VOID
MmZeroPages(IN PVOID Address,
            IN ULONG Size);

XTAPI
VOID
MmpInitializeArchitecture();

#endif /* __XTOSKRNL_I686_MMI_H */
