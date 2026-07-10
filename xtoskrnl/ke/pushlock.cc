/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/pushlock.cc
 * DESCRIPTION:     XT Kernel Push Lock support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Attempts to acquire a push lock for exclusive access via the fast-path.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be acquired.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::AcquireExclusivePushLock(IN PKPUSH_LOCK PushLock)
{
    /* Attempt to swap a NULL pointer with the EXCLUSIVE flag */
    if(RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, NULLPTR, (PVOID)KPUSHLOCK_LOCK))
    {
        /* Lock contention detected, enter the slow-path */
        AcquireWaitExclusivePushLock(PushLock);
    }
}

/**
 * Attempts to acquire a push lock for shared access via the fast-path.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be acquired.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::AcquireSharedPushLock(PKPUSH_LOCK PushLock)
{
    /* Attempt to set the reader flag */
    if(RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, NULLPTR, (PVOID)(KPUSHLOCK_LOCK | KPUSHLOCK_INCREMENT_SHARED)))
    {
        /* Lock contention or existing readers detected, Enter the slow-path */
        AcquireWaitSharedPushLock(PushLock);
    }
}

/**
 * Enqueues the current thread's wait block into the lock's wait list, conditionally assumes responsibility
 * for list optimization and transitions the thread into a wait state until exclusive ownership is acquired.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be acquired.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::AcquireWaitExclusivePushLock(IN PKPUSH_LOCK PushLock)
{
    KPUSH_LOCK OldState, NewState, TempState;
    KPUSH_LOCK_WAIT_BLOCK WaitBlock;
    PVOID ActualState;
    BOOLEAN Optimize;

    /* Initialize a synchronization event */
    KE::Event::InitializeEvent(&WaitBlock.WakeEvent, SynchronizationEvent, FALSE);

    /* Capture the snapshot of the push lock pointer */
    OldState.Ptr = PushLock->Ptr;

    /* Enter contention loop */
    while(TRUE)
    {
        /* Check if the lock is free */
        if(!(OldState.Value & KPUSHLOCK_LOCK))
        {
            /* Assert the LOCK bit */
            NewState.Value = OldState.Value | KPUSHLOCK_LOCK;

            /* Commit the acquisition */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Lock acquired, return */
                return;
            }

            /* The lock state changed, retry */
            OldState.Ptr = ActualState;
            continue;
        }
        else
        {
            /* Prepare the wait block for enqueuing */
            WaitBlock.Flags = KPUSHLOCK_LOCK | KPUSHLOCK_WAITING;
            WaitBlock.Previous = NULLPTR;

            /* Assume no list optimization is required initially */
            Optimize = FALSE;

            /* Check if other threads are already queued */
            if(OldState.Value & KPUSHLOCK_WAITING)
            {
                /* Hook into the head of the existing LIFO list */
                WaitBlock.Last = NULLPTR;
                WaitBlock.Next = (PKPUSH_LOCK_WAIT_BLOCK)(OldState.Value & ~KPUSHLOCK_PTR_BITS);
                WaitBlock.ShareCount = 0;

                /* Construct the new pointer containing wait block and preserved flags */
                NewState.Value = (OldState.Value & KPUSHLOCK_MULTIPLE_SHARED) |
                                 KPUSHLOCK_LOCK |
                                 KPUSHLOCK_WAKING |
                                 KPUSHLOCK_WAITING |
                                 (ULONG_PTR)&WaitBlock;

                /* Check if no one is currently organizing the list */
                if(!(OldState.Value & KPUSHLOCK_WAKING))
                {
                    /* Take responsibility for optimizing the wait list */
                    Optimize = TRUE;
                }
            }
            else
            {
                /* Mark this block as the tail of the new wait list and preserve the shared reader count */
                WaitBlock.Last = &WaitBlock;
                WaitBlock.ShareCount = (ULONG)OldState.Shared;

                /* Check if multiple readers currently hold the lock */
                if(OldState.Shared > 1)
                {
                    /* Inject the wait block and preserve the multiple shared state */
                    NewState.Value = KPUSHLOCK_MULTIPLE_SHARED | KPUSHLOCK_LOCK | KPUSHLOCK_WAITING | (ULONG_PTR)&WaitBlock;
                }
                else
                {
                    /* Clear the share count for a single owner */
                    WaitBlock.ShareCount = 0;

                    /* Inject the wait block for a single owner scenario */
                    NewState.Value = KPUSHLOCK_LOCK | KPUSHLOCK_WAITING | (ULONG_PTR)&WaitBlock;
                }
            }

            /* insert wait block into the PushLock */
            TempState.Ptr = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(TempState.Ptr != OldState.Ptr)
            {
                /* Lost the queue race, retry */
                OldState.Ptr = TempState.Ptr;
                continue;
            }

            /* Check if this thread took the optimization role */
            if(Optimize)
            {
                /* Convert the LIFO wait list into a FIFO list */
                OptimizePushLockList(PushLock, NewState);
            }

            /* Block the thread */
            SpinPushLock(&WaitBlock);

            /* Reset state and retry */
            WaitBlock.ShareCount = 0;
            OldState.Ptr = PushLock->Ptr;
        }
    }
}

