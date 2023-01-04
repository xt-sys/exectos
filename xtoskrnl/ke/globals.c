/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/globals.c
 * DESCRIPTION:     XT kernel global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* Kernel initialization block passed by boot loader */
PKERNEL_INITIALIZATION_BLOCK KeInitializationBlock;

/* Kernel own boot stack */
UCHAR KepKernelBootStackData[KERNEL_STACK_SIZE] = {0};

/* Kernel own fault stack */
UCHAR KepKernelFaultStackData[KERNEL_STACK_SIZE] = {0};

/* Pointer to boot loader provided DbgPrint() routine */
VOID (*LdrDbgPrint)(IN PWCHAR Format, IN ...) = NULL;
