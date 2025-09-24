/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/spinlock.cc
 * DESCRIPTION:     Spinlocks support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
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
