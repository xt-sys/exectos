/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/kthread.cc
 * DESCRIPTION:     XT kernel thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Retrieves a pointer to the system's initial executive thread object.
 *
 * @return This routine returns a pointer to the initial executive thread.
 *
 * @since XT 1.0
 */
XTAPI
PETHREAD
KE::KThread::GetInitialThread(VOID)
{
    return &InitialThread;
}

/**
 * Initializes an Idle Thread.
 *
 * @param IdleProcess
 *        Supplies a pointer to the global Idle Process container.
 *
 * @param IdleThread
 *        Supplies a pointer to the KTHREAD structure being initialized.
 *
 * @param Prcb
 *        Supplies a pointer to the Processor Control Block of the target CPU.
 *
 * @param Stack
 *        Supplies a pointer to the pre-allocated kernel stack for this thread.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::KThread::InitializeIdleThread(IN PKPROCESS IdleProcess,
                                  IN OUT PKTHREAD IdleThread,
                                  IN PKPROCESSOR_CONTROL_BLOCK Prcb,
                                  IN PVOID Stack)
{
    XTSTATUS Status;
    ULONG Cpus;

    /* Get the number of installed CPUs */
    Cpus = KE::Processor::GetInstalledCpus();

    /* Allocate and initialize the primary affinity map for the thread */
    Status = KE::Affinity::CreateAffinityMap(Cpus, &IdleThread->Affinity);
    if(Status != STATUS_SUCCESS)
    {
        /* Affinity map allocation failed, return status code */
        return Status;
    }

    /* Allocate and initialize the user-mode affinity map for the thread */
    Status = KE::Affinity::CreateAffinityMap(Cpus, &IdleThread->UserAffinity);
    if(Status != STATUS_SUCCESS)
    {
        /* Affinity map allocation failed, free affinity map and return status code */
        KE::Affinity::DestroyAffinityMap(IdleThread->Affinity);
        return Status;
    }

    /* Initialize Idle thread */
    KE::KThread::InitializeThread(IdleProcess, IdleThread, NULLPTR, NULLPTR, NULLPTR,
                                  NULLPTR, NULLPTR, Stack, TRUE);

    /* Configure Idle thread scheduling parameters */
    IdleThread->NextProcessor = Prcb->CpuNumber;
    IdleThread->Priority = THREAD_HIGH_PRIORITY;
    IdleThread->State = Running;
    IdleThread->WaitRunLevel = DISPATCH_LEVEL;

    /* Configure thread affinity */
    KE::Affinity::SetProcessorAffinity(IdleThread->Affinity, Prcb->CpuNumber);
    KE::Affinity::SetProcessorAffinity(IdleThread->UserAffinity, Prcb->CpuNumber);

    /* Register CPU as active in the IDLE Process */
    KE::Affinity::AtomicSetProcessorAffinity(IdleProcess->ActiveProcessors, Prcb->CpuNumber);

    /* Return success */
    return STATUS_SUCCESS;
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
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
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
    Thread->Header.SignalState = 0;
    Thread->Header.Size = sizeof(KTHREAD) / sizeof(LONG);
    Thread->Header.DebugActive = FALSE;
    Thread->Header.Type = ThreadObject;

    /* Initialize thread wait list */
    RTL::LinkedList::InitializeListHead(&Thread->Header.WaitListHead);

    /* Initialize thread mutant list head */
    RTL::LinkedList::InitializeListHead(&Thread->MutantListHead);

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

    /* Set the thread service table */
    Thread->ServiceTable = KE::SystemServices::GetSystemServicesDescriptorTable();

    /* Initialize thread lock */
    KE::SpinLock::InitializeSpinLock(&Thread->ThreadLock);

    /* Initialize thread APC */
    Thread->ApcQueueable = TRUE;
    Thread->ApcState.Process = Process;
    Thread->ApcStateIndex = OriginalApcEnvironment;
    Thread->ApcStatePointer[OriginalApcEnvironment] = &Thread->ApcState;
    Thread->ApcStatePointer[AttachedApcEnvironment] = &Thread->SavedApcState;

    /* Initialize APC list heads */
    RTL::LinkedList::InitializeListHead(&Thread->ApcState.ApcListHead[KernelMode]);
    RTL::LinkedList::InitializeListHead(&Thread->ApcState.ApcListHead[UserMode]);

    /* Initialize APC queue lock */
    KE::SpinLock::InitializeSpinLock(&Thread->ApcQueueLock);

    /* Initialize kernel-mode suspend APC */
    KE::Apc::InitializeApc(&Thread->SuspendApc, Thread, OriginalApcEnvironment, SuspendNop,
                           SuspendRundown, SuspendThread, KernelMode, NULLPTR);

    /* Initialize suspend semaphore */
    KE::Semaphore::InitializeSemaphore(&Thread->SuspendSemaphore, 0, 2);

    /* Initialize the builtin timer */
    KE::Timer::InitializeTimer(&Thread->Timer, NotificationTimer);
    TimerWaitBlock = &Thread->WaitBlock[KTIMER_WAIT_BLOCK];
    TimerWaitBlock->Object = &Thread->Timer;
    TimerWaitBlock->WaitKey = STATUS_TIMEOUT;
    TimerWaitBlock->WaitType = WaitAny;
    TimerWaitBlock->WaitListEntry.Flink = &(&Thread->Timer)->Header.WaitListHead;
    TimerWaitBlock->WaitListEntry.Blink = &(&Thread->Timer)->Header.WaitListHead;

    /* Initialize Thread Environment Block and set owner process */
    Thread->EnvironmentBlock = (PTHREAD_ENVIRONMENT_BLOCK)EnvironmentBlock;
    Thread->Process = Process;

    /* Make sure there is a valid stack available */
    if(!Stack)
    {
        /* Allocate new stack */
        Status = MM::KernelPool::AllocateKernelStack(&Stack, KERNEL_STACK_SIZE);
        if(Status != STATUS_SUCCESS || !Stack)
        {
            /* Stack allocation failed */
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        /* Mark allocation as successful */
        Allocation = TRUE;
    }

    /* Setup thread stack */
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
            Thread->InitialStack = NULLPTR;
            Thread->StackBase = NULLPTR;
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
 * @since XT 1.0
 */
XTAPI
VOID
KE::KThread::StartThread(IN OUT PKTHREAD Thread)
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
