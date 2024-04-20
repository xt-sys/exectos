/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/timer.c
 * DESCRIPTION:     Kernel timer object support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
KeCancelTimer(IN PKTIMER Timer)
{
    BOOLEAN Result;
    KRUNLEVEL OldRunLevel;

    /* Set default result value */
    Result = FALSE;

    /* Raise run level and acquire dispatcher lock */
    OldRunLevel = KeRaiseRunLevel(SYNC_LEVEL);
    KeAcquireQueuedSpinLock(DispatcherLock);

    /* Check timer status */
    if(Timer->Header.Inserted)
    {
        /* Remove the timer from the list */
        KepRemoveTimer(Timer);
        Result = TRUE;
    }

    /* Release dispatcher lock and processes the deferred ready list */
    KeReleaseQueuedSpinLock(DispatcherLock);
    KepExitDispatcher(OldRunLevel);

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
KeClearTimer(IN PKTIMER Timer)
{
    /* Clear signal state */
    Timer->Header.SignalState = 0;
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
KeInitializeTimer(OUT PKTIMER Timer,
                  IN KTIMER_TYPE Type)
{
    /* Initialize the header */
    Timer->Header.Type = TimerNotificationObject + Type;
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
KepRemoveTimer(IN OUT PKTIMER Timer)
{
    /* Remove the timer from the list */
    Timer->Header.Inserted = FALSE;
    RtlRemoveEntryList(&Timer->TimerListEntry);
}
