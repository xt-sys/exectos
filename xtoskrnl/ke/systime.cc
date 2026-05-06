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
 * @param CorrectInterruptTime
 *        Specifies if the physical interrupt time should be corrected.
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

/**
 * Sets the maximum and minimum time increment values in 100ns units.
 *
 * @param MinIncrement
 *        Supplies the minimum time increment.
 *
 * @param MaxIncrement
 *        Supplies the maximum time increment.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SystemTime::SetTimeIncrement(IN ULONG MinIncrement,
                                 IN ULONG MaxIncrement)
{
    /* Store resolution boundaries while enforcing a 1ms architectural floor */
    MaximumIncrement = MaxIncrement;
    MinimumIncrement = MAX(MinIncrement, 10000);

    /* Set the tick offset to the maximum increment */
    TickOffset = (LONG)MaxIncrement;

    /* Set the default time adjustment to the full tick period */
    TimeAdjustment = MaxIncrement;
}

/**
 * Services the periodic clock interrupt by advancing the global interrupt time, detecting full
 * system tick boundaries via the tick offset accumulator, and advancing the wall-clock system time.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame representing the interrupted execution context.
 *
 * @param Increment
 *        Supplies the calibrated time delta for this hardware interrupt in 100-nanosecond units.
 *
 * @param RunLevel
 *        Supplies the system run level at which the interrupt was taken.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::SystemTime::UpdateSystemTime(IN PKTRAP_FRAME TrapFrame,
                                 IN ULONG Increment,
                                 IN KRUNLEVEL RunLevel)
{
    LARGE_INTEGER InterruptTime, SystemTime;
    LONG CurrentTickOffset;

    /* Advance the global interrupt time on every hardware tick */
    InterruptTime = KE::SharedData::GetInterruptTime();
    InterruptTime.QuadPart += Increment;
    KE::SharedData::SetInterruptTime(InterruptTime);

    /* Atomically consume the current tick budget and retrieve the pre-decrement value */
    CurrentTickOffset = RTL::Atomic::ExchangeAdd32((PLONG)&TickOffset, -(LONG)Increment);

    /* Determine whether the accumulated increments have crossed the full tick boundary */
    if(CurrentTickOffset <= (LONG)Increment)
    {
        /* A full system tick has elapsed, advance the wall-clock time by the configured adjustment */
        SystemTime = KE::SharedData::GetSystemTime();
        SystemTime.QuadPart += TimeAdjustment;
        KE::SharedData::SetSystemTime(SystemTime);

        /* Update the tick count */
        KE::SharedData::IncrementTickCount();

        /* Reload the tick offset accumulator for the next full tick period */
        TickOffset += MaximumIncrement;

        /* Update processor and thread runtime accounting */
        KE::Dispatcher::UpdateRunTime(TrapFrame, RunLevel);
    }
}
