/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/beep/beep.c
 * DESCRIPTION:     XTLDR Beep Module
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <beep.h>


/* Dummy module information */
XTBL_MODINFO = L"Plays a GRUB compatible tune via PC speaker";

/**
 * Disables the PC speaker.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BpDisableToneBeep()
{
    UCHAR Status;

    /* Stop the PC speaker */
    Status = HlIoPortInByte(0x61);
    HlIoPortOutByte(0x61, Status & 0xFC);
}

/**
 * Enables the PC speaker and plays a sound.
 *
 * @param Pitch
 *        Specifies a pitch (in Hz) of the sound.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BpEnableToneBeep(IN UINT Pitch)
{
    UINT Counter;
    UCHAR Status;

    /* Pitch only in range of 20..20000 */
    if(Pitch < 20)
    {
        Pitch = 20;
    }
    else if(Pitch > 20000)
    {
        Pitch = 20000;
    }

    /* Set the desired frequency of the PIT clock */
    Counter = 0x1234DD / Pitch;
    HlIoPortOutByte(0x43, 0xB6);
    HlIoPortOutByte(0x43, 0xB6);
    HlIoPortOutByte(0x42, (UCHAR) Counter & 0xFF);
    HlIoPortOutByte(0x42, (UCHAR) (Counter >> 8) & 0xFF);

    /* Start the PC speaker */
    Status = HlIoPortInByte(0x61);
    HlIoPortOutByte(0x61, Status | 0x03);
}

/**
 * This routine plays a tune.
 *
 * @param Arguments
 *        Optional list of parameters provided with the command.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BpPlayTune(IN PWCHAR Arguments)
{
    LONG Pitch, Duration, Tempo;
    PWCHAR Argument, LastArgument;

    /* Reset pitch and duration */
    Duration = -1;
    Pitch = -1;
    Tempo = -1;

    /* Tokenize provided list of arguments */
    Argument = RtlTokenizeWideString(Arguments, L" ", &LastArgument);

    /* Iterate over all arguments */
    while(Argument != NULL)
    {
        /* Check if tempo, pitch and duration are set */
        if(Tempo < 0)
        {
            /* Set the tempo */
            Tempo = BpWideStringToNumber(Argument);
        }
        else if(Pitch < 0)
        {
            /* Set the pitch */
            Pitch = BpWideStringToNumber(Argument);
        }
        else
        {
            /* Set the duration */
            Duration = BpWideStringToNumber(Argument);

            /* Check pitch value */
            if(Pitch > 0)
            {
                /* Emit the beep tone */
                BpEnableToneBeep(Pitch);
            }
            else
            {
                /* Stop emitting beep tone */
                BpDisableToneBeep();
            }

            /* Wait for duration time */
            XtLdrProto->Util.SleepExecution(60000 * Duration / Tempo);

            /* Reset pitch and duration */
            Pitch = -1;
            Duration = -1;
        }

        /* Get next argument */
        Argument = RtlTokenizeWideString(NULL, L" ", &LastArgument);
    }

    /* Stop emitting beep tone */
    BpDisableToneBeep();
}

/**
 * Converts a wide string into a number.
 *
 * @param String
 *        Supplies an input wide string.
 *
 * @return This routine returns the number that was converted from the wide string.
 *
 * @since XT 1.0
 */
XTCDECL
UINT
BpWideStringToNumber(IN PWCHAR String)
{
    ULONG Number = 0;

    /* Iterate over all characters until '\0' found */
    do
    {
        /* Check if this is a digit */
        if(*String - '0' < 10)
        {
            /* Add another digit to the number */
            Number *= 10;
            Number += *String - '0';
        }
    }
    while(*++String != L'\0');

    /* Return number */
    return Number;
}

/**
 * This routine is the entry point of the XT EFI boot loader module.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_STATUS Status;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProto);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open the protocol, return error */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Play the tune set in the configuration */
    BpPlayTune(XtLdrProto->Config.GetValue(L"TUNE"));

	/* Return success */
    return STATUS_EFI_SUCCESS;
}
