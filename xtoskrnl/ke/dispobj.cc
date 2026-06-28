/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/dispobj.cc
 * DESCRIPTION:     Kernel Thread Dispatcher Objects
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Satisfies a wait operation specifically for a Mutex object.
 *
 * @param Mutex
 *        Supplies a pointer to the Mutex object being acquired.
 *
 * @param Thread
 *        Supplies a pointer to the thread that is acquiring the Mutex.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::DispatcherObject::SatisfyWaitingMutexObject(IN PKMUTEX Mutex,
                                                IN PKTHREAD Thread)
{
    /* Decrement the signal state */
    Mutex->Header.SignalState--;

    /* Check if the mutex is now fully acquired */
    if(!Mutex->Header.SignalState)
    {
        /* Bind the mutex ownership to the current acquiring thread */
        Mutex->OwnerThread = Thread;

        /* Inherit the APC disable from the mutex */
        Thread->KernelApcDisable = Thread->KernelApcDisable - Mutex->ApcDisable;

        /* Check if the previous owner terminated without releasing the mutex */
        if(Mutex->Abandoned)
        {
            /* Clear the abandonment flag */
            Mutex->Abandoned = FALSE;

            /* Alert the thread */
            Thread->WaitStatus = STATUS_ABANDONED;
        }

        /* Track this mutex in the thread's local list */
        RTL::LinkedList::InsertHeadList(Thread->MutexListHead.Blink, &Mutex->MutexListEntry);
    }
}

/**
 * Satisfies a wait operation for non-mutex dispatcher objects, such as Synchronization Events and Semaphores.
 *
 * @param Object
 *        Supplies a pointer to the dispatcher object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::DispatcherObject::SatisfyWaitingNonMutexObject(IN PDISPATCHER_HEADER Object)
{
    /* Check if the object is a Synchronization Event */
    if((Object->Type & DISPATCHER_OBJECT_TYPE_MASK) == EventSynchronizationObject)
    {
        /* Reset the event */
        Object->SignalState = 0;
    }
    else if(Object->Type == SemaphoreObject)
    {
        /* Decrement the signal state */
        Object->SignalState--;
    }
}

/**
 * Evaluates the object type and routes the wait satisfaction logic to the appropriate specialized handler.
 *
 * @param Object
 *        Supplies a pointer to the dispatcher object being acquired.
 *
 * @param Thread
 *        Supplies a pointer to the thread acquiring the object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::DispatcherObject::SatisfyWaitingObject(IN PDISPATCHER_HEADER Object,
                                           IN PKTHREAD Thread)
{
    /* Check if the object is a Mutex */
    if(Object->Type == MutexObject)
    {
        /* Apply mutex-specific acquisition rules */
        SatisfyWaitingMutexObject((PKMUTEX)Object, Thread);
    }
    else
    {
        /* Apply generic acquisition rules for events, semaphores, and timers */
        SatisfyWaitingNonMutexObject(Object);
    }
}
