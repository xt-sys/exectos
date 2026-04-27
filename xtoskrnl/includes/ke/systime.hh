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

        public:
            STATIC XTAPI VOID GetSystemTime(OUT PLARGE_INTEGER SystemTime);
            STATIC XTAPI VOID SetSystemTime(IN PLARGE_INTEGER NewTime,
                                            OUT PLARGE_INTEGER OldTime,
                                            IN BOOLEAN AdjustInterruptTime,
                                            IN BOOLEAN WriteToRtc);
    };
}

#endif /* __XTOSKRNL_KE_SYSTIME_HH */
