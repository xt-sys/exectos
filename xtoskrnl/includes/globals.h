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


/* ACPI tables cache list */
EXTERN LIST_ENTRY HlpAcpiCacheList;

/* ACPI Root System Description Pointer (RSDP) */
EXTERN PACPI_RSDP HlpAcpiRsdp;

/* ACPI timer information */
EXTERN ACPI_TIMER_INFO HlpAcpiTimerInfo;

/* Active processors count */
EXTERN KAFFINITY HlpActiveProcessors;

/* APIC mode */
EXTERN APIC_MODE HlpApicMode;

/* FrameBuffer information */
EXTERN HAL_FRAMEBUFFER_DATA HlpFrameBufferData;

/* System information */
EXTERN ACPI_SYSTEM_INFO HlpSystemInfo;

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

/* Kernel UBSAN active frame flag */
EXTERN BOOLEAN KepUbsanActiveFrame;

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

/* Processor structures data (THIS IS A TEMPORARY HACK) */
EXTERN UCHAR MmProcessorStructuresData[MAXIMUM_PROCESSORS][KPROCESSOR_STRUCTURES_SIZE];

/* Allocation descriptors dedicated for hardware layer */
EXTERN LOADER_MEMORY_DESCRIPTOR MmpHardwareAllocationDescriptors[MM_HARDWARE_ALLOCATION_DESCRIPTORS];

/* Live address of kernel's hardware heap */
EXTERN PVOID MmpHardwareHeapStart;

/* Information about the current page map */
EXTERN MMPAGEMAP_INFO MmpPageMapInfo;

/* Number of used hardware allocation descriptors */
EXTERN ULONG MmpUsedHardwareAllocationDescriptors;

#endif /* __XTOSKRNL_GLOBALS_H */
