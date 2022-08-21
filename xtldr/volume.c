/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/volume.c
 * DESCRIPTION:     XTLDR volume support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/* List of available block devices */
LIST_ENTRY BlBlockDevices;

EFI_STATUS
BlEnumerateEfiBlockDevices()
{
    PEFI_DEVICE_PATH_PROTOCOL LastNode = NULL;
    PEFI_BLOCK_DEVICE_DATA ParentNode = NULL;
    PEFI_BLOCK_DEVICE_DATA BlockDeviceData;
    PEFI_BLOCK_DEVICE BlockDevice;
    LIST_ENTRY BlockDevices;
    PLIST_ENTRY ListEntry;
    EFI_STATUS Status;
    PEFI_ACPI_HID_DEVICE_PATH AcpiDevice;
    PEFI_HARDDRIVE_DEVICE_PATH HDPath;
    PEFI_BLOCK_IO_MEDIA Media;
    PEFI_GUID PartitionGuid;
    ULONG DriveNumber, PartitionNumber;
    USHORT DriveType;
    ULONG CDCount = 0, FDCount = 0, HDCount = 0, RDCount = 0;

    /* Initialize list entries */
    RtlInitializeListHead(&BlockDevices);
    RtlInitializeListHead(&BlBlockDevices);

    /* Discover EFI block devices and store them in linked list */
    Status = BlpDiscoverEfiBlockDevices(&BlockDevices);
    if(Status != STATUS_EFI_SUCCESS)
    {
        BlDbgPrint(L"ERROR: Failed to discover EFI block devices (status code: %lx)\n", Status);
        return Status;
    }

    /* Identify all discovered devices */
    ListEntry = BlockDevices.Flink;
    while(ListEntry != &BlockDevices)
    {
        /* Take block device from the list */
        BlockDeviceData = CONTAIN_RECORD(ListEntry, EFI_BLOCK_DEVICE_DATA, ListEntry);

        /* Find last node */
        Status = BlpFindLastEfiBlockDeviceNode(BlockDeviceData->DevicePath, &LastNode);
        if(Status != STATUS_EFI_SUCCESS)
        {
            BlDbgPrint(L"WARNING: Block device last node not found\n");
            ListEntry = ListEntry->Flink;
            continue;
        }

        /* Set drive type to 'unknown' by default */
        DriveType = XT_BOOT_DEVICE_UNKNOWN;

        /* Check last node type */
        if(LastNode->Type == EFI_ACPI_DEVICE_PATH && LastNode->SubType == EFI_ACPI_DP)
        {
            /* Check for Floppy EISA identifiers */
            AcpiDevice = (PEFI_ACPI_HID_DEVICE_PATH)LastNode;
            if(AcpiDevice->HID == 0x60441D0 || AcpiDevice->HID == 0x70041D0 || AcpiDevice->HID == 0x70141D1)
            {
                /* Floppy drive found */
                Media = BlockDeviceData->BlockIo->Media;
                DriveType = XT_BOOT_DEVICE_FLOPPY;
                DriveNumber = FDCount++;
                PartitionNumber = 0;

                /* Print debug message */
                BlDbgPrint(L"Found Floppy Disk (DiskNumber: %lu, MediaPresent: %u, RO: %u)\n",
                           DriveNumber, Media->MediaPresent, Media->ReadOnly);
            }
        }
        else if(LastNode->Type == EFI_MEDIA_DEVICE_PATH)
        {
            /* Media device path found */
            if(LastNode->SubType == EFI_MEDIA_CDROM_DP)
            {
                /* Optical drive found */
                Media = BlockDeviceData->BlockIo->Media;
                DriveType = XT_BOOT_DEVICE_CDROM;
                DriveNumber = CDCount++;
                PartitionNumber = 0;

                /* Print debug message */
                BlDbgPrint(L"Found CD-ROM drive (DriveNumber: %lu, MediaPresent: %u, RemovableMedia: %u, RO: %u)\n",
                           DriveNumber, Media->MediaPresent, Media->RemovableMedia, Media->ReadOnly);
            }
            else if(LastNode->SubType == EFI_MEDIA_HARDDRIVE_DP)
            {
                /* Hard disk partition found */
                Media = BlockDeviceData->BlockIo->Media;
                HDPath = (PEFI_HARDDRIVE_DEVICE_PATH)LastNode;
                DriveType = XT_BOOT_DEVICE_HARDDISK;
                DriveNumber = (HDPath->PartitionNumber == 1) ? HDCount++ : HDCount - 1;
                PartitionNumber = HDPath->PartitionNumber;
                PartitionGuid = (PEFI_GUID)HDPath->Signature;

                /* Print debug message */
                BlDbgPrint(L"Found Hard Disk partition (DiskNumber: %lu, PartNumber: %u, "
                           L"MBRType: %u, GUID: {%g}, PartSize: %luB)\n",
                           DriveNumber, PartitionNumber, HDPath->MBRType,
                           PartitionGuid, HDPath->PartitionSize * Media->BlockSize);
            }
            else if(LastNode->SubType == EFI_MEDIA_RAMDISK_DP)
            {
                /* RAM disk found */
                Media = BlockDeviceData->BlockIo->Media;
                DriveType = XT_BOOT_DEVICE_RAMDISK;
                DriveNumber = RDCount++;
                PartitionNumber = 0;

                /* Print debug message */
                BlDbgPrint(L"Found RAM Disk (DiskNumber: %lu, MediaPresent: %u)\n",
                           DriveNumber, Media->MediaPresent);
            }

            if(!BlpFindParentEfiBlockDevice(&BlockDevices, BlockDeviceData, ParentNode))
            {
                BlDbgPrint(L"WARNING: No parent device found, skipping orphaned media device path\n");
                continue;
            }
        }

        /* Make sure the device found has valid type set */
        if(DriveType != XT_BOOT_DEVICE_UNKNOWN)
        {
            /* Allocate memory for block device */
            Status = BlEfiMemoryAllocatePool(sizeof(EFI_BLOCK_DEVICE), (PVOID *)&BlockDevice);
            if(Status != STATUS_EFI_SUCCESS)
            {
                BlDbgPrint(L"ERROR: Unable to allocate memory pool for block device (status code: %lx)\n", Status);
                return STATUS_EFI_OUT_OF_RESOURCES;
            }

            /* Initialize block device */
            BlockDevice->DevicePath = BlpDuplicateDevicePath(BlockDeviceData->DevicePath);
            BlockDevice->DriveType = DriveType;
            BlockDevice->DriveNumber = DriveNumber;
            BlockDevice->PartitionNumber = PartitionNumber;
            BlockDevice->PartitionGuid = PartitionGuid;

            /* Add block device to global list */
            RtlInsertTailList(&BlBlockDevices, &BlockDevice->ListEntry);
        }

        /* Get next entry from linked list */
        ListEntry = ListEntry->Flink;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

EFI_STATUS
BlpDiscoverEfiBlockDevices(OUT PLIST_ENTRY BlockDevices)
{
    EFI_GUID DevicePathGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;
    EFI_GUID IoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    PEFI_DEVICE_PATH_PROTOCOL DevicePath;
    PEFI_BLOCK_DEVICE_DATA BlockDevice;
    UINT_PTR HandlesCount, Index;
    PEFI_HANDLE Handles = NULL;
    PEFI_BLOCK_IO_PROTOCOL Io;
    EFI_STATUS Status;

    /* Locate handles which support the disk I/O interface */
    Status = EfiSystemTable->BootServices->LocateHandleBuffer(ByProtocol, &IoGuid, NULL, &HandlesCount, &Handles);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to locate handles */
        BlDbgPrint(L"ERROR: Failed to locate block devices handles (status code: %lx)\n", Status);
        return Status;
    }

    /* Iterate through all handles */
    for(Index = 0; Index < HandlesCount; Index++)
    {
        /* Print debug message */
        BlDbgPrint(L"Opening %lu block device from %lu discovered\n", Index + 1, HandlesCount);

        /* Open I/O protocol for given handle */
        Io = NULL;
        Status = EfiSystemTable->BootServices->OpenProtocol(Handles[Index], &IoGuid, (PVOID *)&Io, EfiImageHandle,
                                                            NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
        if(Status != STATUS_EFI_SUCCESS || Io == NULL)
        {
            /* Failed to open I/O protocol, skip it */
            BlDbgPrint(L"WARNING: Failed to open EFI Block I/O protocol (status code: %lx)\n", Status);
            continue;
        }

        /* Check if this is iPXE stub */
        if(Io->Media && Io->Media->BlockSize == 1 && Io->Media->MediaId == 0x69505845U)
        {
            /* Skip stub as it is non-functional */
            BlDbgPrint(L"WARNING: iPXE stub block I/O protocol");
            continue;
        }

        /* Check if DevicePath protocol is supported by this handle */
        DevicePath = NULL;
        Status = EfiSystemTable->BootServices->HandleProtocol(Handles[Index], &DevicePathGuid, (PVOID *)&DevicePath);
        if(Status != STATUS_EFI_SUCCESS || DevicePath == NULL)
        {
            /* Device failed to handle DP protocol */
            BlDbgPrint(L"WARNING: Unable to open DevicePath protocol (status code: %lx)\n", Status);
            EfiSystemTable->BootServices->CloseProtocol(Handles[Index], &IoGuid, EfiImageHandle, NULL);
            continue;
        }

        /* Allocate memory for block device */
        Status = BlEfiMemoryAllocatePool(sizeof(*BlockDevice), (PVOID *)&BlockDevice);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            BlDbgPrint(L"ERROR: Unable to allocate memory pool for block device (status code: %lx)\n", Status);
            EfiSystemTable->BootServices->CloseProtocol(Handles[Index], &DevicePathGuid, EfiImageHandle, NULL);
            EfiSystemTable->BootServices->CloseProtocol(Handles[Index], &IoGuid, EfiImageHandle, NULL);
            return Status;
        }

        /* Store new block device into a linked list */
        BlockDevice->BlockIo = Io;
        BlockDevice->DevicePath = DevicePath;
        RtlInsertTailList(BlockDevices, &BlockDevice->ListEntry);
    }

    /* Free handles buffer */
    BlEfiMemoryFreePool(Handles);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

PEFI_DEVICE_PATH_PROTOCOL
BlpDuplicateDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath)
{
    PEFI_DEVICE_PATH_PROTOCOL DevicePathNode;
    PEFI_DEVICE_PATH_PROTOCOL DevicePathClone;
    EFI_STATUS Status;
    UINT Length = 0;

    DevicePathNode = DevicePath;

    /* Get the device path length */
    while(TRUE)
    {
        Length += *(PUINT16)DevicePath->Length;
        if(DevicePathNode->Type == EFI_END_DEVICE_PATH)
        {
            break;
        }
        DevicePathNode = (PEFI_DEVICE_PATH_PROTOCOL)((PUCHAR)DevicePathNode + *(PUINT16)DevicePath->Length);
    }

    /* Check length */
    if(Length == 0)
    {
        /* Nothing to duplicate */
        return NULL;
    }

    /* Allocate memory for the new device path */
    Status = BlEfiMemoryAllocatePool(Length, (PVOID *)&DevicePathClone);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to allocate memory */
        BlDbgPrint(L"ERROR: Unable to allocate memory pool for device path duplicate\n");
        return NULL;
    }

    /* Copy the device path */
    RtlCopyMemory(DevicePathClone, DevicePath, Length);

    /* Return the cloned object */
    return DevicePathClone;
}

