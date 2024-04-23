/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/asm.h
 * DESCRIPTION:     i686 architecture assembly definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_ASMSUP_H
#define __XTOSKRNL_I686_ASMSUP_H


/* KTRAP_FRAME structure offsets */
#define TrapPreviousMode                        0
#define TrapCr2                                 4
#define TrapCr3                                 8
#define TrapDr0                                 12
#define TrapDr1                                 16
#define TrapDr2                                 20
#define TrapDr3                                 24
#define TrapDr6                                 28
#define TrapDr7                                 32
#define TrapSegDs                               36
#define TrapSegEs                               38
#define TrapSegFs                               40
#define TrapSegGs                               42

/* KTRAP_FRAME length related definitions */
#define TRAP_FRAME_SIZE                         100
#define TRAP_REGISTERS_SIZE                     56

#endif /* __XTOSKRNL_AMD64_ASMSUP_H */
