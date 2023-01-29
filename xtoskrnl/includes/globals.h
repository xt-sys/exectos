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
#include ARCH_HEADER(globals.h)


/* FrameBuffer information */
EXTERN HAL_FRAMEBUFFER_DATA HlpFrameBufferData;

/* Kernel initialization block passed by boot loader */
EXTERN PKERNEL_INITIALIZATION_BLOCK KeInitializationBlock;

/* Kernel own boot stack */
EXTERN UCHAR KepKernelBootStackData[KERNEL_STACK_SIZE];

/* Kernel own fault stack */
EXTERN UCHAR KepKernelFaultStackData[KERNEL_STACK_SIZE];

/* Pointer to boot loader provided DbgPrint() routine */
EXTERN VOID (*LdrDbgPrint)(IN PWCHAR Format, IN ...);

#endif /* __XTOSKRNL_GLOBALS_H */
