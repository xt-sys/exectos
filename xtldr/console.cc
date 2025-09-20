/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/console.cc
 * DESCRIPTION:     EFI console support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


/**
 * Clears a specified line on the UEFI text console.
 *
 * @param LineNo
 *        Supplies a line number to clear.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::ClearLine(IN ULONGLONG LineNo)
{
    UINT_PTR Index, ResX, ResY;

    /* Query console mode */
    QueryMode(&ResX, &ResY);

    /* Set cursor position and clear line */
    SetCursorPosition(0, LineNo);
    for(Index = 0; Index < ResX; Index++)
    {
        /* Clear line */
        Write(L" ");
    }
}

/**
 * This routine clears the UEFI console screen.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::ClearScreen()
{
    /* Clear screen */
    EfiSystemTable->ConOut->ClearScreen(EfiSystemTable->ConOut);
}

/**
 * Disables the cursor on the UEFI console.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::DisableCursor()
{
    EfiSystemTable->ConOut->EnableCursor(EfiSystemTable->ConOut, FALSE);
}

/**
 * Enables the cursor on the UEFI console.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::EnableCursor()
{
    EfiSystemTable->ConOut->EnableCursor(EfiSystemTable->ConOut, TRUE);
}

/**
 * This routine initializes the EFI console.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::InitializeConsole()
{
    /* Clear console buffers */
    EfiSystemTable->ConIn->Reset(EfiSystemTable->ConIn, TRUE);
    EfiSystemTable->ConOut->Reset(EfiSystemTable->ConOut, TRUE);
    EfiSystemTable->StdErr->Reset(EfiSystemTable->StdErr, TRUE);

    /* Make sure that current console mode is 80x25 characters, as some broken EFI implementations might
     * set different mode that do not fit on the screen, causing a text to be displayed offscreen */
    if(EfiSystemTable->ConOut->Mode->Mode != 0)
    {
        /* Set console mode to 0, which is standard, 80x25 text mode */
        SetMode(0);
    }

    /* Clear screen and enable cursor */
    SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    ClearScreen();
    EnableCursor();
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
 */
XTCDECL
VOID
Console::Print(IN PCWSTR Format,
               IN ...)
{
    RTL_PRINT_CONTEXT ConsolePrintContext, SerialPrintContext;
    VA_LIST Arguments;

    /* Initialise the print contexts */
    ConsolePrintContext.WriteWideCharacter = PutChar;
    SerialPrintContext.WriteWideCharacter = Debug::PutChar;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    /* Format and print the string to the stdout */
    RTL::WideString::FormatWideString(&ConsolePrintContext, (PWCHAR)Format, Arguments);

    /* Print to serial console only if not running under OVMF */
    if(RTL::WideString::CompareWideString(EfiSystemTable->FirmwareVendor, L"EDK II", 6) != 0)
    {
        /* Check if debugging enabled and if EFI serial port is fully initialized */
        if(DEBUG && (BlpStatus.SerialPort.Flags & COMPORT_FLAG_INIT))
        {
            /* Format and print the string to the serial console */
            RTL::WideString::FormatWideString(&SerialPrintContext, (PWCHAR)Format, Arguments);
        }
    }

    /* Clean up the va_list */
    VA_END(Arguments);
}

/**
 * Writes a character to the default EFI console.
 *
 * @param Character
 *        The integer promotion of the character to be written.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
XTSTATUS
Console::PutChar(IN WCHAR Character)
{
    WCHAR Buffer[2];

    /* Check if character is a newline ('\n') */
    if(Character == L'\n')
    {
        /* Print carriage return ('\r') as well */
        PutChar(L'\r');
    }

    /* Write character to the screen console */
    Buffer[0] = Character;
    Buffer[1] = 0;
    EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, Buffer);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Queries information concerning the output device’s supported text mode.
 *
 * @param ResX
 *        Supplies a buffer to receive the horizontal resolution.
 *
 * @param ResY
 *        Supplies a buffer to receive the vertical resolution.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::QueryMode(OUT PUINT_PTR ResX,
                   OUT PUINT_PTR ResY)
{
    EfiSystemTable->ConOut->QueryMode(EfiSystemTable->ConOut, EfiSystemTable->ConOut->Mode->Mode, ResX, ResY);
}

/**
 * Reads a keystroke from the input device.
 *
 * @param Key
 *        Supplies a pointer to the EFI_INPUT_KEY structure that will receive the keystroke.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::ReadKeyStroke(OUT PEFI_INPUT_KEY Key)
{
    EfiSystemTable->ConIn->ReadKeyStroke(EfiSystemTable->ConIn, Key);
}

/**
 * Resets the console input device and clears its input buffer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::ResetInputBuffer()
{
    EfiSystemTable->ConIn->Reset(EfiSystemTable->ConIn, FALSE);
}

/**
 * Sets the foreground and background colors.
 *
 * @param Attribute
 *        Specifies the foreground and background colors (bits 0..3 are fg, and bits 4..6 are bg color).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::SetAttributes(IN ULONGLONG Attributes)
{
    EfiSystemTable->ConOut->SetAttribute(EfiSystemTable->ConOut, Attributes);
}

/**
 * Sets new coordinates of the console cursor position.
 *
 * @param PosX
 *        Specifies the new X coordinate of the cursor.
 *
 * @param PosY
 *        Specifies the new Y coordinate of the cursor.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::SetCursorPosition(IN ULONGLONG PosX,
                           IN ULONGLONG PosY)
{
    EfiSystemTable->ConOut->SetCursorPosition(EfiSystemTable->ConOut, PosX, PosY);
}

/**
 * Sets the output console device to the requested mode.
 *
 * @param Mode
 *        Supplies a text mode number to set.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Console::SetMode(IN ULONGLONG Mode)
{
    return EfiSystemTable->ConOut->SetMode(EfiSystemTable->ConOut, Mode);
}

/**
 * Displays the string on the device at the current cursor location.
 *
 * @param String
 *        The string to be displayed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Console::Write(IN PCWSTR String)
{
    EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, (PWSTR)String);
}
