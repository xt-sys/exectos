/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/library/modproto.c
 * DESCRIPTION:     XT Boot Loader protocol support for XTLDR modules
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Finds and opens the XT Boot Loader protocol. This routine should be called by module to access XTLDR protocol.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param ProtocolHandler
 *        Receives the pointer to the XT Boot Loader protocol.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlGetXtLdrProtocol(IN PEFI_SYSTEM_TABLE SystemTable,
                   IN EFI_HANDLE ImageHandle,
                   OUT PXTBL_LOADER_PROTOCOL *ProtocolHandler)
{
    EFI_GUID ProtocolGuid = XT_BOOT_LOADER_PROTOCOL_GUID;
    PEFI_HANDLE Handles = NULLPTR;
    EFI_STATUS Status;
    UINT_PTR Count;
    UINT Index;

    /* Try to locate the handles */
    Status = SystemTable->BootServices->LocateHandleBuffer(ByProtocol, &ProtocolGuid, NULLPTR, &Count, &Handles);
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
            Status = SystemTable->BootServices->OpenProtocol(Handles[Index], &ProtocolGuid,
                                                             (PVOID*)ProtocolHandler, ImageHandle, NULLPTR,
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
    SystemTable->BootServices->FreePool(Handles);

    /* Make sure the loaded protocol has been found */
    if(*ProtocolHandler == NULLPTR)
    {
        /* Protocol not found */
        return STATUS_EFI_NOT_FOUND;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
