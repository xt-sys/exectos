/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/asm.h
 * DESCRIPTION:     AMD64 architecture assembly definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_ASMSUP_H
#define __XTOSKRNL_AMD64_ASMSUP_H


/* KTRAP_FRAME structure offsets */
#define TrapXmm0                                0
#define TrapXmm1                                16
#define TrapXmm2                                32
#define TrapXmm3                                48
#define TrapXmm4                                64
#define TrapXmm5                                80
#define TrapXmm6                                96
#define TrapXmm7                                112
#define TrapXmm8                                128
#define TrapXmm9                                144
#define TrapXmm10                               160
#define TrapXmm11                               176
#define TrapXmm12                               192
#define TrapXmm13                               208
#define TrapXmm14                               224
#define TrapXmm15                               240
#define TrapMxCsr                               256
#define TrapPreviousMode                        260
#define TrapCr2                                 264
#define TrapCr3                                 272
#define TrapDr0                                 280
#define TrapDr1                                 288
#define TrapDr2                                 296
#define TrapDr3                                 304
#define TrapDr6                                 312
#define TrapDr7                                 320
#define TrapSegDs                               328
#define TrapSegEs                               330
#define TrapSegFs                               332
#define TrapSegGs                               334

/* KTRAP_FRAME length related definitions */
#define TRAP_FRAME_SIZE                         512
#define TRAP_REGISTERS_SIZE                     176

#endif /* __XTOSKRNL_AMD64_ASMSUP_H */
