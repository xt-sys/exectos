/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/blproto.c
 * DESCRIPTION:     EFI XTLDR protocol API
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>
#include <blmod.h>


/**
 * This routine locates and opens the XT boot loader protocol.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlGetXtLoaderProtocol(PXT_BOOT_LOADER_PROTOCOL *LdrProtocol)
{
    EFI_GUID Guid = XT_BOOT_LOADER_PROTOCOL_GUID;
    PEFI_HANDLE Handles = NULL;
    EFI_STATUS Status;
    UINT_PTR Count;
    UINT Index;

    /* Try to locate the handles */
    Status = EfiSystemTable->BootServices->LocateHandleBuffer(ByProtocol, &Guid, NULL, &Count, &Handles);
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
            Status = EfiSystemTable->BootServices->OpenProtocol(Handles[Index], &Guid, (PVOID*)LdrProtocol,
                                                                EfiImageHandle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

            /* Check if successfully opened the loader protocol */
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Protocol found and successfully opened */
                break;
            }
        }
    }

    /* Free handles */
    EfiSystemTable->BootServices->FreePool(Handles);

    /* Make sure the loaded protocol has been found */
    if(*LdrProtocol == NULL)
    {
        /* Protocol not found */
        return STATUS_EFI_NOT_FOUND;
    }

    /* Return success */
	return STATUS_EFI_SUCCESS;
}
