/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/semphore.c
 * DESCRIPTION:     Semaphores support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * initializes a kernel semaphore object.
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
KeInitializeSemaphore(IN PKSEMAPHORE Semaphore,
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
