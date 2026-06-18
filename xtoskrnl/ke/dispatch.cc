/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/dispatch.cc
 * DESCRIPTION:     Kernel Thread Dispatcher
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Enters the system idle thread loop for the current processor, running continuously when no other
 * threads are scheduled for execution.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Dispatcher::EnterIdleLoop(VOID)
{
    PKTHREAD CurrentThread, NextThread;
    PKPROCESSOR_CONTROL_BLOCK Prcb;

    /* Retrieve the processor control block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Enter the infinite idle loop */
    while(TRUE)
    {
        /* Temporarily enable interrupts and yield the processor to handle pending hardware events */
        AR::CpuFunctions::SetInterruptFlag();
        AR::CpuFunctions::YieldProcessor();
        AR::CpuFunctions::YieldProcessor();
        AR::CpuFunctions::ClearInterruptFlag();

        /* Check for pending deferred ready threads, DPCs, or timer requests */
        if(Prcb->DeferredReadyListHead.Next ||
           Prcb->DpcData[0].DpcQueueDepth ||
           Prcb->TimerRequest)
        {
            /* Unimplemented path */
            UNIMPLEMENTED;
        }

        /* Check if a new thread has been scheduled for execution */
        if(Prcb->NextThread)
        {
            /* Enable interrupts to allow hardware events during context switch preparation */
            AR::CpuFunctions::SetInterruptFlag();

            /* Capture the current and next thread pointers */
            CurrentThread = Prcb->CurrentThread;
            NextThread = Prcb->NextThread;

            /* Update the processor control block with the incoming thread */
            Prcb->NextThread = NULLPTR;
            Prcb->CurrentThread = NextThread;

            /* Transition the incoming thread to the running state */
            NextThread->State = Running;

            /* Start a guarded code block */
            {
                /* Raise runlevel to SYNC level */
                KE::RaiseRunLevel RunLevel(SYNC_LEVEL);

                /* Perform the context switch away from the idle thread */
                KE::Dispatcher::SwitchContext(CurrentThread, APC_LEVEL);
            }
        }
        else
        {
            /* No threads scheduled, enter a low-power processor state and wait for interrupts */
            Prcb->PowerState.IdleFunction(&Prcb->PowerState);
        }
    }
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
KE::Dispatcher::ExitDispatcher(IN KRUNLEVEL OldRunLevel)
{
    UNIMPLEMENTED;

    /* Lower runlevel */
    RunLevel::LowerRunLevel(OldRunLevel);
}

/**
 * Handles the dispatch interrupt by retiring pending DPCs, asking the scheduler for the next runnable thread
 * and performing the context switch.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame representing the interrupted context.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KE::Dispatcher::HandleDispatchInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    PKTHREAD CurrentThread, NextThread;
    PKPROCESSOR_CONTROL_BLOCK Prcb;

    /* Receive the Processor Control Block*/
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Raise runlevel to DISPATCH level */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);

    /* End the interrupt */
    HL::Pic::SendEoi();

    /* Check if there is pending deferred work */
    if(Prcb->DeferredReadyListHead.Next ||
       Prcb->DpcData[0].DpcQueueDepth ||
       Prcb->TimerRequest)
    {
        UNIMPLEMENTED;
    }

    /* Re-enable hardware interrupts */
    AR::CpuFunctions::SetInterruptFlag();

    /* Check if the current thread has exhausted its execution quantum */
    if((Prcb->CurrentThread->Quantum <= 0) && (Prcb->CurrentThread != Prcb->IdleThread))
    {
        /* Trigger the scheduler to recalculate thread parameters */
        KE::Scheduler::ProcessQuantumEnd();
    }
    else if(Prcb->NextThread)
    {
        /* Start a guarded code block */
        {
            /* Lock the processor control block */
            KE::SpinLockGuard ControlBlockGuard(&Prcb->PrcbLock);

            /* Capture the outgoing (preempted) and incoming threads */
            CurrentThread = Prcb->CurrentThread;
            NextThread = Prcb->NextThread;

            /* Acknowledge the pending thread and swap the pointers */
            Prcb->NextThread = NULLPTR;
            Prcb->CurrentThread = NextThread;

            /* Update scheduling states */
            NextThread->State = Running;
            CurrentThread->WaitReason = WrDispatchInt;

            /* Re-queue the preempted thread back into the local run queue or defer it */
            KE::Scheduler::QueueReadyThread(CurrentThread, Prcb);
        }

        /* Perform the context switch */
        SwitchContext(CurrentThread, APC_LEVEL);
    }

    /* Disable hardware interrupts before returning from the handler */
    AR::CpuFunctions::ClearInterruptFlag();
}

