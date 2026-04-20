/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/shell.cc
 * DESCRIPTION:     XT Boot Loader shell
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.hh>


/**
 * Implements the built-in `exit` command. Sets the exit flag to signal the main
 * shell loop to terminate and return control to the boot menu.
 *
 * @param Argc
 *        Supplies the number of arguments provided by the user.
 *
 * @param Argv
 *        Supplies a list of arguments provided by the user.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::CommandExit(IN ULONG Argc,
                   IN PWCHAR *Argv)
{
    /* Signal the main shell loop to stop and return to the boot menu */
    ExitRequest = TRUE;
}

/**
 * Implements the built-in `help` command. Prints a list of available commands alongside their descriptions.
 *
 * @param Argc
 *        Supplies the number of arguments provided by the user.
 *
 * @param Argv
 *        Supplies a list of arguments provided by the user.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::CommandHelp(IN ULONG Argc,
                   IN PWCHAR *Argv)
{
    PXTBL_SHELL_COMMAND CommandEntry;
    PLIST_ENTRY ListEntry;

    /* Print a header line */
    Console::Print(L"Available commands:\n\n");

    /* Walk the registered commands list */
    ListEntry = ShellCommands.Flink;
    while(ListEntry != &ShellCommands)
    {
        /* Retrieve the current command entry */
        CommandEntry = CONTAIN_RECORD(ListEntry, XTBL_SHELL_COMMAND, Flink);

        /* Print the command name in a highlighted color */
        Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_WHITE);
        Console::Print(L"  %-12S", CommandEntry->Command);

        /* Print the description in the default color */
        Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
        Console::Print(L" %S\n", CommandEntry->Description);

        /* Advance to the next entry */
        ListEntry = ListEntry->Flink;
    }
}

/**
 * Implements the built-in `poweroff` command. Shuts down the machine.
 *
 * @param Argc
 *        Supplies the number of arguments provided by the user.
 *
 * @param Argv
 *        Supplies a list of arguments provided by the user.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::CommandPoweroff(IN ULONG Argc,
                       IN PWCHAR *Argv)
{
    /* Attempt to power off the machine */
    Console::Print(L"Powering off...\n");
    EfiUtils::ShutdownSystem();

    /* The poweroff call failed, print error message */
    Console::Print(L"ERROR: Failed to power off the machine\n");
}

/**
 * Implements the built-in `reboot` command. Performs a normal system restart via the EFI runtime services.
 * When the '/EFI' parameter is supplied, the routine instead schedules a reboot into the UEFI firmware setup interface.
 *
 * @param Argc
 *        Supplies the number of arguments provided by the user.
 *
 * @param Argv
 *        Supplies a list of arguments provided by the user.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::CommandReboot(IN ULONG Argc,
                     IN PWCHAR *Argv)
{
    /* Check if the /EFI flag was specified */
    if(Argc > 1 && RTL::WideString::CompareWideStringInsensitive(Argv[1], L"/EFI", 0) == 0)
    {
        /* Attempt to reboot into firmware setup */
        Console::Print(L"Rebooting into UEFI firmware setup...\n");
        EfiUtils::EnterFirmwareSetup();

        /* The firmware does not support this feature, print error message */
        Console::Print(L"ERROR: Reboot into firmware setup interface not supported.\n");
    }
    else
    {
        /* Perform a standard system reboot */
        Console::Print(L"Rebooting...\n");
        EfiUtils::RebootSystem();

        /* The reboot call failed, print error message */
        Console::Print(L"ERROR: Failed to reboot the machine\n");
    }
}

/**
 * Implements the built-in `ver` command. Prints the bootloader identification string.
 *
 * @param Argc
 *        Supplies the number of arguments provided by the user.
 *
 * @param Argv
 *        Supplies a list of arguments provided by the user.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::CommandVersion(IN ULONG Argc,
                      IN PWCHAR *Argv)
{
    /* Check if debugging enabled */
    if(DEBUG)
    {
        /* Print debug version of XTLDR version string */
        Console::Print(L"XTLDR Boot Loader v%d.%d (%s-%s)\n",
                       XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR, XTOS_VERSION_DATE, XTOS_VERSION_HASH);
    }
    else
    {
        /* Print standard XTLDR version string */
        Console::Print(L"XTLDR Boot Loader v%d.%d\n", XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR);
    }
}

