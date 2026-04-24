/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/time.hh
 * DESCRIPTION:     Time conversion support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_TIME_HH
#define __XTOSKRNL_RTL_TIME_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Time
    {
        private:
            STATIC CUSHORT DaysInMonth[2][12];
            STATIC CUSHORT DaysPrecedingMonth[2][12];

        public:
            STATIC XTAPI XTSTATUS TimeFieldsToUnixEpoch(IN PTIME_FIELDS TimeFields,
                                                        OUT PLONGLONG UnixTime);
            STATIC XTAPI XTSTATUS TimeFieldsToXtEpoch(IN PTIME_FIELDS TimeFields,
                                                      OUT PLARGE_INTEGER XtTime);

        private:
            STATIC XTFASTCALL BOOLEAN LeapYear(SHORT Year);

    };
}

#endif /* __XTOSKRNL_RTL_TIME_HH */
