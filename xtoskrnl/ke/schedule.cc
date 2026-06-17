/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/schedule.cc
 * DESCRIPTION:     XT Kernel Thread Scheduler
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Inserts a thread into the current processor's deferred ready list.
 *
 * @param Thread
 *        Supplies a pointer to the thread being deferred.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Scheduler::InsertDeferredReadyList(IN PKTHREAD Thread)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;

    /* Retrieve the Processor Control Block (PRCB) for the executing core */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Tag the thread with the current CPU and update its scheduling state */
    Thread->DeferredProcessor = Prcb->CpuNumber;
    Thread->State = DeferredReady;

    /* Insert the thread into the deferred ready list */
    RTL::LifoQueue::PushEntryList(&Prcb->DeferredReadyListHead, &Thread->SwapListEntry);
}

/**
 * Transitions a thread to the ready state and queues it for execution within the dispatcher database.
 *
 * @param Thread
 *        Supplies a pointer to the thread to be dispatched.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Scheduler::ProcessReadyThread(IN PKTHREAD Thread)
{
    PKPROCESS Process;

    /* Extract the process from the thread's APC state */
    Process = Thread->ApcState.Process;

    /* Verify if the process and thread are resident in physical memory */
    if(Process->State != ProcessInMemory)
    {
        /* Process is swapped out, place the thread in a transition state */
        Thread->State = Transition;
    }
    else if(!Thread->KernelStackResident)
    {
        /* Increment the active stack count and transition the thread */
        Process->StackCount++;
        Thread->State = Transition;
    }
    else
    {
        /* Both process and thread's stack are fully resident, queue the thread for execution */
        InsertDeferredReadyList(Thread);
    }
}

/**
 * Prepares a thread for execution by safely acquiring the dispatcher database lock.
 *
 * @param Thread
 *        Supplies a pointer to the thread to be readied.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Scheduler::ReadyThread(IN PKTHREAD Thread)
{
    /* Raise runlevel to SYNC level and acquire the dispatcher database lock */
    KE::RaiseRunLevel RunLevel(SYNC_LEVEL);
    KE::QueuedSpinLockGuard DispatcherGuard(DispatcherLock);

    /* Evaluate residency and queue the thread */
    ProcessReadyThread(Thread);
}
