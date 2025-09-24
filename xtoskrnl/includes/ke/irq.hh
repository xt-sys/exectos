/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/irq.hh
 * DESCRIPTION:     Kernel interrupts support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_IRQ_HH
#define __XTOSKRNL_KE_IRQ_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Irq
    {
        public:
            STATIC XTAPI VOID SetInterruptHandler(IN ULONG Vector,
                                                  IN PVOID Handler);
    };
}

#endif /* __XTOSKRNL_KE_IRQ_HH */
