/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/dummy/dummy.c
 * DESCRIPTION:     Boot loader dummy module
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI XT Loader Protocol */
PXT_BOOT_LOADER_PROTOCOL EfiXtLdrProtocol;

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
EFI_STATUS
BlXtLoaderModuleMain(EFI_HANDLE ImageHandle,
                     PEFI_SYSTEM_TABLE SystemTable)
{
    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Open the XTLDR protocol */
    BlGetXtLoaderProtocol(ImageHandle, SystemTable, &EfiXtLdrProtocol);

    /* Print message and return success */
    EfiXtLdrProtocol->EfiPrint(L"XTLDR dummy module initialized\n");
    return STATUS_EFI_SUCCESS;
}
