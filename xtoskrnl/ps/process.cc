/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/process.cc
 * DESCRIPTION:     Process Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Creates the global IDLE process.
 *
 * @param Prcb
 *        Supplies a pointer to the Processor Control Block of the Bootstrap Processor.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
PS::Process::CreateIdleProcess(IN PKPROCESSOR_CONTROL_BLOCK Prcb)
{
    ULONG_PTR PageDirectory[2];
    PKPROCESS IdleProcess;
    PKTHREAD IdleThread;
    XTSTATUS Status;

    /* Get initial IDLE thread */
    IdleThread = Prcb->CurrentThread;

    /* Get initial IDLE process */
    IdleProcess = IdleThread->ApcState.Process;

    /* Setup placeholder for page directory entries */
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;

    /* Initialize IDLE process */
    Status = KE::KProcess::InitializeIdleProcess(IdleProcess, PageDirectory);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to initialize IDLE process, return status code */
        return Status;
    }

    /* Initialize IDLE thread */
    return KE::KThread::InitializeIdleThread(IdleProcess, IdleThread, Prcb, AR::ProcessorSupport::GetBootStack());
}

/**
 * Returns a pointer to the process object associated with the currently executing thread.
 *
 * @return This routine returns a pointer to the current process object.
 *
 * @since XT 1.0
 */
XTFASTCALL
PEPROCESS
PS::Process::GetCurrentProcess(VOID)
{
    /* Return the current process */
    return (PEPROCESS)KE::Processor::GetCurrentThread()->ApcState.Process;
}
