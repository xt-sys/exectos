/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/globals.c
 * DESCRIPTION:     XT kernel global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* FrameBuffer information */
HAL_FRAMEBUFFER_DATA HlpFrameBufferData;

/* Kernel initialization block passed by boot loader */
PKERNEL_INITIALIZATION_BLOCK KeInitializationBlock;

/* Pointer to boot loader provided DbgPrint() routine */
VOID (*LdrDbgPrint)(IN PWCHAR Format, IN ...) = NULL;
