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
            STATIC XTAPI VOID SetInterruptHandler(IN ULONG Vector,
                                                  IN PVOID Handler);
    };
}

#endif /* __XTOSKRNL_HL_IRQ_HH */
