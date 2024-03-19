/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/textui.c
 * DESCRIPTION:     Text console User Interface (TUI) support for XT Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Displays a simple TUI-based boot menu.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlDisplayBootMenu()
{
    XTBL_DIALOG_HANDLE Handle;
    PXTBL_BOOTMENU_ITEM MenuEntries = NULL;
    ULONG Index, NumberOfEntries, HighligtedEntryId;
    UINT_PTR EventIndex;
    EFI_EVENT Events[2];
    EFI_INPUT_KEY Key;
    EFI_EVENT TimerEvent;
    EFI_STATUS Status;
    LONG TimeOut;
    PWCHAR TimeOutString;

    /* Initialize boot menu list */
    BlInitializeBootMenuList(MenuEntries, &NumberOfEntries, &HighligtedEntryId);

    /* Get timeout from the configuration */
    TimeOutString = BlGetConfigValue(L"TIMEOUT");
    TimeOut = -1;

    /* Check if timeout is specified */
    if(TimeOutString != NULL)
    {
        /* Convert timeout string to number */
        TimeOut = 0;
        while(*TimeOutString >= '0' && *TimeOutString <= '9')
        {
            TimeOut *= 10;
            TimeOut += *TimeOutString - '0';
            TimeOutString++;
        }
    }

    /* Infinite boot menu loop */
    while(TRUE)
    {
        /* Draw boot menu */
        BlpDrawBootMenu(&Handle);

        /* Check if there is anything to show in the boot menu */
        if(NumberOfEntries > 0) {
            /* Check if all menu entries will fit into the menu box */
            if(NumberOfEntries > Handle.Height - 2)
            {
                /* Too many menu entries, limit entries to match box height (-2 for upper and bottom borders) */
                NumberOfEntries = Handle.Height - 2;
            }

            /* Iterate through all menu entries */
            for(Index = 0; Index < NumberOfEntries; Index++)
            {
                /* Draw menu entry */
                BlpDrawBootMenuEntry(&Handle, MenuEntries[Index].EntryName, Index, Index == HighligtedEntryId);
            }
        }
        else
        {
            /* No menu entries found, show error message */
            BlDisplayErrorDialog(L"XTLDR", L"No boot menu entries found in the configuration. Falling back to shell.");

            /* Exit into XTLDR shell */
            return;
        }

        /* Create a timer event for controlling the timeout of the boot menu */
        Status = EfiSystemTable->BootServices->CreateEvent(EFI_EVENT_TIMER, EFI_TPL_CALLBACK, NULL, NULL, &TimerEvent);
        if(Status == STATUS_EFI_SUCCESS)
        {
            /* Setup new EFI timer */
            Status = EfiSystemTable->BootServices->SetTimer(TimerEvent, TimerPeriodic, 10000000);
        }

        /* Check is EFI timer was successfully created */
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Timer creation failed, disable the timer */
            TimeOut = -1;
        }

        /* Initialize EFI events */
        Events[0] = EfiSystemTable->ConIn->WaitForKey;
        Events[1] = TimerEvent;

        /* Infinite boot menu event loop */
        while(TRUE)
        {
            /* Wait for EFI event */
            BlWaitForEfiEvent(2, Events, &EventIndex);

            /* Check which event was received */
            if(EventIndex == 0)
            {
                /* Key pressed, check if timer is still active */
                if(TimeOut != -1)
                {
                    /* Disable the timer */
                    TimeOut = -1;

                    /* Cancel timer event */
                    EfiSystemTable->BootServices->SetTimer(TimerEvent, TimerCancel, 0);

                    /* Remove the timer message */
                    BlClearConsoleLine(Handle.PosY + Handle.Height + 4);
                }

                /* Read key stroke */
                BlReadKeyStroke(&Key);

                if(Key.ScanCode == 0x03 || Key.UnicodeChar == 0x0D)
                {
                    /* ENTER or RightArrow key pressed, boot the highlighted OS */
                    BlSetConsoleAttributes(Handle.DialogColor | Handle.TextColor);
                    BlClearConsoleLine(Handle.PosY + Handle.Height + 4);
                    BlSetCursorPosition(4, Handle.PosY + Handle.Height + 4);
                    BlConsolePrint(L"Booting '%S' now...\n", MenuEntries[HighligtedEntryId].EntryName);

                    /* Boot the highlighted (chosen) OS */
                    Status = BlInvokeBootProtocol(MenuEntries[HighligtedEntryId].ShortName,
                                                  MenuEntries[HighligtedEntryId].Options);
                    if(Status != STATUS_SUCCESS)
                    {
                        /* Failed to boot OS */
                        BlDebugPrint(L"ERROR: Failed to boot '%S' (Status Code: 0x%zX)\n",
                                     MenuEntries[HighligtedEntryId].EntryName, Status);
                        BlDisplayErrorDialog(L"XTLDR", L"Failed to startup the selected Operating System.");
                    }

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.ScanCode == 0x01)
                {
                    /* UpArrow key pressed, go to previous entry if possible */
                    if(HighligtedEntryId > 0)
                    {
                        /* Highlight previous entry */
                        HighligtedEntryId--;
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId + 1].EntryName,
                                             HighligtedEntryId + 1, FALSE);
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId].EntryName,
                                             HighligtedEntryId, TRUE);
                    }
                }
                else if(Key.ScanCode == 0x02)
                {
                    /* DownArrow key pressed, go to next entry if possible */
                    if(HighligtedEntryId < NumberOfEntries - 1)
                    {
                        /* Highlight next entry */
                        HighligtedEntryId++;
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId - 1].EntryName,
                                             HighligtedEntryId - 1, FALSE);
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId].EntryName,
                                             HighligtedEntryId, TRUE);
                    }
                }
                else if(Key.ScanCode == 0x09)
                {
                    /* PageUp key pressed, go to top entry */
                    if(HighligtedEntryId != 0)
                    {
                        /* Highlight first entry */
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId].EntryName,
                                             HighligtedEntryId, FALSE);
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[0].EntryName, 0, TRUE);

                        /* Update highlighted entry ID */
                        HighligtedEntryId = 0;
                    }
                }
                else if(Key.ScanCode == 0x0A)
                {
                    /* PageDown key pressed, go to bottom entry */
                    if(HighligtedEntryId != NumberOfEntries - 1)
                    {
                        /* Highlight last entry */
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId].EntryName,
                                             HighligtedEntryId, FALSE);
                        BlpDrawBootMenuEntry(&Handle, MenuEntries[NumberOfEntries - 1].EntryName,
                                             NumberOfEntries - 1, TRUE);

                        /* Update highlighted entry ID */
                        HighligtedEntryId = NumberOfEntries - 1;
                    }
                }
                else if(Key.ScanCode == 0x0B)
                {
                    /* F1 key pressed, show help */
                    BlDisplayInfoDialog(L"XTLDR", L"XTLDR, the XTOS Boot Loader for UEFI and EFI-based machines.\n"
                                                  L" \n"
                                                  L"Use arrow keys (Up/Down) to change the highlighted entry and\n"
                                                  L"PgUp/PgDown keys to jump to the first/last position.\n"
                                                  L" \n"
                                                  L"Press ENTER key to boot the highlighted boot menu entry.\n"
                                                  L"Press 'e' key to edit the highlighted menu entry.\n"
                                                  L"Press 's' key to exit into XTLDR shell (enters advanced mode).\n"
                                                  L" \n"
                                                  L"F1 shows this help, F10 reboots into UEFI firmware interface,\n"
                                                  L"F11 reboots the machine and F12 turns it off.\n"
                                                  L" \n"
                                                  L" \n"
                                                  L"XTLDR is a part of the ExectOS Operating System.\n"
                                                  L"Visit https://exectos.eu.org/ for more information.");

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.ScanCode == 0x14)
                {
                    /* F10 key pressed, reboot into UEFI setup interface */
                    BlEnterFirmwareSetup();
                    BlDisplayErrorDialog(L"XTLDR", L"Reboot into firmware setup interface not supported!");

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.ScanCode == 0x15)
                {
                    /* F11 key pressed, reboot the machine */
                    BlRebootSystem();
                    BlDisplayErrorDialog(L"XTLDR", L"Failed to reboot the machine!");

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.ScanCode == 0x16)
                {
                    /* F12 key pressed, shutdown the machine */
                    BlShutdownSystem();
                    BlDisplayErrorDialog(L"XTLDR", L"Failed to shutdown the machine!");

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.UnicodeChar == 0x65)
                {
                    /* 'e' key pressed, edit the highlighted entry */
                    BlDisplayErrorDialog(L"XTLDR", L"Editing boot menu entries is not implemented yet!");

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.UnicodeChar == 0x73)
                {
                    /* 's' key pressed, exit into XTLDR shell */
                    return;
                }
            }
            else
            {
                /* Timer tick, check if time out expired */
                if(TimeOut > 0)
                {
                    /* Update a message and decrease timeout value */
                    BlSetConsoleAttributes(Handle.DialogColor | Handle.TextColor);
                    BlClearConsoleLine(Handle.PosY + Handle.Height + 4);
                    BlSetCursorPosition(4, Handle.PosY + Handle.Height + 4);
                    BlConsolePrint(L"The highlighted position will be booted automatically in %ld seconds.", TimeOut);
                    TimeOut--;
                }
                else if(TimeOut == 0)
                {
                    /* Time out expired, update a message */
                    BlSetConsoleAttributes(Handle.DialogColor | Handle.TextColor);
                    BlClearConsoleLine(Handle.PosY + Handle.Height + 4);
                    BlSetCursorPosition(4, Handle.PosY + Handle.Height + 4);
                    BlConsolePrint(L"Booting '%S' now...\n", MenuEntries[HighligtedEntryId].EntryName);

                    /* Disable the timer just in case booting OS fails */
                    TimeOut = -1;

                    /* Boot the highlighted (default) OS */
                    Status = BlInvokeBootProtocol(MenuEntries[HighligtedEntryId].ShortName,
                                                  MenuEntries[HighligtedEntryId].Options);
                    if(Status != STATUS_SUCCESS)
                    {
                        /* Failed to boot OS */
                        BlDebugPrint(L"ERROR: Failed to boot '%S' (Status Code: 0x%zX)\n",
                                     MenuEntries[HighligtedEntryId].EntryName, Status);
                        BlDisplayErrorDialog(L"XTLDR", L"Failed to startup the selected Operating System.");
                    }
                    break;
                }
            }
        }
    }
}

