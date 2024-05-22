/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/globals.c
 * DESCRIPTION:     Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* Biggest free memory descriptor */
PLOADER_MEMORY_MAPPING MmFreeDescriptor;

/* Highest physical page number */
ULONG_PTR MmHighestPhysicalPage;

/* Lowest physical page number */
ULONG_PTR MmLowestPhysicalPage = -1;

/* Number of physical pages */
ULONG MmNumberOfPhysicalPages;

/* Old biggest free memory descriptor */
LOADER_MEMORY_MAPPING MmOldFreeDescriptor;

/* Page Map Level */
ULONG MmPageMapLevel;

/* Processor structures data (THIS IS A TEMPORARY HACK) */
UCHAR MmProcessorStructuresData[MAXIMUM_PROCESSORS][KPROCESSOR_STRUCTURES_SIZE] = {0};

/* Allocation descriptors dedicated for HAL */
LOADER_MEMORY_MAPPING MmpHalAllocationDescriptors[MM_HAL_ALLOCATION_DESCRIPTORS];

/* Live address of kernel HAL heap */
PVOID MmpHalHeapStart = MM_HAL_HEAP_START_ADDRESS;

/* Number of used HAL allocation descriptors */
ULONG MmpUsedHalAllocationDescriptors = 0;
