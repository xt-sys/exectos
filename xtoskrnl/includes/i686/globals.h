/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/globals.h
 * DESCRIPTION:     XT kernel global variables related to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_GLOBALS_H
#define __XTOSKRNL_I686_GLOBALS_H

#include <xtos.h>


/* Initial GDT */
EXTERN KGDTENTRY ArInitialGdt[GDT_ENTRIES];

/* Initial IDT */
EXTERN KIDTENTRY ArInitialIdt[IDT_ENTRIES];

/* Initial Processor Block */
EXTERN KPROCESSOR_BLOCK ArInitialProcessorBlock;

/* Initial TSS */
EXTERN KTSS ArInitialTss;

/* Double Fault and NMI task gates */
EXTERN UCHAR ArpDoubleFaultTss[KTSS_IO_MAPS];
EXTERN UCHAR ArpNonMaskableInterruptTss[KTSS_IO_MAPS];

/* Kernel own boot stack */
EXTERN UCHAR ArKernelBootStack[KERNEL_STACK_SIZE];

/* Kernel own fault stack */
EXTERN UCHAR ArKernelFaultStack[KERNEL_STACK_SIZE];

#endif /* __XTOSKRNL_I686_GLOBALS_H */