/**
 * Updates the runtime quantum of the currently executing thread and handles preemption.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame representing the interrupted execution context.
 *
 * @param RunLevel
 *        Supplies the system run level at which the interrupt was taken.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Dispatcher::UpdateRunTime(IN PKTRAP_FRAME TrapFrame,
                              IN KRUNLEVEL RunLevel)
{
    PKPROCESSOR_CONTROL_BLOCK ControlBlock;
    PKTHREAD Thread;

    /* Retrieve current processor control block and current thread */
    ControlBlock = KE::Processor::GetCurrentProcessorControlBlock();
    Thread = KE::Processor::GetCurrentThread();

    /* Increment interrupt count */
    ControlBlock->InterruptCount++;

    /* Check if the thread ran in user mode */
    if(TrapFrame->PreviousMode == UserMode)
    {
        /* Atomically increment the process-wide user time */
        RTL::Atomic::Increment32((PLONG)&Thread->ApcState.Process->UserTime);

        /* Increment thread and total time this processor has spent executing in user time */
        ControlBlock->UserTime++;
        Thread->UserTime++;
    }
    else
    {
        /* Increment the total time this processor has spent executing in kernel mode */
        ControlBlock->KernelTime++;

        /* Check if normal kernel thread execution was interrupted */
        if((RunLevel < DISPATCH_LEVEL) || !(ControlBlock->DpcRoutineActive))
        {
            /* Atomically increment the process-wide kernel time */
            RTL::Atomic::Increment32((PLONG)&Thread->ApcState.Process->KernelTime);

            /* Increment the kernel execution time for the current thread */
            Thread->KernelTime++;
        }
        else if(RunLevel > DISPATCH_LEVEL)
        {
            /* Increment the time spent servicing hardware interrupts */
            ControlBlock->InterruptTime++;
        }
        else
        {
            /* Increment the time spent servicing DPCs */
            ControlBlock->DpcTime++;
        }
    }

    /* Calculate the new DPC request rate as a moving average of the current and previous rates */
    ControlBlock->DpcRequestRate = ((ControlBlock->DpcData[0].DpcCount - ControlBlock->DpcLastCount) +
                                    ControlBlock->DpcRequestRate) >> 1;

    /* Snapshot the current DPC count */
    ControlBlock->DpcLastCount = ControlBlock->DpcData[0].DpcCount;

    /* Check if there are pending DPCs, no DPC routine is currently executing, and DPC interrupt is not pending */
    if((ControlBlock->DpcData[0].DpcQueueDepth) &&
       !(ControlBlock->DpcRoutineActive) &&
       !(ControlBlock->DpcInterruptRequested))
    {
        /* Reset the adjustment threshold counter */
        ControlBlock->AdjustDpcThreshold = DPC_ADJUST_THRESHOLD;

        /* Request a DISPATCH level software interrupt to process the pending DPCs */
        HL::Irq::SendSoftwareInterrupt(DISPATCH_LEVEL);

        /* Evaluate if the DPC request rate is below the ideal threshold */
        if((ControlBlock->DpcRequestRate < DPC_IDEAL_RATE) && (ControlBlock->MaximumDpcQueueDepth > 1))
        {
            /* Decrease the maximum queue depth */
            ControlBlock->MaximumDpcQueueDepth--;
        }
    }
    else
    {
        /* Decrement the tuning threshold counter and verify if an adjustment cycle is required */
        if(!(--ControlBlock->AdjustDpcThreshold))
        {
            /* Reset the counter for the next tuning cycle */
            ControlBlock->AdjustDpcThreshold = DPC_ADJUST_THRESHOLD;

            /* Check if the current queue depth limit is below the system-wide absolute maximum */
            if(ControlBlock->MaximumDpcQueueDepth != DPC_MAXIMUM_QUEUE_DEPTH)
            {
                /* Increase the maximum queue depth to batch more DPCs */
                ControlBlock->MaximumDpcQueueDepth++;
            }
        }
    }

    /* Decrement the execution time slice */
    Thread->Quantum -= CLOCK_QUANTUM_DECREMENT;

    /* Check if the thread has exhausted its quantum, ignoring the idle thread */
    if((Thread->Quantum <= 0) && (Thread != ControlBlock->IdleThread))
    {
        /* Request a DISPATCH level software interrupt to preempt the thread */
        HL::Irq::SendSoftwareInterrupt(DISPATCH_LEVEL);
    }
}
