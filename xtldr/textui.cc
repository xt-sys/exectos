/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/textui.cc
 * DESCRIPTION:     Text console User Interface (TUI) support for XT Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.hh>


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
TextUi::DetermineDialogBoxSize(IN OUT PXTBL_DIALOG_HANDLE Handle,
                               IN PCWSTR Message)
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
    MessageLength = RTL::WideString::WideStringLength(Message, 0);
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
    Console::QueryMode(&Handle->ResX, &Handle->ResY);

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
 * Displays a simple TUI-based boot menu.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
TextUi::DisplayBootMenu()
{
    XTBL_DIALOG_HANDLE Handle;
    PXTBL_BOOTMENU_ITEM MenuEntries = NULLPTR;
    ULONG Index;
    ULONG HighligtedEntryId, OldHighligtedEntryId, NumberOfEntries, TopVisibleEntry, VisibleEntries;
    BOOLEAN RedrawBootMenu, RedrawEntries;
    UINT_PTR EventIndex;
    EFI_EVENT Events[2];
    EFI_INPUT_KEY Key;
    EFI_EVENT TimerEvent;
    EFI_STATUS Status;
    LONG TimeOut;
    PWCHAR TimeOutString;

    /* Draw boot menu */
    DrawBootMenu(&Handle);

    /* Initialize boot menu list */
    TopVisibleEntry = 0;
    Status = XtLoader::InitializeBootMenuList(Handle.Width - 4, &MenuEntries, &NumberOfEntries, &HighligtedEntryId);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to initialize boot menu list, exit into XTLDR shell */
        return;
    }

    /* Calculate how many entries can be visible in the menu box */
    VisibleEntries = Handle.Height - 2;

    /* Adjust the view if the default entry is not initially visible */
    if(HighligtedEntryId >= VisibleEntries)
    {
        /* Scroll the view to make the highlighted entry the last visible one */
        TopVisibleEntry = HighligtedEntryId - VisibleEntries + 1;
    }

    /* Get timeout from the configuration */
    Configuration::GetValue(L"TIMEOUT", &TimeOutString);
    TimeOut = -1;

    /* Check if timeout is specified */
    if(TimeOutString != NULLPTR)
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

    /* Set redraw flags to not redraw the menu itself, but fill it with entries */
    RedrawBootMenu = FALSE;
    RedrawEntries = TRUE;

    /* Infinite boot menu loop */
    while(TRUE)
    {
        /* Redraw boot menu frame if requested */
        if(RedrawBootMenu)
        {
            DrawBootMenu(&Handle);
            RedrawBootMenu = FALSE;
            RedrawEntries = TRUE;
        }

        /* Sanity check to ensure we do not display more entries than possible */
        if(VisibleEntries > NumberOfEntries)
        {
            VisibleEntries = NumberOfEntries;
        }

        /* Check if there is anything to show in the boot menu */
        if(NumberOfEntries > 0)
        {
            /* Check if we need to redraw boot menu entries */
            if(RedrawEntries)
            {
                /* Iterate through all menu entries */
                for(Index = 0; Index < VisibleEntries; Index++)
                {
                    /* Draw menu entry */
                    DrawBootMenuEntry(&Handle, MenuEntries[TopVisibleEntry + Index].EntryName,
                                      Index, (BOOLEAN)((TopVisibleEntry + Index) == HighligtedEntryId));
                }

                /* Clear redraw entries flag */
                RedrawEntries = FALSE;
            }
        }
        else
        {
            /* No menu entries found, show error message */
            DisplayErrorDialog(L"XTLDR", L"No boot menu entries found in the configuration. Falling back to shell.");

            /* Exit into XTLDR shell */
            return;
        }

        /* Create a timer event for controlling the timeout of the boot menu */
        Status = EfiSystemTable->BootServices->CreateEvent(EFI_EVENT_TIMER, EFI_TPL_CALLBACK, NULLPTR, NULLPTR, &TimerEvent);
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

        /* Flush keyboard buffer out of any keystrokes */
        EfiSystemTable->ConIn->Reset(EfiSystemTable->ConIn, FALSE);

        /* Store old highlighted entry */
        OldHighligtedEntryId = HighligtedEntryId;

        /* Infinite boot menu event loop */
        while(TRUE)
        {
            /* Wait for EFI event */
            EfiUtils::WaitForEfiEvent(2, Events, &EventIndex);

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
                    Console::ClearLine(Handle.PosY + Handle.Height + 4);
                }

                /* Read key stroke */
                Console::ReadKeyStroke(&Key);

                if(Key.ScanCode == 0x03 || Key.UnicodeChar == 0x0D)
                {
                    /* ENTER or RightArrow key pressed, boot the highlighted OS */
                    Console::SetAttributes(Handle.DialogColor | Handle.TextColor);
                    Console::ClearLine(Handle.PosY + Handle.Height + 4);
                    Console::SetCursorPosition(4, Handle.PosY + Handle.Height + 4);
                    Console::Print(L"Booting '%S' now...\n", MenuEntries[HighligtedEntryId].FullName);

                    /* Boot the highlighted (chosen) OS */
                    Status = Protocol::InvokeBootProtocol(MenuEntries[HighligtedEntryId].ShortName,
                                                          MenuEntries[HighligtedEntryId].Options);
                    if(Status != STATUS_SUCCESS)
                    {
                        /* Failed to boot OS */
                        Debug::Print(L"ERROR: Failed to boot '%S' (Status Code: 0x%zX)\n",
                                     MenuEntries[HighligtedEntryId].FullName, Status);
                        DisplayErrorDialog(L"XTLDR", L"Failed to startup the selected Operating System.");
                        RedrawBootMenu = TRUE;
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
                        OldHighligtedEntryId = HighligtedEntryId;
                        HighligtedEntryId--;

                        /* Check if we need to scroll the view */
                        if(HighligtedEntryId < TopVisibleEntry)
                        {
                            /* Scroll the view */
                            TopVisibleEntry = HighligtedEntryId;
                            RedrawEntries = TRUE;
                            break;
                        }

                        /* Redraw new highlighted entry and the old one */
                        DrawBootMenuEntry(&Handle, MenuEntries[OldHighligtedEntryId].EntryName,
                                          OldHighligtedEntryId - TopVisibleEntry, (BOOLEAN)FALSE);
                        DrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId].EntryName,
                                          HighligtedEntryId - TopVisibleEntry, (BOOLEAN)TRUE);
                    }
                }
                else if(Key.ScanCode == 0x02)
                {
                    /* DownArrow key pressed, go to next entry if possible */
                    if(HighligtedEntryId < NumberOfEntries - 1)
                    {
                        /* Highlight next entry */
                        OldHighligtedEntryId = HighligtedEntryId;
                        HighligtedEntryId++;

                        /* Check if we need to scroll the view */
                        if(HighligtedEntryId >= TopVisibleEntry + VisibleEntries)
                        {
                            /* Scroll the view */
                            TopVisibleEntry = HighligtedEntryId - VisibleEntries + 1;
                            RedrawEntries = TRUE;
                            break;
                        }

                        /* Redraw new highlighted entry and the old one */
                        DrawBootMenuEntry(&Handle, MenuEntries[OldHighligtedEntryId].EntryName,
                                          OldHighligtedEntryId - TopVisibleEntry, (BOOLEAN)FALSE);
                        DrawBootMenuEntry(&Handle, MenuEntries[HighligtedEntryId].EntryName,
                                          HighligtedEntryId - TopVisibleEntry, (BOOLEAN)TRUE);
                    }
                }
                else if(Key.ScanCode == 0x09)
                {
                    /* PageUp key pressed, go to top entry */
                    if(HighligtedEntryId != 0)
                    {
                        /* Highlight first entry */
                        HighligtedEntryId = 0;
                        TopVisibleEntry = 0;
                        RedrawEntries = TRUE;
                        break;
                    }
                }
                else if(Key.ScanCode == 0x0A)
                {
                    /* PageDown key pressed, go to bottom entry */
                    if(HighligtedEntryId != NumberOfEntries - 1)
                    {
                        /* Highlight last entry */
                        HighligtedEntryId = NumberOfEntries - 1;
                        TopVisibleEntry = (NumberOfEntries > VisibleEntries) ? (NumberOfEntries - VisibleEntries) : 0;
                        RedrawEntries = TRUE;
                        break;
                    }
                }
                else if(Key.ScanCode == 0x0B)
                {
                    /* F1 key pressed, show help */
                    DisplayInfoDialog(L"XTLDR", L"XTLDR, the XTOS Boot Loader for UEFI and EFI-based machines.\n"
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
                    RedrawBootMenu = TRUE;
                    break;
                }
                else if(Key.ScanCode == 0x14)
                {
                    /* F10 key pressed, reboot into UEFI setup interface */
                    EfiUtils::EnterFirmwareSetup();
                    DisplayErrorDialog(L"XTLDR", L"Reboot into firmware setup interface not supported!");
                    RedrawBootMenu = TRUE;

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.ScanCode == 0x15)
                {
                    /* F11 key pressed, reboot the machine */
                    EfiUtils::RebootSystem();
                    DisplayErrorDialog(L"XTLDR", L"Failed to reboot the machine!");
                    RedrawBootMenu = TRUE;

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.ScanCode == 0x16)
                {
                    /* F12 key pressed, shutdown the machine */
                    EfiUtils::ShutdownSystem();
                    DisplayErrorDialog(L"XTLDR", L"Failed to shutdown the machine!");
                    RedrawBootMenu = TRUE;

                    /* Break from boot menu event loop to redraw whole boot menu */
                    break;
                }
                else if(Key.UnicodeChar == 0x65)
                {
                    /* 'e' key pressed, edit the highlighted entry */
                    DisplayEditMenu(&MenuEntries[HighligtedEntryId]);
                    RedrawBootMenu = TRUE;

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
                    Console::SetAttributes(Handle.DialogColor | Handle.TextColor);
                    Console::ClearLine(Handle.PosY + Handle.Height + 4);
                    Console::SetCursorPosition(4, Handle.PosY + Handle.Height + 4);
                    Console::Print(L"The highlighted position will be booted automatically in %ld seconds.", TimeOut);
                    TimeOut--;
                }
                else if(TimeOut == 0)
                {
                    /* Time out expired, update a message */
                    Console::SetAttributes(Handle.DialogColor | Handle.TextColor);
                    Console::ClearLine(Handle.PosY + Handle.Height + 4);
                    Console::SetCursorPosition(4, Handle.PosY + Handle.Height + 4);
                    Console::Print(L"Booting '%S' now...\n", MenuEntries[HighligtedEntryId].FullName);

                    /* Disable the timer just in case booting OS fails */
                    TimeOut = -1;

                    /* Boot the highlighted (default) OS */
                    Status = Protocol::InvokeBootProtocol(MenuEntries[HighligtedEntryId].ShortName,
                                                          MenuEntries[HighligtedEntryId].Options);
                    if(Status != STATUS_SUCCESS)
                    {
                        /* Failed to boot OS */
                        Debug::Print(L"ERROR: Failed to boot '%S' (Status Code: 0x%zX)\n",
                                     MenuEntries[HighligtedEntryId].FullName, Status);
                        DisplayErrorDialog(L"XTLDR", L"Failed to startup the selected Operating System.");
                        RedrawBootMenu = TRUE;
                    }
                    break;
                }
            }
        }
    }
}

