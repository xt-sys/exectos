/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/globals.c
 * DESCRIPTION:     Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* Biggest free memory descriptor */
PLOADER_MEMORY_DESCRIPTOR MmFreeDescriptor;

/* Highest physical page number */
ULONG_PTR MmHighestPhysicalPage;

/* Lowest physical page number */
ULONG_PTR MmLowestPhysicalPage = -1;

/* Number of physical pages */
ULONG MmNumberOfPhysicalPages;

/* Old biggest free memory descriptor */
LOADER_MEMORY_DESCRIPTOR MmOldFreeDescriptor;

/* Page Map Level */
ULONG MmPageMapLevel;

/* Processor structures data (THIS IS A TEMPORARY HACK) */
UCHAR MmProcessorStructuresData[MAXIMUM_PROCESSORS][KPROCESSOR_STRUCTURES_SIZE] = {0};

/* Allocation descriptors dedicated for hardware layer */
LOADER_MEMORY_DESCRIPTOR MmpHardwareAllocationDescriptors[MM_HARDWARE_ALLOCATION_DESCRIPTORS];

/* Live address of kernel's hardware heap */
PVOID MmpHardwareHeapStart = MM_HARDWARE_HEAP_START_ADDRESS;

/* Architecture-specific memory extension */
BOOLEAN MmpMemoryExtension;

/* Number of used hardware allocation descriptors */
ULONG MmpUsedHardwareAllocationDescriptors = 0;