/**
 * Displays a red error dialog box with the specified caption and message.
 *
 * @param Caption
 *        Supplies a caption string put on the dialog box.
 *
 * @param Message
 *        Supplies a message string put on the dialog box.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlDisplayErrorDialog(IN PWCHAR Caption,
                     IN PWCHAR Message)
{
    XTBL_DIALOG_HANDLE Handle;
    EFI_INPUT_KEY Key;
    UINT_PTR Index;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_ERROR_BOX | XTBL_TUI_DIALOG_ACTIVE_BUTTON;

    /* Determine dialog window size and position */
    BlpDetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    BlDisableConsoleCursor();
    BlpDrawDialogBox(&Handle, Caption, Message);

    /* Draw active button */
    BlpDrawDialogButton(&Handle);

    /* Initialize key stroke */
    Key.ScanCode = 0;
    Key.UnicodeChar = 0;

    /* Wait until ENTER or ESC key is pressed */
    while(Key.ScanCode != 0x17 && Key.UnicodeChar != 0x0D)
    {
        /* Wait for key press and read key stroke */
        BlWaitForEfiEvent(1, &EfiSystemTable->ConIn->WaitForKey, &Index);
        BlReadKeyStroke(&Key);
        BlResetConsoleInputBuffer();
    }

    /* Clear screen to remove dialog box */
    BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    BlClearConsoleScreen();
}

