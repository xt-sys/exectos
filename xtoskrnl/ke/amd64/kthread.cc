/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/kthread.cc
 * DESCRIPTION:     AMD64 thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

/**
 * Initializes CPU architecture dependent context of a thread.
 *
 * @param Thread
 *        Supplies a pointer to the thread being initialized.
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
 * @param ContextRecord
 *        Supplies a pointer to a context record which stores the initial state of the user mode thread.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KThread::InitializeThreadContext(IN PKTHREAD Thread,
                           IN PKSYSTEM_ROUTINE SystemRoutine,
                           IN PKSTART_ROUTINE StartRoutine,
                           IN PVOID StartContext,
                           IN PCONTEXT ContextRecord)
{
    PKTHREAD_INIT_FRAME ThreadFrame;

    /* Set initial thread frame */
    ThreadFrame = ((PKTHREAD_INIT_FRAME)Thread->InitialStack) - 1;

    /* Fill floating point save area with zeroes */
    RtlZeroMemory(&ThreadFrame->NpxFrame, sizeof(FLOATING_SAVE_AREA));

    /* Check if context provided for this thread */
    if(ContextRecord)
    {
        /* User mode thread needs further initialization, this is not completed */
        UNIMPLEMENTED;

        /* Fill exception and trap frames with zeroes */
        RtlZeroMemory(&ThreadFrame->ExceptionFrame, sizeof(KEXCEPTION_FRAME));
        RtlZeroMemory(&ThreadFrame->TrapFrame, sizeof(KTRAP_FRAME));

        /* Disable debug registers and enable context registers */
        ContextRecord->ContextFlags &= ~CONTEXT_DEBUG_REGISTERS | CONTEXT_CONTROL;

        /* Align the stack and reserve space for 4 parameters and return value */
        ContextRecord->Rsp = (ContextRecord->Rsp & ~15) - 40;

        /* Set CS and SS segments for user mode */
        ContextRecord->SegCs = KGDT_R3_CODE | RPL_MASK;
        ContextRecord->SegSs = KGDT_R3_DATA | RPL_MASK;

        /* This is user mode thread */
        Thread->PreviousMode = UserMode;

        /* Enable floating point state */
        Thread->NpxState = NPX_STATE_SCRUB;

        /* Set initial floating point state */
        ThreadFrame->NpxFrame.ControlWord = 0x27F;
        ThreadFrame->NpxFrame.TagWord = 0xFFFF;

        /* Clear DR6 and DR7 registers */
        ThreadFrame->TrapFrame.Dr6 = 0;
        ThreadFrame->TrapFrame.Dr7 = 0;

        /* Set initial MXCSR register value */
        ThreadFrame->TrapFrame.MxCsr = INITIAL_MXCSR;

        /* Initialize exception frame */
        ThreadFrame->ExceptionFrame.P1Home = (ULONG64)StartContext;
        ThreadFrame->ExceptionFrame.P2Home = (ULONG64)StartRoutine;
        ThreadFrame->ExceptionFrame.P3Home = (ULONG64)SystemRoutine;
        ThreadFrame->ExceptionFrame.P4Home = (ULONG64)SystemRoutine;
    }
    else
    {
        /* This is kernel mode thread */
        Thread->PreviousMode = KernelMode;

        /* Disable floating point state */
        Thread->NpxState = NPX_STATE_UNUSED;

        /* Set thread start address */
        ThreadFrame->StartFrame.Return = (ULONG64)NULL;
    }

    /* Initialize thread startup information */
    ThreadFrame->StartFrame.P1Home = (ULONG64)StartContext;
    ThreadFrame->StartFrame.P2Home = (ULONG64)StartRoutine;
    ThreadFrame->StartFrame.P3Home = (ULONG64)SystemRoutine;
    ThreadFrame->StartFrame.P4Home = (ULONG64)SystemRoutine;

    /* Initialize switch frame */
    ThreadFrame->SwitchFrame.ApcBypass = APC_LEVEL;
    ThreadFrame->SwitchFrame.MxCsr = INITIAL_MXCSR;
    ThreadFrame->SwitchFrame.Rbp = (ULONG64)&ThreadFrame->TrapFrame;

    /* Set thread stack */
    Thread->KernelStack = &ThreadFrame->SwitchFrame;
}

} /* namespace */