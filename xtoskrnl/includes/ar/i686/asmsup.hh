/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ar/i686/asmsup.hh
 * DESCRIPTION:     Architecture-specific assembler prototypes
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_AR_ASMSUP_HH
#define __XTOSKRNL_AR_ASMSUP_HH

#include <xtos.hh>


/* TrampolineEnableXpa end address to calculate trampoline size */
XTCLINK PVOID ArEnableExtendedPhysicalAddressingEnd[];

/* External array of pointers to the interrupt handlers */
XTCLINK ULONG_PTR ArInterruptEntry[256];

/* TrampolineApStartup end address to calculate trampoline size */
XTCLINK PVOID ArStartApplicationProcessorEnd[];

/* External array of pointers to the trap handlers */
XTCLINK ULONG_PTR ArTrapEntry[256];

/* Forward reference for assembler code */
XTCLINK
XTCDECL
VOID
ArEnableExtendedPhysicalAddressing(IN ULONG_PTR PageMap);

XTCLINK
XTCDECL
VOID
ArHandleSpuriousInterrupt(VOID);

XTCLINK
XTCDECL
VOID
ArStartApplicationProcessor(VOID);

#endif /* __XTOSKRNL_AR_ASMSUP_HH */
