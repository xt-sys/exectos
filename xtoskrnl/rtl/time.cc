/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/time.cc
 * DESCRIPTION:     Time conversion support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Determines if a given year is a leap year in the Gregorian calendar.
 *
 * @param Year
 *        Supplies the year to be checked.
 *
 * @return This routine returns TRUE if the year is a leap year, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
RTL::Time::LeapYear(SHORT Year)
{
    return ((Year % 4 == 0 && Year % 100 != 0) || (Year % 400 == 0)) ? TRUE : FALSE;
}

/**
 * Converts a TIME_FIELDS calendar structure to a 64-bit Unix timestamp.
 *
 * @param TimeFields
 *        Supplies a pointer to a fully populated TIME_FIELDS structure.
 *
 * @param UnixTime
 *        Supplies a pointer to a 64-bit integer that receives the number of
 *        seconds elapsed since January 1, 1970.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RTL::Time::TimeFieldsToUnixEpoch(IN PTIME_FIELDS TimeFields,
                                 OUT PLONGLONG UnixTime)
{
    LONGLONG TotalSeconds;
    LARGE_INTEGER XtTime;
    XTSTATUS Status;

    /* Convert to XT Epoch */
    Status = TimeFieldsToXtEpoch(TimeFields, &XtTime);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to convert to XT Epoch, return error code */
        return Status;
    }

    /* Convert 100-nanosecond intervals (TICKS) to whole seconds */
    TotalSeconds = XtTime.QuadPart / TIME_TICKS_PER_SECOND;

    /* Subtract the number of seconds between January 1, 1601 and January 1, 1970 */
    *UnixTime = TotalSeconds - 11644473600LL;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Converts a TIME_FIELDS calendar structure to a 64-bit XT timestamp.
 *
 * @param TimeFields
 *        Supplies a pointer to a fully populated TIME_FIELDS structure.
 *
 * @param Time
 *        Supplies a pointer to variable that receives the converted time value in 100-nanosecond
 *        intervals since January 1, 1601.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RTL::Time::TimeFieldsToXtEpoch(IN PTIME_FIELDS TimeFields,
                               OUT PLARGE_INTEGER XtTime)
{
    ULONG Leap, ElapsedYears, ElapsedDays;
    ULONGLONG TotalSeconds;

    /* Check leap year */
    Leap = LeapYear(TimeFields->Year) ? 1 : 0;

    /* Validate input data */
    if(TimeFields->Hour < 0 || TimeFields->Hour > 23 ||
       TimeFields->Minute < 0 || TimeFields->Minute > 59 ||
       TimeFields->Second < 0 || TimeFields->Second > 59 ||
       TimeFields->Milliseconds < 0 || TimeFields->Milliseconds > 999 ||
       TimeFields->Year < 1601 || TimeFields->Year > 30827 ||
       TimeFields->Month < 1 || TimeFields->Month > 12 ||
       TimeFields->Day < 1 || TimeFields->Day > DaysInMonth[Leap][TimeFields->Month - 1])
    {
        /* Invalid input data, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Calculate days elapsed in previous years */
    ElapsedYears = (ULONG)(TimeFields->Year - 1601);
    ElapsedDays = (ElapsedYears * 365) + (ElapsedYears / 4) - (ElapsedYears / 100) + (ElapsedYears / 400);

    /* Add days elapsed in previous months of the current year */
    ElapsedDays += DaysPrecedingMonth[Leap][TimeFields->Month - 1];

    /* Add days elapsed in the current month */
    ElapsedDays += (TimeFields->Day - 1);

    /* Calculate a total number of seconds */
    TotalSeconds = ((ULONGLONG)ElapsedDays * TIME_SECONDS_PER_DAY) +
                   ((ULONGLONG)TimeFields->Hour * TIME_SECONDS_PER_HOUR) +
                   ((ULONGLONG)TimeFields->Minute * TIME_SECONDS_PER_MINUTE) +
                   (ULONGLONG)TimeFields->Second;

    /* Convert to 100-ns intervals and slap milliseconds on top */
    XtTime->QuadPart = (TotalSeconds * TIME_TICKS_PER_SECOND) +
                       ((ULONGLONG)TimeFields->Milliseconds * TIME_TICKS_PER_MILLISECOND);

    /* Return success */
    return STATUS_SUCCESS;
}
