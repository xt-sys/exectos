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
    LONGLONG ElapsedDays, TotalSeconds;
    LONG Month, Year;
    ULONG Leap;

    /* Check leap year */
    Leap = LeapYear(TimeFields->Year) ? 1 : 0;

    /* 2. Validate input data */
    if(TimeFields->Hour < 0 || TimeFields->Hour > 23 ||
       TimeFields->Minute < 0 || TimeFields->Minute > 59 ||
       TimeFields->Second < 0 || TimeFields->Second > 59 ||
       TimeFields->Year < 0 || TimeFields->Year > 30827 ||
       TimeFields->Month < 1 || TimeFields->Month > 12 ||
       TimeFields->Day < 1 || TimeFields->Day > DaysInMonth[Leap][TimeFields->Month - 1])
    {
        /* Invalid input data, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Copy year and month */
    Year = (LONG)TimeFields->Year;
    Month = (LONG)TimeFields->Month;

    /* Put February at the end of the calculation cycle, making leap day handling implicit in the year division */
    if(0 >= (LONG)(Month -= 2))
    {
        /* Adjust month and year */
        Month += 12;
        Year -= 1;
    }

    /* Calculate absolute elapsed days */
    ElapsedDays = (LONGLONG)(Year/4 - Year/100 + Year/400 + 367*Month/12 + TimeFields->Day) +
                  (LONGLONG)Year*365 - 719499LL;

    /* Calculate total seconds */
    TotalSeconds = (ElapsedDays * TIME_SECONDS_PER_DAY) +
                   ((LONGLONG)TimeFields->Hour * TIME_SECONDS_PER_HOUR) +
                   ((LONGLONG)TimeFields->Minute * TIME_SECONDS_PER_MINUTE) +
                   (LONGLONG)TimeFields->Second;

    /* Output the final Unix timestamp */
    *UnixTime = TotalSeconds;

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
    LONGLONG ElapsedDays, TotalSeconds;
    LONG Month, Year;
    ULONG Leap;

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

    /* Copy year and month */
    Year = (LONG)TimeFields->Year;
    Month = (LONG)TimeFields->Month;

    /* Put February at the end of the calculation cycle, making leap day handling implicit in the year division */
    if(0 >= (LONG)(Month -= 2))
    {
        /* Adjust month and year */
        Month += 12;
        Year -= 1;
    }

    /* Calculate absolute elapsed days */
    ElapsedDays = (LONGLONG)(Year/4 - Year/100 + Year/400 + 367*Month/12 + TimeFields->Day) +
                  (LONGLONG)Year*365 - 584725LL;

    /* Calculate total seconds */
    TotalSeconds = (ElapsedDays * TIME_SECONDS_PER_DAY) +
                   ((LONGLONG)TimeFields->Hour * TIME_SECONDS_PER_HOUR) +
                   ((LONGLONG)TimeFields->Minute * TIME_SECONDS_PER_MINUTE) +
                   (LONGLONG)TimeFields->Second;

    /* Convert to 100-ns intervals and slap milliseconds on top */
    XtTime->QuadPart = (TotalSeconds * TIME_TICKS_PER_SECOND) +
                       ((LONGLONG)TimeFields->Milliseconds * TIME_TICKS_PER_MILLISECOND);

    /* Return success */
    return STATUS_SUCCESS;
}
