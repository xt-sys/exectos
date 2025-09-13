/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/dbgio.cc
 * DESCRIPTION:     Kernel Debugger I/O routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Prints a formatted string using the configured debug output providers.
 *
 * @param Format
 *        Supplies the format string.
 *
 * @param ...
 *        Supplies the variable argument list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KD::DebugIo::DbgPrint(PCWSTR Format, ...)
{
    VA_LIST Arguments;
    PLIST_ENTRY DispatchTableEntry;
    PKD_DISPATCH_TABLE DispatchTable;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    DispatchTableEntry = Providers.Flink;
    while(DispatchTableEntry != &Providers)
    {
        DispatchTable = CONTAIN_RECORD(DispatchTableEntry, KD_DISPATCH_TABLE, ListEntry);

        RTL::WideString::FormatWideString(&DispatchTable->PrintContext, (PWCHAR)Format, Arguments);

        DispatchTableEntry = DispatchTableEntry->Flink;
    }

    /* Clean up the va_list */
    VA_END(Arguments);
}

/**
 * Detects and enables the kernel's debug ports based on the 'DEBUG' parameter passed to the kernel.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KD::DebugIo::DetectDebugPorts(VOID)
{
    PCWSTR DebugOption;
    XTSTATUS Status;

    /* Get debug parameter */
    Status = KE::BootInformation::GetKernelParameter(L"DEBUG", &DebugOption);
    if(Status != STATUS_SUCCESS)
    {
        /* Debug parameter not found, disable debugging */
        DebugMode.Enabled = FALSE;
        return Status;
    }

    /* Skip parameter name and check if it is set */
    DebugOption += 5;
    if(*DebugOption != L'=')
    {
        /* Debug parameter not set, disable debugging */
        DebugMode.Enabled = FALSE;
        return STATUS_INVALID_PARAMETER;
    }

    /* Skip '=' symbol */
    DebugOption++;

    /* Iterate over all debug ports */
    while(*DebugOption != L'\0' && *DebugOption != L' ')
    {
        /* Check what port is set for debugging */
        if(RTL::WideString::CompareWideStringInsensitive(DebugOption, L"COM", 3) == 0)
        {
            /* Enable serial port debugging mode */
            DebugMode.Mode |= DEBUG_PROVIDER_COMPORT;

            /* Read COM port number */
            DebugOption += 3;
            while(*DebugOption >= '0' && *DebugOption <= '9')
            {
                /* Get port number */
                DebugMode.ComPortNumber *= 10;
                DebugMode.ComPortNumber += *DebugOption - '0';
                DebugOption++;
            }

            /* Check if custom COM port address supplied */
            if(DebugMode.ComPortNumber == 0 && RTL::WideString::CompareWideStringInsensitive(DebugOption, L":0x", 3) == 0)
            {
                /* COM port address provided */
                DebugOption += 3;
                while((*DebugOption >= '0' && *DebugOption <= '9') ||
                      (*DebugOption >= 'A' && *DebugOption <= 'F') ||
                      (*DebugOption >= 'a' && *DebugOption <= 'f'))
                {
                    /* Get port address */
                    DebugMode.ComPortAddress *= 16;
                    if(*DebugOption >= '0' && *DebugOption <= '9')
                    {
                        DebugMode.ComPortAddress += *DebugOption - '0';
                    }
                    else if(*DebugOption >= 'A' && *DebugOption <= 'F')
                    {
                        DebugMode.ComPortAddress += *DebugOption - 'A' + 10;
                    }
                    else if(*DebugOption >= 'a' && *DebugOption <= 'f')
                    {
                        DebugMode.ComPortAddress += *DebugOption - 'a' + 10;
                    }
                    DebugOption++;
                }
            }

            /* Look for additional COM port parameters */
            if(*DebugOption == ',')
            {
                /* Baud rate provided */
                DebugOption++;
                while(*DebugOption >= '0' && *DebugOption <= '9')
                {
                    /* Get baud rate */
                    DebugMode.ComPortBaudRate *= 10;
                    DebugMode.ComPortBaudRate += *DebugOption - '0';
                    DebugOption++;
                }
            }
        }
        else if(RTL::WideString::CompareWideStringInsensitive(DebugOption, L"SCREEN", 6) == 0)
        {
            /* Enable framebuffer debugging mode */
            DebugMode.Mode |= DEBUG_PROVIDER_FRAMEBUFFER;
            DebugOption += 6;
        }
        else if(*DebugOption == L';')
        {
            /* Skip separator */
            DebugOption++;
        }
        else
        {
            /* Invalid debug option, skip it */
            while(*DebugOption != L'\0' && *DebugOption != L' ' && *DebugOption != L';')
            {
                /* Advance debug option */
                DebugOption++;
            }

        }
    }

    /* Ensure at least one debug port is enabled */
    if(DebugMode.Mode != 0)
    {
        /* Enable debugging */
        DebugMode.Enabled = TRUE;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the kernel's debugger I/O providers.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KD::DebugIo::InitializeDebugIoProviders(VOID)
{
    ULONG Index;
    XTSTATUS ProviderStatus, Status;

    /* Initialize debug providers list */
    RTL::LinkedList::InitializeListHead(&Providers);

    RTL::Memory::ZeroMemory(&DebugMode, sizeof(KD_DEBUG_MODE));
    DetectDebugPorts();

    /* Iterate over providers initialization routines */
    for(Index = 0; Index < KDBG_PROVIDERS_COUNT; Index++)
    {
        /* Initialize provider */
        ProviderStatus = IoProvidersInitRoutines[Index]();
        Status = (Status || ProviderStatus);
    }

    /* Initialize debug print routine */
    SetPrintRoutine((PKD_PRINT_ROUTINE)DbgPrint);

    /* Return status code */
    return Status;
}

/**
 * Initializes the framebuffer device provider for the kernel debugger.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KD::DebugIo::InitializeFrameBufferProvider(VOID)
{
    STATIC KD_DISPATCH_TABLE DispatchTable;
    ULONG Height, Width;

    /* Check if framebuffer provider is enabled */
    if(DebugMode.Enabled && (DebugMode.Mode & DEBUG_PROVIDER_FRAMEBUFFER) == 0)
    {
        /* Screen is not enabled, no need to initialize provider */
        return STATUS_PORT_DISCONNECTED;
    }

    /* Ensure frame buffer is initialized and get FB resolution */
    HL::FrameBuffer::InitializeFrameBuffer();
    HL::FrameBuffer::GetFrameBufferResolution(&Width, &Height);

    /* Print debug message */
    DebugPrint(L"Initializing debug console at framebuffer device (%lu x %lu)\n", Width, Height);

    /* Initialize scroll region to full screen and white font color */
    HL::FrameBuffer::InitializeScrollRegion(0, 0, Width - 1, Height - 1, 0xFFFFFFFF);

    /* Initialize screen dispatch table */
    DispatchTable.PrintContext.WriteWideCharacter = HL::FrameBuffer::DisplayCharacter;
    RTL::LinkedList::InsertHeadList(&Providers, &DispatchTable.ListEntry);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the serial port device provider for the kernel debugger.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KD::DebugIo::InitializeSerialPortProvider(VOID)
{
    STATIC KD_DISPATCH_TABLE DispatchTable;
    XTSTATUS Status;

    /* Check if serial port provider is enabled */
    if(DebugMode.Enabled && (DebugMode.Mode & DEBUG_PROVIDER_COMPORT) == 0)
    {
        /* Serial port is not enabled, no need to initialize provider */
        return STATUS_PORT_DISCONNECTED;
    }

    /* Check if custom COM port address supplied */
    if(!DebugMode.ComPortAddress)
    {
        /* We support only a pre-defined number of ports */
        if(DebugMode.ComPortNumber > COMPORT_COUNT)
        {
            /* Fail if wrong/unsupported port used */
            return STATUS_INVALID_PARAMETER;
        }

        /* Check if serial port is set */
        if(DebugMode.ComPortNumber == 0)
        {
            /* Use COM1 by default */
            DebugMode.ComPortNumber = 1;
        }

        /* Set custom port address based on the port number and print debug message */
        DebugMode.ComPortAddress = SerialPortList[DebugMode.ComPortNumber - 1];
        DebugPrint(L"Initializing debug console at serial port (COM%lu, BaudRate: %lu)\n",
                   DebugMode.ComPortNumber, DebugMode.ComPortBaudRate);
    }
    else
    {
        /* Custom port address supplied, print debug message */
        DebugPrint(L"Initializing debug console at serial port (0x%lX, BaudRate: %lu)\n",
                   DebugMode.ComPortAddress, DebugMode.ComPortBaudRate);
    }

    /* Initialize COM port */
    Status = HL::ComPort::InitializeComPort(&SerialPort, (PUCHAR)UlongToPtr(DebugMode.ComPortAddress), DebugMode.ComPortBaudRate);
    if(Status != STATUS_SUCCESS)
    {
        /* Serial port initialization failed */
        return Status;
    }

    /* Initialize serial port dispatch table */
    DispatchTable.PrintContext.WriteWideCharacter = SerialWriteCharacter;
    RTL::LinkedList::InsertHeadList(&Providers, &DispatchTable.ListEntry);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Configures the kernel's debug print routine by setting a new output handler.
 *
 * @param DebugPrintRoutine
 *        Supplies a pointer to the new debug print routine.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KD::DebugIo::SetPrintRoutine(PKD_PRINT_ROUTINE DebugPrintRoutine)
{
    /* Set debug print routine */
    KdPrint = DebugPrintRoutine;
}


/**
 * Writes a character to the serial console.
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
KD::DebugIo::SerialWriteCharacter(WCHAR Character)
{
    WCHAR Buffer[2];

    /* Write character to the serial console */
    Buffer[0] = Character;
    Buffer[1] = 0;
    return HL::ComPort::WriteComPort(&SerialPort, Buffer[0]);
}
