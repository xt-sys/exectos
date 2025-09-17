/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/beep/beep.cc
 * DESCRIPTION:     XTLDR Beep Module
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <beep.hh>


/* Beep module information */
MODULE_AUTHOR(L"Rafal Kupiec <belliash@codingworkshop.eu.org>");
MODULE_DESCRIPTION(L"Plays a GRUB compatible tune via PC speaker");
MODULE_LICENSE(L"GPLv3");
MODULE_VERSION(L"0.1");


/**
 * Disables the PC speaker.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Beep::DisableToneBeep()
{
    UCHAR Status;

    /* Stop the PC speaker */
    Status = HlReadPort8(0x61);
    HlWritePort8(0x61, Status & 0xFC);
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
Beep::EnableToneBeep(IN UINT Pitch)
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
    HlWritePort8(0x43, 0xB6);
    HlWritePort8(0x43, 0xB6);
    HlWritePort8(0x42, (UCHAR) Counter & 0xFF);
    HlWritePort8(0x42, (UCHAR) (Counter >> 8) & 0xFF);

    /* Start the PC speaker */
    Status = HlReadPort8(0x61);
    HlWritePort8(0x61, Status | 0x03);
}

/**
 * Initializes BEEP module by opening XTLDR protocol and playing the tune.
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
Beep::InitializeModule(IN EFI_HANDLE ImageHandle,
                       IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_STATUS Status;
    PWCHAR Tune;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open the protocol, return error */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Play the tune set in the configuration */
    XtLdrProtocol->Config.GetValue(L"TUNE", &Tune);
    PlayTune(Tune);

    /* Return success */
    return STATUS_EFI_SUCCESS;
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
Beep::PlayTune(IN PWCHAR Arguments)
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
    while(Argument != NULLPTR)
    {
        /* Check if tempo, pitch and duration are set */
        if(Tempo < 0)
        {
            /* Set the tempo */
            Tempo = WideStringToNumber(Argument);
        }
        else if(Pitch < 0)
        {
            /* Set the pitch */
            Pitch = WideStringToNumber(Argument);
        }
        else
        {
            /* Set the duration */
            Duration = WideStringToNumber(Argument);

            /* Check pitch value */
            if(Pitch > 0)
            {
                /* Emit the beep tone */
                EnableToneBeep(Pitch);
            }
            else
            {
                /* Stop emitting beep tone */
                DisableToneBeep();
            }

            /* Wait for duration time */
            XtLdrProtocol->Util.SleepExecution(60000 * Duration / Tempo);

            /* Reset pitch and duration */
            Pitch = -1;
            Duration = -1;
        }

        /* Get next argument */
        Argument = RtlTokenizeWideString(NULLPTR, L" ", &LastArgument);
    }

    /* Stop emitting beep tone */
    DisableToneBeep();
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
Beep::WideStringToNumber(IN PWCHAR String)
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
    /* Initialize BEEP module */
    return Beep::InitializeModule(ImageHandle, SystemTable);
}
