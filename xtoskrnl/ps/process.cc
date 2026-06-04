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
VOID
PS::Process::CreateIdleProcess(IN PKPROCESSOR_CONTROL_BLOCK Prcb)
{
    ULONG_PTR PageDirectory[2];
    PKPROCESS IdleProcess;
    PKTHREAD IdleThread;

    /* Get initial IDLE thread */
    IdleThread = Prcb->CurrentThread;

    /* Get initial IDLE process */
    IdleProcess = IdleThread->ApcState.Process;

    /* Setup placeholder for page directory entries */
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;

    /* Initialize Idle process and Idle thread */
    KE::KProcess::InitializeIdleProcess(IdleProcess, PageDirectory);
    KE::KThread::InitializeIdleThread(IdleProcess, IdleThread, Prcb, AR::ProcessorSupport::GetBootStack());
}
