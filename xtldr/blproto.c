/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/blproto.c
 * DESCRIPTION:     EFI XTLDR protocol API
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>
#include <blproto.h>


/**
 * This routine locates and opens the XT boot loader protocol.
 *
 * @param LdrProtocol
 *        Supplies the address where a pointer to the loader protocol is returned.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlGetXtLoaderProtocol(OUT PXT_BOOT_LOADER_PROTOCOL *LdrProtocol)
{
    EFI_GUID Guid = XT_BOOT_LOADER_PROTOCOL_GUID;

    /* Load XTLDR protocol */
    return BlLoadXtProtocol((PVOID *)LdrProtocol, &Guid);
}

/**
 * This routine locates and opens the requested XT protocol.
 *
 * @param ProtocolHandler
 *        Supplies the address where a pointer to the opened protocol is returned.
 *
 * @param ProtocolGuid
 *        Supplies a pointer to the unique protocol GUID.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlLoadXtProtocol(OUT PVOID *ProtocolHandler,
                 IN PEFI_GUID ProtocolGuid)
{
    PEFI_HANDLE Handles = NULL;
    EFI_STATUS Status;
    UINT_PTR Count;
    UINT Index;

    /* Try to locate the handles */
    Status = EfiSystemTable->BootServices->LocateHandleBuffer(ByProtocol, ProtocolGuid, NULL, &Count, &Handles);
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
            Status = EfiSystemTable->BootServices->OpenProtocol(Handles[Index], ProtocolGuid,
                                                                ProtocolHandler, EfiImageHandle, NULL,
                                                                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

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
    if(*ProtocolHandler == NULL)
    {
        /* Protocol not found */
        return STATUS_EFI_NOT_FOUND;
    }

    /* Return success */
	return STATUS_EFI_SUCCESS;
}