/**
 * Enqueues the current thread into the wait list if the lock is held exclusively or if a writer is already waiting.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be acquired.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::AcquireWaitSharedPushLock(IN PKPUSH_LOCK PushLock)
{
    KPUSH_LOCK OldState, NewState, TempState;
    KPUSH_LOCK_WAIT_BLOCK WaitBlock;
    PVOID ActualState;
    BOOLEAN Optimize;

    /* Initialize a synchronization event */
    KE::Event::InitializeEvent(&WaitBlock.WakeEvent, SynchronizationEvent, FALSE);

    /* Capture the snapshot of the push lock pointer */
    OldState.Ptr = PushLock->Ptr;

    /* Enter contention loop */
    while(TRUE)
    {
        /* Check if the lock is free */
        if(!(OldState.Value & (KPUSHLOCK_LOCK | KPUSHLOCK_WAITING)))
        {
            /* Increment the shared reader count */
            NewState.Value = OldState.Value + KPUSHLOCK_INCREMENT_SHARED;

            /* Commit the shared acquisition */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Lock acquired, return */
                return;
            }

            /* The lock state changed, retry */
            OldState.Ptr = ActualState;
            continue;
        }
        else
        {
            /* Prepare the wait block for enqueuing as a reader */
            WaitBlock.Flags = KPUSHLOCK_WAITING;
            WaitBlock.Previous = NULLPTR;

            /* Assume no list optimization is required */
            Optimize = FALSE;

            /* Check if other threads are already queued */
            if(OldState.Value & KPUSHLOCK_WAITING)
            {
                /* Hook into the head of the existing LIFO wait list */
                WaitBlock.Last = NULLPTR;
                WaitBlock.Next = (PKPUSH_LOCK_WAIT_BLOCK)(OldState.Value & ~KPUSHLOCK_PTR_BITS);
                WaitBlock.ShareCount = 0;

                /* Construct the new pointer containing wait block and preserved flags */
                NewState.Value = (OldState.Value & KPUSHLOCK_MULTIPLE_SHARED) |
                                 KPUSHLOCK_LOCK |
                                 KPUSHLOCK_WAITING |
                                 (ULONG_PTR)&WaitBlock;

                /* Check if no one is currently organizing the list */
                if(!(OldState.Value & KPUSHLOCK_WAKING))
                {
                    /* Take responsibility for optimizing the wait list */
                    Optimize = TRUE;
                }
            }
            else
            {
                /* Mark this block as the tail of the new wait list and preserve the shared reader count */
                WaitBlock.Last = &WaitBlock;
                WaitBlock.ShareCount = (ULONG)OldState.Shared;

                /* Check if multiple readers currently hold the lock */
                if(OldState.Shared > 1)
                {
                    /* Inject the wait block and preserve the multiple shared state */
                    NewState.Value = KPUSHLOCK_MULTIPLE_SHARED | KPUSHLOCK_LOCK | KPUSHLOCK_WAITING | (ULONG_PTR)&WaitBlock;
                }
                else
                {
                    /* Inject the wait block for a single owner */
                    NewState.Value = KPUSHLOCK_LOCK | KPUSHLOCK_WAITING | (ULONG_PTR)&WaitBlock;
                }
            }

            /* Insert the wait block into the PushLock */
            TempState.Ptr = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(TempState.Ptr != OldState.Ptr)
            {
                /* Lost the queue race, retry */
                OldState.Ptr = TempState.Ptr;
                continue;
            }

            /* Check if this thread took the optimization role */
            if(Optimize)
            {
                /* Convert the LIFO wait list into a FIFO list */
                OptimizePushLockList(PushLock, NewState);
            }

            /* Block the thread */
            SpinPushLock(&WaitBlock);

            /* Exit loop */
            return;
        }
    }
}

