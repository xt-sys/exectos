/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/kthread.c
 * DESCRIPTION:     XT kernel thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes the thread.
 *
 * @param Process
 *        Supplies a pointer to the process that owns the thread.
 *
 * @param Thread
 *        Supplies a pointer to thread that will be initialized.
 *
 * @param SystemRoutine
 *        Supplies a pointer to the routine called during first scheduling.
 *
 * @param StartRoutine
 *        Supplies a pointer to the routine called during thread startup.
 *
 * @param StartContext
 *        Supplies a pointer to a context data that will be passed to start routine.
 *
 * @param Context
 *        Supplies a pointer to the context frame containing state of the user mode thread.
 *
 * @param EnvironmentBlock
 *        Supplies a pointer to the environment block of the thread.
 *
 * @param Stack
 *        Supplies a pointer to the stack of the thread.
 *
 * @return This routine returns a status code.
 *
 * @since NT 3.5
 */
XTAPI
XTSTATUS
KeInitializeThread(IN PKPROCESS Process,
                   IN OUT PKTHREAD Thread,
                   IN PKSYSTEM_ROUTINE SystemRoutine,
                   IN PKSTART_ROUTINE StartRoutine,
                   IN PVOID StartContext,
                   IN PCONTEXT Context,
                   IN PVOID EnvironmentBlock,
                   IN PVOID Stack)
{
    PKWAIT_BLOCK TimerWaitBlock;
    BOOLEAN Allocation;
    XTSTATUS Status;
    ULONG Index;

    /* Initialize thread dispatcher header */
    Thread->Header.Type = ThreadObject;
    Thread->Header.SignalState = 0;

    /* Initialize thread wait list */
    RtlInitializeListHead(&Thread->Header.WaitListHead);

    /* Initialize thread mutant list head */
    RtlInitializeListHead(&Thread->MutantListHead);

    /* Initialize the builtin wait blocks */
    for(Index = 0; Index <= KTHREAD_WAIT_BLOCK; Index++)
    {
        Thread->WaitBlock[Index].Thread = Thread;
    }

    /* Initialize stack resident and stack swap */
    Thread->AutoAlignment = Process->AutoAlignment;
    Thread->StackResident = TRUE;
    Thread->StackSwap = TRUE;
    Thread->SwapBusy = FALSE;

    /* Set priority adjustment reason */
    Thread->AdjustReason = AdjustNone;

    /* Initialize thread lock */
    KeInitializeSpinLock(&Thread->ThreadLock);

    /* Set thread service table */
    Thread->ServiceTable = KeServiceDescriptorTable;

    /* Initialize thread APC */
    Thread->ApcStatePointer[0] = &Thread->ApcState;
    Thread->ApcStatePointer[1] = &Thread->SavedApcState;
    Thread->ApcQueueable = TRUE;
    Thread->ApcState.Process = Process;
    Thread->Process = Process;

    /* Initialize APC list heads */
    RtlInitializeListHead(&Thread->ApcState.ApcListHead[KernelMode]);
    RtlInitializeListHead(&Thread->ApcState.ApcListHead[UserMode]);

    /* Initialize APC queue lock */
    KeInitializeSpinLock(&Thread->ApcQueueLock);

    /* Initialize kernel-mode suspend APC */
    KeInitializeApc(&Thread->SuspendApc, Thread, OriginalApcEnvironment, KepSuspendNop,
                    KepSuspendRundown, KepSuspendThread, KernelMode, NULL);

    /* Initialize suspend semaphore */
    KeInitializeSemaphore(&Thread->SuspendSemaphore, 0, 2);

    /* Initialize the builtin timer */
    KeInitializeTimer(&Thread->Timer);
    TimerWaitBlock = &Thread->WaitBlock[KTIMER_WAIT_BLOCK];
    TimerWaitBlock->Object = &Thread->Timer;
    TimerWaitBlock->WaitKey = STATUS_TIMEOUT;
    TimerWaitBlock->WaitType = WaitAny;
    TimerWaitBlock->WaitListEntry.Flink = &(&Thread->Timer)->Header.WaitListHead;
    TimerWaitBlock->WaitListEntry.Blink = &(&Thread->Timer)->Header.WaitListHead;

    /* Initialize Thread Environment Block*/
    Thread->EnvironmentBlock = EnvironmentBlock;

    Thread->InitialStack = Stack;
    Thread->StackBase = Stack;
    Thread->StackLimit = Stack - KERNEL_STACK_SIZE;

    __try
    {
        /* Initialize thread context */
        KepInitializeThreadContext(Thread, SystemRoutine, StartRoutine, StartContext, Context);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        /* Failed to initialize thread context */
        return STATUS_UNSUCCESSFUL;
    }

    /* Mark thread as initialized and run it */
    Thread->State = Initialized;
    KeStartThread(Thread);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Starts the thread.
 *
 * @param Thread
 *        Supplies a pointer to the thread.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTAPI
VOID
KeStartThread(IN PKTHREAD Thread)
{
    UNIMPLEMENTED;
}

/**
 * Suspend APC-built thread NOP routine. It takes no actions.
 *
 * @param Apc
 *        Supplies a pointer to the APC object.
 *
 * @param NormalRoutine
 *        Supplies a pointer to the normal routine set during the APC initialization. Unused by this routine.
 *
 * @param NormalContext
 *        Supplies a pointer a context data set during the APC initialization. Unused by this routine.
 *
 * @param SystemArgument1
 *        Supplies a pointer to an unused system argument.
 *
 * @param SystemArgument2
 *        Supplies a pointer to an unused system argument.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KepSuspendNop(IN PKAPC Apc,
              IN OUT PKNORMAL_ROUTINE *NormalRoutine,
              IN OUT PVOID *NormalContext,
              IN OUT PVOID *SystemArgument1,
              IN OUT PVOID *SystemArgument2)
{
    /* No action here */
}

/**
 * Suspend APC-built thread rundown routine. It takes no actions.
 *
 * @param Apc
 *        Supplies a pointer to the APC object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KepSuspendRundown(IN PKAPC Apc)
{
    /* No action here */
}

/**
 * Suspends thread execution by waiting on the thread's semaphore.
 *
 * @param NormalContext
 *        Supplies a pointer a context data set during the APC initialization. Unused by this routine.
 *
 * @param SystemArgument1
 *        Supplies a pointer to an unused system argument.
 *
 * @param SystemArgument2
 *        Supplies a pointer to an unused system argument.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KepSuspendThread(IN PVOID NormalContext,
                 IN PVOID SystemArgument1,
                 IN PVOID SystemArgument2)
{
    UNIMPLEMENTED;
}
