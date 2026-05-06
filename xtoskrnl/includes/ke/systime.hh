/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/systime.hh
 * DESCRIPTION:     Timebase and system clock support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_SYSTIME_HH
#define __XTOSKRNL_KE_SYSTIME_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class SystemTime
    {
        private:
            STATIC LARGE_INTEGER BootTime;
            STATIC ULONG MaximumIncrement;
            STATIC ULONG MinimumIncrement;
            STATIC LONG TickOffset;
            STATIC ULONG TimeAdjustment;

        public:
            STATIC XTAPI VOID GetSystemTime(OUT PLARGE_INTEGER SystemTime);
            STATIC XTAPI VOID SetSystemTime(IN PLARGE_INTEGER NewTime,
                                            OUT PLARGE_INTEGER OldTime,
                                            IN BOOLEAN AdjustInterruptTime,
                                            IN BOOLEAN WriteToRtc);
            STATIC XTAPI VOID SetTimeIncrement(IN ULONG MinIncrement,
                                               IN ULONG MaxIncrement);
            STATIC XTFASTCALL VOID UpdateSystemTime(IN PKTRAP_FRAME TrapFrame,
                                                    IN ULONG Increment,
                                                    IN KRUNLEVEL RunLevel);
    };
}

#endif /* __XTOSKRNL_KE_SYSTIME_HH */
