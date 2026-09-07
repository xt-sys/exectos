/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/amd64/debug.cc
 * DESCRIPTION:     Kernel Debugger
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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
