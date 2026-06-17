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
