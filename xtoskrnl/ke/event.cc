/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/event.cc
 * DESCRIPTION:     Kernel events support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

/**
 * Clears the signal state of the event.
 *
 * @param Event
 *        Supplies a pointer to the event object.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
Event::ClearEvent(IN PKEVENT Event)
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
 * @since NT 3.5
 */
XTAPI
VOID
Event::InitializeEvent(OUT PKEVENT Event,
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
 * @since NT 3.5
 */
XTAPI
LONG
Event::SetEvent(IN PKEVENT Event,
                IN KPRIORITY Increment,
                IN BOOLEAN Wait)
{
    UNIMPLEMENTED;

    return 0;
}

} /* namespace */
