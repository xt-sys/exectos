/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/systime.cc
 * DESCRIPTION:     Timebase and system clock support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Returns the current system time.
 *
 * @param SystemTime
 *        Supplies a pointer to a variable that receives the current system time.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SystemTime::GetSystemTime(OUT PLARGE_INTEGER SystemTime)
{
    LARGE_INTEGER CurrentTime;

    /* Fetch the time using the lock-free shared data mechanism and return it */
    CurrentTime = KE::SharedData::GetSystemTime();
    SystemTime->QuadPart = CurrentTime.QuadPart;
}

/**
 * Sets the system time, updates the boot time, and optionally updates the hardware Real-Time Clock (RTC).
 *
 * @param NewTime
 *        Supplies a pointer to the new system time.
 *
 * @param OldTime
 *        Supplies a pointer to a variable that receives the previous system time.
 *
 * @param AdjustInterruptTime
 *        Specifies if the physical interrupt time should be adjusted.
 *
 * @param WriteToRtc
 *        Specifies if the new system time should be written to RTC.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SystemTime::SetSystemTime(IN PLARGE_INTEGER NewTime,
                              OUT PLARGE_INTEGER OldTime,
                              IN BOOLEAN CorrectInterruptTime,
                              IN BOOLEAN WriteToRtc)
{
    LARGE_INTEGER TimeDelta;
    TIME_FIELDS TimeFields;

    /* Check if the new system time should be written to the RTC */
    if(WriteToRtc)
    {
        /* Convert the new system time to a human-readable calendar structure */
        RTL::Time::XtEpochToTimeFields(NewTime, &TimeFields);
    }

    /* Start a guarded code block */
    {
        /* Raise runlevel to HIGH level */
        KE::RaiseRunLevel Runlevel(HIGH_LEVEL);

        /* Save the previous system time for the caller and for delta calculation */
        GetSystemTime(OldTime);

        /* Write new system time to the Kernel Shared Data */
        KE::SharedData::SetSystemTime(*NewTime);

        /* Check if the new system time should be written to the RTC */
        if(WriteToRtc)
        {
            /* Update the hardware CMOS clock */
            HL::Rtc::SetRealTimeClock(&TimeFields);
        }

        /* Calculate the time difference */
        TimeDelta.QuadPart = NewTime->QuadPart - OldTime->QuadPart;

        /* Update the boot time by the delta */
        BootTime.QuadPart += TimeDelta.QuadPart;
    }

    /* Check if interrupt time needs to be corrected */
    if(CorrectInterruptTime)
    {
        UNIMPLEMENTED;
    }
}
