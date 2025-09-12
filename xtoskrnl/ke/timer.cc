/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/timer.cc
 * DESCRIPTION:     Kernel timer object support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Cancels the timer.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @return This routine returns TRUE if the cancelled timer was set, or FALSE otherwise.
 *
 * @since NT 3.5
 */
XTAPI
BOOLEAN
KE::Timer::CancelTimer(IN PKTIMER Timer)
{
    BOOLEAN Result;
    KRUNLEVEL RunLevel;

    /* Set default result value */
    Result = FALSE;

    /* Raise run level and acquire dispatcher lock */
    RunLevel = KeRaiseRunLevel(SYNC_LEVEL);
    SpinLock::AcquireQueuedSpinLock(DispatcherLock);

    /* Check timer status */
    if(Timer->Header.Inserted)
    {
        /* Remove the timer from the list */
        RemoveTimer(Timer);
        Result = TRUE;
    }

    /* Release dispatcher lock and process the deferred ready list */
    SpinLock::ReleaseQueuedSpinLock(DispatcherLock);
    KThread::ExitDispatcher(RunLevel);

    /* Return result */
    return Result;
}

/**
 * Clears the signal state of the timer.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @return This routine does not return any value.
 *
 * @since NT 4.0
 */
XTAPI
VOID
KE::Timer::ClearTimer(IN PKTIMER Timer)
{
    /* Clear signal state */
    Timer->Header.SignalState = 0;
}

/**
 * Reads the current signal state of the given timer.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @return This routine returns TRUE if the timer is set, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
KE::Timer::GetState(IN PKTIMER Timer)
{
    /* Return timer state */
    return (BOOLEAN)Timer->Header.SignalState;
}

/**
 * Initializes an extended kernel timer.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @param Type
 *        Supplies the type of the timer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Timer::InitializeTimer(OUT PKTIMER Timer,
                           IN KTIMER_TYPE Type)
{
    /* Initialize the header */
    Timer->Header.Type = TimerNotificationObject + (UCHAR)Type;
    Timer->Header.Inserted = 0;
    Timer->Header.SignalState = 0;

    /* Initialize the timer data */
    Timer->DueTime.QuadPart = 0;
    Timer->Period = 0;

    /* Initialize linked lists */
    RtlInitializeListHead(&Timer->Header.WaitListHead);
    RtlInitializeListHead(&Timer->TimerListEntry);
}

/**
 * Queries the timer's interrupt due time.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @return This routine returns the time remaining on the timer, or 0 if the timer is not set.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
KE::Timer::QueryTimer(IN PKTIMER Timer)
{
    KRUNLEVEL RunLevel;
    ULONGLONG DueTime;

    /* Set initial due time */
    DueTime = 0;

    /* Raise run level and acquire dispatcher lock */
    RunLevel = KeRaiseRunLevel(SYNC_LEVEL);
    SpinLock::AcquireQueuedSpinLock(DispatcherLock);

    /* Check timer status */
    if(Timer->Header.Inserted)
    {
        /* Get timer's due time */
        DueTime = Timer->DueTime.QuadPart;
    }

    /* Release dispatcher lock and process the deferred ready list */
    SpinLock::ReleaseQueuedSpinLock(DispatcherLock);
    KThread::ExitDispatcher(RunLevel);

    /* Return timer's due time */
    return DueTime;
}

/**
 * Sets the supplied timer to expire at the specified time.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @param DueTime
 *        Supplies the time at which the timer should expire (both absolute and relative times are supported).
 *
 * @param Period
 *        Supplies the timer period.
 *
 * @param Dpc
 *        Supplies a pointer to a Deferred Procedure Call (DPC) object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Timer::SetTimer(IN PKTIMER Timer,
                    IN LARGE_INTEGER DueTime,
                    IN LONG Period,
                    IN PKDPC Dpc)
{
    UNIMPLEMENTED;
}

/**
 * Removes a specified timer from the timer list.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Timer::RemoveTimer(IN OUT PKTIMER Timer)
{
    /* Remove the timer from the list */
    Timer->Header.Inserted = FALSE;
    RtlRemoveEntryList(&Timer->TimerListEntry);
}
