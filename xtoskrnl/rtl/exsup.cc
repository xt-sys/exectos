/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/exsup.cc
 * DESCRIPTION:     Exception handling
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Raises a structured exception using an exception record.
 *
 * @param ExceptionRecord
 *        Supplies a pointer to the exception record containing details.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Exception::RaiseException(IN PEXCEPTION_RECORD ExceptionRecord)
{
    /* Not implemented */
    UNIMPLEMENTED;

    /* Disable interrupts */
    AR::CpuFunctions::ClearInterruptFlag();

    /* Halt the system */
    KE::Crash::Panic(ExceptionRecord->ExceptionCode);
}

/**
 * Raises a structured exception based on the provided status code.
 *
 * @param Status
 *        Supplies the status code to be raised as an exception.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Exception::RaiseStatus(IN XTSTATUS Status)
{
    EXCEPTION_RECORD ExceptionRecord;

    /* Not implemented */
    UNIMPLEMENTED;

    /* Create a basic exception record */
    ExceptionRecord.ExceptionAddress = _ReturnAddress();
    ExceptionRecord.ExceptionCode = Status;
    ExceptionRecord.ExceptionRecord = NULLPTR;
    ExceptionRecord.NumberParameters = 0;
    ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

    /* Dispatch the exception */
    RaiseException(&ExceptionRecord);
}

/**
 * Determines whether a SEH exception should be handled or lead to a kernel panic based on the processor mode.
 *
 * @return This routine returns EXCEPTION_EXECUTE_HANDLER if the CPU was in UserMode,
 *         or EXCEPTION_CONTINUE_SEARCH if the CPU was in KernelMode.
 *
 * @since XT 1.0
 */
XTAPI
LONG
RTL::Exception::SystemFilter(VOID)
{
    /* Check if the exception originated from kernel-mode execution */
    if(KE::Processor::GetCurrentThread()->PreviousMode == KernelMode)
    {
        /* Kernel-mode fault, force a kernel panic */
        return EXCEPTION_CONTINUE_SEARCH;
    }
    else
    {
        /* User-mode fault, catch it and return the error status gracefully */
        return EXCEPTION_EXECUTE_HANDLER;
    }
}
