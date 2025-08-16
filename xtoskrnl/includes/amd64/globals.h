/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/globals.h
 * DESCRIPTION:     XT kernel global variables related to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_GLOBALS_H
#define __XTOSKRNL_AMD64_GLOBALS_H

#include <xtos.h>


/* Initial GDT */
EXTERN KGDTENTRY ArInitialGdt[GDT_ENTRIES];

/* Initial IDT */
EXTERN KIDTENTRY ArInitialIdt[IDT_ENTRIES];

/* Initial Processor Block */
EXTERN KPROCESSOR_BLOCK ArInitialProcessorBlock;

/* Initial TSS */
EXTERN KTSS ArInitialTss;

/* Kernel own boot stack */
EXTERN UCHAR ArKernelBootStack[KERNEL_STACK_SIZE];

/* Kernel own fault stack */
EXTERN UCHAR ArKernelFaultStack[KERNEL_STACK_SIZE];

/* Page mapping routines for systems using 4-level paging (PML4) */
EXTERN CMMPAGEMAP_ROUTINES MmpPml4Routines;

/* Page mapping routines for systems using 5-level paging (PML5) */
EXTERN CMMPAGEMAP_ROUTINES MmpPml5Routines;

#endif /* __XTOSKRNL_AMD64_GLOBALS_H */
