/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/xtldr.c
 * DESCRIPTION:     UEFI XT Bootloader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;

/* Serial port configuration */
CPPORT EfiSerialPort;

/**
 * This routine is the entry point of the XT EFI boot loader.
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
EFI_STATUS
BlStartXtLoader(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_STATUS Status;

    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Initialize EFI console */
    BlConsoleInitialize();

    /* Early initialize COM port for debugging */
    Status = BlComPortInitialize();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Initialization failed, try printing error to stdout and serial console */
        BlEfiPrint(L"ERROR: Failed to initialize serial console\n");
    }

    /* Disable watchdog timer */
    Status = EfiSystemTable->BootServices->SetWatchdogTimer(0, 0x10000, 0, NULL);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to disable the timer, print message */
        BlDbgPrint(L"WARNING: Failed to disable watchdog timer\n");
    }

    /* Infinite bootloader loop */
    for(;;);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
