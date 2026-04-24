/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/rtc.hh
 * DESCRIPTION:     Hardware Real-Time Clock (RTC) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_RTC_HH
#define __XTOSKRNL_HL_RTC_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Rtc
    {
        public:
            STATIC XTAPI XTSTATUS GetRealTimeClock(OUT PTIME_FIELDS Time);
            STATIC XTAPI XTSTATUS SetRealTimeClock(IN PTIME_FIELDS Time);
    };
}

#endif /* __XTOSKRNL_HL_RTC_HH */
