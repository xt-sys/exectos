/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/globals.c
 * DESCRIPTION:     Architecture independent global variables related to KE subsystem
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* Pointer to boot loader provided DbgPrint() routine */
VOID (*KeDbgPrint)(IN PCWSTR Format, IN ...) = NULL;

/* Kernel initialization block passed by boot loader */
PKERNEL_INITIALIZATION_BLOCK KeInitializationBlock;

/* Kernel initial process */
EPROCESS KeInitialProcess;

/* Kernel initial thread */
ETHREAD KeInitialThread;

/* Kernel service descriptor table */
KSERVICE_DESCRIPTOR_TABLE KeServiceDescriptorTable[KSERVICE_TABLES_COUNT];

/* Kernel process list */
LIST_ENTRY KepProcessListHead;

/* Kernel system resources list */
LIST_ENTRY KepSystemResourcesListHead;

/* Kernel system resources lock */
KSPIN_LOCK KepSystemResourcesLock;

/* Kernel UBSAN active frame flag */
BOOLEAN KepUbsanActiveFrame = FALSE;
