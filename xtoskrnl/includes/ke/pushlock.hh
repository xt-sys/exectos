/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/pushlock.hh
 * DESCRIPTION:     XT Kernel Push Lock support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_PUSHLOCK_HH
#define __XTOSKRNL_KE_PUSHLOCK_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class PushLock
    {
        public:
            STATIC XTFASTCALL VOID AcquireExclusivePushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID AcquireSharedPushLock(PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID AcquireWaitExclusivePushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID AcquireWaitSharedPushLock(IN OUT PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID BlockPushLock(IN PKPUSH_LOCK PushLock,
                                                 IN PKPUSH_LOCK_WAIT_BLOCK WaitBlock);
            STATIC XTFASTCALL BOOLEAN ConvertSharedPushLockToExclusive(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID InitializePushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID ReleaseExclusivePushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID ReleasePushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID ReleaseSharedPushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID ReleaseWaitExclusivePushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID ReleaseWaitSharedPushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID ReleaseWaitPushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL BOOLEAN TryAcquireExclusivePushLock(PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID TryWakePushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID UnblockPushLock(IN PKPUSH_LOCK PushLock,
                                                   IN PKPUSH_LOCK_WAIT_BLOCK WaitBlock);
            STATIC XTFASTCALL VOID WaitOnPushLock(IN PKPUSH_LOCK PushLock);
            STATIC XTFASTCALL VOID WakePushLockWaiters(IN PKPUSH_LOCK PushLock, IN KPUSH_LOCK OldState);

        private:
            STATIC XTINLINE VOID OptimizePushLockList(IN PKPUSH_LOCK PushLock, IN KPUSH_LOCK OldValue);
            STATIC XTINLINE VOID SpinPushLock(IN PKPUSH_LOCK_WAIT_BLOCK WaitBlock);
    };
}

#endif /* __XTOSKRNL_KE_PUSHLOCK_HH */
