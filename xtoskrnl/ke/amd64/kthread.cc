/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/kthread.cc
 * DESCRIPTION:     AMD64 thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


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
KE::KThread::InitializeThreadContext(IN PKTHREAD Thread,
                                     IN PKSYSTEM_ROUTINE SystemRoutine,
                                     IN PKSTART_ROUTINE StartRoutine,
                                     IN PVOID StartContext,
                                     IN PCONTEXT ContextRecord)
{
    PKTHREAD_INIT_FRAME ThreadFrame;
    CONTEXT ContextFrame;

    /* Set initial thread frame */
    ThreadFrame = ((PKTHREAD_INIT_FRAME)Thread->InitialStack) - 1;

    /* Fill floating point save area with zeroes */
    RTL::Memory::ZeroMemory(&ThreadFrame->NpxFrame, sizeof(FLOATING_SAVE_AREA));

    /* Check if context provided for this thread */
    if(ContextRecord)
    {
        /* Make a local copy of the context to avoid mutating caller's memory */
        RTL::Memory::CopyMemory(&ContextFrame, ContextRecord, sizeof(CONTEXT));

        /* Disable debug registers and enable context registers */
        ContextFrame.ContextFlags |= CONTEXT_CONTROL;
        ContextFrame.ContextFlags &= ~CONTEXT_DEBUG_REGISTERS;

        /* Align the stack and reserve space for 4 parameters and return value */
        ContextFrame.Rsp = (ContextFrame.Rsp & ~15) - 40;

        /* Set CS and SS segments for user mode */
        ContextFrame.SegCs = KGDT_R3_CODE | RPL_MASK;
        ContextFrame.SegSs = KGDT_R3_DATA | RPL_MASK;

        /* Fill exception and trap frames with zeroes */
        RTL::Memory::ZeroMemory(&ThreadFrame->ExceptionFrame, sizeof(KEXCEPTION_FRAME));
        RTL::Memory::ZeroMemory(&ThreadFrame->TrapFrame, sizeof(KTRAP_FRAME));

        /* Translate Context frame to Trap frame */
        KE::Processor::RestoreProcessorContext(&ThreadFrame->TrapFrame, &ThreadFrame->ExceptionFrame,
                                               &ContextFrame, ContextFrame.ContextFlags);

        /* This is user mode thread */
        Thread->PreviousMode = UserMode;
        ThreadFrame->TrapFrame.PreviousMode = UserMode;

        /* Enable floating point state */
        Thread->NpxState = NPX_STATE_SCRUB;

        /* Set initial floating point state */
        ThreadFrame->NpxFrame.ControlWord = 0x27F;
        ThreadFrame->NpxFrame.DataOffset = 0;
        ThreadFrame->NpxFrame.DataSelector = 0;
        ThreadFrame->NpxFrame.ErrorOffset = 0;
        ThreadFrame->NpxFrame.ErrorOpcode = 0;
        ThreadFrame->NpxFrame.ErrorSelector = 0;
        ThreadFrame->NpxFrame.StatusWord = 0;
        ThreadFrame->NpxFrame.TagWord = 0xFFFF;

        /* Set initial MXCSR register value */
        ThreadFrame->TrapFrame.MxCsr = INITIAL_MXCSR;

        /* Clear DR6 and DR7 registers */
        ThreadFrame->TrapFrame.Dr6 = 0;
        ThreadFrame->TrapFrame.Dr7 = 0;

        /* Terminate the Exception Handler List */
        ThreadFrame->TrapFrame.ExceptionFrame = (ULONGLONG)NULLPTR;

        /* Initialize exception frame */
        ThreadFrame->ExceptionFrame.P1Home = (ULONGLONG)StartContext;
        ThreadFrame->ExceptionFrame.P2Home = (ULONGLONG)StartRoutine;
        ThreadFrame->ExceptionFrame.P3Home = (ULONGLONG)SystemRoutine;
        ThreadFrame->ExceptionFrame.P4Home = (ULONGLONG)SystemRoutine;

        /* Set the routine that will handle the thread finishing its initialization and transition it to UserMode */
        ThreadFrame->StartFrame.Return = (ULONG64)SwitchToUserMode;
    }
    else
    {
        /* This is kernel mode thread */
        Thread->PreviousMode = KernelMode;

        /* Disable floating point state */
        Thread->NpxState = NPX_STATE_UNUSED;

        /* Set the routine that will handle a system thread that unexpectedly finished its execution */
        ThreadFrame->StartFrame.Return = (ULONG64)HandleSystemThreadExit;
    }

    /* Initialize thread startup information */
    ThreadFrame->StartFrame.P1Home = (ULONGLONG)StartContext;
    ThreadFrame->StartFrame.P2Home = (ULONGLONG)StartRoutine;
    ThreadFrame->StartFrame.P3Home = (ULONGLONG)SystemRoutine;
    ThreadFrame->StartFrame.P4Home = (ULONGLONG)SystemRoutine;

    /* Initialize switch frame */
    ThreadFrame->SwitchFrame.ApcBypass = APC_LEVEL;
    ThreadFrame->SwitchFrame.MxCsr = INITIAL_MXCSR;
    ThreadFrame->SwitchFrame.Rbp = (ULONGLONG)&ThreadFrame->TrapFrame;
    ThreadFrame->SwitchFrame.Return = (ULONGLONG)RunThread;

    /* Set thread stack boundaries */
    Thread->InitialStack = (PVOID)&ThreadFrame->NpxFrame;
    Thread->KernelStack = &ThreadFrame->SwitchFrame;
}
