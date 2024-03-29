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

#endif /* __XTOSKRNL_GLOBALS_H */