/**
 * Prepares a push lock wait block and enqueues it onto the lock's contention stack.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock being contended.
 *
 * @param WaitBlock
 *        Supplies a pointer to the caller-allocated wait block to be enqueued.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::BlockPushLock(IN PKPUSH_LOCK PushLock,
                            IN PKPUSH_LOCK_WAIT_BLOCK WaitBlock)
{
    PVOID OldValue;

    /* Initialize the synchronization event */
    KE::Event::InitializeEvent(&WaitBlock->WakeEvent, SynchronizationEvent, FALSE);

    /* Set the wait flag */
    WaitBlock->Flags = KPUSHLOCK_WAITING;

    /* Enter a retry loop */
    while(TRUE)
    {
        /* Capture the current head of the wait stack */
        OldValue = *(VOLATILE PVOID *)&PushLock->Ptr;

        /* Link new wait block to the captured head */
        WaitBlock->Next = (PKPUSH_LOCK_WAIT_BLOCK)OldValue;

        /* Attempt to swap the lock's head pointer*/
        if (RTL::Atomic::CompareExchangePointer((PVOID *)&PushLock->Ptr, (PVOID)WaitBlock, OldValue) == OldValue)
        {
            /* Successfully enqueued the wait block, return */
            return;
        }
    }
}

/**
 * Attempts to convert a push lock from shared to exclusive access.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock structure.
 *
 * @return This routine returns TRUE if the lock was successfully converted to exclusive mode, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
KE::PushLock::ConvertSharedPushLockToExclusive(IN PKPUSH_LOCK PushLock)
{
    PVOID OldValue;

    /* Swap the push lock state */
    OldValue = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr,
                                                   (PVOID)KPUSHLOCK_INCREMENT_SHARED,
                                                   (PVOID)KPUSHLOCK_LOCK);

    /* Return conversion result */
    return (OldValue == (PVOID)KPUSHLOCK_INCREMENT_SHARED);
}

/**
 * Initializes a push lock.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be initialized.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 */
XTFASTCALL
VOID
KE::PushLock::InitializePushLock(IN PKPUSH_LOCK PushLock)
{
    /* Initialize the push lock state */
    PushLock->Value = 0;
}

