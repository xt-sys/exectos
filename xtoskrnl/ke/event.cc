/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/event.cc
 * DESCRIPTION:     Kernel events support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Clears the signal state of the event.
 *
 * @param Event
 *        Supplies a pointer to the event object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Event::ClearEvent(IN PKEVENT Event)
{
    /* Clear event's signal state */
    Event->Header.SignalState = FALSE;
}

/**
 * Initializes a kernel event.
 *
 * @param Event
 *        Supplies a pointer to the event object.
 *
 * @param EventType
 *        Specifies an event type.
 *
 * @param InitialState
 *        Specifies the initial signal state of the event.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Event::InitializeEvent(OUT PKEVENT Event,
                           IN KEVENT_TYPE EventType,
                           IN BOOLEAN InitialState)
{
    /* Initialize event dispatcher header */
    Event->Header.Type = EventType;
    Event->Header.SignalState = InitialState;

    /* Initialize event wait list */
    RtlInitializeListHead(&Event->Header.WaitListHead);
}

/**
 * Sets new signal state and satisfy waits if possible.
 *
 * @param Event
 *        Supplies a pointer to the event object.
 *
 * @param Increment
 *        Specifies an event priority boost value.
 *
 * @param Wait
 *        Specifies whether to call kernel wait routines or not.
 *
 * @return This routine returns the previous signal state of the event.
 *
 * @since XT 1.0
 */
XTAPI
LONG
KE::Event::SetEvent(IN PKEVENT Event,
                    IN KPRIORITY Increment,
                    IN BOOLEAN Wait)
{
    UNIMPLEMENTED;

    return 0;
}

/**
 * Sets an event to the signaled state and attempts to boost the priority of the waiting thread.
 *
 * @param Event
 *        Supplies a pointer to the dispatcher object (Event) to be signaled.
 *
 * @param WaitingThread
 *        supplies an optional pointer to a variable that receives the unblocked thread.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Event::SetEventBoostPriority(IN PKEVENT Event,
                                 IN OUT PKTHREAD* WaitingThread)
{
    /* Not implemented, active polling only */
    UNIMPLEMENTED;

    /* Set the signal state */
    Event->Header.SignalState = 1;

    /* Check if the caller requested the unblocked thread */
    if(WaitingThread != NULLPTR)
    {
        /* Return NULLPTR for now */
        *WaitingThread = NULLPTR;
    }
}
