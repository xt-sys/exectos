/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/i686/exsup.cc
 * DESCRIPTION:     Exception handling for i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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

XTCLINK
XTCDECL
INT
_except_handler3(PEXCEPTION_RECORD ExceptionRecord,
                 PVOID Registration,
                 PCONTEXT Context,
                 PVOID Dispatcher)
{
    UNIMPLEMENTED;

    /* Handler not found */
    return 0;
}

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
