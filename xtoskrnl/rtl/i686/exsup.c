/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/i686/exsup.c
 * DESCRIPTION:     Exception handling for i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
