/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/shell.cc
 * DESCRIPTION:     XT Boot Loader shell
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Starts XTLDR shell.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlStartLoaderShell()
{
    /* Initialize console */
    BlInitializeConsole();

    /* Print prompt */
    BlpPrintShellPrompt();
    for(;;);
}

/**
 * Prints XTLDR shell prompt.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpPrintShellPrompt()
{
    /* Set prompt color */
    BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_YELLOW);

    /* Print prompt */
    BlConsolePrint(L"XTLDR> ");

    /* Reset standard shell colors */
    BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
}
