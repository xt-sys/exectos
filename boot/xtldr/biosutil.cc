/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/biosutil.cc
 * DESCRIPTION:     Legacy BIOS support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.hh>


/**
 * Clears the entire screen and moves the cursor to the top-left corner.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BiosUtils::ClearScreen()
{
    VOLATILE PUSHORT VgaBuffer = (PUSHORT)0xB8000;
    USHORT Blank;
    UINT Index;

    /* Set blank character */
    Blank = (0x0F << 8) | L' ';

    /* Fill the entire screen with blank characters */
    for(Index = 0; Index < VgaWidth * VgaHeight; Index++)
    {
        VgaBuffer[Index] = Blank;
    }

    /* Reset cursor position to the top-left corner */
    CursorX = 0;
    CursorY = 0;

    /* Update the hardware cursor position */
    UpdateCursor();
}

/**
 * Formats the input string and prints it out to the screen.
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
XTCDECL
VOID
BiosUtils::Print(IN PCWSTR Format,
                 IN ...)
{
    RTL_PRINT_CONTEXT PrintContext;
    VA_LIST Arguments;

    /* Initialise the print contexts */
    PrintContext.WriteWideCharacter = PutChar;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    /* Format and print the string to the stdout */
    RTL::WideString::FormatWideString(&PrintContext, (PWCHAR)Format, Arguments);

    /* Clean up the va_list */
    VA_END(Arguments);
}

/**
 * Writes a single wide character to the screen using legacy BIOS VGA text mode.
 *
 * @param Character
 *        The wide character to be printed.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
XTSTATUS
BiosUtils::PutChar(IN WCHAR Character)
{
    VOLATILE PUSHORT VgaBuffer = (PUSHORT)0xB8000;
    USHORT VgaCharacter;

    /* Handle special characters */
    if(Character == L'\n')
    {
        /* Move to the next line */
        CursorX = 0;
        CursorY++;
    }
    else if(Character == L'\r')
    {
        /* Move to the beginning of the current line */
        CursorX = 0;
    }
    else
    {
        /* Print character and move cursor to the right */
        VgaCharacter = (0x0F << 8) | (Character & 0xFF);
        VgaBuffer[CursorY * VgaWidth + CursorX] = VgaCharacter;
        CursorX++;
    }

    /* Handle moving to the next line if cursor is at the end of the line */
    if(CursorX >= VgaWidth)
    {
        CursorX = 0;
        CursorY++;
    }

    /* Handle scrolling if cursor is at the end of the screen */
    if(CursorY >= VgaHeight)
    {
        ScrollScreen();
        CursorY = VgaHeight - 1;
    }

    /* Update the hardware cursor position */
    UpdateCursor();

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Scrolls the entire screen content up by one line and clears the last line.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BiosUtils::ScrollScreen()
{
    VOLATILE PUSHORT VgaBuffer = (PUSHORT)0xB8000;
    USHORT Blank;
    UINT Index;

    /* Set blank character */
    Blank = (0x0F << 8) | L' ';

    /* Move every line up by one */
    for(Index = 0; Index < (VgaHeight - 1) * VgaWidth; Index++)
    {
        VgaBuffer[Index] = VgaBuffer[Index + VgaWidth];
    }

    /* Clear the last line */
    for(Index = (VgaHeight - 1) * VgaWidth; Index < VgaHeight * VgaWidth; Index++)
    {
        VgaBuffer[Index] = Blank;
    }
}

/**
 * Updates the hardware cursor position on the screen.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BiosUtils::UpdateCursor()
{
    USHORT Position;

    /* Calculate cursor position */
    Position = CursorY * VgaWidth + CursorX;

    /* Send command to set the high byte of the cursor position */
    HL::IoPort::WritePort8(0x3D4, 0x0E);
    HL::IoPort::WritePort8(0x3D5, (UCHAR)((Position >> 8) & 0xFF));

    /* Send command to set the low byte of the cursor position */
    HL::IoPort::WritePort8(0x3D4, 0x0F);
    HL::IoPort::WritePort8(0x3D5, (UCHAR)(Position & 0xFF));
}