EFI_STATUS
BlpFindLastEfiBlockDeviceNode(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                              OUT PEFI_DEVICE_PATH_PROTOCOL *LastNode)
{
    PEFI_DEVICE_PATH_PROTOCOL EndNode, NextNode;

    /* Make sure end is not reached yet */
    if(DevicePath->Type == EFI_END_DEVICE_PATH)
    {
        /* End reached, nothing to do */
        LastNode = NULL;
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Fast forward to the last node */
    EndNode = DevicePath;
    while(EndNode->Type != EFI_END_DEVICE_PATH)
    {
        NextNode = EndNode;
        EndNode = (PEFI_DEVICE_PATH_PROTOCOL)((PUCHAR)EndNode + *(PUSHORT)EndNode->Length);
    }

    /* Store last node found */
    *LastNode = NextNode;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

BOOLEAN
BlpFindParentEfiBlockDevice(IN PLIST_ENTRY BlockDevices,
                            IN PEFI_BLOCK_DEVICE_DATA ChildNode,
                            OUT PEFI_BLOCK_DEVICE_DATA ParentNode)
{
    PEFI_DEVICE_PATH_PROTOCOL ChildDevicePath, ParentDevicePath;
    PEFI_BLOCK_DEVICE_DATA BlockDeviceData;
    UINT ChildLength, ParentLength;
    PLIST_ENTRY ListEntry;

    ListEntry = BlockDevices->Flink;
    while(ListEntry != BlockDevices)
    {
        /* Take block device from the list */
        BlockDeviceData = CONTAIN_RECORD(ListEntry, EFI_BLOCK_DEVICE_DATA, ListEntry);

        ChildDevicePath = ChildNode->DevicePath;
        ParentDevicePath = BlockDeviceData->DevicePath;

        /* Iterate nodes */
        while(TRUE)
        {
            /* Check if the parent device is a match */
            if(ParentDevicePath->Type == EFI_END_DEVICE_PATH)
            {
                /* Parent device is a match */
                ParentNode = BlockDeviceData;
                return TRUE;
            }

            /* Get child and parent node lengths */
            ChildLength = *(PUINT16)ChildDevicePath->Length;
            ParentLength = *(PUINT16)ParentDevicePath->Length;

            /* Check if lengths match */
            if(ChildLength != ParentLength)
            {
                /* Lengths do not match, this is not a valid parent */
                break;
            }

            /* Move to the next child and parent nodes */
            ChildDevicePath = (PEFI_DEVICE_PATH_PROTOCOL)((PUCHAR)ChildDevicePath + ChildLength);
            ParentDevicePath = (PEFI_DEVICE_PATH_PROTOCOL)((PUCHAR)ParentDevicePath + ParentLength);
        }

        /* Get next entry from linked list */
        ListEntry = ListEntry->Flink;
    }

    /* Apparently not found a parent node */
    return FALSE;
}
