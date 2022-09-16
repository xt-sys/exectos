/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/blproto.c
 * DESCRIPTION:     EFI XTLDR protocol API
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * This routine locates and opens the XT boot loader protocol.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @param LoaderProtocol
 *        Supplies the address where a pointer to the bootloader protocol is returned.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlGetXtLoaderProtocol(EFI_HANDLE ImageHandle,
                      PEFI_SYSTEM_TABLE SystemTable,
                      PXT_BOOT_LOADER_PROTOCOL *LoaderProtocol)
{
    EFI_GUID Guid = XT_BOOT_LOADER_PROTOCOL_GUID;
    PEFI_HANDLE Handles = NULL;
    EFI_STATUS Status;
    UINT_PTR Count;
    UINT Index;

    /* Try to locate the handles */
    Status = SystemTable->BootServices->LocateHandleBuffer(ByProtocol, &Guid, NULL, &Count, &Handles);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to get handles */
        return Status;
    }

    /* Check if any handles returned */
    if(Count > 0)
    {
        /* Iterate through all given handles */
        for(Index = 0; Index < Count; Index++)
        {
            /* Try to open protocol */
            Status = SystemTable->BootServices->OpenProtocol(Handles[Index], &Guid, (PVOID *)&LoaderProtocol,
                                                             ImageHandle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

            /* Check if successfully opened the loader protocol */
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Protocol found */
                break;
            }
        }
    }

    /* Free handles */
    SystemTable->BootServices->FreePool(Handles);

    /* Make sure the loaded protocol has been found */
    if(LoaderProtocol == NULL)
    {
        /* Protocol not found */
        return STATUS_EFI_NOT_FOUND;
    }

    /* Return success */
	return STATUS_EFI_SUCCESS;
}
