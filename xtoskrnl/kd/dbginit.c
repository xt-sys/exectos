/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/dbginit.c
 * DESCRIPTION:     Kernel Debugger initialization
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Initializes the kernel's debugger I/O providers.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KdInitializeDebugIoProviders(VOID)
{
    ULONG Index;
    XTSTATUS ProviderStatus, Status;

    /* Initialize debug providers list */
    RtlInitializeListHead(&KdpProviders);

    RtlZeroMemory(&KdpDebugMode, sizeof(KD_DEBUG_MODE));
    KdpDetectDebugPorts();

    /* Iterate over providers initialization routines */
    for(Index = 0; Index < KDBG_PROVIDERS_COUNT; Index++)
    {
        /* Initialize provider */
        ProviderStatus = KdpIoProvidersInitRoutines[Index]();
        Status = (Status || ProviderStatus);
    }

    /* Initialize debug print routine */
    KdSetPrintRoutine(KdpDebugPrint);

    /* Return status code */
    return Status;
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
KdSetPrintRoutine(PVOID DebugPrintRoutine)
{
    /* Set debug print routine */
    KdPrint = DebugPrintRoutine;
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
KdpDetectDebugPorts(VOID)
{
    PCWSTR DebugOption;
    XTSTATUS Status;

    /* Get debug parameter */
    Status = KeGetKernelParameter(L"DEBUG", &DebugOption);
    if(Status != STATUS_SUCCESS)
    {
        /* Debug parameter not found, disable debugging */
        KdpDebugMode.Enabled = FALSE;
        return Status;
    }

    /* Skip parameter name and check if it is set */
    DebugOption += 5;
    if(*DebugOption != L'=')
    {
        /* Debug parameter not set, disable debugging */
        KdpDebugMode.Enabled = FALSE;
        return STATUS_INVALID_PARAMETER;
    }

    /* Skip '=' symbol */
    DebugOption++;

    /* Iterate over all debug ports */
    while(*DebugOption != L'\0' && *DebugOption != L' ')
    {
        /* Check what port is set for debugging */
        if(RtlCompareWideStringInsensitive(DebugOption, L"COM", 3) == 0)
        {
            /* Enable serial port debugging mode */
            KdpDebugMode.Mode |= DEBUG_PROVIDER_COMPORT;

            /* Read COM port number */
            DebugOption += 3;
            while(*DebugOption >= '0' && *DebugOption <= '9')
            {
                /* Get port number */
                KdpDebugMode.ComPortNumber *= 10;
                KdpDebugMode.ComPortNumber += *DebugOption - '0';
                DebugOption++;
            }

            /* Check if custom COM port address supplied */
            if(KdpDebugMode.ComPortNumber == 0 && RtlCompareWideStringInsensitive(DebugOption, L":0x", 3) == 0)
            {
                /* COM port address provided */
                DebugOption += 3;
                while((*DebugOption >= '0' && *DebugOption <= '9') ||
                      (*DebugOption >= 'A' && *DebugOption <= 'F') ||
                      (*DebugOption >= 'a' && *DebugOption <= 'f'))
                {
                    /* Get port address */
                    KdpDebugMode.ComPortAddress *= 16;
                    if(*DebugOption >= '0' && *DebugOption <= '9')
                    {
                        KdpDebugMode.ComPortAddress += *DebugOption - '0';
                    }
                    else if(*DebugOption >= 'A' && *DebugOption <= 'F')
                    {
                        KdpDebugMode.ComPortAddress += *DebugOption - 'A' + 10;
                    }
                    else if(*DebugOption >= 'a' && *DebugOption <= 'f')
                    {
                        KdpDebugMode.ComPortAddress += *DebugOption - 'a' + 10;
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
                    KdpDebugMode.ComPortBaudRate *= 10;
                    KdpDebugMode.ComPortBaudRate += *DebugOption - '0';
                    DebugOption++;
                }
            }
        }
        else if(RtlCompareWideStringInsensitive(DebugOption, L"SCREEN", 6) == 0)
        {
            /* Enable framebuffer debugging mode */
            KdpDebugMode.Mode |= DEBUG_PROVIDER_FRAMEBUFFER;
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
    if(KdpDebugMode.Mode != 0)
    {
        /* Enable debugging */
        KdpDebugMode.Enabled = TRUE;
    }

    /* Return success */
    return STATUS_SUCCESS;
}
