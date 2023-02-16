/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/timer.c
 * DESCRIPTION:     Kernel timer object support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes a kernel timer.
 *
 * @param Timer
 *        Supplies a pointer to a time object.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
KeInitializeTimer(OUT PKTIMER Timer)
{
    /* Initialize the timer */
    KeInitializeTimerEx(Timer, NotificationTimer);
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
 * @since NT 3.5
 */
XTAPI
VOID
KeInitializeTimerEx(OUT PKTIMER Timer,
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
