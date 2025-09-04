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
EXTERN HL_FRAMEBUFFER_DATA HlpFrameBufferData;

/* Scroll region information */
EXTERN HL_SCROLL_REGION_DATA HlpScrollRegionData;

/* System information */
EXTERN ACPI_SYSTEM_INFO HlpSystemInfo;

/* Pointer to DbgPrint() routine */
EXTERN PKD_PRINT_ROUTINE KdPrint;

/* Kernel Debugger mode */
EXTERN KD_DEBUG_MODE KdpDebugMode;

/* Debugger I/O providers initialization routines */
EXTERN PKD_INIT_ROUTINE KdpIoProvidersInitRoutines[KDBG_PROVIDERS_COUNT];

/* List of active I/O providers */
EXTERN LIST_ENTRY KdpProviders;

/* Debugger's serial port handle */
EXTERN CPPORT KdpSerialPort;

/* Pre-defined serial port addresses */
EXTERN ULONG KdpSerialPortList[COMPORT_COUNT];


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

/* Pointers to page map routines for the current paging mode */
EXTERN PCMMPAGEMAP_ROUTINES MmpPageMapRoutines;

/* Number of used hardware allocation descriptors */
EXTERN ULONG MmpUsedHardwareAllocationDescriptors;

#endif /* __XTOSKRNL_GLOBALS_H */