/**
 * Optimizes the push lock wait list by converting it from a singly-linked LIFO list into a doubly-linked FIFO list.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock whose list is being optimized.
 *
 * @param OldValue
 *        Supplies the snapshot of the push lock state containing the pointer to the list head.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTINLINE
VOID
KE::PushLock::OptimizePushLockList(IN PKPUSH_LOCK PushLock,
                                   IN KPUSH_LOCK OldValue)
{
    PKPUSH_LOCK_WAIT_BLOCK CurrentBlock, NextBlock;

    /* Extract the pointer to the head of the wait block list */
    CurrentBlock = (PKPUSH_LOCK_WAIT_BLOCK)(OldValue.Value & ~KPUSHLOCK_PTR_BITS);

    /* Ensure the list is not empty */
    if(CurrentBlock)
    {
        /* Traverse the list */
        while((NextBlock = CurrentBlock->Next))
        {
            /* Link the next block back to the current block */
            NextBlock->Previous = CurrentBlock;
            CurrentBlock = NextBlock;
        }
    }
}

/**
 * Releases a push lock that was acquired for exclusive access.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::ReleaseExclusivePushLock(IN PKPUSH_LOCK PushLock)
{
    KPUSH_LOCK OldState;

    /* Subtract the LOCK bit */
    OldState.Value = RTL::Atomic::ExchangeAdd64((PLONG_PTR)&PushLock->Value, -(SSIZE_T)KPUSHLOCK_LOCK);

    /* Check if waiters are queued and no one is currently waking them */
    if((OldState.Value & KPUSHLOCK_WAITING) && !(OldState.Value & KPUSHLOCK_WAKING))
    {
        /* Initiate the wake sequence */
        TryWakePushLock(PushLock);
    }
}

/**
 * Attempts to release the lock immediately via the fast-path.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::ReleasePushLock(IN PKPUSH_LOCK PushLock)
{
    KPUSH_LOCK NewState, OldState;

    /* Capture the snapshot of the push lock pointer */
    OldState.Ptr = PushLock->Ptr;

    /* Check if the push lock is held by multiple readers */
    if(OldState.Value & KPUSHLOCK_MULTIPLE_SHARED)
    {
        /* Decrement the shared reader count */
        NewState.Value = OldState.Value - KPUSHLOCK_INCREMENT_SHARED;
    }
    else
    {
        /* Clear the push lock state */
        NewState.Value = 0;
    }

    /* Check if there are waiting threads or if the swap fails due to state changes */
    if((OldState.Value & KPUSHLOCK_WAITING) ||
       (RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr) != OldState.Ptr))
    {
        /* Contention or waiters present, execute the slow-path release */
        ReleaseWaitPushLock(PushLock);
    }
}

/**
 * Releases a push lock that was acquired for shared access.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTINLINE
XTFASTCALL
VOID
KE::PushLock::ReleaseSharedPushLock(IN PKPUSH_LOCK PushLock)
{
    /* Attempt to clear the lock */
    if(RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, (PVOID)(KPUSHLOCK_LOCK | KPUSHLOCK_INCREMENT_SHARED), NULLPTR)
                                           != (PVOID)(KPUSHLOCK_LOCK | KPUSHLOCK_INCREMENT_SHARED))
    {
        /* Contention or multiple readers detected, call slow-path */
        ReleaseWaitSharedPushLock(PushLock);
    }
}

/**
 * Drops the lock bit and transitions into the waking phase.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::ReleaseWaitExclusivePushLock(IN PKPUSH_LOCK PushLock)
{
    KPUSH_LOCK NewState, OldState;
    PVOID ActualState;

    /* Capture the snapshot of the push lock pointer */
    OldState.Ptr = PushLock->Ptr;

    /* Enter release loop */
    while(TRUE)
    {
        /* Strip the lock bit from the current state */
        NewState.Value = OldState.Value & ~KPUSHLOCK_LOCK;

        /* Check if waiters are queued and no thread is waking them */
        if((OldState.Value & KPUSHLOCK_WAITING) && !(OldState.Value & KPUSHLOCK_WAKING))
        {
            /* Add the waking flag to the state */
            NewState.Value |= KPUSHLOCK_WAKING;
        }

        /* Commit the modified state */
        ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
        if(ActualState == OldState.Ptr)
        {
            /* Verify if acquired the waking role */
            if((OldState.Value & KPUSHLOCK_WAITING) && !(OldState.Value & KPUSHLOCK_WAKING))
            {
                /* Execute the wake sequence */
                WakePushLockWaiters(PushLock, NewState);
            }
            return;
        }

        /* State changed, update snapshot and retry */
        OldState.Ptr = ActualState;
    }
}

