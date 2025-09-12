/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/semphore.cc
 * DESCRIPTION:     Semaphores support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Initializes a kernel semaphore object.
 *
 * @param Semaphore
 *        Supplies a pointer to a semaphore object.
 *
 * @param Count
 *        Specifies the initial count value of the semaphore.
 *
 * @param Limit
 *        Specifies a maximum count value of the semaphore.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
KE::Semaphore::InitializeSemaphore(IN PKSEMAPHORE Semaphore,
                                   IN LONG Count,
                                   IN LONG Limit)
{
    /* Initialize semaphore header and limit */
    Semaphore->Header.Type = SemaphoreObject;
    Semaphore->Header.SignalState = Count;
    Semaphore->Limit = Limit;

    /* Initialize semaphore wait list */
    RtlInitializeListHead(&Semaphore->Header.WaitListHead);
}

/**
 * Reads semaphore's current signal state.
 *
 * @param Semaphore
 *        Supplies a pointer to a semaphore object.
 *
 * @return This routine returns the current signal state of the semaphore.
 *
 * @since XT 1.0
 */
XTAPI
LONG
KE::Semaphore::ReadState(IN PKSEMAPHORE Semaphore)
{
    /* Return semaphore's signal state */
    return Semaphore->Header.SignalState;
}

/**
 * Releases a semaphore.
 *
 * @param Semaphore
 *        Supplies a pointer to a semaphore object.
 *
 * @param Increment
 *        Specifies the priority increment value of the semaphore.
 *
 * @param Adjustment
 *        Specifies adjustment value added to the semaphore's initial count value.
 *
 * @param Wait
 *        Determines whether release of the semaphore will be followed by a kernel wait routine call or not.
 *
 * @return This routine returns a previous signal state of the semaphore.
 *
 * @since NT 3.5
 */
XTAPI
LONG
KE::Semaphore::ReleaseSemaphore(IN PKSEMAPHORE Semaphore,
                                IN KPRIORITY Increment,
                                IN LONG Adjustment,
                                IN BOOLEAN Wait)
{
    UNIMPLEMENTED;
    return 0;
}
