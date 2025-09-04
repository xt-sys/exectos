/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/dbgio.c
 * DESCRIPTION:     Kernel Debugger I/O routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


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
KdpDebugPrint(PCWSTR Format, ...)
{
    VA_LIST Arguments;
    PLIST_ENTRY DispatchTableEntry;
    PKD_DISPATCH_TABLE DispatchTable;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    DispatchTableEntry = KdpProviders.Flink;
    while(DispatchTableEntry != &KdpProviders)
    {
        DispatchTable = CONTAIN_RECORD(DispatchTableEntry, KD_DISPATCH_TABLE, ListEntry);

        RtlFormatWideString(&DispatchTable->PrintContext, (PWCHAR)Format, Arguments);

        DispatchTableEntry = DispatchTableEntry->Flink;
    }

    /* Clean up the va_list */
    VA_END(Arguments);
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
KdpInitializeFrameBufferProvider(VOID)
{
    STATIC KD_DISPATCH_TABLE DispatchTable;
    ULONG Height, Width;

    /* Check if framebuffer provider is enabled */
    if(KdpDebugMode.Enabled && (KdpDebugMode.Mode & DEBUG_PROVIDER_FRAMEBUFFER) == 0)
    {
        /* Screen is not enabled, no need to initialize provider */
        return STATUS_PORT_DISCONNECTED;
    }

    /* Ensure frame buffer is initialized and get FB resolution */
    HlInitializeFrameBuffer();
    HlGetFrameBufferResolution(&Width, &Height);

    /* Print debug message */
    DebugPrint(L"Initializing debug console at framebuffer device (%lu x %lu)\n", Width, Height);

    /* Initialize scroll region to full screen and white font color */
    HlInitializeScrollRegion(0, 0, Width - 1, Height - 1, 0xFFFFFFFF);

    /* Initialize screen dispatch table */
    DispatchTable.PrintContext.WriteWideCharacter = HlDisplayCharacter;
    RtlInsertHeadList(&KdpProviders, &DispatchTable.ListEntry);

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
KdpInitializeSerialPortProvider(VOID)
{
    STATIC KD_DISPATCH_TABLE DispatchTable;
    XTSTATUS Status;

    /* Check if serial port provider is enabled */
    if(KdpDebugMode.Enabled && (KdpDebugMode.Mode & DEBUG_PROVIDER_COMPORT) == 0)
    {
        /* Serial port is not enabled, no need to initialize provider */
        return STATUS_PORT_DISCONNECTED;
    }

    /* Check if custom COM port address supplied */
    if(!KdpDebugMode.ComPortAddress)
    {
        /* We support only a pre-defined number of ports */
        if(KdpDebugMode.ComPortNumber > COMPORT_COUNT)
        {
            /* Fail if wrong/unsupported port used */
            return STATUS_INVALID_PARAMETER;
        }

        /* Check if serial port is set */
        if(KdpDebugMode.ComPortNumber == 0)
        {
            /* Use COM1 by default */
            KdpDebugMode.ComPortNumber = 1;
        }

        /* Set custom port address based on the port number and print debug message */
        KdpDebugMode.ComPortAddress = KdpSerialPortList[KdpDebugMode.ComPortNumber - 1];
        DebugPrint(L"Initializing debug console at serial port (COM%lu, BaudRate: %lu)\n",
                   KdpDebugMode.ComPortNumber, KdpDebugMode.ComPortBaudRate);
    }
    else
    {
        /* Custom port address supplied, print debug message */
        DebugPrint(L"Initializing debug console at serial port (0x%lX, BaudRate: %lu)\n",
                   KdpDebugMode.ComPortAddress, KdpDebugMode.ComPortBaudRate);
    }

    /* Initialize COM port */
    Status = HlInitializeComPort(&KdpSerialPort, UlongToPtr(KdpDebugMode.ComPortAddress), KdpDebugMode.ComPortBaudRate);
    if(Status != STATUS_SUCCESS)
    {
        /* Serial port initialization failed */
        return Status;
    }

    /* Initialize serial port dispatch table */
    DispatchTable.PrintContext.WriteWideCharacter = KdpSerialWriteCharacter;
    RtlInsertHeadList(&KdpProviders, &DispatchTable.ListEntry);

    /* Return success */
    return STATUS_SUCCESS;
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
KdpSerialWriteCharacter(WCHAR Character)
{
    WCHAR Buffer[2];

    /* Write character to the serial console */
    Buffer[0] = Character;
    Buffer[1] = 0;
    return HlComPortPutByte(&KdpSerialPort, Buffer[0]);
}