/**
 * Displays a blue informational dialog box with the specified caption and message.
 *
 * @param Caption
 *        Supplies a caption string put on the dialog box.
 *
 * @param Message
 *        Supplies a message string put on the dialog box.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlDisplayInfoDialog(IN PWCHAR Caption,
                    IN PWCHAR Message)
{
    XTBL_DIALOG_HANDLE Handle;
    EFI_INPUT_KEY Key;
    UINT_PTR Index;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_GENERIC_BOX | XTBL_TUI_DIALOG_ACTIVE_BUTTON;

    /* Determine dialog window size and position */
    BlpDetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    BlDisableConsoleCursor();
    BlpDrawDialogBox(&Handle, Caption, Message);

    /* Draw active button */
    BlpDrawDialogButton(&Handle);

    /* Initialize key stroke */
    Key.ScanCode = 0;
    Key.UnicodeChar = 0;

    /* Wait until ENTER or ESC key is pressed */
    while(Key.ScanCode != 0x17 && Key.UnicodeChar != 0x0D)
    {
        /* Wait for key press and read key stroke */
        BlWaitForEfiEvent(1, &EfiSystemTable->ConIn->WaitForKey, &Index);
        BlReadKeyStroke(&Key);
        BlResetConsoleInputBuffer();
    }

    /* Clear screen to remove dialog box */
    BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    BlClearConsoleScreen();
}

