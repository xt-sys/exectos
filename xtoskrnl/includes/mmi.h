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
MmAllocateHardwareMemory(IN PFN_NUMBER PageCount,
                         IN BOOLEAN Aligned,
                         OUT PPHYSICAL_ADDRESS Buffer);

XTAPI
XTSTATUS
MmAllocateKernelStack(IN PVOID *Stack,
                      IN BOOLEAN LargeStack,
                      IN UCHAR SystemNode);

XTAPI
XTSTATUS
MmAllocateProcessorStructures(IN ULONG CpuNumber,
                              OUT PVOID *StructuresData);

XTAPI
VOID
MmFlushTlb(VOID);

XTAPI
VOID
MmFreeKernelStack(IN PVOID Stack,
                  IN BOOLEAN LargeStack);

XTAPI
VOID
MmFreeProcessorStructures(IN PVOID StructuresData);

XTAPI
VOID
MmInitializeMemoryManager(VOID);

XTAPI
XTSTATUS
MmMapHardwareMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                    IN PFN_NUMBER PageCount,
                    IN BOOLEAN FlushTlb,
                    OUT PVOID *VirtualAddress);

XTAPI
VOID
MmMarkHardwareMemoryWriteThrough(IN PVOID VirtualAddress,
                                 IN PFN_NUMBER PageCount);

XTAPI
VOID
MmRemapHardwareMemory(IN PVOID VirtualAddress,
                      IN PHYSICAL_ADDRESS PhysicalAddress,
                      IN BOOLEAN FlushTlb);

XTAPI
XTSTATUS
MmUnmapHardwareMemory(IN PVOID VirtualAddress,
                      IN PFN_NUMBER PageCount,
                      IN BOOLEAN FlushTlb);

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
