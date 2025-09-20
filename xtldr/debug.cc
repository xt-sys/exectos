/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/debug.cc
 * DESCRIPTION:     XT Boot Loader debugging support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


/**
 * Enables I/O space access to all serial controllers found on the PCI(E) root bridge.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Debug::ActivateSerialIOController()
{
    EFI_GUID PciGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
    PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL PciDev;
    USHORT Bus, Device, Function, Command;
    UINT_PTR Index, PciHandleSize;
    PEFI_HANDLE PciHandle = NULLPTR;
    PCI_COMMON_HEADER PciHeader;
    EFI_STATUS Status;
    ULONGLONG Address;

    /* Allocate memory for single EFI_HANDLE, what should be enough in most cases */
    PciHandleSize = sizeof(EFI_HANDLE);
    Status = Memory::AllocatePool(PciHandleSize, (PVOID*)&PciHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Get all instances of PciRootBridgeIo */
    Status = EfiSystemTable->BootServices->LocateHandle(ByProtocol, &PciGuid, NULLPTR, &PciHandleSize, PciHandle);
    if(Status == STATUS_EFI_BUFFER_TOO_SMALL)
    {
        /* Reallocate more memory as requested by UEFI */
        Memory::FreePool(PciHandle);
        Status = Memory::AllocatePool(PciHandleSize, (PVOID*)&PciHandle);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory reallocation failure */
            return Status;
        }

        /* Second attempt to get instances of PciRootBridgeIo */
        Status = EfiSystemTable->BootServices->LocateHandle(ByProtocol, &PciGuid, NULLPTR, &PciHandleSize, PciHandle);
    }

    /* Make sure successfully obtained PciRootBridgeIo instances */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get PciRootBridgeIo instances */
        return Status;
    }

    /* Enumerate all devices for each handle, which decides a segment and a bus number range */
    for(Index = 0; Index < (PciHandleSize / sizeof(EFI_HANDLE)); Index++)
    {
        /* Get inferface from the protocol */
        Status = EfiSystemTable->BootServices->HandleProtocol(PciHandle[Index], &PciGuid, (PVOID*)&PciDev);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to get interface */
            return Status;
        }

        /* Enumerate whole PCI bridge */
        for(Bus = 0; Bus <= PCI_MAX_BRIDGE_NUMBER; Bus++)
        {
            /* Enumerate all devices for each bus */
            for(Device = 0; Device < PCI_MAX_DEVICES; Device++)
            {
                /* Enumerate all functions for each devices */
                for(Function = 0; Function < PCI_MAX_FUNCTION; Function++)
                {
                    /* Read configuration space */
                    Address = ((ULONGLONG)((((UINT_PTR) Bus) << 24) + (((UINT_PTR) Device) << 16) +
                                           (((UINT_PTR) Function) << 8) + ((UINT_PTR) 0)));
                    PciDev->Pci.Read(PciDev, EfiPciIoWidthUint32, Address, sizeof (PciHeader) / sizeof (UINT), &PciHeader);

                    /* Check if device exists */
                    if(PciHeader.VendorId == PCI_INVALID_VENDORID)
                    {
                        /* Skip non-existen device */
                        continue;
                    }

                    /* Check if device is serial controller or multiport serial controller */
                    if(PciHeader.BaseClass == 0x07 && (PciHeader.SubClass == 0x00 || PciHeader.SubClass == 0x02))
                    {
                        /* Enable I/O space access */
                        Address |= 0x4;
                        Command = PCI_ENABLE_IO_SPACE;
                        Status = PciDev->Pci.Write(PciDev, EfiPciIoWidthUint16, Address, 1, &Command);
                    }
                }
            }
        }
    }

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine initializes the XTLDR debug console.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Debug::InitializeDebugConsole()
{
    ULONG PortAddress, PortNumber, BaudRate;
    PWCHAR DebugConfiguration, DebugPort, LastPort;
    EFI_STATUS Status;

    /* Set default serial port options */
    PortAddress = 0;
    PortNumber = 0;
    BaudRate = 0;

    /* Get debug configuration */
    Configuration::GetValue(L"DEBUG", &DebugConfiguration);

    /* Make sure any debug options are provided and debug console is not initialized yet */
    if(DebugConfiguration && BlpStatus.DebugPort == 0)
    {
        /* Find all debug ports */
        DebugPort = RTL::WideString::TokenizeWideString(DebugConfiguration, L";", &LastPort);

        /* Iterate over all debug ports */
        while(DebugPort != NULLPTR)
        {
            /* Check what port is set for debugging */
            if(RTL::WideString::CompareWideStringInsensitive(DebugPort, L"COM", 3) == 0)
            {
                /* Read COM port number */
                DebugPort += 3;
                while(*DebugPort >= '0' && *DebugPort <= '9')
                {
                    /* Get port number */
                    PortNumber *= 10;
                    PortNumber += *DebugPort - '0';
                    DebugPort++;
                }

                /* Check if custom COM port address supplied */
                if(PortNumber == 0 && RTL::WideString::CompareWideStringInsensitive(DebugPort, L":0x", 3) == 0)
                {
                    /* COM port address provided */
                    DebugPort += 3;
                    while((*DebugPort >= '0' && *DebugPort <= '9') ||
                          (*DebugPort >= 'A' && *DebugPort <= 'F') ||
                          (*DebugPort >= 'a' && *DebugPort <= 'f'))
                    {
                        /* Get port address */
                        PortAddress *= 16;
                        if(*DebugPort >= '0' && *DebugPort <= '9')
                        {
                            PortAddress += *DebugPort - '0';
                        }
                        else if(*DebugPort >= 'A' && *DebugPort <= 'F')
                        {
                            PortAddress += *DebugPort - 'A' + 10;
                        }
                        else if(*DebugPort >= 'a' && *DebugPort <= 'f')
                        {
                            PortAddress += *DebugPort - 'a' + 10;
                        }
                        DebugPort++;
                    }
                }

                /* Look for additional COM port parameters */
                if(*DebugPort == ',')
                {
                    /* Baud rate provided */
                    DebugPort++;
                    while(*DebugPort >= '0' && *DebugPort <= '9')
                    {
                        /* Get baud rate */
                        BaudRate *= 10;
                        BaudRate += *DebugPort - '0';
                        DebugPort++;
                    }
                }

                /* Enable debug port */
                BlpStatus.DebugPort |= XTBL_DEBUGPORT_SERIAL;
            }
            else if(RTL::WideString::CompareWideStringInsensitive(DebugPort, L"SCREEN", 5) == 0)
            {
                /* Enable debug port */
                BlpStatus.DebugPort |= XTBL_DEBUGPORT_SCREEN;
            }
            else
            {
                /* Unsupported debug port specified */
                Console::Print(L"ERROR: Unsupported debug port ('%S') specified\n", DebugPort);
                EfiUtils::SleepExecution(3000);
            }

            /* Take next debug port */
            DebugPort = RTL::WideString::TokenizeWideString(NULLPTR, L";", &LastPort);
        }

        /* Check if serial debug port is enabled */
        if(BlpStatus.DebugPort & XTBL_DEBUGPORT_SERIAL)
        {
            /* Try to initialize COM port */
            Status = InitializeSerialPort(PortNumber, PortAddress, BaudRate);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Remove serial debug port, as COM port initialization failed and return */
                BlpStatus.DebugPort &= ~XTBL_DEBUGPORT_SERIAL;
                return Status;
            }
        }
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine initializes the serial debug console.
 *
 * @param PortNumber
 *        Supplies a port number.
 *
 * @param PortAddress
 *        Supplies an address of the COM port.
 *
 * @param BaudRate
 *        Supplies an optional port baud rate.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Debug::InitializeSerialPort(IN ULONG PortNumber,
                            IN ULONG PortAddress,
                            IN ULONG BaudRate)
{
    EFI_STATUS EfiStatus;
    XTSTATUS Status;

    /* Check if custom COM port address supplied */
    if(!PortAddress)
    {
        /* We support only a pre-defined number of ports */
        if(PortNumber > COMPORT_COUNT)
        {
            /* Fail if wrong/unsupported port used */
            return STATUS_INVALID_PARAMETER;
        }

        /* Check if serial port is set */
        if(PortNumber == 0)
        {
            /* Use COM1 by default */
            PortNumber = 1;
        }

        /* Set custom port address based on the port number and print debug message */
        PortAddress = BlComPortList[PortNumber - 1];
        Console::Print(L"Initializing serial console at port COM%d\n", PortNumber);
    }
    else
    {
        /* Custom port address supplied, print debug message */
        Console::Print(L"Initializing serial console at COM port address: 0x%lX\n", PortAddress);
    }

    /* Initialize COM port */
    Status = HL::ComPort::InitializeComPort(&BlpStatus.SerialPort, (PUCHAR)UlongToPtr(PortAddress), BaudRate);

    /* Port not found under supplied address */
    if(Status == STATUS_NOT_FOUND && PortAddress)
    {
        /* This might be PCI(E) serial controller, try to activate I/O space access first */
        EfiStatus = ActivateSerialIOController();
        if(EfiStatus == STATUS_EFI_SUCCESS)
        {
            /* Try to reinitialize COM port */
            Console::Print(L"Enabled I/O space access for all PCI(E) serial controllers found\n");
            Status = HL::ComPort::InitializeComPort(&BlpStatus.SerialPort, (PUCHAR)UlongToPtr(PortAddress), BaudRate);
        }
    }

    /* Check COM port initialization status code */
    if(Status != STATUS_SUCCESS)
    {
        /* Serial port initialization failed, mark as not ready */
        return STATUS_EFI_NOT_READY;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine formats the input string and prints it out to the debug ports.
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
Debug::Print(IN PCWSTR Format,
             IN ...)
{
    RTL_PRINT_CONTEXT ConsolePrintContext, SerialPrintContext;
    VA_LIST Arguments;

    /* Check if debugging enabled and if EFI serial port is fully initialized */
    if(DEBUG)
    {
        /* Initialize the print contexts */
        ConsolePrintContext.WriteWideCharacter = Console::PutChar;
        SerialPrintContext.WriteWideCharacter = PutChar;

        /* Initialise the va_list */
        VA_START(Arguments, Format);

        /* Check if serial debug port is enabled */
        if((BlpStatus.DebugPort & XTBL_DEBUGPORT_SERIAL) && (BlpStatus.SerialPort.Flags & COMPORT_FLAG_INIT))
        {
            /* Format and print the string to the serial console */
            RTL::WideString::FormatWideString(&SerialPrintContext, (PWCHAR)Format, Arguments);
        }

        /* Check if screen debug port is enabled and Boot Services are still available */
        if((BlpStatus.DebugPort & XTBL_DEBUGPORT_SCREEN) && (BlpStatus.BootServices == TRUE))
        {
            /* Format and print the string to the screen */
            RTL::WideString::FormatWideString(&ConsolePrintContext, (PWCHAR)Format, Arguments);
        }

        /* Clean up the va_list */
        VA_END(Arguments);
    }
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
Debug::PutChar(IN WCHAR Character)
{
    WCHAR Buffer[2];

    /* Write character to the serial console */
    Buffer[0] = Character;
    Buffer[1] = 0;
    return HL::ComPort::WriteComPort(&BlpStatus.SerialPort, Buffer[0]);
}
