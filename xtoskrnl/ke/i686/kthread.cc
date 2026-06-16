/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/kthread.cc
 * DESCRIPTION:     I686 thread manipulation support
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
    PFX_SAVE_FORMAT FxSaveFormat;
    CONTEXT ContextFrame;

    /* Set initial thread frame */
    ThreadFrame = ((PKTHREAD_INIT_FRAME)Thread->InitialStack) - 1;

    /* Fill floating point save area with zeroes */
    RTL::Memory::ZeroMemory(&ThreadFrame->NpxFrame, sizeof(FX_SAVE_AREA));

    /* Check if context provided for this thread */
    if(ContextRecord)
    {
        /* Make a local copy of the context to avoid mutating caller's memory */
        RTL::Memory::CopyMemory(&ContextFrame, ContextRecord, sizeof(CONTEXT));

        /* Disable debug registers and enable control and extended registers */
        ContextFrame.ContextFlags |= (CONTEXT_CONTROL | CONTEXT_EXTENDED_REGISTERS);
        ContextFrame.ContextFlags &= ~CONTEXT_DEBUG_REGISTERS;

        /* Fill trap frame with zeroes */
        RTL::Memory::ZeroMemory(&ThreadFrame->TrapFrame, sizeof(KTRAP_FRAME));

        /* Translate Context frame to Trap frame */
        KE::Processor::RestoreProcessorContext(&ThreadFrame->TrapFrame, NULLPTR,
                                               &ContextFrame, ContextFrame.ContextFlags);

        /* This is user mode thread */
        Thread->PreviousMode = UserMode;
        ThreadFrame->StartFrame.UserMode = TRUE;
        ThreadFrame->TrapFrame.PreviousMode = UserMode;

        /* Disable coprocessor floating point state */
        Thread->NpxState = NPX_STATE_UNLOADED;
        Thread->NpxRunLevel = PASSIVE_LEVEL;

        /* Set initial floating point state */
        FxSaveFormat = (PFX_SAVE_FORMAT)ContextRecord->ExtendedRegisters;
        FxSaveFormat->ControlWord = 0x27F;
        FxSaveFormat->DataOffset = 0;
        FxSaveFormat->DataSelector = 0;
        FxSaveFormat->ErrorOffset = 0;
        FxSaveFormat->ErrorSelector = 0;
        FxSaveFormat->StatusWord = 0;
        FxSaveFormat->TagWord = 0;
        ContextFrame.FloatSave.Cr0NpxState = 0;
        ThreadFrame->NpxFrame.Cr0NpxState = 0;
        ThreadFrame->NpxFrame.NpxSavedCpu = 0;

        /* Set initial MXCSR register value */
        FxSaveFormat->MxCsr = 0x1F80;

        /* Clear DR6 and DR7 registers */
        ThreadFrame->TrapFrame.Dr6 = 0;
        ThreadFrame->TrapFrame.Dr7 = 0;

        /* Set DS, ES and SS segments for user mode */
        ThreadFrame->TrapFrame.SegDs |= RPL_MASK;
        ThreadFrame->TrapFrame.SegEs |= RPL_MASK;
        ThreadFrame->TrapFrame.SegSs |= RPL_MASK;

        /* Set the routine that will handle the thread finishing its initialization and transition it to UserMode */
        ThreadFrame->StartFrame.Return = (ULONG)SwitchToUserMode;
    }
    else
    {
        /* This is kernel mode thread */
        Thread->PreviousMode = KernelMode;
        ThreadFrame->StartFrame.UserMode = FALSE;

        /* Disable coprocessor floating point state */
        Thread->NpxState = NPX_STATE_UNLOADED;

        /* Set initial floating point state */
        ThreadFrame->NpxFrame.FxArea.ControlWord = 0x27F;
        ThreadFrame->NpxFrame.FxArea.MxCsr = 0x1F80;

        /* Set the routine that will handle a system thread that unexpectedly finished its execution */
        ThreadFrame->StartFrame.Return = (ULONG)HandleSystemThreadExit;
    }

    /* Initialize thread startup information */
    ThreadFrame->StartFrame.P1Home = (ULONG)StartContext;
    ThreadFrame->StartFrame.P2Home = (ULONG)StartRoutine;
    ThreadFrame->StartFrame.P3Home = (ULONG)SystemRoutine;

    /* Initialize switch frame */
    ThreadFrame->SwitchFrame.ApcBypass = APC_LEVEL;
    ThreadFrame->SwitchFrame.ExceptionList = (PEXCEPTION_REGISTRATION_RECORD) - 1;
    ThreadFrame->SwitchFrame.Return = (ULONG)RunThread;

    /* Set thread stack boundaries */
    Thread->InitialStack = (PVOID)&ThreadFrame->NpxFrame;
    Thread->KernelStack = &ThreadFrame->SwitchFrame;
}

/**
 * Serves as the initial execution point for all threads after first context switch.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::KThread::RunThread(VOID)
{
}
