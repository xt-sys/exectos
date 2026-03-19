/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/guard.hh
 * DESCRIPTION:     Memory Manager synchronization guard
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_GUARD_HH
#define __XTOSKRNL_MM_GUARD_HH

#include <xtos.hh>
#include <ke/runlevel.hh>
#include <ke/spinlock.hh>


/* Memory Manager */
namespace MM
{
    class PoolLockGuard
    {
        private:
            BOOLEAN Locked;
            MMPOOL_TYPE LockPoolType;
            KRUNLEVEL PreviousRunLevel;

        public:
            PoolLockGuard(IN MMPOOL_TYPE PoolType)
            {
                LockPoolType = PoolType;

                /* Determine the appropriate synchronization mechanism based on the requested pool type */
                if(LockPoolType == NonPagedPool)
                {
                    /* Elevate the runlevel to DISPATCH_LEVEL */
                    PreviousRunLevel = KE::RunLevel::RaiseRunLevel(DISPATCH_LEVEL);

                    /* Acquire the global queued spinlock protecting the non-paged pool */
                    KE::SpinLock::AcquireQueuedSpinLock(NonPagedPoolLock);
                }
                else
                {
                    /* Paged pool requires a mutex, currently unimplemented */
                    UNIMPLEMENTED;
                }

                /* Mark the guard as actively holding the lock */
                Locked = TRUE;
            }

            ~PoolLockGuard(VOID)
            {
                /* Automatically release the held lock upon going out of scope */
                Release();
            }

            PoolLockGuard(const PoolLockGuard&) = delete;
            PoolLockGuard& operator=(const PoolLockGuard&) = delete;

            VOID Release(VOID)
            {
                /* Check if the guard is currently holding a lock */
                if(!Locked)
                {
                    /* Return, to prevent a double-free */
                    return;
                }

                /* Determine the appropriate synchronization mechanism based on the requested pool type */
                if(LockPoolType == NonPagedPool)
                {
                    /* Release the non-paged pool spinlock and subsequently restore the original runlevel */
                    KE::SpinLock::ReleaseQueuedSpinLock(NonPagedPoolLock);
                    KE::RunLevel::LowerRunLevel(PreviousRunLevel);
                }
                else
                {
                    /* Paged pool requires a mutex, currently unimplemented */
                    UNIMPLEMENTED;
                }

                /* Update the internal state, indicating that the lock is no longer held */
                Locked = FALSE;
            }
    };
}

#endif /* __XTOSKRNL_MM_GUARD_HH */