/**
 * Looks up the given command name in the registered shell commands list and invokes the corresponding handler.
 *
 * @param Argc
 *        Supplies the number of arguments in the argument vector, including the command name itself.
 *
 * @param Argv
 *        Supplies a pointer to the argument vector. First argument is the command name.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::ExecuteCommand(IN ULONG Argc,
                      IN PWCHAR *Argv)
{
    PXTBL_SHELL_COMMAND CommandEntry;
    PLIST_ENTRY ListEntry;

    /* Walk through the list of registered shell commands */
    ListEntry = ShellCommands.Flink;
    while(ListEntry != &ShellCommands)
    {
        /* Retrieve the shell command entry from the list node */
        CommandEntry = CONTAIN_RECORD(ListEntry, XTBL_SHELL_COMMAND, Flink);

        /* Perform a case-insensitive comparison against the command name */
        if(RTL::WideString::CompareWideStringInsensitive(CommandEntry->Command, Argv[0], 0) == 0)
        {
            /* Command matches, invoke its handler and return */
            CommandEntry->Handler(Argc, Argv);
            return;
        }

        /* Advance to the next registered command */
        ListEntry = ListEntry->Flink;
    }

    /* No matching command was found, print error message */
    Console::Print(L"ERROR: '%S' is not recognized as a valid command.\n", Argv[0]);
}

/**
 * Splits the supplied raw command line string into an argument count and an argument vector suitable
 * for command dispatch. The input string is tokenized by whitespace.
 *
 * @param CommandLine
 *        Supplies a mutable wide-character string containing the raw command line.
 *
 * @param Argc
 *        Receives the number of arguments found in the command line.
 *
 * @param Argv
 *        Receives a pointer to an allocated array of wide-character string pointers, one for each argument.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Shell::ParseCommand(IN PWCHAR CommandLine,
                    OUT PULONG Argc,
                    OUT PWCHAR **Argv)
{
    PWCHAR *ArgumentVector, TempLine;
    ULONG ArgumentCount;
    EFI_STATUS Status;

    /* Initialize argument count */
    ArgumentCount = 0;

    /* Count the tokens to determine the size of the argument vector */
    TempLine = CommandLine;
    while(*TempLine != L'\0')
    {
        /* Skip leading spaces */
        while(*TempLine == L' ')
        {
            /* Move to the next character */
            TempLine++;
        }

        /* Check if the end of the string was reached */
        if(*TempLine == L'\0')
        {
            /* End of the string, break the loop */
            break;
        }

        /* One more argument found */
        ArgumentCount++;

        /* Skip the characters of the token */
        while(*TempLine != L'\0' && *TempLine != L' ')
        {
            /* Move to the next character */
            TempLine++;
        }
    }

    /* Check if the command line was empty */
    if(ArgumentCount == 0)
    {
        /* Set argument count and vector to zero and NULL */
        *Argc = 0;
        *Argv = NULLPTR;

        /* Return success */
        return STATUS_EFI_SUCCESS;
    }

    /* Allocate memory for the argument vector */
    Status = Memory::AllocatePool(ArgumentCount * sizeof(PWCHAR), (PVOID *)&ArgumentVector);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, return status code */
        return Status;
    }

    /* Reset argument count and temp line */
    ArgumentCount = 0;
    TempLine = CommandLine;

    /* Walk through the command line */
    while(*TempLine != L'\0')
    {
        /* Skip leading whitespace */
        while(*TempLine == L' ')
        {
            /* Move to the next character */
            TempLine++;
        }

        /* Check if the end of the string was reached */
        if(*TempLine == L'\0')
        {
            /* End of string reached, break the loop */
            break;
        }

        /* Record token */
        ArgumentVector[ArgumentCount] = TempLine;
        ArgumentCount++;

        /* Advance past the token characters */
        while(*TempLine != L'\0' && *TempLine != L' ')
        {
            /* Move to the next character */
            TempLine++;
        }

        /* Check if token was NULL-terminated */
        if(*TempLine != L'\0')
        {
            /* NULL-terminate the token and move to the next character */
            *TempLine = L'\0';
            TempLine++;
        }
    }

    /* Return results to the caller */
    *Argc = ArgumentCount;
    *Argv = ArgumentVector;
    return STATUS_EFI_SUCCESS;
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

    /* Print prompt at the start of the line */
    Console::Print(L"\rXTLDR> ");

    /* Reset standard shell colors */
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
}

