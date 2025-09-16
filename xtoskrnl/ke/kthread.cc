/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/kthread.cc
 * DESCRIPTION:     XT kernel thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


XTAPI
PETHREAD
KE::KThread::GetInitialThread(VOID)
{
    return &InitialThread;
}

/**
 * Exits the dispatcher, switches context to a new thread and lowers runlevel to its original state.
 *
 * @param OldRunLevel
 *        Supplies the original runlevel state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::KThread::ExitDispatcher(IN KRUNLEVEL OldRunLevel)
{
    UNIMPLEMENTED;

    /* Lower runlevel */
    RunLevel::LowerRunLevel(OldRunLevel);
}

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
KE::KThread::InitializeThread(IN PKPROCESS Process,
                              IN OUT PKTHREAD Thread,
                              IN PKSYSTEM_ROUTINE SystemRoutine,
                              IN PKSTART_ROUTINE StartRoutine,
                              IN PVOID StartContext,
                              IN PCONTEXT Context,
                              IN PVOID EnvironmentBlock,
                              IN PVOID Stack,
                              IN BOOLEAN RunThread)
{
    PKWAIT_BLOCK TimerWaitBlock;
    BOOLEAN Allocation;
    XTSTATUS Status;
    ULONG Index;

    /* No stack allocation was done yet */
    Allocation = FALSE;

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
    SpinLock::InitializeSpinLock(&Thread->ThreadLock);

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
    SpinLock::InitializeSpinLock(&Thread->ApcQueueLock);

    /* Initialize kernel-mode suspend APC */
    Apc::InitializeApc(&Thread->SuspendApc, Thread, OriginalApcEnvironment, SuspendNop,
                       SuspendRundown, SuspendThread, KernelMode, NULL);

    /* Initialize suspend semaphore */
    Semaphore::InitializeSemaphore(&Thread->SuspendSemaphore, 0, 2);

    /* Initialize the builtin timer */
    Timer::InitializeTimer(&Thread->Timer, NotificationTimer);
    TimerWaitBlock = &Thread->WaitBlock[KTIMER_WAIT_BLOCK];
    TimerWaitBlock->Object = &Thread->Timer;
    TimerWaitBlock->WaitKey = STATUS_TIMEOUT;
    TimerWaitBlock->WaitType = WaitAny;
    TimerWaitBlock->WaitListEntry.Flink = &(&Thread->Timer)->Header.WaitListHead;
    TimerWaitBlock->WaitListEntry.Blink = &(&Thread->Timer)->Header.WaitListHead;

    /* Initialize Thread Environment Block*/
    Thread->EnvironmentBlock = (PTHREAD_ENVIRONMENT_BLOCK)EnvironmentBlock;

    /* Make sure there is a valid stack available */
    if(!Stack)
    {
        /* Allocate new stack */
        Status = MM::KernelPool::AllocateKernelStack(&Stack, FALSE, 0);
        if(Status != STATUS_SUCCESS || !Stack)
        {
            /* Stack allocation failed */
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        /* Mark allocation as successful */
        Allocation = TRUE;
    }

    Thread->InitialStack = Stack;
    Thread->StackBase = Stack;
    Thread->StackLimit = (PVOID)((ULONG_PTR)Stack - KERNEL_STACK_SIZE);

    __try
    {
        /* Initialize thread context */
        InitializeThreadContext(Thread, SystemRoutine, StartRoutine, StartContext, Context);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        /* Failed to initialize thread context, check stack allocation */
        if(Allocation)
        {
            /* Deallocate stack */
            MM::KernelPool::FreeKernelStack(Stack, FALSE);
            Thread->InitialStack = NULL;
            Thread->StackBase = NULL;
        }

        /* Thread initialization failed */
        return STATUS_UNSUCCESSFUL;
    }

    /* Mark thread as initialized and run it */
    Thread->State = Initialized;

    /* Check if thread should be started */
    if(RunThread)
    {
        /* Start thread */
        StartThread(Thread);
    }

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
KE::KThread::StartThread(IN PKTHREAD Thread)
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
KE::KThread::SuspendNop(IN PKAPC Apc,
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
KE::KThread::SuspendRundown(IN PKAPC Apc)
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
KE::KThread::SuspendThread(IN PVOID NormalContext,
                           IN PVOID SystemArgument1,
                           IN PVOID SystemArgument2)
{
    UNIMPLEMENTED;
}