/**
 * Displays a blue informational dialog box with the specified caption and message and an input field.
 *
 * @param Caption
 *        Specifies a caption string put on the dialog box.
 *
 * @param Message
 *        Specifies a message string put on the dialog box.
 *
 * @param InputFieldText
 *        Specifies a pointer to the input field text that will be edited.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlDisplayInputDialog(IN PWCHAR Caption,
                     IN PWCHAR Message,
                     IN PWCHAR *InputFieldText)
{
    SIZE_T InputFieldLength, TextCursorPosition, TextIndex, TextPosition;
    XTBL_DIALOG_HANDLE Handle;
    PWCHAR InputFieldBuffer;
    SIZE_T BufferLength;
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;
    UINT_PTR Index;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_GENERIC_BOX | XTBL_TUI_DIALOG_ACTIVE_INPUT | XTBL_TUI_DIALOG_INACTIVE_BUTTON;

    /* Determine dialog window size and position */
    BlpDetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    BlDisableConsoleCursor();
    BlpDrawDialogBox(&Handle, Caption, Message);

    /* Draw inactive button */
    BlpDrawDialogButton(&Handle);

    /* Draw active input field */
    BlpDrawDialogInputField(&Handle, *InputFieldText);

    /* Initialize key stroke */
    Key.ScanCode = 0;
    Key.UnicodeChar = 0;

    /* Get initial input text length and allocate a buffer */
    BufferLength = RtlWideStringLength(*InputFieldText, 0);
    Status = BlAllocateMemoryPool(BufferLength * sizeof(WCHAR), (PVOID *)&InputFieldBuffer);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, print error message and return */
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
        BlDisplayErrorDialog(L"XTLDR", L"Failed to allocate memory for input field buffer.");
        return;
    }

    /* Copy input text into edit buffer */
    RtlCopyMemory(InputFieldBuffer, *InputFieldText, BufferLength * sizeof(WCHAR));
    InputFieldBuffer[BufferLength] = L'\0';

    /* Determine input field length */
    InputFieldLength = BufferLength;
    if(InputFieldLength > Handle.Width - 8)
    {
        InputFieldLength = Handle.Width - 8;
    }

    /* Start at first character */
    TextPosition = 0;
    BlSetCursorPosition(Handle.PosX + 4 + TextPosition, Handle.PosY + Handle.Height - 4);

    /* Wait until ENTER or ESC key is pressed */
    while(TRUE)
    {
        /* Wait for key press and read key stroke */
        BlWaitForEfiEvent(1, &EfiSystemTable->ConIn->WaitForKey, &Index);
        BlReadKeyStroke(&Key);
        BlResetConsoleInputBuffer();

        /* Check key press scan code */
        if(Key.ScanCode == 0x17)
        {
            /* ESC key pressed, return */
            break;
        }
        else if(Key.UnicodeChar == 0x09)
        {
            /* TAB key pressed, toggle input field and button */
            Handle.Attributes ^= (XTBL_TUI_DIALOG_ACTIVE_INPUT | XTBL_TUI_DIALOG_INACTIVE_INPUT);
            Handle.Attributes ^= (XTBL_TUI_DIALOG_ACTIVE_BUTTON | XTBL_TUI_DIALOG_INACTIVE_BUTTON);
        }
        else if(Key.ScanCode == 0x03)
        {
            /* RIGHT key pressed, move cursor forward */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT && TextPosition < InputFieldLength)
            {
                TextPosition++;
            }
        }
        else if(Key.ScanCode == 0x04)
        {
            /* LEFT key pressed, move cursor back */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT && TextPosition > 0)
            {
                TextPosition--;
            }
        }
        else if(Key.ScanCode == 0x05)
        {
            /* HOME key pressed, move cursor to the beginning */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
            {
                TextPosition = 0;
            }
        }
        else if(Key.ScanCode == 0x06)
        {
            /* END key pressed, move cursor to the end */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
            {
                TextPosition = InputFieldLength;
            }
        }
        else if(Key.ScanCode == 0x08)
        {
            /* DELETE key pressed, delete character */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
            {
                if(InputFieldLength > 0 && TextPosition < InputFieldLength)
                {
                    RtlMoveMemory(InputFieldBuffer + TextPosition, InputFieldBuffer + TextPosition + 1,
                                  (InputFieldLength - TextPosition) * sizeof(WCHAR));
                    InputFieldLength--;
                    InputFieldBuffer[InputFieldLength] = 0;
                }
            }
        }
        else if(Key.UnicodeChar == 0x08)
        {
            /* BACKSPACE key pressed, delete character */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
            {
                if(InputFieldLength > 0 && TextPosition > 0 && TextPosition <= InputFieldLength)
                {
                    TextPosition--;
                    RtlMoveMemory(InputFieldBuffer + TextPosition, InputFieldBuffer + TextPosition + 1,
                                  (InputFieldLength - TextPosition) * sizeof(WCHAR));
                    InputFieldLength--;
                    InputFieldBuffer[InputFieldLength] = 0;
                }
            }
        }
        else if(Key.UnicodeChar == 0x0D)
        {
            /* ENTER key pressed, update input buffer */
            *InputFieldText = InputFieldBuffer;
            break;
        }
        else
        {
            /* Other key pressed, add character to the buffer */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT && Key.UnicodeChar != 0)
            {
                RtlMoveMemory(InputFieldBuffer + TextPosition + 1, InputFieldBuffer + TextPosition,
                              (InputFieldLength - TextPosition) * sizeof(WCHAR));
                InputFieldBuffer[TextPosition] = Key.UnicodeChar;
                TextPosition++;
                InputFieldLength++;
                InputFieldBuffer[InputFieldLength] = 0;
            }
        }

        if(TextPosition > (Handle.Width - 9))
        {
            TextIndex = TextPosition - (Handle.Width - 9);
            TextCursorPosition = Handle.Width - 9;
        }
        else
        {
            TextIndex = 0;
            TextCursorPosition = TextPosition;
        }

        /* Redraw input field and button */
        BlpDrawDialogButton(&Handle);
        BlpDrawDialogInputField(&Handle, &InputFieldBuffer[TextIndex]);

        if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
        {
            BlSetCursorPosition(Handle.PosX + 4 + TextCursorPosition, Handle.PosY + Handle.Height - 4);
        }
    }

    /* Clear screen to remove dialog box */
    BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    BlClearConsoleScreen();
}

/**
 * Displays a blue informational dialog box with the specified caption and message and a progress bar.
 *
 * @param Caption
 *        Supplies a caption string put on the dialog box.
 *
 * @param Message
 *        Supplies a message string put on the dialog box.
 *
 * @param Percentage
 *        Specifies the percentage progress of the progress bar.
 *
 * @return This routine returns a dialog box handle needed to update the progress bar.
 *
 * @since XT 1.0
 */
XTCDECL
XTBL_DIALOG_HANDLE
BlDisplayProgressDialog(IN PWCHAR Caption,
                        IN PWCHAR Message,
                        IN UCHAR Percentage)
{
    XTBL_DIALOG_HANDLE Handle;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_GENERIC_BOX | XTBL_TUI_DIALOG_PROGRESS_BAR;

    /* Determine dialog window size and position */
    BlpDetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    BlDisableConsoleCursor();
    BlpDrawDialogBox(&Handle, Caption, Message);

    /* Draw active button */
    BlpDrawDialogProgressBar(&Handle, Percentage);

    /* Return dialog handle */
    return Handle;
}

