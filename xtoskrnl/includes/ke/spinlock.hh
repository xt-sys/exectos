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
        public:
            STATIC XTFASTCALL VOID AcquireQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);
            STATIC XTFASTCALL VOID AcquireSpinLock(IN OUT PKSPIN_LOCK SpinLock);
            STATIC XTAPI VOID InitializeSpinLock(IN PKSPIN_LOCK SpinLock);
            STATIC XTFASTCALL VOID ReleaseQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel);
            STATIC XTFASTCALL VOID ReleaseSpinLock(IN OUT PKSPIN_LOCK SpinLock);
    };
}

#endif /* __XTOSKRNL_KE_SPINLOCK_HH */
