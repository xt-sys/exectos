/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/irq.hh
 * DESCRIPTION:     Interrupts support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_IRQ_HH
#define __XTOSKRNL_HL_IRQ_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Irq
    {
        public:
            STATIC XTAPI PVOID QueryInterruptHandler(IN ULONG Vector);
            STATIC XTAPI PVOID QuerySystemInterruptHandler(IN ULONG Vector);
            STATIC XTCDECL VOID HandleUnexpectedInterrupt(IN PKTRAP_FRAME TrapFrame);
            STATIC XTAPI VOID RegisterInterruptHandler(IN ULONG Vector,
                                                       IN PVOID Handler);
            STATIC XTAPI VOID RegisterSystemInterruptHandler(IN ULONG Vector,
                                                             IN PINTERRUPT_HANDLER Handler);
    };
}

#endif /* __XTOSKRNL_HL_IRQ_HH */