/**
 * Updates the progress bar on the dialog box.
 *
 * @param Handle
 *        Supplies a pointer to the dialog box handle.
 *
 * @param Message
 *        Supplies a new message that will be put on the dialog box, while updating the progress bar.
 *
 * @param Percentage
 *        Specifies the new percentage progress of the progress bar.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlUpdateProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                    IN PWCHAR Message,
                    IN UCHAR Percentage)
{
    /* Check if message needs an update */
    if(Message != NULL)
    {
        /* Update a message on the dialog box */
        BlpDrawDialogMessage(Handle, Message);
    }

    /* Update progress bar */
    BlpDrawDialogProgressBar(Handle, Percentage);
}

/**
 * Determines dialog box size based on enabled components and message length.
 *
 * @param Handle
 *        Supplies a pointer to the dialog box handle.
 *
 * @param Message
 *        Supplies a pointer to the message string put on the dialog box.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDetermineDialogBoxSize(IN OUT PXTBL_DIALOG_HANDLE Handle,
                          IN PWCHAR Message)
{
    UINT_PTR Width, Height, LineLength;
    SIZE_T Index, MessageLength;
    UCHAR Attributes;
    ULONG Mask;

    /* Set minimum dialog window size */
    Height = 4;
    Width = 36;

    /* Zero line length */
    LineLength = 0;

    /* Adjust window height according to enabled components */
    Mask = 1;
    Attributes = Handle->Attributes;
    while(Mask)
    {
        /* Check enabled components that affect dialog window size */
        switch(Attributes & Mask)
        {
            case XTBL_TUI_DIALOG_ACTIVE_BUTTON:
            case XTBL_TUI_DIALOG_INACTIVE_BUTTON:
                Height += 1;
                break;
            case XTBL_TUI_DIALOG_ACTIVE_INPUT:
            case XTBL_TUI_DIALOG_INACTIVE_INPUT:
            case XTBL_TUI_DIALOG_PROGRESS_BAR:
                Height += 2;
                break;
        }

        /* Update component attributes mask */
        Attributes &= ~Mask;
        Mask <<= 1;
    }

    /* Check if input field is active */
    if(Handle->Attributes & (XTBL_TUI_DIALOG_ACTIVE_INPUT | XTBL_TUI_DIALOG_INACTIVE_INPUT))
    {
        /* Set maximum dialog window width to fit input field */
        Width = XTBL_TUI_MAX_DIALOG_WIDTH;
    }

    /* Get message length and count dialog window dimensions */
    MessageLength = RtlWideStringLength(Message, 0);
    for(Index = 0; Index < MessageLength; Index++)
    {
        /* Check if this is multiline message */
        if(Message[Index] == L'\n' || Index == MessageLength - 1)
        {
            /* Check if this line exceeds current dialog window width */
            if(LineLength > Width)
            {
                /* Update dialog window width */
                Width = LineLength;
            }

            /* Increase dialog window height to fit next line */
            Height++;
            LineLength = 0;
        }
        else
        {
            /* Increase dialog window width to fit next character */
            LineLength++;
        }
    }

    /* Add more space to dialog window to fit side borders */
    Width += 4;

    /* Get console resolution */
    BlQueryConsoleMode(&Handle->ResX, &Handle->ResY);

    /* Make sure dialog window fits in the buffer */
    if(Width > XTBL_TUI_MAX_DIALOG_WIDTH)
    {
        /* Set maximum dialog window width */
        Width = XTBL_TUI_MAX_DIALOG_WIDTH;
    }

    /* Make sure dialog window fits on the screen (X axis) and it is not too small for input field */
    if(Width > (Handle->ResX - 2))
    {
        /* Set maximum dialog window width */
        Width = Handle->ResX - 2;
    }

    /* Make sure dialog window fits on the screen (Y axis)*/
    if(Height > (Handle->ResY - 2))
    {
        /* Set maximum dialog window height */
        Height = Handle->ResY - 2;
    }

    /* Set dialog window final dimensions */
    Handle->PosX = (Handle->ResX - Width) / 2;
    Handle->PosY = (Handle->ResY - Height) / 2;
    Handle->Width = Width;
    Handle->Height = Height;
}

