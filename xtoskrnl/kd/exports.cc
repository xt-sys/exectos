/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/exports.cc
 * DESCRIPTION:     C-compatible API wrappers for exported kernel functions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Prints a formatted string using the configured debug output providers.
 *
 * @param Format
 *        Supplies the format string.
 *
 * @param ...
 *        Supplies the variable argument list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
DbgPrint(PCWSTR Format,
         ...)
{
    VA_LIST Arguments;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    KD::DebugIo::DbgPrintEx(Format, Arguments);

    /* Clean up the va_list */
    VA_END(Arguments);
}
