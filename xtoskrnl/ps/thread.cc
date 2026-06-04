/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/thread.cc
 * DESCRIPTION:     Thread Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Allocates and initializes a per-processor IDLE thread for an Application Processor (AP).
 *
 * @param Prcb
 *        Supplies a pointer to the Processor Control Block of the target processor.
 *
 * @param Stack
 *        Supplies a pointer to the pre-allocated kernel stack for the new idle thread.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
PS::Thread::CreateIdleThread(IN PKPROCESSOR_CONTROL_BLOCK Prcb,
                             IN PVOID Stack)
{
    PKPROCESS IdleProcess;
    PETHREAD IdleThread;
    XTSTATUS Status;

    /* Retrieve the global IDLE process container */
    IdleProcess = KE::KProcess::GetIdleProcess();

    /* Allocate the Executive Thread object */
    Status = MM::Allocator::AllocatePool(NonPagedPool, sizeof(ETHREAD), (PVOID*)&IdleThread);
    if(Status != STATUS_SUCCESS)
    {
        /* Memory allocation failed, return the status code */
        return Status;
    }

    /* Zero the allocated memory to prevent uninitialized data */
    RTL::Memory::ZeroMemory(IdleThread, sizeof(ETHREAD));

    /* Register the new thread within the Processor Control Block */
    Prcb->CurrentThread = &IdleThread->ThreadControlBlock;
    Prcb->IdleThread = &IdleThread->ThreadControlBlock;

    /* Initialize the IDLE thread */
    return KE::KThread::InitializeIdleThread(IdleProcess, &IdleThread->ThreadControlBlock, Prcb, Stack);
}
