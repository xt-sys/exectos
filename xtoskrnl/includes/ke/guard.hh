/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/guard.hh
 * DESCRIPTION:     Kernel synchronization guard
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_GUARD_HH
#define __XTOSKRNL_KE_GUARD_HH

#include <xtos.hh>
#include <ke/spinlock.hh>

/* Kernel Library */
namespace KE
{
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
            PKSPIN_LOCK Lock;

        public:
            SpinLockGuard(IN OUT PKSPIN_LOCK SpinLock)
            {
                Lock = SpinLock;
                KE::SpinLock::AcquireSpinLock(Lock);
            }

            ~SpinLockGuard()
            {
                KE::SpinLock::ReleaseSpinLock(Lock);
            }

            SpinLockGuard(const SpinLockGuard&) = delete;
            SpinLockGuard& operator=(const SpinLockGuard&) = delete;
    };
}

#endif /* __XTOSKRNL_KE_GUARD_HH */