/**
 * Handles decrements under contention and delegates to the wake mechanism if threads are queued
 * and the lock becomes fully available.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::ReleaseWaitPushLock(IN PKPUSH_LOCK PushLock)
{
    PKPUSH_LOCK_WAIT_BLOCK WaitBlock;
    KPUSH_LOCK NewState, OldState;
    PVOID ActualState;

    /* Capture the snapshot of the push lock pointer */
    OldState.Ptr = PushLock->Ptr;

    /* Enter release loop */
    while(TRUE)
    {
        /* Check if there are no waiters queued */
        if(!(OldState.Value & KPUSHLOCK_WAITING))
        {
            /* Check if the lock is held by multiple readers */
            if(OldState.Value & KPUSHLOCK_MULTIPLE_SHARED)
            {
                /* Decrement the shared reader count in the new state */
                NewState.Value = OldState.Value - KPUSHLOCK_INCREMENT_SHARED;
            }
            else
            {
                /* Clear the new state */
                NewState.Value = 0;
            }

            /* Commit the new state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Lock released, return */
                return;
            }

            /* State changed, retry */
            OldState.Ptr = ActualState;
            continue;
        }

        /* Check if multiple readers hold the lock while waiters are queued */
        if(OldState.Value & KPUSHLOCK_MULTIPLE_SHARED)
        {
            /* Extract the wait block list head */
            WaitBlock = (PKPUSH_LOCK_WAIT_BLOCK)(OldState.Value & ~KPUSHLOCK_PTR_BITS);

            /* Traverse to find the last wait block */
            while(WaitBlock->Next)
            {
                /* Check if the last block pointer is already cached */
                if(WaitBlock->Last)
                {
                    /* Jump directly to the cached last block */
                    WaitBlock = WaitBlock->Last;
                    break;
                }

                /* Advance to the next wait block */
                WaitBlock = WaitBlock->Next;
            }

            /* Check if the last block is a reader block */
            if(WaitBlock->ShareCount > 0)
            {
                /* Decrement the block's reader share count */
                if(RTL::Atomic::Decrement32((PLONG)&WaitBlock->ShareCount) > 0)
                {
                    /* There are still active readers, return */
                    return;
                }
            }
        }

        /* Strip the lock and multiple shared flags from the current state */
        NewState.Value = OldState.Value & ~(KPUSHLOCK_LOCK | KPUSHLOCK_MULTIPLE_SHARED);

        /* Check if another thread is already handling the wake process */
        if(OldState.Value & KPUSHLOCK_WAKING)
        {
            /* Commit the new state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Lock released, return */
                return;
            }
        }
        else
        {
            /* Add the waking flag to the new state */
            NewState.Value |= KPUSHLOCK_WAKING;

            /* Commit the new state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Assumed the waking role, execute wake sequence and return */
                WakePushLockWaiters(PushLock, NewState);
                return;
            }
        }

        /* State changed, update snapshot and retry */
        OldState.Ptr = ActualState;
    }
}