/**
 * Draws a text UI-based boot menu.
 *
 * @param Handle
 *        Supplies a pointer to the boot menu handle.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDrawBootMenu(OUT PXTBL_DIALOG_HANDLE Handle)
{
    /* Query console screen resolution */
    BlQueryConsoleMode(&Handle->ResX, &Handle->ResY);

    /* Set boot menu parameters */
    Handle->Attributes = 0;
    Handle->DialogColor = EFI_TEXT_BGCOLOR_BLACK;
    Handle->TextColor = EFI_TEXT_FGCOLOR_LIGHTGRAY;
    Handle->PosX = 3;
    Handle->PosY = 3;
    Handle->Width = Handle->ResX - 6;
    Handle->Height = Handle->ResY - 10;

    /* Clear screen and disable cursor */
    BlSetConsoleAttributes(Handle->DialogColor | Handle->TextColor);
    BlClearConsoleScreen();
    BlDisableConsoleCursor();

    /* Check if debugging enabled */
    if(DEBUG)
    {
        /* Print debug version of XTLDR banner */
        BlSetCursorPosition((Handle->ResX - 44) / 2, 1);
        BlConsolePrint(L"XTLDR Boot Loader v%d.%d (%s-%s)\n",
                       XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR, XTOS_VERSION_DATE, XTOS_VERSION_HASH);
    }
    else
    {
        /* Print standard XTLDR banner */
        BlSetCursorPosition((Handle->ResX - 22) / 2, 1);
        BlConsolePrint(L"XTLDR Boot Loader v%d.%d\n", XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR);
    }

    /* Draw empty dialog box for boot menu */
    BlpDrawDialogBox(Handle, NULL, NULL);

    /* Print help message below the boot menu */
    BlSetCursorPosition(0, Handle->PosY + Handle->Height);
    BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    BlConsolePrint(L"    Use cursors to change the selection. Press ENTER key to boot the chosen\n"
                    "    Operating System, 'e' to edit it before booting or 's' for XTLDR shell.\n"
                    "    Additional help available after pressing F1 key.");
}

/**
 * Draws boot menu entry at the specified position.
 *
 * @param Handle
 *        Supplies a pointer to the boot menu handle.
 *
 * @param MenuEntry
 *        Supplies a pointer to the buffer containing a menu entry name.
 *
 * @param Position
 *        Specifies entry position on the list in the boot menu.
 *
 * @param Highlighted
 *        Specifies whether this entry should be highlighted or not.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDrawBootMenuEntry(IN PXTBL_DIALOG_HANDLE Handle,
                     IN PWCHAR MenuEntry,
                     IN UINT Position,
                     IN BOOLEAN Highlighted)
{
    UINT Index;

    /* Check whether this entry should be highlighted */
    if(Highlighted)
    {
        /* Highlight this entry */
        BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_LIGHTGRAY | EFI_TEXT_FGCOLOR_BLACK);
    }
    else
    {
        /* Set default colors */
        BlSetConsoleAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    }

    /* Move cursor to the right position */
    BlSetCursorPosition(5, 4 + Position);

    /* Clear menu entry */
    for(Index = 0; Index < Handle->Width - 4; Index++)
    {
        BlConsolePrint(L" ");
    }

    /* Print menu entry */
    BlSetCursorPosition(5, 4 + Position);
    BlConsolePrint(L"%S\n", MenuEntry);
}

/**
 * Draws dialog box with caption and message.
 *
 * @param Handle
 *        Supplies a pointer to the dialog box handle.
 *
 * @param Caption
 *        Specifies a caption string put on the dialog box.
 *
 * @param Message
 *        Specifies a message string put on the dialog box.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDrawDialogBox(IN OUT PXTBL_DIALOG_HANDLE Handle,
                 IN PWCHAR Caption,
                 IN PWCHAR Message)
{
    WCHAR BoxLine[XTBL_TUI_MAX_DIALOG_WIDTH];
    SIZE_T CaptionLength;
    UINT_PTR PosX, PosY;

    /* Set dialog colors */
    if(Handle->Attributes & XTBL_TUI_DIALOG_ERROR_BOX)
    {
        /* Error dialog with red background and brown button */
        Handle->DialogColor = EFI_TEXT_BGCOLOR_RED;
        Handle->TextColor = EFI_TEXT_FGCOLOR_WHITE;
    }
    else if(Handle->Attributes & XTBL_TUI_DIALOG_GENERIC_BOX)
    {
        /* Generic dialog with blue background and cyan button */
        Handle->DialogColor = EFI_TEXT_BGCOLOR_BLUE;
        Handle->TextColor = EFI_TEXT_FGCOLOR_WHITE;
    }

    /* Set dialog box colors */
    BlSetConsoleAttributes(Handle->DialogColor | 0x0F);

    /* Iterate through dialog box lines */
    for(PosY = Handle->PosY; PosY < Handle->PosY + Handle->Height; PosY++)
    {
        /* Set cursor position in the appropriate place */
        BlSetCursorPosition(Handle->PosX, PosY);

        /* Draw dialog box */
        if(PosY == Handle->PosY)
        {
            /* Draw top line of the dialog box, starting from the left corner */
            BoxLine[0] = EFI_TEXT_BOX_DOWN_RIGHT;

            /* Check if there is a caption for this dialog */
            if(Caption != NULL)
            {
                /* Get caption length */
                CaptionLength = RtlWideStringLength(Caption, 0);

                /* Start caption area with vertical line */
                BoxLine[1] = EFI_TEXT_BOX_VERTICAL_LEFT;

                /* Fill caption area with spaces */
                for(PosX = 2; PosX < CaptionLength + 4; PosX++)
                {
                    BoxLine[PosX] = ' ';
                }

                /* End caption area with vertical line */
                BoxLine[CaptionLength + 4] = EFI_TEXT_BOX_VERTICAL_RIGHT;
            }
            else
            {
                /* No caption, -4 because of left and right vertical lines and corresponding spaces */
                CaptionLength = -4;
            }

            /* Draw bottom line */
            for(PosX = CaptionLength + 5; PosX < Handle->Width - 1; PosX++)
            {
                BoxLine[PosX] = EFI_TEXT_BOX_HORIZONTAL;
            }

            /* End with top right corner */
            BoxLine[Handle->Width - 1] = EFI_TEXT_BOX_DOWN_LEFT;
        }
        else if(PosY == Handle->PosY + Handle->Height - 1)
        {
            /* Draw bottom line of the dialog box, starting from the left corner */
            BoxLine[0] = EFI_TEXT_BOX_UP_LEFT;

            /* Fill bottom with horizontal line */
            for(PosX = 1; PosX < Handle->Width - 1; PosX++)
            {
                BoxLine[PosX] = EFI_TEXT_BOX_HORIZONTAL;
            }

            /* End with bottom right corner */
            BoxLine[Handle->Width - 1] = EFI_TEXT_BOX_UP_RIGHT;
        }
        else
        {
            /* Draw the middle of the dialog box */
            BoxLine[0] = EFI_TEXT_BOX_VERTICAL;

            /* Fill dialog box with spaces */
            for(PosX = 1; PosX < Handle->Width - 1; PosX++)
            {
                BoxLine[PosX] = ' ';
            }

            /* End with vertical line */
            BoxLine[Handle->Width - 1] = EFI_TEXT_BOX_VERTICAL;
        }

        /* Add null terminator to the end of the line */
        BoxLine[Handle->Width] = 0;

        /* Write the line to the console */
        BlConsoleWrite(BoxLine);
    }

    /* Make sure there is a caption to print */
    if(Caption != NULL)
    {
    /* Write dialog box caption */
        BlSetCursorPosition(Handle->PosX + 3, Handle->PosY);
        BlConsolePrint(L"%S", Caption);
    }

    /* Make sure there is a message to print */
    if(Message != NULL)
    {
        /* Write a message on the dialog box */
        BlpDrawDialogMessage(Handle, Message);
    }
}

