/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/console.c
 * DESCRIPTION:     EFI console support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * This routine clears the UEFI console screen.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
BlConsoleClearScreen()
{
    EfiSystemTable->ConOut->ClearScreen(EfiSystemTable->ConOut);
}

/**
 * This routine initializes the EFI console.
 * 
 * @return This routine returns status code.
 * 
 * @since XT 1.0
 */
VOID
BlConsoleInitialize()
{
    /* Clear console buffers */
    EfiSystemTable->ConIn->Reset(EfiSystemTable->ConIn, TRUE);
    EfiSystemTable->ConOut->Reset(EfiSystemTable->ConOut, TRUE);
    EfiSystemTable->StdErr->Reset(EfiSystemTable->StdErr, TRUE);

    /* Clear screen */
    BlConsoleClearScreen();

    /* Enable cursor */
    EfiSystemTable->ConOut->EnableCursor(EfiSystemTable->ConOut, TRUE);
}

/**
 * Writes a character to the default EFI console.
 *
 * @param Character
 *        The integer promotion of the character to be written.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
BlConsolePutChar(IN USHORT Character)
{
    USHORT Buffer[2];

    /* Write character to the screen console */
    Buffer[0] = Character;
    Buffer[1] = 0;
    EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, Buffer);
}