/**
 * Handles decrements under contention and delegates to the wake mechanism if needed.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::ReleaseWaitSharedPushLock(IN PKPUSH_LOCK PushLock)
{
    PKPUSH_LOCK_WAIT_BLOCK WaitBlock;
    KPUSH_LOCK NewState, OldState;
    PVOID ActualState;

    /* Capture the snapshot of the pointer */
    OldState.Ptr = PushLock->Ptr;

    /* Enter release loop */
    while(TRUE)
    {
        /* Check if there are no waiters currently queued */
        if(!(OldState.Value & KPUSHLOCK_WAITING))
        {
            /* Check if the lock is held by multiple readers */
            if(OldState.Value & KPUSHLOCK_MULTIPLE_SHARED)
            {
                /* Decrement the shared reader count in the new state */
                NewState.Value = OldState.Value - KPUSHLOCK_INCREMENT_SHARED;
            }
            else
            {
                /* Clear the new state completely */
                NewState.Value = 0;
            }

            /* Commit the new state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr) return;

            /* State changed, retry */
            OldState.Ptr = ActualState;
            continue;
        }

        /* Check if multiple readers hold the lock while waiters are queued */
        if(OldState.Value & KPUSHLOCK_MULTIPLE_SHARED)
        {
            /* Extract the wait block list head */
            WaitBlock = (PKPUSH_LOCK_WAIT_BLOCK)(OldState.Value & ~KPUSHLOCK_PTR_BITS);

            /* Traverse to find the last wait block */
            while(WaitBlock->Next)
            {
                /* Check if the last block pointer is already cached */
                if(WaitBlock->Last)
                {
                    /* Jump to the cached last block */
                    WaitBlock = WaitBlock->Last;
                    break;
                }

                /* Advance to the next wait block */
                WaitBlock = WaitBlock->Next;
            }

            /* Check if the last block is a reader block */
            if(WaitBlock->ShareCount > 0)
            {
                /* Decrement the block's reader share count */
                if(RTL::Atomic::Decrement32((PLONG)&WaitBlock->ShareCount) > 0) return;
            }
        }

        /* Strip the lock and multiple shared flags from the current state */
        NewState.Value = OldState.Value & ~(KPUSHLOCK_LOCK | KPUSHLOCK_MULTIPLE_SHARED);

        /* Check if another thread is already handling the wake process */
        if(OldState.Value & KPUSHLOCK_WAKING)
        {
            /* Commit the new state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr) return;
        }
        else
        {
            /* Add the waking flag to the new state */
            NewState.Value |= KPUSHLOCK_WAKING;

            /* Commit the new state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Execute wake sequence */
                WakePushLockWaiters(PushLock, NewState);
                return;
            }
        }

        /* State changed, update snapshot and retry */
        OldState.Ptr = ActualState;
    }
}

/**
 * Handles the blocking phase for a thread waiting on a push lock.
 *
 * @param WaitBlock
 *        Supplies a pointer to the thread's local wait block allocated on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTINLINE
VOID
KE::PushLock::SpinPushLock(IN PKPUSH_LOCK_WAIT_BLOCK WaitBlock)
{
    ULONG SpinCount;

    /* Initialize the spin count */
    SpinCount = KPUSH_LOCK_SPIN_COUNT;

    /* Enter adaptive Spin loop */
    while(SpinCount--)
    {
        /* Check if the unlocker cleared our wait flag */
        if(!(WaitBlock->Flags & KPUSHLOCK_WAITING))
        {
            /* Wait flag cleared, return */
            return;
        }

        /* Yield processor */
        AR::CpuFunctions::YieldProcessor();
    }

    /* Test and clear the WAIT bit */
    if(RTL::Atomic::BitTestAndReset((PLONG)&WaitBlock->Flags, 1))
    {
        /* Delegate to the Dispatcher */
        KE::Dispatcher::WaitForSingleObject(&WaitBlock->WakeEvent, WrPushLock, KernelMode, FALSE, NULLPTR);
    }
}

/**
 * Attempts to acquire a push lock for exclusive access without entering a wait state.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to be acquired.
 *
 * @return This routine returns TRUE if the exclusive lock was successfully acquired, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
KE::PushLock::TryAcquireExclusivePushLock(PKPUSH_LOCK PushLock)
{
    /* Acquire the lock */
    if(RTL::Atomic::BitTestAndSet((PLONG)&PushLock->Value, KPUSHLOCK_INDEX))
    {
        /* Lock not acquired, return FALSE */
        return FALSE;
    }

    /* Lock acquired, return TRUE */
    return TRUE;
}