/**
 * Draws an active or inactive button in the dialog box, depending on the attributes.
 *
 * @param Handle
 *        Supplies a pointer to the dialog box handle.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDrawDialogButton(IN PXTBL_DIALOG_HANDLE Handle)
{
    ULONG ButtonColor, TextColor;

    /* Set dialog button colors */
    if(Handle->Attributes & XTBL_TUI_DIALOG_ACTIVE_BUTTON)
    {
        /* This is an active button */
        if(Handle->Attributes & XTBL_TUI_DIALOG_ERROR_BOX)
        {
            /* This is an error dialog box */
            ButtonColor = EFI_TEXT_BGCOLOR_BROWN;
            TextColor = EFI_TEXT_FGCOLOR_WHITE;
        }
        else
        {
            /* This is a generic dialog box */
            ButtonColor = EFI_TEXT_BGCOLOR_CYAN;
            TextColor = EFI_TEXT_FGCOLOR_WHITE;
        }
    }
    else
    {
        /* This is an inactive button */
        ButtonColor = EFI_TEXT_BGCOLOR_LIGHTGRAY;
        TextColor = EFI_TEXT_FGCOLOR_BLACK;
    }

    /* Disable cursor and draw dialog button */
    BlDisableConsoleCursor();
    BlSetConsoleAttributes(ButtonColor | TextColor);
    BlSetCursorPosition(Handle->ResX / 2 - 4, Handle->PosY + Handle->Height - 2);
    BlConsolePrint(L"[  OK  ]");
}

