/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/spinlock.cc
 * DESCRIPTION:     Spinlocks support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Acquires a specified queued spinlock.
 *
 * @param LockLevel
 *        Supplies the queued spinlock level.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::SpinLock::AcquireQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel)
{
    /* Acquire the queued spinlock */
    AcquireSpinLock(KE::Processor::GetCurrentProcessorControlBlock()->LockQueue[LockLevel].Lock);
}

/**
 * Acquires a kernel spin lock.
 *
 * @param SpinLock
 *        Supplies a pointer to the kernel spin lock.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::SpinLock::AcquireSpinLock(IN OUT PKSPIN_LOCK SpinLock)
{
    /* Try to acquire the lock */
    while(RTL::Atomic::BitTestAndSet((PLONG)SpinLock, 0))
    {
        /* Wait until locked is cleared */
        while(*(VOLATILE PKSPIN_LOCK)SpinLock & 1)
        {
                /* Yield processor and keep waiting */
                AR::CpuFunc::YieldProcessor();
        }
    }

    /* Add an explicit memory barrier */
    AR::CpuFunc::ReadWriteBarrier();
}

/**
 * Initializes all kernel spinlocks.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SpinLock::InitializeAllLocks()
{
    /* Initialize all spin locks */
    InitializeSpinLock(&DispatcherLockQueue);
    InitializeSpinLock(&PfnLockQueue);
    InitializeSpinLock(&SystemSpaceLockQueue);
}

/**
 * Initializes spinlock queues for current processor.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SpinLock::InitializeLockQueues()
{
    PKPROCESSOR_CONTROL_BLOCK ControlBlock;

    /* Get current processor control block */
    ControlBlock = KE::Processor::GetCurrentProcessorControlBlock();

    /* Initialize PCB lock queues */
    ControlBlock->LockQueue[DispatcherLock].Lock = &DispatcherLockQueue;
    ControlBlock->LockQueue[DispatcherLock].Next = NULLPTR;
    ControlBlock->LockQueue[ExpansionLock].Lock = &ExpansionLockQueue;
    ControlBlock->LockQueue[ExpansionLock].Next = NULLPTR;
    ControlBlock->LockQueue[PfnLock].Lock = &PfnLockQueue;
    ControlBlock->LockQueue[PfnLock].Next = NULLPTR;
    ControlBlock->LockQueue[SystemSpaceLock].Lock = &SystemSpaceLockQueue;
    ControlBlock->LockQueue[SystemSpaceLock].Next = NULLPTR;
    ControlBlock->LockQueue[VacbLock].Lock = &VacbLockQueue;
    ControlBlock->LockQueue[VacbLock].Next = NULLPTR;
    ControlBlock->LockQueue[MasterLock].Lock = &MasterLockQueue;
    ControlBlock->LockQueue[MasterLock].Next = NULLPTR;
    ControlBlock->LockQueue[NonPagedAllocPoolLock].Lock = &NonPagedAllocLockQueue;
    ControlBlock->LockQueue[NonPagedAllocPoolLock].Next = NULLPTR;
    ControlBlock->LockQueue[IoCancelLock].Lock = &IoCancelLockQueue;
    ControlBlock->LockQueue[IoCancelLock].Next = NULLPTR;
    ControlBlock->LockQueue[WorkQueueLock].Lock = &WorkLockQueue;
    ControlBlock->LockQueue[WorkQueueLock].Next = NULLPTR;
    ControlBlock->LockQueue[IoVpbLock].Lock = &IoVpbLockQueue;
    ControlBlock->LockQueue[IoVpbLock].Next = NULLPTR;
    ControlBlock->LockQueue[IoDatabaseLock].Lock = &IoDatabaseLockQueue;
    ControlBlock->LockQueue[IoDatabaseLock].Next = NULLPTR;
    ControlBlock->LockQueue[IoCompletionLock].Lock = &IoCompletionLockQueue;
    ControlBlock->LockQueue[IoCompletionLock].Next = NULLPTR;
    ControlBlock->LockQueue[FileSystemLock].Lock = &FileSystemLockQueue;
    ControlBlock->LockQueue[FileSystemLock].Next = NULLPTR;
    ControlBlock->LockQueue[AfdWorkQueueLock].Lock = NULLPTR;
    ControlBlock->LockQueue[AfdWorkQueueLock].Next = NULLPTR;
    ControlBlock->LockQueue[BcbLock].Lock = NULLPTR;
    ControlBlock->LockQueue[BcbLock].Next = NULLPTR;
    ControlBlock->LockQueue[NonPagedPoolLock].Lock = &NonPagedPoolLockQueue;
    ControlBlock->LockQueue[NonPagedPoolLock].Next = NULLPTR;
    ControlBlock->LockQueue[ReservedSystemLock].Lock = NULLPTR;
    ControlBlock->LockQueue[ReservedSystemLock].Next = NULLPTR;
    ControlBlock->LockQueue[TimerTableLock].Lock = &TimerTableLockQueue;
    ControlBlock->LockQueue[TimerTableLock].Next = NULLPTR;
}

/**
 * Initializes a kernel spinlock object.
 *
 * @param SpinLock
 *        Supplies a pointer to a kernel spin lock.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
KE::SpinLock::InitializeSpinLock(IN PKSPIN_LOCK SpinLock)
{
    /* Zero initialize spinlock */
    *SpinLock = 0;
}

/**
 * Releases a queued spinlock.
 *
 * @param LockLevel
 *        Supplies the queued spinlock level.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::SpinLock::ReleaseQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel)
{
    /* Clear the lock */
    ReleaseSpinLock(KE::Processor::GetCurrentProcessorControlBlock()->LockQueue[LockLevel].Lock);
}

/**
 * Releases a kernel spin lock.
 *
 * @param SpinLock
 *        Supplies a pointer to the kernel spin lock.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::SpinLock::ReleaseSpinLock(IN OUT PKSPIN_LOCK SpinLock)
{
    /* Clear the lock */
    RTL::Atomic::And32((PLONG)SpinLock, 0);

    /* Add an explicit memory barrier */
    AR::CpuFunc::ReadWriteBarrier();
}

/**
 * Tests a kernel spin lock.
 *
 * @param SpinLock
 *        Supplies a pointer to the kernel spin lock.
 *
 * @return This routine returns TRUE if the lock is free, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
TestSpinLock(IN PKSPIN_LOCK SpinLock)
{
    /* Check if the lock is free */
    if(*SpinLock)
    {
        /* Spinlock is busy, yield processor and return FALSE */
        AR::CpuFunc::YieldProcessor();
        return FALSE;
    }

    /* Spinlock is free, return TRUE */
    return TRUE;
}
