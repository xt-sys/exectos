/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mmi.h
 * DESCRIPTION:     Memory manager routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_MMI_H
#define __XTOSKRNL_MMI_H

#include <xtos.h>


/* Memory Manager routines forward references */
XTAPI
XTSTATUS
MmAllocateKernelStack(IN PVOID *Stack,
                      IN BOOLEAN LargeStack,
                      IN UCHAR SystemNode);

XTAPI
VOID
MmFreeKernelStack(IN PVOID Stack,
                  IN BOOLEAN LargeStack);

XTAPI
VOID
MmInitializeMemoryManager(VOID);

XTAPI
VOID
MmpScanMemoryDescriptors(VOID);

XTAPI
BOOLEAN
MmpVerifyMemoryTypeFree(LOADER_MEMORY_TYPE MemoryType);

XTAPI
BOOLEAN
MmpVerifyMemoryTypeInvisible(LOADER_MEMORY_TYPE MemoryType);

#endif /* __XTOSKRNL_MMI_H */
