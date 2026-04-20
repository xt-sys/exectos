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
 *        Supplies the number of arguments.
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
 *        Supplies the number of arguments.
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
 * Implements the built-in `reboot` command. Performs a normal system restart via the EFI runtime services.
 * When the '/EFI' parameter is supplied, the routine instead schedules a reboot into the UEFI firmware setup interface.
 *
 * @param Argc
 *        Supplies the number of arguments.
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
 *        Supplies the number of arguments.
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
            /* Command matches, invoke its handler */
            CommandEntry->Handler(Argc, Argv);

            /* Print a trailing blank line for visual separation and return */
            Console::Print(L"\n");
            return;
        }

        /* Advance to the next registered command */
        ListEntry = ListEntry->Flink;
    }

    /* No matching command was found, print error message */
    Console::Print(L"ERROR: '%S' is not recognized as a valid command.\n\n", Argv[0]);
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
    PWCHAR Token, SavePtr, TempLine;
    PWCHAR *ArgumentVector;
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

    /* Print prompt */
    Console::Print(L"XTLDR> ");

    /* Reset standard shell colors */
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
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
    ULONG CursorPosition;
    UINT_PTR EventIndex;
    EFI_INPUT_KEY Key;

    /* Start with an empty buffer */
    CursorPosition = 0;
    Buffer[0] = L'\0';

    /* Read characters until the user submits the command line */
    while(TRUE)
    {
        /* Wait until a key event is available */
        EfiUtils::WaitForEfiEvent(1, &(XtLoader::GetEfiSystemTable()->ConIn->WaitForKey), &EventIndex);

        /* Read the keystroke from the input device */
        Console::ReadKeyStroke(&Key);

        /* Check the keystroke */
        if(Key.UnicodeChar == 0x0D)
        {
            /* ENTER key pressed - terminate the buffer and move to a new line */
            Buffer[CursorPosition] = L'\0';
            Console::Print(L"\n");

            /* Return the command line to the caller */
            return;
        }
        else if(Key.ScanCode == 0x17)
        {
            /* ESC key pressed - discard the current input, move to a new line and reprint the prompt */
            Buffer[0] = L'\0';
            Console::Print(L"\n");
            PrintPrompt();
            CursorPosition = 0;

            /* Continue reading the command line */
            continue;
        }
        else if(Key.UnicodeChar == 0x08)
        {
            /* Backspace key pressed - erase the last character from the buffer */
            if(CursorPosition > 0)
            {
                /* Erase the last character from the buffer */
                CursorPosition--;
                Buffer[CursorPosition] = L'\0';

                /* Move the cursor back, overwrite the character with a space, then move back again */
                Console::Print(L"\b \b");
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
        if(CursorPosition < BufferSize - 1)
        {
            /* Append the character to the buffer and NULL-terminate it */
            Buffer[CursorPosition] = Key.UnicodeChar;
            CursorPosition++;
            Buffer[CursorPosition] = L'\0';

            /* Echo the character to the console */
            Console::PutChar(Key.UnicodeChar);
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
    WCHAR CommandLine[XTBL_SHELL_MAX_LINE_LENGTH];
    EFI_STATUS Status;
    PWCHAR *ArgumentVector;
    ULONG ArgumentCount;

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
        ReadCommand(CommandLine, XTBL_SHELL_MAX_LINE_LENGTH);

        /* Parse the command line into a list of arguments */
        Status = ParseCommand(CommandLine, &ArgumentCount, &ArgumentVector);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Parsing failed, print error and continue */
            Console::Print(L"ERROR: Failed to parse command line (Status: 0x%llx).\n", Status);
            continue;
        }

        /* Check if command line is empty */
        if(ArgumentCount == 0)
        {
            /* Skip empty command line */
            continue;
        }

        /* Dispatch the command */
        ExecuteCommand(ArgumentCount, ArgumentVector);

        /* Free the argument vector */
        Memory::FreePool(ArgumentVector);
    }
}
