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

/**
 * Converts a 64-bit Unix timestamp into a TIME_FIELDS calendar structure.
 *
 * @param UnixTime
 *        Supplies a pointer to a 64-bit integer that contains the number of
 *        seconds elapsed since January 1, 1970.
 *
 * @param TimeFields
 *        Supplies a pointer to a TIME_FIELDS structure that receives the converted calendar data.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RTL::Time::UnixEpochToTimeFields(IN PLONGLONG UnixTime,
                                 OUT PTIME_FIELDS TimeFields)
{
    LONG CalculatedDay, CalculatedMonth, CalculatedYear;
    LONGLONG Era, SecondsOfDay, TotalSeconds, UnixDays;
    ULONG DayOfEra, DayOfYear, YearOfEra;

    /* Validate pointers */
    if(UnixTime == NULLPTR || TimeFields == NULLPTR)
    {
        /* Invalid pointers provided, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Extract total seconds */
    TotalSeconds = *UnixTime;

    /* Check sign */
    if(TotalSeconds >= 0)
    {
        /* Divide TotalSeconds by seconds per day and compute the remainder */
        UnixDays = TotalSeconds / TIME_SECONDS_PER_DAY;
        SecondsOfDay = TotalSeconds % TIME_SECONDS_PER_DAY;
    }
    else
    {
        /* Subtract seconds per day minus one from TotalSeconds and divide by seconds per day */
        UnixDays = (TotalSeconds - (TIME_SECONDS_PER_DAY - 1)) / TIME_SECONDS_PER_DAY;
        SecondsOfDay = TotalSeconds % TIME_SECONDS_PER_DAY;

        /* Check if SecondsOfDay is less than zero */
        if(SecondsOfDay < 0)
        {
            /* Add seconds per day to SecondsOfDay */
            SecondsOfDay += TIME_SECONDS_PER_DAY;
        }
    }

    /* Unix timestamps do not natively support milliseconds */
    TimeFields->Milliseconds = 0;

    /* Shift epoch from 1970-01-01 to 0000-03-01 to align with leap year cycles */
    UnixDays += 719468LL;

    /* Calculate the 400-year era */
    Era = (UnixDays >= 0 ? UnixDays : UnixDays - 146096) / 146097;

    /* Calculate the day within the current era */
    DayOfEra = (ULONG)(UnixDays - Era * 146097);

    /* Calculate the year within the current era */
    YearOfEra = (DayOfEra - (DayOfEra / 1460) + (DayOfEra / 36524) - (DayOfEra / 146096)) / 365;

    /* Combine era and year-of-era to get the absolute year */
    CalculatedYear = (LONG)(YearOfEra + Era * 400);

    /* Calculate the day of the year */
    DayOfYear = DayOfEra - (365 * YearOfEra + (YearOfEra / 4) - (YearOfEra / 100));

    /* Calculate the month */
    CalculatedMonth = (5 * DayOfYear + 2) / 153;

    /* Calculate the exact day of the month */
    CalculatedDay = DayOfYear - (153 * CalculatedMonth + 2) / 5 + 1;

    /* Shift the month back to the standard Gregorian calendar */
    CalculatedMonth += (CalculatedMonth < 10 ? 3 : -9);

    /* If the month is January or February, it belongs to the next computational year */
    CalculatedYear += (CalculatedMonth <= 2 ? 1 : 0);

    /* Validate computed year */
    if(CalculatedYear < 0 || CalculatedYear > 30827)
    {
        /* Year is out of bounds, return error code */
        return STATUS_NOT_SUPPORTED;
    }

    /* Populate the output structure */
    TimeFields->Day = (SHORT)CalculatedDay;
    TimeFields->Month = (SHORT)CalculatedMonth;
    TimeFields->Year = (SHORT)CalculatedYear;
    TimeFields->Hour = (SHORT)(SecondsOfDay / TIME_SECONDS_PER_HOUR);
    TimeFields->Minute = (SHORT)((SecondsOfDay % TIME_SECONDS_PER_HOUR) / TIME_SECONDS_PER_MINUTE);
    TimeFields->Second = (SHORT)(SecondsOfDay % TIME_SECONDS_PER_MINUTE);
    TimeFields->Milliseconds = 0;
    TimeFields->Weekday = (SHORT)(((UnixDays + 4) % 7 + 7) % 7);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Converts a 64-bit XT timestamp into a TIME_FIELDS calendar structure.
 *
 * @param XtTime
 *        Supplies a pointer to a variable that contains the time value in
 *        100-nanosecond intervals since January 1, 1601.
 *
 * @param TimeFields
 *        Supplies a pointer to a TIME_FIELDS structure that receives the converted calendar data.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RTL::Time::XtEpochToTimeFields(IN PLARGE_INTEGER XtTime,
                               OUT PTIME_FIELDS TimeFields)
{
    ULONG CurrentYear, Days, Leap, Periods1Year, Periods4Years, Periods100Years, Periods400Years;
    ULONGLONG TotalSeconds, ElapsedDays, TotalSecondsOfDay;

    /* Validate pointers */
    if(XtTime == NULLPTR || TimeFields == NULLPTR)
    {
        /* Invalid pointers provided, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* The XT Epoch starts at 1601, negative absolute system time is not supported */
    if(XtTime->QuadPart < 0)
    {
        /* Invalid time value, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Extract ticks into whole seconds and remaining milliseconds */
    TotalSeconds = XtTime->QuadPart / TIME_TICKS_PER_SECOND;
    TimeFields->Milliseconds = (SHORT)((XtTime->QuadPart % TIME_TICKS_PER_SECOND) / TIME_TICKS_PER_MILLISECOND);

    /* Split total seconds into absolute elapsed days and time of the current day */
    ElapsedDays = TotalSeconds / TIME_SECONDS_PER_DAY;
    TotalSecondsOfDay = TotalSeconds % TIME_SECONDS_PER_DAY;

    /* Calculate hour, minute, and second */
    TimeFields->Hour = (SHORT)(TotalSecondsOfDay / TIME_SECONDS_PER_HOUR);
    TimeFields->Minute = (SHORT)((TotalSecondsOfDay % TIME_SECONDS_PER_HOUR) / TIME_SECONDS_PER_MINUTE);
    TimeFields->Second = (SHORT)(TotalSecondsOfDay % TIME_SECONDS_PER_MINUTE);

    /* Calculate weekday */
    TimeFields->Weekday = (SHORT)((ElapsedDays + 1) % 7);

    /* Calculate the year using the Gregorian leap year cycles */
    Days = (ULONG)ElapsedDays;
    CurrentYear = 1601;

    /* Calculate completed 400-year periods */
    Periods400Years = Days / 146097;
    CurrentYear += Periods400Years * 400;
    Days %= 146097;

    /* Calculate completed 100-year periods */
    Periods100Years = Days / 36524;
    if(Periods100Years == 4)
    {
        /* The leap year at the very end of a 400-year cycle */
        Periods100Years = 3;
    }

    /* Update the current year and remaining days */
    CurrentYear += Periods100Years * 100;
    Days -= Periods100Years * 36524;

    /* Calculate completed 4-year periods */
    Periods4Years = Days / 1461;
    CurrentYear += Periods4Years * 4;
    Days %= 1461;

    /* Calculate completed 1-year periods */
    Periods1Year = Days / 365;
    if(Periods1Year == 4)
    {
        /* The leap year at the end of a normal 4-year cycle */
        Periods1Year = 3;
    }

    /* Update the current year and remaining days */
    CurrentYear += Periods1Year;
    Days -= Periods1Year * 365;

    /* Set the calculated year */
    TimeFields->Year = (SHORT)CurrentYear;

    /* Check if the calculated year is a leap year */
    Leap = LeapYear(TimeFields->Year) ? 1 : 0;
    TimeFields->Month = 1;

    /* Subtract days of each month to find the current month */
    while(Days >= DaysInMonth[Leap][TimeFields->Month - 1])
    {
        /* Calculate the days of the current month and advance the month index */
        Days -= DaysInMonth[Leap][TimeFields->Month - 1];
        TimeFields->Month++;
    }

    /* Store the remainder as the 1-based day of the month */
    TimeFields->Day = (SHORT)(Days + 1);

    /* Return success */
    return STATUS_SUCCESS;
}
