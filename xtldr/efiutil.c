/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/efiutil.c
 * DESCRIPTION:     EFI utilities
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * This routine initializes the COM port debug console.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlComPortInitialize()
{
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    ULONG PortNumber, BaudRate;
    PWCHAR Argument, CommandLine, LastArg;
    EFI_STATUS EfiStatus;
    XTSTATUS Status;

    /* Set default serial port options */
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

    /* Initialize COM port */
    Status = HlInitializeComPort(&EfiSerialPort, PortNumber, BaudRate);
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
VOID
BlDbgPrint(IN PUINT16 Format,
           IN ...)
{
    VA_LIST Arguments;

    /* Check if EFI serial port is fully initialized */
    if(EfiSerialPort.Flags & COMPORT_FLAG_INIT)
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
 * This routine allocates one or more 4KB pages.
 *
 * @param Size
 *        The number of contiguous 4KB pages to allocate.
 *
 * @param Memory
 *        The pointer to a physical address.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryAllocatePages(IN UINT64 Size,
                         OUT PEFI_PHYSICAL_ADDRESS Memory)
{
    return EfiSystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiRuntimeServicesData,
                                                       EFI_SIZE_TO_PAGES(Size), Memory);
}

/**
 * This routine allocates a pool memory.
 *
 * @param Size
 *        The number of bytes to allocate from the pool.
 *
 * @param Memory
 *        The pointer to a physical address.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryAllocatePool(IN UINT_PTR Size,
                        OUT PVOID *Memory)
{
    /* Allocate pool */
    return EfiSystemTable->BootServices->AllocatePool(EfiLoaderData, Size, Memory);
}

/**
 * This routine frees memory pages.
 *
 * @param Size
 *        The number of contiguous 4 KB pages to free.
 *
 * @param Memory
 *        The base physical address of the pages to be freed.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryFreePages(IN UINT64 Size,
                     IN EFI_PHYSICAL_ADDRESS Memory)
{
    return EfiSystemTable->BootServices->FreePages(Memory, Size);
}

/**
 * Returns pool memory to the system.
 *
 * @param Memory
 *        The pointer to the buffer to free.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryFreePool(IN PVOID Memory)
{
    /* Free pool */
    return EfiSystemTable->BootServices->FreePool(Memory);
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
        /* Check if EFI serial port is fully initialized */
        if(EfiSerialPort.Flags & COMPORT_FLAG_INIT)
        {
            /* Format and print the string to the serial console */
            BlStringPrint(BlComPortPutChar, Format, Arguments);
        }
    }

    /* Clean up the va_list */
    VA_END(Arguments);
}
