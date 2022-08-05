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
 *
 */
EFI_STATUS
XtLoaderStartup(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_STATUS Status;

    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Initialize EFI console */
    Status = BlConsoleInitialize();
    if(Status != STATUS_EFI_SUCCESS) {
        /* TODO: Display error message on the serial console */
        /* Temporarily return error code */
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Infinite bootloader loop */
    for(;;);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
