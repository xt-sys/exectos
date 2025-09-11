/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/po/idle.hh
 * DESCRIPTION:     Processor idle functionality support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_PO_IDLE_HH
#define __XTOSKRNL_PO_IDLE_HH

#include <xtos.hh>


/* Runtime Library */
namespace PO
{
    class Idle
    {
        public:
            STATIC XTAPI VOID InitializeProcessorIdleState(IN OUT PKPROCESSOR_CONTROL_BLOCK Prcb);

        private:
            STATIC XTFASTCALL VOID Idle0Function(IN PPROCESSOR_POWER_STATE PowerState);
            STATIC XTAPI VOID PerfIdle(IN PPROCESSOR_POWER_STATE PowerState);
            STATIC XTAPI VOID PerfIdleDpc(IN PKDPC Dpc,
                                          IN PVOID DeferredContext,
                                          IN PVOID SystemArgument1,
                                          IN PVOID SystemArgument2);
    };
}

#endif /* __XTOSKRNL_PO_IDLE_HH */