/**
 * Initiates the wake sequence for queued waiters if the lock is free and no other thread is already performing
 * the wake operation.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to evaluate.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::TryWakePushLock(IN PKPUSH_LOCK PushLock)
{
    KPUSH_LOCK NewState, OldState;
    PVOID ActualState;

    /* Capture the snapshot of the pointer */
    OldState.Ptr = PushLock->Ptr;

    /* Enter evaluation loop */
    while(TRUE)
    {
        /* Check if already waking, locked, or no one is waiting */
        if((OldState.Value & KPUSHLOCK_WAKING) ||
           (OldState.Value & KPUSHLOCK_LOCK) ||
           !(OldState.Value & KPUSHLOCK_WAITING))
        {
            /* Abort */
            return;
        }

        /* Add the waking flag to the state */
        NewState.Value = OldState.Value | KPUSHLOCK_WAKING;

        /* Commit the waking state */
        ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
        if(ActualState == OldState.Ptr)
        {
            /* Assumed the waking role, execute wake sequence */
            WakePushLockWaiters(PushLock, NewState);
            return;
        }

        /* State changed, update snapshot and retry */
        OldState.Ptr = ActualState;
    }
}

/**
 * Unblocks an executive push lock and wakes up any pending threads waiting on it.
 *
 * @param PushLock
 *        Supplies a pointer to the executive push lock being unblocked.
 *
 * @param CurrentWaitBlock
 *        Optionally supplies a pointer to the current thread's push lock wait block.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::UnblockPushLock(IN PKPUSH_LOCK PushLock,
                              IN PKPUSH_LOCK_WAIT_BLOCK WaitBlock)
{
    PKPUSH_LOCK_WAIT_BLOCK CurrentWaitBlock, NextWaitBlock;

    /* Capture the entire chain of wait blocks and clear the push lock pointer */
    CurrentWaitBlock = (PKPUSH_LOCK_WAIT_BLOCK)RTL::Atomic::ExchangePointer((PVOID *)&PushLock->Ptr, NULLPTR);

    /* Check if any wait blocks are present */
    if(CurrentWaitBlock)
    {
        /* Raise runlevel to DISPATCH level */
        KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL, CurrentWaitBlock->Next);

        /* Iterate through the captured chain of wait blocks */
        while(CurrentWaitBlock)
        {
            /* Cache the next block pointer */
            NextWaitBlock = CurrentWaitBlock->Next;

            /* Test and clear the lock flag */
            if(!RTL::Atomic::BitTestAndReset((VOLATILE PLONG)&CurrentWaitBlock->Flags, KPUSHLOCK_LOCK))
            {
                /* Boost priority and signal the wake event */
                KE::Event::SetEventBoostPriority(&CurrentWaitBlock->WakeEvent, NULLPTR);
            }

            /* Advance to the next wait block */
            CurrentWaitBlock = NextWaitBlock;
        }
    }

    /* Check if a wait context was provided and its wait flag is pending */
    if(WaitBlock && (WaitBlock->Flags & KPUSHLOCK_WAITING))
    {
        /* Suspend the thread until push lock wake event is signaled */
        KE::Dispatcher::WaitForSingleObject(&WaitBlock->WakeEvent, WrPushLock, KernelMode, FALSE, NULLPTR);
    }
}

