/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/shell.cc
 * DESCRIPTION:     XT Boot Loader shell
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


/**
 * Starts XTLDR shell.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::StartLoaderShell()
{
    /* Initialize console */
    Console::InitializeConsole();

    /* Print prompt */
    PrintPrompt();
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
Shell::PrintPrompt()
{
    /* Set prompt color */
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_YELLOW);

    /* Print prompt */
    Console::Print(L"XTLDR> ");

    /* Reset standard shell colors */
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
}