/**
 * Prints the whole prompt line, including the current command line and the cursor position.
 *
 * @param Buffer
 *        Supplies a pointer to the buffer containing the command line.
 *
 * @param BufferLength
 *        Supplies the buffer text length.
 *
 * @param CursorPosition
 *        Supplies the current cursor position.
 *
 * @param PreviousBufferLength
 *        Supplies the previous buffer text length to clear artifacts.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::PrintPromptLine(IN PWCHAR Buffer,
                       IN ULONG BufferLength,
                       IN ULONG CursorPosition,
                       IN ULONG PreviousBufferLength)
{
    INT32 TargetX, TargetY;
    WCHAR SavedChar;
    ULONG Index;

    /* Print the prompt */
    PrintPrompt();

    /* Temporarily truncate the string to capture cursor position */
    SavedChar = Buffer[CursorPosition];
    Buffer[CursorPosition] = L'\0';

    /* Print up to the cursor position */
    Console::Print(L"%S", Buffer);

    /* Capture target cursor coordinates from the EFI text mode structure */
    TargetX = XtLoader::GetEfiSystemTable()->ConOut->Mode->CursorColumn;
    TargetY = XtLoader::GetEfiSystemTable()->ConOut->Mode->CursorRow;

    /* Restore the character and print the remainder of the buffer */
    Buffer[CursorPosition] = SavedChar;
    Console::Print(L"%S", Buffer + CursorPosition);

    /* Check if the previous buffer was longer than the current one */
    if(PreviousBufferLength > BufferLength)
    {
        /* Clear artifacts from the previous longer line */
        for(Index = 0; Index < (PreviousBufferLength - BufferLength); Index++)
        {
            /* Print a white space */
            Console::Print(L" ");
        }
    }

    /* Move the cursor back to the correct target position */
    Console::SetCursorPosition(TargetX, TargetY);
}

