/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/kthread.c
 * DESCRIPTION:     I686 thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
KepInitializeThreadContext(IN PKTHREAD Thread,
                           IN PKSYSTEM_ROUTINE SystemRoutine,
                           IN PKSTART_ROUTINE StartRoutine,
                           IN PVOID StartContext,
                           IN PCONTEXT ContextRecord)
{
    PKTHREAD_INIT_FRAME ThreadFrame;
    PFX_SAVE_FORMAT FxSaveFormat;

    /* Set initial thread frame */
    ThreadFrame = ((PKTHREAD_INIT_FRAME)Thread->InitialStack) - sizeof(KTHREAD_INIT_FRAME);

    /* Fill floating point save area with zeroes */
    RtlZeroMemory(&ThreadFrame->NpxFrame, sizeof(FX_SAVE_AREA));

    /* Check if context provided for this thread */
    if(ContextRecord)
    {
        /* User mode thread needs further initialization, this is not completed */
        UNIMPLEMENTED;

        /* Fill trap frame with zeroes */
        RtlZeroMemory(&ThreadFrame->TrapFrame, sizeof(KTRAP_FRAME));

        /* Disable debug registers and enable context registers */
        ContextRecord->ContextFlags &= ~CONTEXT_DEBUG_REGISTERS | CONTEXT_CONTROL;

        /* This is user mode thread */
        ThreadFrame->StartFrame.UserMode = TRUE;
        Thread->PreviousMode = UserMode;

        /* Disable coprocessor floating point state */
        Thread->NpxState = NPX_STATE_UNLOADED;
        Thread->Header.NpxIrql = PASSIVE_LEVEL;

        /* Set initial floating point state */
        FxSaveFormat = (PFX_SAVE_FORMAT)ContextRecord->ExtendedRegisters;
        FxSaveFormat->ControlWord = 0x27F;
        FxSaveFormat->MxCsr = 0x1F80;
        ContextRecord->FloatSave.Cr0NpxState = 0;
        ThreadFrame->NpxFrame.Cr0NpxState = 0;
        ThreadFrame->NpxFrame.NpxSavedCpu = 0;

        /* Clear DR6 and DR7 registers */
        ThreadFrame->TrapFrame.Dr6 = 0;
        ThreadFrame->TrapFrame.Dr7 = 0;

        /* Set exception list pointer in the trap frame */
        ThreadFrame->TrapFrame.ExceptionList = (PEXCEPTION_REGISTRATION_RECORD) - 1;

        /* Set DS, ES and SS segments for user mode */
        ThreadFrame->TrapFrame.SegDs |= RPL_MASK;
        ThreadFrame->TrapFrame.SegEs |= RPL_MASK;
        ThreadFrame->TrapFrame.HardwareSegSs |= RPL_MASK;

        /* Set debug mark in the trap frame */
        ThreadFrame->TrapFrame.DbgMark = 0x8BADF00D;

        /* Set user mode thread in the trap frame */
        ThreadFrame->TrapFrame.PreviousMode = UserMode;
    }
    else
    {
        /* This is kernel mode thread */
        ThreadFrame->StartFrame.UserMode = FALSE;
        Thread->PreviousMode = KernelMode;

        /* Disable coprocessor floating point state */
        Thread->NpxState = NPX_STATE_UNLOADED;

        /* Set initial floating point state */
        ThreadFrame->NpxFrame.FxArea.ControlWord = 0x27F;
        ThreadFrame->NpxFrame.FxArea.MxCsr = 0x1F80;
    }

    /* Initialize thread startup information */
    ThreadFrame->StartFrame.StartContext = StartContext;
    ThreadFrame->StartFrame.StartRoutine = StartRoutine;
    ThreadFrame->StartFrame.SystemRoutine = SystemRoutine;

    /* Initialize switch frame */
    ThreadFrame->SwitchFrame.ApcBypassDisabled = TRUE;
    ThreadFrame->SwitchFrame.ExceptionList = (PEXCEPTION_REGISTRATION_RECORD) - 1;

    /* Set thread stack */
    Thread->KernelStack = &ThreadFrame->SwitchFrame;
}
