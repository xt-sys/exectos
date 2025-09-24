/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/timer.hh
 * DESCRIPTION:     Kernel timer object support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_TIMER_HH
#define __XTOSKRNL_KE_TIMER_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Timer
    {
        public:
            STATIC XTAPI BOOLEAN CancelTimer(IN PKTIMER Timer);
            STATIC XTAPI VOID ClearTimer(IN PKTIMER Timer);
            STATIC XTAPI BOOLEAN GetState(IN PKTIMER Timer);
            STATIC XTAPI VOID InitializeTimer(OUT PKTIMER Timer,
                                              IN KTIMER_TYPE Type);
            STATIC XTAPI ULONGLONG QueryTimer(IN PKTIMER Timer);
            STATIC XTAPI VOID SetTimer(IN PKTIMER Timer,
                                       IN LARGE_INTEGER DueTime,
                                       IN LONG Period,
                                       IN PKDPC Dpc);

        private:
            STATIC XTAPI VOID RemoveTimer(IN OUT PKTIMER Timer);
    };
}

#endif /* __XTOSKRNL_KE_TIMER_HH */
