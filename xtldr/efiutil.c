/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/efiutil.c
 * DESCRIPTION:     EFI utilities
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * This routine formats the input string and prints it out to the serial console.
 *
 * @param Format
 *        The formatted string that is to be written to the output.
 *
 * @param ...
 *        Depending on the format string, this routine might expect a sequence of additional arguments.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
BlDbgPrint(IN PUINT16 Format,
           IN ...)
{
    VA_LIST Arguments;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    /* Format and print the string to the serial console */
    BlStringPrint(BlComPortPutChar, Format, Arguments);

    /* Clean up the va_list */
    VA_END(Arguments);
}

/**
 * This routine formats the input string and prints it out to the stdout and serial console.
 *
 * @param Format
 *        The formatted string that is to be written to the output.
 *
 * @param ...
 *        Depending on the format string, this routine might expect a sequence of additional arguments.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @todo Check if GOP is active and use it instead of default conout protocol
 */
VOID
BlEfiPrint(IN PUINT16 Format,
           IN ...)
{
    VA_LIST Arguments;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    /* Format and print the string to the stdout */
    BlStringPrint(BlConsolePutChar, Format, Arguments);

    /* Format and print the string to the serial console */
    BlStringPrint(BlComPortPutChar, Format, Arguments);

    /* Clean up the va_list */
    VA_END(Arguments);
}
