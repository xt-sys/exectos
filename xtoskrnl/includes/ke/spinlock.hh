/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/spinlock.hh
 * DESCRIPTION:     Spinlocks support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_SPINLOCK_HH
#define __XTOSKRNL_KE_SPINLOCK_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class SpinLock
    {
        private:
            STATIC KSPIN_LOCK DispatcherLockQueue;
            STATIC KSPIN_LOCK ExpansionLockQueue;
            STATIC KSPIN_LOCK FileSystemLockQueue;
            STATIC KSPIN_LOCK IoCancelLockQueue;
            STATIC KSPIN_LOCK IoCompletionLockQueue;
            STATIC KSPIN_LOCK IoDatabaseLockQueue;
            STATIC KSPIN_LOCK IoVpbLockQueue;
            STATIC KSPIN_LOCK MasterLockQueue;
            STATIC KSPIN_LOCK NonPagedAllocLockQueue;
            STATIC KSPIN_LOCK NonPagedPoolLockQueue;
            STATIC KSPIN_LOCK PfnLockQueue;
            STATIC KSPIN_LOCK SystemSpaceLockQueue;
            STATIC KSPIN_LOCK TimerTableLockQueue;
            STATIC KSPIN_LOCK VacbLockQueue;
            STATIC KSPIN_LOCK WorkLockQueue;

        public:
            STATIC XTFASTCALL VOID AcquireQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);
            STATIC XTFASTCALL VOID AcquireSpinLock(IN OUT PKSPIN_LOCK SpinLock);
            STATIC XTAPI VOID InitializeAllLocks();
            STATIC XTAPI VOID InitializeLockQueues();
            STATIC XTAPI VOID InitializeSpinLock(IN PKSPIN_LOCK SpinLock);
            STATIC XTFASTCALL VOID ReleaseQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);
            STATIC XTFASTCALL VOID ReleaseSpinLock(IN OUT PKSPIN_LOCK SpinLock);
            STATIC XTFASTCALL BOOLEAN TestSpinLock(IN PKSPIN_LOCK SpinLock);
    };

    class QueuedSpinLockGuard
    {
        private:
            KSPIN_LOCK_QUEUE_LEVEL QueuedLockLevel;

        public:
            QueuedSpinLockGuard(IN OUT KSPIN_LOCK_QUEUE_LEVEL LockLevel)
            {
                QueuedLockLevel = LockLevel;
                KE::SpinLock::AcquireQueuedSpinLock(QueuedLockLevel);
            }

            ~QueuedSpinLockGuard()
            {
                KE::SpinLock::ReleaseQueuedSpinLock(QueuedLockLevel);
            }

            QueuedSpinLockGuard(const QueuedSpinLockGuard&) = delete;
            QueuedSpinLockGuard& operator=(const QueuedSpinLockGuard&) = delete;
    };

    class SpinLockGuard
    {
        private:
            PKSPIN_LOCK SpinLock;

        public:
            SpinLockGuard(IN OUT PKSPIN_LOCK SpinLock)
            {
                KE::SpinLock::AcquireSpinLock(SpinLock);
            }

            ~SpinLockGuard()
            {
                KE::SpinLock::ReleaseSpinLock(SpinLock);
            }

            SpinLockGuard(const SpinLockGuard&) = delete;
            SpinLockGuard& operator=(const SpinLockGuard&) = delete;
    };
}

#endif /* __XTOSKRNL_KE_SPINLOCK_HH */
