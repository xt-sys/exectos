/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/debug.cc
 * DESCRIPTION:     Kernel Debugger
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Determines whether the interactive kernel debugger is currently active and controlling the execution flow.
 *
 * @return This routine returns TRUE if the kernel debugger is active, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
KD::Debugger::DebuggerActive(VOID)
{
    return Active;
}

/**
 * Serves as the primary entry point for the interactive kernel debugger.
 *
 * @param TrapFrame
 *        Supplies a pointer to the processor's trap frame captured at the moment the breakpoint or exception occurred.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KD::Debugger::EnterDebugger(IN PKTRAP_FRAME TrapFrame)
{
    /* Mark the debugger as active */
    Active = TRUE;

    /* Print debug message and enter an infinite loop */
    DebugPrint(L"\n\n*** KDebugger Entered at RIP 0x%.16llX ***\n", TrapFrame->Rip);
    for(;;);

    /* Mark the debugger as inactive */
    Active = FALSE;
}

/**
 * Processes a processor state change and acts as the primary event loop for the debugger.
 *
 * @param ExceptionRecord
 *        Supplies a pointer to the exception record that triggered the state change.
 *
 * @param Context
 *        Supplies a pointer to the processor's context frame.
 *
 * @param SecondChance
 *        Supplies a boolean value indicating whether this is a second-chance exception.
 *
 * @return This routine returns TRUE if the exception was handled by the debugger, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
KD::Debugger::ProcessCpuStateChange(IN PEXCEPTION_RECORD ExceptionRecord,
                                    IN OUT PCONTEXT Context,
                                    IN BOOLEAN SecondChance)
{
    return FALSE;
}

/**
 * Transfers active control to a previously frozen processor.
 *
 * @return This routine returns a value indicating how execution should proceed after the debugging session concludes.
 *
 * @since XT 1.0
 */
XTAPI
KCONTINUE_STATUS
KD::Debugger::SwitchProcessor(VOID)
{
    EXCEPTION_RECORD ExceptionRecord;
    PKPROCESSOR_CONTROL_BLOCK Prcb;

    /* Get processor control block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Construct an exception record */
    ExceptionRecord.ExceptionAddress = (PVOID)&Prcb->ProcessorState.ContextFrame.Rip;
    ExceptionRecord.ExceptionCode = STATUS_WAKE_SYSTEM_DEBUGGER;
    ExceptionRecord.ExceptionRecord  = &ExceptionRecord;

    /* Pass the synthetic exception and the processor context to the debugger */
    return (KCONTINUE_STATUS)ProcessCpuStateChange(&ExceptionRecord, &Prcb->ProcessorState.ContextFrame, FALSE);
}
