/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/debug.c
 * DESCRIPTION:     XT Boot Loader debugging support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


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
BlDebugPrint(IN PUSHORT Format,
             IN ...)
{
    RTL_PRINT_CONTEXT ConsolePrintContext, SerialPrintContext;
    VA_LIST Arguments;

    /* Check if debugging enabled and if EFI serial port is fully initialized */
    if(DEBUG)
    {
        /* Initialize the print contexts */
        ConsolePrintContext.WriteWideCharacter = BlpConsolePutChar;
        SerialPrintContext.WriteWideCharacter = BlpDebugPutChar;

        /* Initialise the va_list */
        VA_START(Arguments, Format);

        /* Check if serial debug port is enabled */
        if((BlpStatus.DebugPort & XTBL_DEBUGPORT_SERIAL) && (BlpStatus.SerialPort.Flags & COMPORT_FLAG_INIT))
        {
            /* Format and print the string to the serial console */
            RtlFormatWideString(&SerialPrintContext, (PWCHAR)Format, Arguments);
        }

        /* Check if screen debug port is enabled and Boot Services are still available */
        if((BlpStatus.DebugPort & XTBL_DEBUGPORT_SCREEN) && (BlpStatus.BootServices == TRUE))
        {
            /* Format and print the string to the screen */
            RtlFormatWideString(&ConsolePrintContext, (PWCHAR)Format, Arguments);
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
BlpDebugPutChar(IN USHORT Character)
{
    USHORT Buffer[2];

    /* Write character to the serial console */
    Buffer[0] = Character;
    Buffer[1] = 0;
    HlComPortPutByte(&BlpStatus.SerialPort, Buffer[0]);

    /* Return success */
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
BlpInitializeDebugConsole()
{
    ULONG PortAddress, PortNumber, BaudRate;
    PWCHAR DebugConfiguration, DebugPort, LastPort;
    EFI_STATUS Status;

    /* Set default serial port options */
    PortAddress = 0;
    PortNumber = 0;
    BaudRate = 0;

    /* Get debug configuration */
    DebugConfiguration = BlGetConfigValue(L"DEBUG");

    /* Make sure any debug options are provided and debug console is not initialized yet */
    if(DebugConfiguration && BlpStatus.DebugPort == 0)
    {
        /* Find all debug ports */
        DebugPort = RtlTokenizeWideString(DebugConfiguration, L";", &LastPort);

        /* Iterate over all debug ports */
        while(DebugPort != NULL)
        {
            /* Check what port is set for debugging */
            if(RtlCompareWideStringInsensitive(DebugPort, L"COM", 3) == 0)
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
                if(PortNumber == 0 && RtlCompareWideStringInsensitive(DebugPort, L":0x", 3) == 0)
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
            else if(RtlCompareWideStringInsensitive(DebugPort, L"SCREEN", 5) == 0)
            {
                /* Enable debug port */
                BlpStatus.DebugPort |= XTBL_DEBUGPORT_SCREEN;
            }
            else
            {
                /* Unsupported debug port specified */
                BlConsolePrint(L"ERROR: Unsupported debug port ('%S') specified\n", DebugPort);
                BlSleepExecution(3000);
            }

            /* Take next debug port */
            DebugPort = RtlTokenizeWideString(NULL, L";", &LastPort);
        }

        /* Check if serial debug port is enabled */
        if(BlpStatus.DebugPort & XTBL_DEBUGPORT_SERIAL)
        {
            /* Try to initialize COM port */
            Status = BlpInitializeSerialPort(PortNumber, PortAddress, BaudRate);
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
BlpInitializeSerialPort(IN ULONG PortNumber,
                        IN ULONG PortAddress,
                        IN ULONG BaudRate)
{
    EFI_STATUS EfiStatus;
    XTSTATUS Status;

    /* Print debug message depending on port settings */
    if(PortAddress)
    {
        BlConsolePrint(L"Initializing serial console at COM port address: 0x%lx\n", PortAddress);
    }
    else
    {
        BlConsolePrint(L"Initializing serial console at port COM%d\n", PortNumber);
    }

    /* Initialize COM port */
    Status = HlInitializeComPort(&BlpStatus.SerialPort, PortNumber, UlongToPtr(PortAddress), BaudRate);

    /* Port not found under supplied address */
    if(Status == STATUS_NOT_FOUND && PortAddress)
    {
        /* This might be PCI(E) serial controller, try to activate I/O space access first */
        EfiStatus = BlpActivateSerialIOController();
        if(EfiStatus == STATUS_EFI_SUCCESS)
        {
            /* Try to reinitialize COM port */
            BlConsolePrint(L"Enabled I/O space access for all PCI(E) serial controllers found\n");
            Status = HlInitializeComPort(&BlpStatus.SerialPort, PortNumber, UlongToPtr(PortAddress), BaudRate);
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