/**
 * Displays a simple TUI-based edit menu.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
TextUi::DisplayEditMenu(IN PXTBL_BOOTMENU_ITEM MenuEntry)
{
    ULONG HighligtedOptionId, Index, NumberOfOptions, OldHighligtedOptionId, TopVisibleEntry, VisibleEntries;
    XTBL_DIALOG_HANDLE Handle;
    BOOLEAN RedrawEditMenu, RedrawEntries;
    EFI_INPUT_KEY Key;
    UINT_PTR EventIndex;
    PWCHAR NewValue, OriginalValue, Value, ValueToEdit;
    PCWSTR OptionName, *EditableOptions;
    EFI_STATUS Status;

    /* Draw edit menu */
    DrawEditMenu(&Handle);

    /* Get the list of user editable options */
    Configuration::GetEditableOptions(&EditableOptions, &NumberOfOptions);

    /* Calculate how many entries can be visible in the menu box */
    VisibleEntries = Handle.Height - 2;

    /* Assume the first option is highlighted by default */
    HighligtedOptionId = 0;
    OldHighligtedOptionId = 0;
    TopVisibleEntry = 0;

    /* Set redraw flags to not redraw the menu itself, but fill it with entries */
    RedrawEditMenu = FALSE;
    RedrawEntries = TRUE;

    /* Infinite edit menu loop */
    while(TRUE)
    {
        /* Redraw edit menu frame if requested */
        if(RedrawEditMenu)
        {
            DrawEditMenu(&Handle);
            RedrawEditMenu = FALSE;
            RedrawEntries = TRUE;
        }

        /* Sanity check to ensure we do not display more entries than possible */
        if(VisibleEntries > NumberOfOptions)
        {
            VisibleEntries = NumberOfOptions;
        }

        /* Check if we need to redraw boot menu entries */
        if(RedrawEntries)
        {
            /* Iterate through all menu entries */
            for(Index = 0; Index < VisibleEntries; Index++)
            {
                /* Draw menu entry */
                Configuration::GetBootOptionValue(MenuEntry->Options, EditableOptions[TopVisibleEntry + Index], &Value);
                DrawEditMenuEntry(&Handle, EditableOptions[TopVisibleEntry + Index], Value, Index,
                                  (BOOLEAN)((TopVisibleEntry + Index) == HighligtedOptionId));

                /* Free allocated value string if needed */
                if(Value != NULLPTR)
                {
                    Memory::FreePool(Value);
                }
            }

            /* Clear redraw entries flag */
            RedrawEntries = FALSE;
        }

        /* Wait for EFI event and read key stroke */
        EfiUtils::WaitForEfiEvent(1, &EfiSystemTable->ConIn->WaitForKey, &EventIndex);
        Console::ReadKeyStroke(&Key);

        /* Check key press scan code */
        if(Key.UnicodeChar == 0x0D || Key.UnicodeChar == 0x65)
        {
            /* ENTER or 'e' key pressed, edit the highlighted option */
            OptionName = EditableOptions[HighligtedOptionId];
            Configuration::GetBootOptionValue(MenuEntry->Options, OptionName, &OriginalValue);

            /* If the original value is NULLPTR, use an empty string for editing */
            if(OriginalValue == NULLPTR)
            {
                ValueToEdit = (PWCHAR)L"";
            }
            else
            {
                ValueToEdit = OriginalValue;
            }

            /* Display input dialog to edit the option value */
            NewValue = ValueToEdit;
            DisplayInputDialog(OptionName, L"Enter new value:", &NewValue);

            /* Check if the value was changed */
            if(NewValue != ValueToEdit)
            {
                /* Update the boot option with the new value and free the old value */
                Configuration::SetBootOptionValue(MenuEntry->Options, OptionName, NewValue);
                Memory::FreePool(NewValue);
            }

            /* Free the original value if it was allocated */
            if(OriginalValue != NULLPTR)
            {
                Memory::FreePool(OriginalValue);
            }

            /* Mark the edit menu for redraw */
            RedrawEditMenu = TRUE;
        }
        else if(Key.ScanCode == 0x01)
        {
            /* UpArrow key pressed, go to previous entry if possible */
            if(HighligtedOptionId > 0)
            {
                /* Highlight previous entry */
                OldHighligtedOptionId = HighligtedOptionId;
                HighligtedOptionId--;

                /* Check if we need to scroll the view */
                if(HighligtedOptionId < TopVisibleEntry)
                {
                    /* Scroll the view */
                    TopVisibleEntry = HighligtedOptionId;
                    RedrawEntries = TRUE;
                    continue;
                }

                /* Redraw old highlighted entry */
                Configuration::GetBootOptionValue(MenuEntry->Options, EditableOptions[OldHighligtedOptionId], &Value);
                DrawEditMenuEntry(&Handle, EditableOptions[OldHighligtedOptionId], Value, OldHighligtedOptionId - TopVisibleEntry, (BOOLEAN)FALSE);

                /* Free allocated value string if needed */
                if(Value != NULLPTR)
                {
                    Memory::FreePool(Value);
                }

                /* Redraw new highlighted entry */
                Configuration::GetBootOptionValue(MenuEntry->Options, EditableOptions[HighligtedOptionId], &Value);
                DrawEditMenuEntry(&Handle, EditableOptions[HighligtedOptionId], Value, HighligtedOptionId - TopVisibleEntry, (BOOLEAN)TRUE);

                /* Free allocated value string if needed */
                if(Value != NULLPTR)
                {
                    Memory::FreePool(Value);
                }
            }
        }
        else if(Key.ScanCode == 0x02)
        {
            /* DownArrow key pressed, go to next entry if possible */
            if(HighligtedOptionId < NumberOfOptions - 1)
            {
                /* Highlight next entry */
                OldHighligtedOptionId = HighligtedOptionId;
                HighligtedOptionId++;

                /* Check if we need to scroll the view */
                if(HighligtedOptionId >= TopVisibleEntry + VisibleEntries)
                {
                    /* Scroll the view */
                    TopVisibleEntry = HighligtedOptionId - VisibleEntries + 1;
                    RedrawEntries = TRUE;
                    continue;
                }

                /* Redraw old highlighted entry */
                Configuration::GetBootOptionValue(MenuEntry->Options, EditableOptions[OldHighligtedOptionId], &Value);
                DrawEditMenuEntry(&Handle, EditableOptions[OldHighligtedOptionId], Value, OldHighligtedOptionId - TopVisibleEntry, (BOOLEAN)FALSE);

                /* Free allocated value string if needed */
                if(Value != NULLPTR)
                {
                    Memory::FreePool(Value);
                }

                /* Redraw new highlighted entry */
                Configuration::GetBootOptionValue(MenuEntry->Options, EditableOptions[HighligtedOptionId], &Value);
                DrawEditMenuEntry(&Handle, EditableOptions[HighligtedOptionId], Value, HighligtedOptionId - TopVisibleEntry, (BOOLEAN)TRUE);

                /* Free allocated value string if needed */
                if(Value != NULLPTR)
                {
                    Memory::FreePool(Value);
                }
            }
        }
        else if(Key.ScanCode == 0x09)
        {
            /* PageUp key pressed, go to top entry */
            if(HighligtedOptionId != 0)
            {
                /* Highlight first entry */
                HighligtedOptionId = 0;
                TopVisibleEntry = 0;
                RedrawEntries = TRUE;
            }
        }
        else if(Key.ScanCode == 0x0A)
        {
            /* PageDown key pressed, go to bottom entry */
            if(HighligtedOptionId != NumberOfOptions - 1)
            {
                /* Highlight last entry */
                HighligtedOptionId = NumberOfOptions - 1;
                TopVisibleEntry = (NumberOfOptions > VisibleEntries) ? (NumberOfOptions - VisibleEntries) : 0;
                RedrawEntries = TRUE;
            }
        }
        else if(Key.UnicodeChar == 0x02)
        {
            /* CTRL-B key pressed, boot the OS */
            Console::SetAttributes(Handle.DialogColor | Handle.TextColor);
            Console::ClearLine(Handle.PosY + Handle.Height + 4);
            Console::SetCursorPosition(4, Handle.PosY + Handle.Height + 4);
            Console::Print(L"Booting '%S' now...\n", MenuEntry->FullName);

            /* Boot the OS */
            Status = Protocol::InvokeBootProtocol(MenuEntry->ShortName, MenuEntry->Options);
            if(Status != STATUS_SUCCESS)
            {
                /* Failed to boot OS */
                Debug::Print(L"ERROR: Failed to boot '%S' (Status Code: 0x%zX)\n", MenuEntry->FullName, Status);
                DisplayErrorDialog(L"XTLDR", L"Failed to startup the selected Operating System.");
                RedrawEditMenu = TRUE;
            }

            /* Return to the edit menu */
            continue;
        }
        else if(Key.ScanCode == 0x17)
        {
            /* ESC key pressed, exit edit menu */
            break;
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
TextUi::DisplayErrorDialog(IN PCWSTR Caption,
                           IN PCWSTR Message)
{
    XTBL_DIALOG_HANDLE Handle;
    EFI_INPUT_KEY Key;
    UINT_PTR Index;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_ERROR_BOX | XTBL_TUI_DIALOG_ACTIVE_BUTTON;

    /* Determine dialog window size and position */
    DetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    Console::DisableCursor();
    DrawDialogBox(&Handle, Caption, Message);

    /* Draw active button */
    DrawButton(&Handle);

    /* Initialize key stroke */
    Key.ScanCode = 0;
    Key.UnicodeChar = 0;

    /* Wait until ENTER or ESC key is pressed */
    while(Key.ScanCode != 0x17 && Key.UnicodeChar != 0x0D)
    {
        /* Wait for key press and read key stroke */
        EfiUtils::WaitForEfiEvent(1, &EfiSystemTable->ConIn->WaitForKey, &Index);
        Console::ReadKeyStroke(&Key);
        Console::ResetInputBuffer();
    }

    /* Clear screen to remove dialog box */
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    Console::ClearScreen();
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
TextUi::DisplayInfoDialog(IN PCWSTR Caption,
                          IN PCWSTR Message)
{
    XTBL_DIALOG_HANDLE Handle;
    EFI_INPUT_KEY Key;
    UINT_PTR Index;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_GENERIC_BOX | XTBL_TUI_DIALOG_ACTIVE_BUTTON;

    /* Determine dialog window size and position */
    DetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    Console::DisableCursor();
    DrawDialogBox(&Handle, Caption, Message);

    /* Draw active button */
    DrawButton(&Handle);

    /* Initialize key stroke */
    Key.ScanCode = 0;
    Key.UnicodeChar = 0;

    /* Wait until ENTER or ESC key is pressed */
    while(Key.ScanCode != 0x17 && Key.UnicodeChar != 0x0D)
    {
        /* Wait for key press and read key stroke */
        EfiUtils::WaitForEfiEvent(1, &EfiSystemTable->ConIn->WaitForKey, &Index);
        Console::ReadKeyStroke(&Key);
        Console::ResetInputBuffer();
    }

    /* Clear screen to remove dialog box */
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    Console::ClearScreen();
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
TextUi::DisplayInputDialog(IN PCWSTR Caption,
                           IN PCWSTR Message,
                           IN OUT PWCHAR *InputFieldText)
{
    SIZE_T InputFieldLength, TextCursorPosition, TextIndex, TextPosition;
    XTBL_DIALOG_HANDLE Handle;
    PWCHAR InputFieldBuffer;
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;
    UINT_PTR Index;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_GENERIC_BOX | XTBL_TUI_DIALOG_ACTIVE_INPUT | XTBL_TUI_DIALOG_INACTIVE_BUTTON;

    /* Determine dialog window size and position */
    DetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    Console::DisableCursor();
    DrawDialogBox(&Handle, Caption, Message);

    /* Draw inactive button */
    DrawButton(&Handle);

    /* Draw active input field */
    DrawInputField(&Handle, *InputFieldText);

    /* Initialize key stroke */
    Key.ScanCode = 0;
    Key.UnicodeChar = 0;

    /* Determine input field length */
    InputFieldLength = RTL::WideString::WideStringLength(*InputFieldText, 0);

    /* Allocate a buffer for storing the input field text */
    Status = Memory::AllocatePool(2048 * sizeof(WCHAR), (PVOID *)&InputFieldBuffer);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, print error message and return */
        Debug::Print(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
        DisplayErrorDialog(L"XTLDR", L"Failed to allocate memory for input field buffer.");
        return;
    }

    /* Copy input text into edit buffer */
    RTL::Memory::CopyMemory(InputFieldBuffer, *InputFieldText, InputFieldLength * sizeof(WCHAR));
    InputFieldBuffer[InputFieldLength] = L'\0';

    /* Start at first character */
    TextPosition = 0;
    Console::SetCursorPosition(Handle.PosX + 4 + TextPosition, Handle.PosY + Handle.Height - 4);

    /* Wait until ENTER or ESC key is pressed */
    while(TRUE)
    {
        /* Wait for key press and read key stroke */
        EfiUtils::WaitForEfiEvent(1, &EfiSystemTable->ConIn->WaitForKey, &Index);
        Console::ReadKeyStroke(&Key);

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
                /* Check if buffer is not empty */
                if(InputFieldLength > 0 && TextPosition < InputFieldLength)
                {
                    /* Delete character */
                    RTL::Memory::MoveMemory(InputFieldBuffer + TextPosition, InputFieldBuffer + TextPosition + 1,
                                  (InputFieldLength - TextPosition) * sizeof(WCHAR));

                    /* Decrement length and NULL terminate string */
                    InputFieldLength--;
                    InputFieldBuffer[InputFieldLength] = L'\0';
                }
            }
        }
        else if(Key.UnicodeChar == 0x08)
        {
            /* BACKSPACE key pressed, delete character */
            if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
            {
                /* Check if buffer is not empty */
                if(InputFieldLength > 0 && TextPosition > 0 && TextPosition <= InputFieldLength)
                {
                    /* Move memory to overwrite the character to the left of the cursor */
                    RTL::Memory::MoveMemory(InputFieldBuffer + TextPosition - 1, InputFieldBuffer + TextPosition,
                                  (InputFieldLength - TextPosition + 1) * sizeof(WCHAR));

                    /* Decrement length, position and NULL terminate string */
                    TextPosition--;
                    InputFieldLength--;
                    InputFieldBuffer[InputFieldLength] = L'\0';
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
                /* Check if buffer is full */
                if(InputFieldLength < 2047)
                {
                    /* Insert character at current position */
                    RTL::Memory::MoveMemory(InputFieldBuffer + TextPosition + 1, InputFieldBuffer + TextPosition,
                                  (InputFieldLength - TextPosition) * sizeof(WCHAR));
                    InputFieldBuffer[TextPosition] = Key.UnicodeChar;

                    /* Increment length, position and NULL terminate string */
                    TextPosition++;
                    InputFieldLength++;
                    InputFieldBuffer[InputFieldLength] = L'\0';
                }
            }
        }

        /* Calculate text index and cursor position */
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
        DrawButton(&Handle);
        DrawInputField(&Handle, &InputFieldBuffer[TextIndex]);

        /* Set cursor position if input field is active */
        if(Handle.Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
        {
            Console::SetCursorPosition(Handle.PosX + 4 + TextCursorPosition, Handle.PosY + Handle.Height - 4);
        }
    }

    /* Clear screen to remove dialog box */
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    Console::ClearScreen();
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
TextUi::DisplayProgressDialog(IN PCWSTR Caption,
                              IN PCWSTR Message,
                              IN UCHAR Percentage)
{
    XTBL_DIALOG_HANDLE Handle;

    /* Set dialog window attributes */
    Handle.Attributes = XTBL_TUI_DIALOG_GENERIC_BOX | XTBL_TUI_DIALOG_PROGRESS_BAR;

    /* Determine dialog window size and position */
    DetermineDialogBoxSize(&Handle, Message);

    /* Disable cursor and draw dialog box */
    Console::DisableCursor();
    DrawDialogBox(&Handle, Caption, Message);

    /* Draw active button */
    DrawProgressBar(&Handle, Percentage);

    /* Return dialog handle */
    return Handle;
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
TextUi::DrawBootMenu(OUT PXTBL_DIALOG_HANDLE Handle)
{
    /* Query console screen resolution */
    Console::QueryMode(&Handle->ResX, &Handle->ResY);

    /* Set boot menu parameters */
    Handle->Attributes = 0;
    Handle->DialogColor = EFI_TEXT_BGCOLOR_BLACK;
    Handle->TextColor = EFI_TEXT_FGCOLOR_LIGHTGRAY;
    Handle->PosX = 3;
    Handle->PosY = 3;
    Handle->Width = Handle->ResX - 6;
    Handle->Height = Handle->ResY - 10;

    /* Clear screen and disable cursor */
    Console::SetAttributes(Handle->DialogColor | Handle->TextColor);
    Console::ClearScreen();
    Console::DisableCursor();

    /* Check if debugging enabled */
    if(DEBUG)
    {
        /* Print debug version of XTLDR banner */
        Console::SetCursorPosition((Handle->ResX - 44) / 2, 1);
        Console::Print(L"XTLDR Boot Loader v%d.%d (%s-%s)\n",
                       XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR, XTOS_VERSION_DATE, XTOS_VERSION_HASH);
    }
    else
    {
        /* Print standard XTLDR banner */
        Console::SetCursorPosition((Handle->ResX - 22) / 2, 1);
        Console::Print(L"XTLDR Boot Loader v%d.%d\n", XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR);
    }

    /* Draw empty dialog box for boot menu */
    DrawDialogBox(Handle, NULLPTR, NULLPTR);

    /* Print help message below the boot menu */
    Console::SetCursorPosition(0, Handle->PosY + Handle->Height);
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    Console::Print(L"    Use cursors to change the selection. Press ENTER key to boot the chosen\n"
                   L"    Operating System, 'e' to edit it before booting or 's' for XTLDR shell.\n"
                   L"    Additional help available after pressing F1 key.");
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
TextUi::DrawBootMenuEntry(IN PXTBL_DIALOG_HANDLE Handle,
                          IN PWCHAR MenuEntry,
                          IN UINT Position,
                          IN BOOLEAN Highlighted)
{
    UINT Index;

    /* Move cursor to the right position */
    Console::SetCursorPosition(5, 4 + Position);

    /* Check whether this entry should be highlighted */
    if(Highlighted)
    {
        /* Highlight this entry */
        Console::SetAttributes(EFI_TEXT_BGCOLOR_LIGHTGRAY | EFI_TEXT_FGCOLOR_BLACK);
    }
    else
    {
        /* Set default colors */
        Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    }

    /* Clear menu entry */
    for(Index = 0; Index < Handle->Width - 4; Index++)
    {
        Console::Print(L" ");
    }

    /* Print menu entry */
    Console::SetCursorPosition(5, 4 + Position);
    Console::Print(L"%S\n", MenuEntry);
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
TextUi::DrawDialogBox(IN OUT PXTBL_DIALOG_HANDLE Handle,
                      IN PCWSTR Caption,
                      IN PCWSTR Message)
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
    Console::SetAttributes(Handle->DialogColor | 0x0F);

    /* Iterate through dialog box lines */
    for(PosY = Handle->PosY; PosY < Handle->PosY + Handle->Height; PosY++)
    {
        /* Set cursor position in the appropriate place */
        Console::SetCursorPosition(Handle->PosX, PosY);

        /* Draw dialog box */
        if(PosY == Handle->PosY)
        {
            /* Draw top line of the dialog box, starting from the left corner */
            BoxLine[0] = EFI_TEXT_BOX_DOWN_RIGHT;

            /* Check if there is a caption for this dialog */
            if(Caption != NULLPTR)
            {
                /* Get caption length */
                CaptionLength = RTL::WideString::WideStringLength(Caption, 0);

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

        /* Add NULL terminator to the end of the line */
        BoxLine[Handle->Width] = 0;

        /* Write the line to the console */
        Console::Write(BoxLine);
    }

    /* Make sure there is a caption to print */
    if(Caption != NULLPTR)
    {
    /* Write dialog box caption */
        Console::SetCursorPosition(Handle->PosX + 3, Handle->PosY);
        Console::Print(L"%S", Caption);
    }

    /* Make sure there is a message to print */
    if(Message != NULLPTR)
    {
        /* Write a message on the dialog box */
        DrawMessage(Handle, Message);
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
TextUi::DrawButton(IN PXTBL_DIALOG_HANDLE Handle)
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
    Console::DisableCursor();
    Console::SetAttributes(ButtonColor | TextColor);
    Console::SetCursorPosition(Handle->ResX / 2 - 4, Handle->PosY + Handle->Height - 2);
    Console::Print(L"[  OK  ]");
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
TextUi::DrawInputField(IN PXTBL_DIALOG_HANDLE Handle,
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
    Console::SetAttributes(InputColor | TextColor);
    Position = (Handle->Attributes & (XTBL_TUI_DIALOG_ACTIVE_BUTTON | XTBL_TUI_DIALOG_INACTIVE_BUTTON)) ? 4 : 3;
    Console::SetCursorPosition(Handle->PosX + 4, Handle->PosY + Handle->Height - Position);

    /* Draw input field */
    for(Index = 0; Index < Handle->Width - 8; Index++)
    {
        /* Fill input field with spaces */
        InputField[Index] = L' ';
    }

    /* Disable cursor and write input field to console */
    Console::DisableCursor();
    Console::Write(InputField);

    /* Check input field text length */
    Length = RTL::WideString::WideStringLength(InputFieldText, 0);
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

    /* Add NULL terminator to the end of the line */
    InputField[Handle->Width] = 0;

    /* Write input field text */
    Console::SetCursorPosition(Handle->PosX + 4, Handle->PosY + Handle->Height - Position);
    Console::Write(InputField);

    /* Check if this is an active input field */
    if(Handle->Attributes & XTBL_TUI_DIALOG_ACTIVE_INPUT)
    {
        /* Enable cursor for active input field */
        Console::EnableCursor();
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
TextUi::DrawMessage(IN PXTBL_DIALOG_HANDLE Handle,
                    IN PCWSTR Message)
{
    PWCHAR Msg, MsgLine, LastMsgLine;
    SIZE_T Index, Length, LineLength;
    EFI_STATUS Status;
    ULONG Line;

    /* Allocate memory for dialog box message */
    Length = RTL::WideString::WideStringLength(Message, 0);
    Status = Memory::AllocatePool(Length * sizeof(WCHAR), (PVOID *)&Msg);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, print debug message and return */
        Debug::Print(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
        return;
    }

    /* Make a copy of dialog box message */
    RTL::Memory::CopyMemory(Msg, Message, Length * sizeof(WCHAR));
    Msg[Length] = 0;

    /* Tokenize dialog box message */
    MsgLine = RTL::WideString::TokenizeWideString(Msg, L"\n", &LastMsgLine);

    /* Iterate through message lines */
    Line = 0;
    while(MsgLine)
    {
        /* Determine line length */
        LineLength = RTL::WideString::WideStringLength(MsgLine, 0);

        /* Write line in the dialog box */
        Console::SetCursorPosition(Handle->PosX + 2, Handle->PosY + 2 + Line);
        Console::SetAttributes(Handle->DialogColor | Handle->TextColor);
        Console::Print(L"%S", MsgLine);

        /* Check if message line is shorter than the dialog box working area */
        if(LineLength < Handle->Width - 4)
        {
            /* Fill the rest of the line with spaces */
            for(Index = LineLength; Index < Handle->Width - 4; Index++)
            {
                Console::Print(L" ");
            }
        }

        /* Get next line */
        MsgLine = RTL::WideString::TokenizeWideString(NULLPTR, L"\n", &LastMsgLine);
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
TextUi::DrawProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                        IN UCHAR Percentage)
{
    UINT_PTR Index, ProgressLength, ProgressBarLength;
    WCHAR ProgressBar[XTBL_TUI_MAX_DIALOG_WIDTH];
    UINT_PTR Position;

    /* Determine progress bar length and calculate progress */
    ProgressBarLength = Handle->Width - 8;
    ProgressLength = (ProgressBarLength * Percentage) / 100;

    /* Set progress bar color and position */
    Console::SetAttributes(EFI_TEXT_FGCOLOR_YELLOW);
    Position = (Handle->Attributes & (XTBL_TUI_DIALOG_ACTIVE_BUTTON | XTBL_TUI_DIALOG_INACTIVE_BUTTON)) ? 4 : 3;
    Console::SetCursorPosition(Handle->PosX + 4, Handle->PosY + Handle->Height - Position);

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
    Console::DisableCursor();
    Console::Write(ProgressBar);
}

/**
 * Draws a text-based boot edition menu.
 *
 * @param Handle
 *        Supplies a pointer to the edition menu handle.
 *
 * @return This function does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
TextUi::DrawEditMenu(OUT PXTBL_DIALOG_HANDLE Handle)
{
    /* Query console screen resolution */
    Console::QueryMode(&Handle->ResX, &Handle->ResY);

    /* Set boot menu parameters */
    Handle->Attributes = 0;
    Handle->DialogColor = EFI_TEXT_BGCOLOR_BLACK;
    Handle->TextColor = EFI_TEXT_FGCOLOR_LIGHTGRAY;
    Handle->PosX = 3;
    Handle->PosY = 3;
    Handle->Width = Handle->ResX - 6;
    Handle->Height = Handle->ResY - 10;

    /* Clear screen and disable cursor */
    Console::SetAttributes(Handle->DialogColor | Handle->TextColor);
    Console::ClearScreen();
    Console::DisableCursor();

    /* Check if debugging enabled */
    if(DEBUG)
    {
        /* Print debug version of XTLDR banner */
        Console::SetCursorPosition((Handle->ResX - 44) / 2, 1);
        Console::Print(L"XTLDR Boot Loader v%d.%d (%s-%s)\n",
                       XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR, XTOS_VERSION_DATE, XTOS_VERSION_HASH);
    }
    else
    {
        /* Print standard XTLDR banner */
        Console::SetCursorPosition((Handle->ResX - 22) / 2, 1);
        Console::Print(L"XTLDR Boot Loader v%d.%d\n", XTLDR_VERSION_MAJOR, XTLDR_VERSION_MINOR);
    }

    /* Draw empty dialog box for boot menu */
    DrawDialogBox(Handle, L"Edit Options", NULLPTR);

    /* Print help message below the edit menu */
    Console::SetCursorPosition(0, Handle->PosY + Handle->Height);
    Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    Console::Print(L"    Use cursors to change the selection. Press ENTER key to edit the chosen\n"
                   L"    option, ESC to return to the main boot menu or CTRL-B to boot.\n");
}

/**
 * Draws edit menu entry at the specified position.
 *
 * @param Handle
 *        Supplies a pointer to the boot menu handle.
 *
 * @param OptionName
 *        Supplies a pointer to the buffer containing a part of the menu entry name (an option name).
 *
 * @param OptionValue
 *        Supplies a pointer to the buffer containing a part of the menu entry name (an option value).
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
EFI_STATUS
TextUi::DrawEditMenuEntry(IN PXTBL_DIALOG_HANDLE Handle,
                          IN PCWSTR OptionName,
                          IN PCWSTR OptionValue,
                          IN UINT Position,
                          IN BOOLEAN Highlighted)
{
    BOOLEAN Allocation;
    PCWSTR DisplayValue, ShortValue;
    UINT Index;
    ULONG OptionNameLength, OptionValueLength, OptionWidth;
    EFI_STATUS Status;

    /* Assume no allocation was made */
    Allocation = FALSE;

    /* Set display value depending on input */
    DisplayValue = (OptionValue != NULLPTR) ? OptionValue : L"";

    /* Determine lengths */
    OptionNameLength = RTL::WideString::WideStringLength(OptionName, 0);
    OptionValueLength = RTL::WideString::WideStringLength(DisplayValue, 0);
    OptionWidth = Handle->Width - 4 - (OptionNameLength + 2);

    /* Check if value needs to be truncated */
    if(OptionValueLength > OptionWidth)
    {
        /* Allocate buffer for new, shortened value */
        Status = Memory::AllocatePool((OptionWidth + 1) * sizeof(WCHAR), (PVOID *)&ShortValue); // This allocates PWCHAR
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure, print debug message and return */
            Debug::Print(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
            return Status;
        }

        /* Copy a desired value length into the allocated buffer and append "..." */
        RTL::Memory::CopyMemory((PWCHAR)ShortValue, DisplayValue, (OptionWidth - 3) * sizeof(WCHAR));
        RTL::Memory::CopyMemory((PWCHAR)ShortValue + OptionWidth - 3, L"...", 3 * sizeof(WCHAR));
        ((PWCHAR)ShortValue)[OptionWidth] = L'\0';

        /* Mark that allocation was made and set new display value */
        Allocation = TRUE;
        DisplayValue = ShortValue;
    }

    /* Move cursor to the right position */
    Console::SetCursorPosition(5, 4 + Position);

    /* Check whether this entry should be highlighted */
    if(Highlighted)
    {
        /* Highlight this entry */
        Console::SetAttributes(EFI_TEXT_BGCOLOR_LIGHTGRAY | EFI_TEXT_FGCOLOR_BLACK);
    }
    else
    {
        /* Set default colors */
        Console::SetAttributes(EFI_TEXT_BGCOLOR_BLACK | EFI_TEXT_FGCOLOR_LIGHTGRAY);
    }

    /* Clear menu entry */
    for(Index = 0; Index < Handle->Width - 4; Index++)
    {
        Console::Print(L" ");
    }

    /* Print menu entry */
    Console::SetCursorPosition(5, 4 + Position);
    Console::Print(L"%S: %S", OptionName, DisplayValue);

    /* Check if allocation was made */
    if(Allocation)
    {
        /* Free allocated memory */
        Memory::FreePool((PVOID)DisplayValue);
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
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
TextUi::UpdateProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                          IN PCWSTR Message,
                          IN UCHAR Percentage)
{
    /* Check if message needs an update */
    if(Message != NULLPTR)
    {
        /* Update a message on the dialog box */
        DrawMessage(Handle, Message);
    }

    /* Update progress bar */
    DrawProgressBar(Handle, Percentage);
}
