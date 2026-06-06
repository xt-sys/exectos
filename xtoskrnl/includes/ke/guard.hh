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
            BOOLEAN Owned;

        public:
            QueuedSpinLockGuard(IN OUT KSPIN_LOCK_QUEUE_LEVEL LockLevel,
                                IN BOOLEAN Acquire = TRUE)
            {
                QueuedLockLevel = LockLevel;
                Owned = Acquire;
                if(Owned)
                {
                    KE::SpinLock::AcquireQueuedSpinLock(QueuedLockLevel);
                }
            }

            ~QueuedSpinLockGuard()
            {
                if(Owned)
                {
                    KE::SpinLock::ReleaseQueuedSpinLock(QueuedLockLevel);
                }
            }

            QueuedSpinLockGuard(const QueuedSpinLockGuard&) = delete;
            QueuedSpinLockGuard& operator=(const QueuedSpinLockGuard&) = delete;
    };

    class SpinLockGuard
    {
        private:
            PKSPIN_LOCK Lock;
            BOOLEAN Owned;

        public:
            SpinLockGuard(IN OUT PKSPIN_LOCK SpinLock,
                          IN BOOLEAN Acquire = TRUE)
            {
                Lock = SpinLock;
                Owned = Acquire;
                if(Owned)
                {
                    KE::SpinLock::AcquireSpinLock(Lock);
                }
            }

            ~SpinLockGuard()
            {
                if(Owned)
                {
                    KE::SpinLock::ReleaseSpinLock(Lock);
                }
            }

            SpinLockGuard(const SpinLockGuard&) = delete;
            SpinLockGuard& operator=(const SpinLockGuard&) = delete;
    };
}

#endif /* __XTOSKRNL_KE_GUARD_HH */
