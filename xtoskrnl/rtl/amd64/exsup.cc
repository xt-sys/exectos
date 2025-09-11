/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/amd64/exsup.cc
 * DESCRIPTION:     Exception handling for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


XTCLINK
XTAPI
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
