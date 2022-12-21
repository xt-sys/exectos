/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/ketypes.h
 * DESCRIPTION:     Kernel services related structures definitions specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_KETYPES_H
#define __XTDK_I686_KETYPES_H


/* Static Kernel-Mode address start */
#define KSEG0_BASE                        0x80000000

/* XTOS Kernel address base */
#define KSEG0_KERNEL_BASE                 0x01800000

/* XTOS Kernel stack size */
#define KERNEL_STACK_SIZE                 0x4000

#endif /* __XTDK_I686_KETYPES_H */