/**
 * Wakes threads waiting on a PushLock.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock being processed.
 *
 * @param OldState
 *        Supplies the current snapshot of the push lock state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::WakePushLockWaiters(IN PKPUSH_LOCK PushLock, IN KPUSH_LOCK OldState)
{
    PKPUSH_LOCK_WAIT_BLOCK FirstWaitBlock, PreviousWaitBlock, WaitBlock;
    KPUSH_LOCK NewState;
    PVOID ActualState;

    /* Loop while the push lock is held by another thread */
    while(OldState.Value & KPUSHLOCK_LOCK)
    {
        /* Strip the waking flag */
        NewState.Value = OldState.Value & ~KPUSHLOCK_WAKING;

        /* Commit the modified state */
        ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
        if(ActualState == OldState.Ptr)
        {
            /* Lock released, return */
            return;
        }

        /* Update the snapshot pointer */
        OldState.Ptr = ActualState;
    }

    /* Extract the first wait block pointer */
    FirstWaitBlock = (PKPUSH_LOCK_WAIT_BLOCK)(OldState.Value & ~KPUSHLOCK_PTR_BITS);
    WaitBlock = FirstWaitBlock;

    /* Loop through the wait block list */
    while(WaitBlock->Next)
    {
        /* Check if the last block pointer is cached */
        if(WaitBlock->Last)
        {
            /* Jump to the cached last block */
            WaitBlock = WaitBlock->Last;
            break;
        }

        /* Store the current block as the previous block */
        PreviousWaitBlock = WaitBlock;

        /* Advance to the next block and set the back-pointer to the previous block */
        WaitBlock = WaitBlock->Next;
        WaitBlock->Previous = PreviousWaitBlock;
    }

    /* Point WaitBlock to the oldest waiter */
    PreviousWaitBlock = WaitBlock->Previous;

    /* Check if the target is a reader block */
    if(!(WaitBlock->Flags & KPUSHLOCK_LOCK) || (PreviousWaitBlock == NULLPTR))
    {
        /* Enter clearing loop */
        while(TRUE)
        {
            /* Clear the new state */
            NewState.Value = 0;

            /* Commit the cleared state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Cleared the lock state, break the loop */
                break;
            }

            /* Update the snapshot pointer */
            OldState.Ptr = ActualState;
        }
    }
    else
    {
        /* Update the cached last block pointer and unlink the target block */
        FirstWaitBlock->Last = PreviousWaitBlock;
        WaitBlock->Previous = NULLPTR;

        /* Enter clearing loop */
        while(TRUE)
        {
            /* Strip the waking flag from the current state */
            NewState.Value = OldState.Value & ~KPUSHLOCK_WAKING;

            /* Commit the modified state */
            ActualState = RTL::Atomic::CompareExchangePointer(&PushLock->Ptr, OldState.Ptr, NewState.Ptr);
            if(ActualState == OldState.Ptr)
            {
                /* Cleared the waking flag, break the loop */
                break;
            }

            /* Update the snapshot pointer */
            OldState.Ptr = ActualState;
        }
    }

    /* Raise runlevel to DISPATCH level if there are multiple blocks */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL, WaitBlock->Previous != NULLPTR);

    /* Traverse the reversed list */
    while(WaitBlock)
    {
        /* Save previous wait block */
        PreviousWaitBlock = WaitBlock->Previous;

        /* Clear the WAIT flag */
        if(!RTL::Atomic::BitTestAndReset((PLONG)&WaitBlock->Flags, 1))
        {
            /* Thread is fully asleep, Signal its event and boost priority */
            KE::Event::SetEventBoostPriority(&WaitBlock->WakeEvent, NULLPTR);
        }

        /* Go to the next block */
        WaitBlock = PreviousWaitBlock;
    }
}

/**
 * Synchronizes the current thread with the PushLock.
 *
 * @param PushLock
 *        Supplies a pointer to the push lock to wait on.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::PushLock::WaitOnPushLock(IN PKPUSH_LOCK PushLock)
{
    KPUSH_LOCK State;

    /* Capture the snapshot of the pointer */
    State.Ptr = PushLock->Ptr;

    /* Verify if the lock is actively held by any thread */
    if(State.Value & KPUSHLOCK_LOCK)
    {
        /* Force serialization by acquiring the lock */
        AcquireExclusivePushLock(PushLock);

        /* Immediately release the lock */
        ReleaseExclusivePushLock(PushLock);
    }
}
