/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/efiutil.c
 * DESCRIPTION:     EFI utilities
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * Enables I/O space access to all serial controllers found on the PCI(E) root bridge.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlActivateSerialControllerIO()
{
    EFI_GUID PciGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
    PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL PciDev;
    USHORT Bus, Device, Function, Command;
    UINT_PTR Index, PciHandleSize;
    PEFI_HANDLE PciHandle = NULL;
    PCI_COMMON_HEADER PciHeader;
    EFI_STATUS Status;
    UINT64 Address;

    /* Allocate memory for single EFI_HANDLE, what should be enough in most cases */
    PciHandleSize = sizeof(EFI_HANDLE);
    Status = BlEfiMemoryAllocatePool(PciHandleSize, (PVOID*)&PciHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Get all instances of PciRootBridgeIo */
    Status = EfiSystemTable->BootServices->LocateHandle(ByProtocol, &PciGuid, NULL, &PciHandleSize, PciHandle);
    if(Status == STATUS_EFI_BUFFER_TOO_SMALL)
    {
        /* Reallocate more memory as requested by UEFI */
        BlEfiMemoryFreePool(PciHandle);
        Status = BlEfiMemoryAllocatePool(PciHandleSize, (PVOID*)&PciHandle);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory reallocation failure */
            return Status;
        }

        /* Second attempt to get instances of PciRootBridgeIo */
        Status = EfiSystemTable->BootServices->LocateHandle(ByProtocol, &PciGuid, NULL, &PciHandleSize, PciHandle);
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
                    Address = ((UINT64)((((UINT_PTR) Bus) << 24) + (((UINT_PTR) Device) << 16) +
                                        (((UINT_PTR) Function) << 8) + ((UINT_PTR) 0)));
                    PciDev->Pci.Read(PciDev, 2, Address, sizeof (PciHeader) / sizeof (UINT32), &PciHeader);

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
                        Status = PciDev->Pci.Write(PciDev, 1, Address, 1, &Command);
                    }
                }
            }
        }
    }

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine initializes the COM port debug console.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlComPortInitialize()
{
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    ULONG PortAddress, PortNumber, BaudRate;
    PWCHAR Argument, CommandLine, LastArg;
    EFI_STATUS EfiStatus;
    XTSTATUS Status;

    /* Set default serial port options */
    PortAddress = 0;
    PortNumber = 0;
    BaudRate = 0;

    /* Handle loaded image protocol */
    EfiStatus = EfiSystemTable->BootServices->HandleProtocol(EfiImageHandle, &LIPGuid, (PVOID *)&LoadedImage);
    if(EfiStatus == STATUS_EFI_SUCCESS)
    {

        /* Check if launched from UEFI shell */
        if(LoadedImage && LoadedImage->LoadOptions)
        {
            /* Store arguments passed from UEFI shell */
            CommandLine = (PWCHAR)LoadedImage->LoadOptions;

            /* Find command in command line */
            Argument = RtlWideStringTokenize(CommandLine, L" ", &LastArg);

            /* Iterate over all arguments passed to boot loader */
            while(Argument != NULL)
            {
                /* Check if this is DEBUG parameter */
                if(RtlWideStringCompare(Argument, L"DEBUG=", 6) == 0)
                {
                    /* Skip to the argument value */
                    Argument += 6;

                    /* Make sure COM port is being used */
                    if(RtlWideStringCompare(Argument, L"COM", 3))
                    {
                        /* Invalid debug port specified */
                        BlEfiPrint(L"ERROR: Invalid debug port specified, falling back to defaults\n");
                        break;
                    }

                    /* Read COM port number */
                    Argument += 3;
                    while(*Argument >= '0' && *Argument <= '9')
                    {
                        /* Get port number */
                        PortNumber *= 10;
                        PortNumber += *Argument - '0';
                        Argument++;
                    }

                    /* Check if custom COM port address supplied */
                    if(PortNumber == 0 && RtlWideStringCompare(Argument, L":0x", 3) == 0)
                    {
                        /* COM port address provided */
                        Argument += 3;
                        while((*Argument >= '0' && *Argument <= '9') ||
                              (*Argument >= 'A' && *Argument <= 'F') ||
                              (*Argument >= 'a' && *Argument <= 'f'))
                        {
                            /* Get port address */
                            PortAddress *= 16;
                            if(*Argument >= '0' && *Argument <= '9')
                            {
                                PortAddress += *Argument - '0';
                            }
                            else if(*Argument >= 'A' && *Argument <= 'F')
                            {
                                PortAddress += *Argument - 'A' + 10;
                            }
                            else if(*Argument >= 'a' && *Argument <= 'f')
                            {
                                PortAddress += *Argument - 'a' + 10;
                            }
                            Argument++;
                        }
                    }

                    /* Look for additional COM port parameters */
                    if(*Argument == ',')
                    {
                        /* Baud rate provided */
                        Argument++;
                        while(*Argument >= '0' && *Argument <= '9')
                        {
                            /* Get baud rate */
                            BaudRate *= 10;
                            BaudRate += *Argument - '0';
                            Argument++;
                        }
                    }

                    /* No need to check next arguments */
                    break;
                }

                /* Take next argument */
                Argument = RtlWideStringTokenize(NULL, L" ", &LastArg);
            }
        }
    }

    /* Print debug message depending on port settings */
    if(PortAddress)
    {
        BlEfiPrint(L"Initializing serial console at COM port address: %lx\n", PortAddress);
    }
    else
    {
        BlEfiPrint(L"Initializing serial console at port COM%d\n", PortNumber);
    }

    /* Initialize COM port */
    Status = HlInitializeComPort(&EfiSerialPort, PortNumber, UlongToPtr(PortAddress), BaudRate);

    /* Port not found under supplied address */
    if(Status == STATUS_NOT_FOUND && PortAddress)
    {
        /* This might be PCI(E) serial controller, try to activate I/O space access first */
        EfiStatus = BlActivateSerialControllerIO();
        if(EfiStatus == STATUS_EFI_SUCCESS)
        {
            /* Try to reinitialize COM port */
            BlEfiPrint(L"Enabled I/O space access for all PCI(E) serial controllers found\n");
            Status = HlInitializeComPort(&EfiSerialPort, PortNumber, UlongToPtr(PortAddress), BaudRate);
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
 * Writes a character to the serial console.
 *
 * @param Character
 *        The integer promotion of the character to be written.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlComPortPutChar(IN USHORT Character)
{
    USHORT Buffer[2];

    /* Write character to the serial console */
    Buffer[0] = Character;
    Buffer[1] = 0;

    HlComPortPutByte(&EfiSerialPort, Buffer[0]);
}

/**
 * This routine formats the input string and prints it out to the serial console.
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
BlDbgPrint(IN PUINT16 Format,
           IN ...)
{
    VA_LIST Arguments;

    /* Check if debugging enabled and if EFI serial port is fully initialized */
    if(DEBUG && (EfiSerialPort.Flags & COMPORT_FLAG_INIT))
    {
        /* Initialise the va_list */
        VA_START(Arguments, Format);

        /* Format and print the string to the serial console */
        BlStringPrint(BlComPortPutChar, Format, Arguments);

        /* Clean up the va_list */
        VA_END(Arguments);
    }
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
 *
 * @todo Check if GOP is active and use it instead of default conout protocol
 */
XTCDECL
VOID
BlEfiPrint(IN PUINT16 Format,
           IN ...)
{
    VA_LIST Arguments;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    /* Format and print the string to the stdout */
    BlStringPrint(BlConsolePutChar, Format, Arguments);

    /* Print to serial console only if not running under OVMF */
    if(RtlWideStringCompare(EfiSystemTable->FirmwareVendor, L"EDK II", 6) != 0)
    {
        /* Check if debugging enabled and if EFI serial port is fully initialized */
        if(DEBUG && (EfiSerialPort.Flags & COMPORT_FLAG_INIT))
        {
            /* Format and print the string to the serial console */
            BlStringPrint(BlComPortPutChar, Format, Arguments);
        }
    }

    /* Clean up the va_list */
    VA_END(Arguments);
}