/**
 * Reads a complete line of input from the shell console into the supplied buffer.
 *
 * @param Buffer
 *        Supplies a pointer to a wide-character buffer that receives the entered command line.
 *
 * @param BufferSize
 *        Supplies the capacity of the buffer, in wide characters.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::ReadCommand(OUT PWCHAR Buffer,
                   IN ULONG BufferSize)
{
    ULONG BufferLength, CursorPosition, OldBufferLength;
    UINT_PTR EventIndex;
    EFI_INPUT_KEY Key;

    /* Start with an empty buffer */
    CursorPosition = 0;
    BufferLength = 0;
    Buffer[0] = L'\0';

    /* Reset history index */
    HistoryIndex = HistoryCount;

    /* Read characters until the user submits the command line */
    while(TRUE)
    {
        /* Wait until a key event is available */
        EfiUtils::WaitForEfiEvent(1, &(XtLoader::GetEfiSystemTable()->ConIn->WaitForKey), &EventIndex);

        /* Read the keystroke from the input device */
        Console::ReadKeyStroke(&Key);

        /* Capture the previous line length to wipe possible artifacts */
        OldBufferLength = BufferLength;

        /* Check the keystroke */
        if(Key.UnicodeChar == 0x0D)
        {
            /* ENTER key pressed, terminate the buffer and move to a new line */
            Buffer[BufferLength] = L'\0';
            Console::Print(L"\n");

            /* Check if the buffer is not empty */
            if(BufferLength > 0)
            {
                /* Check if the history is not full */
                if(HistoryCount < XTBL_SH_HISTORY_ENTRIES)
                {
                    /* Store command in history and increment history count */
                    RTL::Memory::CopyMemory(History[HistoryCount], Buffer, (BufferLength + 1) * sizeof(WCHAR));
                    HistoryCount++;
                }
                else
                {
                    /* Shift history entries to fit new command */
                    RTL::Memory::MoveMemory(History[0],
                                            History[1],
                                            (XTBL_SH_HISTORY_ENTRIES - 1) * XTBL_SH_MAX_LINE_LENGTH * sizeof(WCHAR));
                    RTL::Memory::CopyMemory(History[XTBL_SH_HISTORY_ENTRIES - 1],
                                            Buffer,
                                            (BufferLength + 1) * sizeof(WCHAR));
                }
            }

            /* Return the command line to the caller */
            return;
        }
        else if(Key.ScanCode == 0x01)
        {
            /* UP key pressed, go back in history */
            if(HistoryIndex > 0)
            {
                /* Decrement history index */
                HistoryIndex--;

                /* Copy history entry to buffer and update cursor position */
                BufferLength = RTL::WideString::WideStringLength(History[HistoryIndex], XTBL_SH_MAX_LINE_LENGTH - 1);
                RTL::Memory::CopyMemory(Buffer, History[HistoryIndex], (BufferLength + 1) * sizeof(WCHAR));
                CursorPosition = BufferLength;

                /* Reprint the prompt line */
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue to the next iteration */
            continue;
        }
        else if(Key.ScanCode == 0x02)
        {
            /* DOWN key pressed, go forward in history */
            if(HistoryIndex < HistoryCount)
            {
                /* Increment history index */
                HistoryIndex++;

                /* Check if we are at the end of history */
                if(HistoryIndex == HistoryCount)
                {
                    /* End of history, show empty prompt */
                    Buffer[0] = L'\0';
                    BufferLength = 0;
                    CursorPosition = 0;
                }
                else
                {
                    /* Copy history entry to buffer and update cursor position */
                    BufferLength = RTL::WideString::WideStringLength(History[HistoryIndex],
                                                                     XTBL_SH_MAX_LINE_LENGTH - 1);
                    RTL::Memory::CopyMemory(Buffer, History[HistoryIndex], (BufferLength + 1) * sizeof(WCHAR));
                    CursorPosition = BufferLength;
                }

                /* Reprint the prompt line */
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue to the next iteration */
            continue;
        }
        else if(Key.ScanCode == 0x03)
            {
            /* RIGHT key pressed, move cursor right */
            if(CursorPosition < BufferLength)
            {
                /* Increment cursor position */
                CursorPosition++;

                /* Reprint the prompt line */
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue to the next iteration */
            continue;
        }
        else if(Key.ScanCode == 0x04)
        {
            /* LEFT key pressed, move cursor left */
            if(CursorPosition > 0)
            {
                /* Decrement cursor position */
                CursorPosition--;

                /* Reprint the prompt line */
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue to the next iteration */
            continue;
        }
        else if(Key.ScanCode == 0x05)
        {
            /* HOME key pressed, move cursor to beginning */
            if (CursorPosition > 0)
            {
                /* Set cursor position to beginning of the line and reprint the prompt line */
                CursorPosition = 0;
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue to the next iteration */
            continue;
        }
        else if(Key.ScanCode == 0x06)
        {
            /* END key pressed, move cursor to end */
            if (CursorPosition < BufferLength)
            {
                /* Set cursor position to end of the line and reprint the prompt line */
                CursorPosition = BufferLength;
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue to the next iteration */
            continue;
        }
        else if(Key.ScanCode == 0x17)
        {
            /* ESC key pressed, discard the current input, move to a new line and reprint the prompt */
            Buffer[0] = L'\0';
            Console::Print(L"\n");
            PrintPrompt();

            /* Reset cursor position, buffer length and history index */
            CursorPosition = 0;
            BufferLength = 0;
            HistoryIndex = HistoryCount;

            /* Continue reading the command line */
            continue;
        }
        else if(Key.ScanCode == 0x08)
        {
            /* DELETE key pressed, remove character at cursor */
            if(CursorPosition < BufferLength)
            {
                /* Move memory to remove the character at cursor */
                RTL::Memory::MoveMemory(Buffer + CursorPosition,
                                        Buffer + CursorPosition + 1,
                                        (BufferLength - CursorPosition) * sizeof(WCHAR));

                /* Decrement buffer length and reprint the prompt line */
                BufferLength--;
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue to the next iteration */
            continue;
        }
        else if(Key.UnicodeChar == 0x08)
        {
            /* BACKSPACE key pressed, delete character before cursor */
            if(CursorPosition > 0)
            {
                /* Move memory to remove the character before cursor */
                RTL::Memory::MoveMemory(Buffer + CursorPosition - 1,
                                        Buffer + CursorPosition,
                                        (BufferLength - CursorPosition + 1) * sizeof(WCHAR));

                /* Decrement cursor position and buffer length */
                CursorPosition--;
                BufferLength--;

                /* Reprint the prompt line */
                PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
            }

            /* Continue reading the command line */
            continue;
        }
        else if(Key.UnicodeChar == 0)
        {
            /* Ignore non-printable characters */
            continue;
        }

        /* Make sure there is room in the buffer (reserve one slot for NULL terminator) */
        if(BufferLength < BufferSize - 1)
        {
            /* Insert character in the middle or end of the buffer */
            RTL::Memory::MoveMemory(Buffer + CursorPosition + 1,
                                    Buffer + CursorPosition,
                                    (BufferLength - CursorPosition + 1) * sizeof(WCHAR));
            Buffer[CursorPosition] = Key.UnicodeChar;

            /* Increment cursor position and buffer length */
            CursorPosition++;
            BufferLength++;

            /* Reprint the prompt line */
            PrintPromptLine(Buffer, BufferLength, CursorPosition, OldBufferLength);
        }
    }
}

/**
 * Registers a new command in the XTLDR shell.
 *
 * @param Command
 *        Supplies the command keyword that the user types at the shell prompt.
 *
 * @param Description
 *        Supplies a short help string displayed by the 'help' command.
 *
 * @param Handler
 *        Supplies a pointer to the function that implements the command.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Shell::RegisterCommand(IN PCWSTR Command,
                       IN PCWSTR Description,
                       IN PBL_SHELL_COMMAND Handler)
{
    PXTBL_SHELL_COMMAND CommandEntry;
    PLIST_ENTRY ListEntry;
    EFI_STATUS Status;

    /* Verify that a command with this name has not already been registered */
    ListEntry = ShellCommands.Flink;
    while(ListEntry != &ShellCommands)
    {
        /* Retrieve the existing shell command entry */
        CommandEntry = CONTAIN_RECORD(ListEntry, XTBL_SHELL_COMMAND, Flink);

        /* Compare command names case-insensitively */
        if(RTL::WideString::CompareWideStringInsensitive(CommandEntry->Command, Command, 0) == 0)
        {
            /* Duplicate command name, return error */
            return STATUS_EFI_INVALID_PARAMETER;
        }

        /* Advance to the next entry */
        ListEntry = ListEntry->Flink;
    }

    /* Allocate memory for the new command entry */
    Status = Memory::AllocatePool(sizeof(XTBL_SHELL_COMMAND), (PVOID *)&CommandEntry);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, return error */
        return STATUS_EFI_OUT_OF_RESOURCES;
    }

    /* Populate the new command entry */
    CommandEntry->Command = (PWCHAR)Command;
    CommandEntry->Description = (PWCHAR)Description;
    CommandEntry->Handler = Handler;

    /* Append the command to the global shell commands list */
    RTL::LinkedList::InsertTailList(&ShellCommands, &CommandEntry->Flink);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Registers all built-in shell commands that are provided by the XTLDR.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::RegisterBuiltinCommands()
{
    /* Register all built-in shell commands */
    RegisterCommand(L"exit", L"Exits the shell and returns to the boot menu", CommandExit);
    RegisterCommand(L"help", L"Displays a list of all available shell commands", CommandHelp);
    RegisterCommand(L"poweroff", L"Shuts down the machine", CommandPoweroff);
    RegisterCommand(L"reboot", L"Reboots the machine (/EFI to enter firmware setup)", CommandReboot);
    RegisterCommand(L"ver", L"Displays the boot loader version information", CommandVersion);
}

/**
 * Initializes the command list, registers the built-in commands and enters an interactive XTLDR shell loop.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Shell::StartLoaderShell()
{
    WCHAR CommandLine[XTBL_SH_MAX_LINE_LENGTH];
    PWCHAR *ArgumentVector;
    ULONG ArgumentCount;
    EFI_STATUS Status;

    /* Initialize console */
    Console::InitializeConsole();

    /* Initialize the shell commands list */
    RTL::LinkedList::InitializeListHead(&ShellCommands);

    /* Register all built-in commands */
    RegisterBuiltinCommands();

    /* Clear the shell exit request flag */
    ExitRequest = FALSE;

    /* Main XTLDR shell loop */
    while(!ExitRequest)
    {
        /* Display the shell prompt */
        PrintPrompt();

        /* Read a command line */
        ReadCommand(CommandLine, XTBL_SH_MAX_LINE_LENGTH);

        /* Parse the command line into a list of arguments */
        Status = ParseCommand(CommandLine, &ArgumentCount, &ArgumentVector);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Parsing failed, print error and continue */
            Console::Print(L"ERROR: Failed to parse command line (Status: 0x%llx).\n\n", Status);
            continue;
        }

        /* Check if command line is empty */
        if(ArgumentCount == 0)
        {
            /* Skip empty command line */
            continue;
        }

        /* Check if command line starts with a comment symbol (#) */
        if(ArgumentVector[0][0] != L'#')
        {
            /* Dispatch the command */
            ExecuteCommand(ArgumentCount, ArgumentVector);
        }

        /* Free the argument vector */
        Memory::FreePool(ArgumentVector);

        /* Print a trailing blank line for visual separation */
        Console::Print(L"\n");
    }
}
