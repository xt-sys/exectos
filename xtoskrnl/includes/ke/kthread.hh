/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/kthread.hh
 * DESCRIPTION:     XT kernel thread manipulation support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_KTHREAD_HH
#define __XTOSKRNL_KE_KTHREAD_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class KThread
    {
        private:
            STATIC ETHREAD InitialThread;

        public:
            STATIC XTAPI VOID AttachThread(IN PKTHREAD Thread);
            STATIC XTFASTCALL VOID EnterCriticalRegion();
            STATIC XTFASTCALL VOID EnterCriticalRegion(IN PKTHREAD Thread);
            STATIC XTFASTCALL VOID EnterGuardedRegion();
            STATIC XTFASTCALL VOID EnterGuardedRegion(IN PKTHREAD Thread);
            STATIC XTAPI PETHREAD GetInitialThread(VOID);
            STATIC XTAPI XTSTATUS InitializeIdleThread(IN PKPROCESS IdleProcess,
                                                       IN OUT PKTHREAD IdleThread,
                                                       IN PKPROCESSOR_CONTROL_BLOCK Prcb,
                                                       IN PVOID Stack);
            STATIC XTAPI XTSTATUS InitializeThread(IN PKPROCESS Process,
                                                   IN OUT PKTHREAD Thread,
                                                   IN PKSYSTEM_ROUTINE SystemRoutine,
                                                   IN PKSTART_ROUTINE StartRoutine,
                                                   IN PVOID StartContext,
                                                   IN PCONTEXT Context,
                                                   IN PVOID EnvironmentBlock,
                                                   IN PVOID Stack,
                                                   IN BOOLEAN AttachToProcess);
            STATIC XTFASTCALL VOID LeaveCriticalRegion();
            STATIC XTFASTCALL VOID LeaveCriticalRegion(IN PKTHREAD Thread);
            STATIC XTFASTCALL VOID LeaveGuardedRegion();
            STATIC XTFASTCALL VOID LeaveGuardedRegion(IN PKTHREAD Thread);

        private:
            STATIC XTAPI VOID HandleSystemThreadExit(VOID);
            STATIC XTAPI VOID InitializeThreadContext(IN PKTHREAD Thread,
                                                      IN PKSYSTEM_ROUTINE SystemRoutine,
                                                      IN PKSTART_ROUTINE StartRoutine,
                                                      IN PVOID StartContext,
                                                      IN PCONTEXT ContextRecord);
            STATIC XTAPI VOID RunThread(VOID);
            STATIC XTAPI VOID SuspendNop(IN PKAPC Apc,
                                         IN OUT PKNORMAL_ROUTINE *NormalRoutine,
                                         IN OUT PVOID *NormalContext,
                                         IN OUT PVOID *SystemArgument1,
                                         IN OUT PVOID *SystemArgument2);
            STATIC XTAPI VOID SuspendRundown(IN PKAPC Apc);
            STATIC XTAPI VOID SuspendThread(IN PVOID NormalContext,
                                            IN PVOID SystemArgument1,
                                            IN PVOID SystemArgument2);
            STATIC XTAPI VOID SwitchToUserMode(VOID);
    };
}

#endif /* __XTOSKRNL_KE_KTHREAD_HH */
