/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/i686/exsup.cc
 * DESCRIPTION:     Exception handling for i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Handles SEH structured exception frames.
 *
 * @param ExceptionRecord
 *        A pointer to the exception record.
 *
 * @param EstablisherFrame
 *        The address of the base of the fixed stack allocation.
 *
 * @param ContextRecord
 *       A pointer to the context record at the time the exception was raised.
 *
 * @param DispatcherContext
 *      A pointer to the dispatcher context for the function.
 *
 * @return This routine returns an exception disposition value if the exception was not handled by any filter.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
EXCEPTION_DISPOSITION
__C_specific_handler(IN PEXCEPTION_RECORD ExceptionRecord,
                     IN PVOID EstablisherFrame,
                     IN OUT PCONTEXT ContextRecord,
                     IN OUT PVOID DispatcherContext)
{
    UNIMPLEMENTED;

    /* Continue execution */
    return ExceptionContinueExecution;
}

/**
 * Handles C++ structured exception frames. This implementation displays a panic screen and halts the system.
 *
 * @param ExceptionRecord
 *        A pointer to the exception record that is passed to the possible catch statements.
 *
 * @param EstablisherFrame
 *        A pointer to the stack frame that is used to handle the exception.
 *
 * @param ContextRecord
 *       A pointer to the context record (not used on Intel CPUs).
 *
 * @param DispatcherContext
 *      A pointer to the dispatcher provding information about function entry and stack frame (not used on Intel CPUs).
 *
 * @return This routine returns an exception disposition value if the exception was not handled by any filter.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
EXCEPTION_DISPOSITION
__CxxFrameHandler3(IN PEXCEPTION_RECORD ExceptionRecord,
                   IN PVOID EstablisherFrame,
                   IN OUT PCONTEXT ContextRecord,
                   IN OUT PVOID DispatcherContext)
{
    UNIMPLEMENTED;

    /* Disable interrupts and hang */
    AR::CpuFunc::ClearInterruptFlag();
    KE::Crash::Panic(0); // CXX_FRAME_HANDLER_CALLED

    /* Continue search */
    return ExceptionContinueSearch;
}

/**
 * Finds the appropriate exception handler to process the current exception.
 *
 * @param ExceptionRecord
 *        A pointer to the exception record providing information about the specific exception.
 *
 * @param Registration
 *        A pointer to the record that indicates which scope table should be used to find the exception handler.
 *
 * @param Context
 *        Reserved.
 *
 * @param Dispatcher
 *        Reserved.
 *
 * @return This routine returns DISPOSITION_DISMISS or DISPOSITION_CONTINUE_SEARCH.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
INT
_except_handler3(IN PEXCEPTION_RECORD ExceptionRecord,
                 IN PVOID Registration,
                 IN PCONTEXT Context,
                 IN PVOID Dispatcher)
{
    UNIMPLEMENTED;

    /* Handler not found */
    return 0;
}

/**
 * Handles pure virtual function call error. This implementation displays a panic screen and halts the system.
 *
 * @return This function does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
_purecall(VOID)
{
    UNIMPLEMENTED;

    /* Disable interrupts and hang */
    AR::CpuFunc::ClearInterruptFlag();
    KE::Crash::Panic(0); // PURE_VIRTUAL_FUNCTION_CALL
}
