/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/hardware.c
 * DESCRIPTION:     EFI hardware support for XT Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Enables I/O space access to all serial controllers found on the PCI(E) root bridge.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpActivateSerialIOController()
{
    EFI_GUID PciGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
    PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL PciDev;
    USHORT Bus, Device, Function, Command;
    UINT_PTR Index, PciHandleSize;
    PEFI_HANDLE PciHandle = NULLPTR;
    PCI_COMMON_HEADER PciHeader;
    EFI_STATUS Status;
    ULONGLONG Address;

    /* Allocate memory for single EFI_HANDLE, what should be enough in most cases */
    PciHandleSize = sizeof(EFI_HANDLE);
    Status = BlAllocateMemoryPool(PciHandleSize, (PVOID*)&PciHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Get all instances of PciRootBridgeIo */
    Status = EfiSystemTable->BootServices->LocateHandle(ByProtocol, &PciGuid, NULLPTR, &PciHandleSize, PciHandle);
    if(Status == STATUS_EFI_BUFFER_TOO_SMALL)
    {
        /* Reallocate more memory as requested by UEFI */
        BlFreeMemoryPool(PciHandle);
        Status = BlAllocateMemoryPool(PciHandleSize, (PVOID*)&PciHandle);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory reallocation failure */
            return Status;
        }

        /* Second attempt to get instances of PciRootBridgeIo */
        Status = EfiSystemTable->BootServices->LocateHandle(ByProtocol, &PciGuid, NULLPTR, &PciHandleSize, PciHandle);
    }

    /* Make sure successfully obtained PciRootBridgeIo instances */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get PciRootBridgeIo instances */
        return Status;
    }

    /* Enumerate all devices for each handle, which decides a segment and a bus number range */
    for(Index = 0; Index < (PciHandleSize / sizeof(EFI_HANDLE)); Index++)
    {
        /* Get inferface from the protocol */
        Status = EfiSystemTable->BootServices->HandleProtocol(PciHandle[Index], &PciGuid, (PVOID*)&PciDev);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to get interface */
            return Status;
        }

        /* Enumerate whole PCI bridge */
        for(Bus = 0; Bus <= PCI_MAX_BRIDGE_NUMBER; Bus++)
        {
            /* Enumerate all devices for each bus */
            for(Device = 0; Device < PCI_MAX_DEVICES; Device++)
            {
                /* Enumerate all functions for each devices */
                for(Function = 0; Function < PCI_MAX_FUNCTION; Function++)
                {
                    /* Read configuration space */
                    Address = ((ULONGLONG)((((UINT_PTR) Bus) << 24) + (((UINT_PTR) Device) << 16) +
                                           (((UINT_PTR) Function) << 8) + ((UINT_PTR) 0)));
                    PciDev->Pci.Read(PciDev, 2, Address, sizeof (PciHeader) / sizeof (UINT), &PciHeader);

                    /* Check if device exists */
                    if(PciHeader.VendorId == PCI_INVALID_VENDORID)
                    {
                        /* Skip non-existen device */
                        continue;
                    }

                    /* Check if device is serial controller or multiport serial controller */
                    if(PciHeader.BaseClass == 0x07 && (PciHeader.SubClass == 0x00 || PciHeader.SubClass == 0x02))
                    {
                        /* Enable I/O space access */
                        Address |= 0x4;
                        Command = PCI_ENABLE_IO_SPACE;
                        Status = PciDev->Pci.Write(PciDev, 1, Address, 1, &Command);
                    }
                }
            }
        }
    }

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}
