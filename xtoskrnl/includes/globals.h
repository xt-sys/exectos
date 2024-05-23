/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/globals.h
 * DESCRIPTION:     XT kernel global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_GLOBALS_H
#define __XTOSKRNL_GLOBALS_H

#include <xtos.h>


/* Active processors count */
EXTERN KAFFINITY HlpActiveProcessors;

/* APIC mode */
EXTERN HAL_APIC_MODE HlpApicMode;

/* FrameBuffer information */
EXTERN HAL_FRAMEBUFFER_DATA HlpFrameBufferData;

/* Processors identity table */
EXTERN HAL_PROCESSOR_IDENTITY HlpProcessorsIdentity[MAXIMUM_PROCESSORS];

/* Pointer to boot loader provided DbgPrint() routine */
EXTERN VOID (*KeDbgPrint)(IN PWCHAR Format, IN ...);

/* Kernel initialization block passed by boot loader */
EXTERN PKERNEL_INITIALIZATION_BLOCK KeInitializationBlock;

/* Kernel initial process */
EXTERN EPROCESS KeInitialProcess;

/* Kernel initial thread */
EXTERN ETHREAD KeInitialThread;

/* Kernel service descriptor table */
EXTERN KSERVICE_DESCRIPTOR_TABLE KeServiceDescriptorTable[KSERVICE_TABLES_COUNT];

/* Kernel process list */
EXTERN LIST_ENTRY KepProcessListHead;

/* Kernel system resources list */
EXTERN LIST_ENTRY KepSystemResourcesListHead;

/* Kernel system resources lock */
EXTERN KSPIN_LOCK KepSystemResourcesLock;

/* Biggest free memory descriptor */
EXTERN PLOADER_MEMORY_DESCRIPTOR MmFreeDescriptor;

/* Highest physical page number */
EXTERN ULONG_PTR MmHighestPhysicalPage;

/* Lowest physical page number */
EXTERN ULONG_PTR MmLowestPhysicalPage;

/* Number of physical pages */
EXTERN ULONG MmNumberOfPhysicalPages;

/* Old biggest free memory descriptor */
EXTERN LOADER_MEMORY_DESCRIPTOR MmOldFreeDescriptor;

/* Page Map Level */
EXTERN ULONG MmPageMapLevel;

/* Processor structures data (THIS IS A TEMPORARY HACK) */
EXTERN UCHAR MmProcessorStructuresData[MAXIMUM_PROCESSORS][KPROCESSOR_STRUCTURES_SIZE];

/* Allocation descriptors dedicated for HAL */
EXTERN LOADER_MEMORY_DESCRIPTOR MmpHalAllocationDescriptors[MM_HAL_ALLOCATION_DESCRIPTORS];

/* Live address of kernel HAL heap */
EXTERN PVOID MmpHalHeapStart;

/* Architecture-specific memory extension */
EXTERN BOOLEAN MmpMemoryExtension;

/* Number of used HAL allocation descriptors */
EXTERN ULONG MmpUsedHalAllocationDescriptors;

#endif /* __XTOSKRNL_GLOBALS_H */
