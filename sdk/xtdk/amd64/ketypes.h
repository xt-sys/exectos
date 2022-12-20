/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/ketypes.h
 * DESCRIPTION:     Kernel services related structures definitions specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_KETYPES_H
#define __XTDK_AMD64_KETYPES_H


/* Static Kernel-Mode address start */
#define KSEG0_BASE                        0xFFFFF80000000000

/* XTOS Kernel address base */
#define KSEG0_KERNEL_BASE                 0x0000000800000000

/* XTOS Kernel stack size */
#define KERNEL_STACK_SIZE                 8

#endif /* __XTDK_AMD64_KETYPES_H */