/**
 * Draws an active or inactive input field in the dialog box, depending on the attributes.
 *
 * @param Handle
 *        Supplies a pointer to the dialog box handle.
 *
 * @param InputFieldText
 *        Supplies a pointer to the wide char string that will be displayed in the input field.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDrawDialogInputField(IN PXTBL_DIALOG_HANDLE Handle,
                        IN PWCHAR InputFieldText)
{
    WCHAR InputField[XTBL_TUI_MAX_DIALOG_WIDTH];
    ULONG InputColor, TextColor;
    UINT_PTR Index, Position;
    SIZE_T Length;

    /* Set dialog button colors */
    if(Handle->Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
    {
        /* This is an active input field */
        if(Handle->Attributes & XTBL_TUI_DIALOG_ERROR_BOX)
        {
            /* This is an error dialog box */
            InputColor = EFI_TEXT_BGCOLOR_BROWN;
            TextColor = EFI_TEXT_FGCOLOR_WHITE;
        }
        else
        {
            /* This is a generic dialog box */
            InputColor = EFI_TEXT_BGCOLOR_CYAN;
            TextColor = EFI_TEXT_FGCOLOR_WHITE;
        }
    }
    else
    {
        /* This is an inactive input field */
        InputColor = EFI_TEXT_BGCOLOR_LIGHTGRAY;
        TextColor = EFI_TEXT_FGCOLOR_BLACK;
    }

    /* Set progress bar color and position */
    BlSetConsoleAttributes(InputColor | TextColor);
    Position = (Handle->Attributes & (XTBL_TUI_DIALOG_ACTIVE_BUTTON | XTBL_TUI_DIALOG_INACTIVE_BUTTON)) ? 4 : 3;
    BlSetCursorPosition(Handle->PosX + 4, Handle->PosY + Handle->Height - Position);

    /* Draw input field */
    for(Index = 0; Index < Handle->Width - 8; Index++)
    {
        /* Fill input field with spaces */
        InputField[Index] = L' ';
    }

    /* Disable cursor and write input field to console */
    BlDisableConsoleCursor();
    BlConsoleWrite(InputField);

    /* Check input field text length */
    Length = RtlWideStringLength(InputFieldText, 0);
    if(Length > (Handle->Width - 9))
    {
        /* Text longer than input field width, display only part of it */
        Length = Handle->Width - 9;
    }

    /* Copy a part of input field text to input field */
    for(Index = 0; Index < Length; Index++)
    {
        /* Write input field text */
        InputField[Index] = InputFieldText[Index];
    }

    /* Add null terminator to the end of the line */
    InputField[Handle->Width] = 0;

    /* Write input field text */
    BlSetCursorPosition(Handle->PosX + 4, Handle->PosY + Handle->Height - Position);
    BlConsoleWrite(InputField);

    /* Check if this is an active input field */
    if(Handle->Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
    {
        /* Enable cursor for active input field */
        BlEnableConsoleCursor();
    }
}

/**
 * Draws a message on the dialog box specified by the handle.
 *
 * @param Handle
 *        Supplies a pointer to the dialog box handle.
 *
 * @param Message
 *        Supplies a message that will be displayed on the dialog box.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDrawDialogMessage(IN PXTBL_DIALOG_HANDLE Handle,
                     IN PWCHAR Message)
{
    PWCHAR Msg, MsgLine, LastMsgLine;
    SIZE_T Index, Length, LineLength;
    EFI_STATUS Status;
    ULONG Line;

    /* Allocate memory for dialog box message */
    Length = RtlWideStringLength(Message, 0);
    Status = BlAllocateMemoryPool(Length * sizeof(WCHAR), (PVOID *)&Msg);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, print debug message and return */
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
        return;
    }

    /* Make a copy of dialog box message */
    RtlCopyMemory(Msg, Message, Length * sizeof(WCHAR));
    Msg[Length] = 0;

    /* Tokenize dialog box message */
    MsgLine = RtlTokenizeWideString(Msg, L"\n", &LastMsgLine);

    /* Iterate through message lines */
    Line = 0;
    while(MsgLine)
    {
        /* Determine line length */
        LineLength = RtlWideStringLength(MsgLine, 0);

        /* Write line in the dialog box */
        BlSetCursorPosition(Handle->PosX + 2, Handle->PosY + 2 + Line);
        BlSetConsoleAttributes(Handle->DialogColor | Handle->TextColor);
        BlConsolePrint(L"%S", MsgLine);

        /* Check if message line is shorter than the dialog box working area */
        if(LineLength < Handle->Width - 4)
        {
            /* Fill the rest of the line with spaces */
            for(Index = LineLength; Index < Handle->Width - 4; Index++)
            {
                BlConsolePrint(L" ");
            }
        }

        /* Get next line */
        MsgLine = RtlTokenizeWideString(NULL, L"\n", &LastMsgLine);
        Line++;
    }
}

/**
 * Draws a progress bar component in the dialog box.
 *
 * @param Handle
 *        Supplies a pointer to the dialog box handle.
 *
 * @param Percentage
 *        Specifies the percentage progress of the progress bar.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpDrawDialogProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                         IN UCHAR Percentage)
{
    UINT_PTR Index, ProgressLength, ProgressBarLength;
    WCHAR ProgressBar[XTBL_TUI_MAX_DIALOG_WIDTH];
    UINT_PTR Position;

    /* Determine progress bar length and calculate progress */
    ProgressBarLength = Handle->Width - 8;
    ProgressLength = (ProgressBarLength * Percentage) / 100;

    /* Set progress bar color and position */
    BlSetConsoleAttributes(EFI_TEXT_FGCOLOR_YELLOW);
    Position = (Handle->Attributes & (XTBL_TUI_DIALOG_ACTIVE_BUTTON | XTBL_TUI_DIALOG_INACTIVE_BUTTON)) ? 4 : 3;
    BlSetCursorPosition(Handle->PosX + 4, Handle->PosY + Handle->Height - Position);

    /* Draw progress bar */
    for(Index = 0; Index < ProgressBarLength; Index++)
    {
        /* Fill progress bar */
        if(Index < ProgressLength)
        {
            /* Fill with full block */
            ProgressBar[Index] = EFI_TEXT_BOX_FULL_BLOCK;
        }
        else
        {
            /* Fill with light block */
            ProgressBar[Index] = EFI_TEXT_BOX_LIGHT_BLOCK;
        }
    }

    /* Terminate progress bar string */
    ProgressBar[Index] = 0;

    /* Disable cursor and write progress bar to console */
    BlDisableConsoleCursor();
    BlConsoleWrite(ProgressBar);
}
