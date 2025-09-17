/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/volume.cc
 * DESCRIPTION:     XTLDR volume support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.h>


/**
 * This routine closes an EFI volume handle.
 *
 * @param VolumeHandle
 *        Specifies a handle of opened volume.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlCloseVolume(IN PEFI_HANDLE VolumeHandle)
{
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    /* Make sure a handle specified */
    if(VolumeHandle != NULLPTR)
    {
        /* Close a handle */
        return EfiSystemTable->BootServices->CloseProtocol(VolumeHandle, &LIPGuid, EfiImageHandle, NULLPTR);
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Discovers and enumerates a block devices available to EFI system.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlEnumerateBlockDevices()
{
    EFI_GUID LoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID BlockIoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    EFI_HANDLE BootDeviceHandle = NULLPTR, DeviceHandle = NULLPTR;
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    PEFI_DEVICE_PATH_PROTOCOL DevicePath = NULLPTR, LastNode = NULLPTR;
    PEFI_BLOCK_DEVICE_DATA ParentNode = NULLPTR;
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

    /* Get the device handle of the image that is running */
    Status = EfiSystemTable->BootServices->HandleProtocol(EfiImageHandle, &LoadedImageProtocolGuid, (VOID**)&LoadedImage);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get boot device handle */
        BlDebugPrint(L"ERROR: Failed to get boot device handle (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Save the boot device handle */
    BootDeviceHandle = LoadedImage->DeviceHandle;

    /* Initialize list entries */
    RtlInitializeListHead(&BlockDevices);
    RtlInitializeListHead(&EfiBlockDevices);

    /* Discover EFI block devices and store them in linked list */
    Status = BlpDiscoverEfiBlockDevices(&BlockDevices);
    if(Status != STATUS_EFI_SUCCESS)
    {
        BlDebugPrint(L"ERROR: Failed to discover EFI block devices (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Identify all discovered devices */
    ListEntry = BlockDevices.Flink;
    while(ListEntry != &BlockDevices)
    {
        /* Get data for the next discovered device. */
        BlockDeviceData = CONTAIN_RECORD(ListEntry, EFI_BLOCK_DEVICE_DATA, ListEntry);
        PartitionGuid = NULLPTR;

        /* Find last node */
        Status = BlpFindLastBlockDeviceNode(BlockDeviceData->DevicePath, &LastNode);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Skip this device if its last node cannot be found, as it is required for classification */
            BlDebugPrint(L"WARNING: Block device last node not found\n");
            ListEntry = ListEntry->Flink;
            continue;
        }

        /* Initialize drive type before attempting to classify the device */
        DriveType = XTBL_BOOT_DEVICE_UNKNOWN;

        /* Locate the parent for this block device to ensure it is not an orphaned entry */
        if(!BlpFindParentBlockDevice(&BlockDevices, BlockDeviceData, &ParentNode))
        {
            /* Orphaned device found. Log a warning and skip it as it cannot be properly classified */
            BlDebugPrint(L"WARNING: No parent device found, skipping orphaned media device path\n");
            ListEntry = ListEntry->Flink;
            continue;
        }

        /* Verify that media information is available, as some devices may not report it */
        if(!BlockDeviceData->BlockIo->Media)
        {
            /* The device is unusable without media info, log a warning and skip it */
            BlDebugPrint(L"WARNING: Block device is missing media information\n");
            ListEntry = ListEntry->Flink;
            continue;
        }
        Media = BlockDeviceData->BlockIo->Media;

        /* Check last node type */
        if(LastNode->Type == EFI_ACPI_DEVICE_PATH && LastNode->SubType == EFI_ACPI_DP)
        {
            /* Check for Floppy EISA identifiers */
            AcpiDevice = (PEFI_ACPI_HID_DEVICE_PATH)LastNode;
            if(AcpiDevice->HID == 0x60441D0 || AcpiDevice->HID == 0x70041D0 || AcpiDevice->HID == 0x70141D1)
            {
                /* Floppy drive found */
                DriveType = XTBL_BOOT_DEVICE_FLOPPY;
                DriveNumber = FDCount++;
                PartitionNumber = 0;

                /* Print debug message */
                BlDebugPrint(L"Found Floppy Disk (DiskNumber: %lu, MediaPresent: %u, RO: %u)\n",
                             DriveNumber, Media->MediaPresent, Media->ReadOnly);
            }
        }
        else if((LastNode->Type == EFI_MEDIA_DEVICE_PATH && LastNode->SubType == EFI_MEDIA_CDROM_DP) ||
                (LastNode->Type == EFI_MESSAGING_DEVICE_PATH &&
                 (LastNode->SubType == EFI_MESSAGING_ATAPI_DP || LastNode->SubType == EFI_MESSAGING_SATA_DP) &&
                 Media->MediaPresent && Media->RemovableMedia))
        {
            /* Optical drive found */
            DriveType = XTBL_BOOT_DEVICE_CDROM;
            DriveNumber = CDCount++;
            PartitionNumber = 0;

            /* Print debug message */
            BlDebugPrint(L"Found CD-ROM drive (DriveNumber: %lu, MediaPresent: %u, RemovableMedia: %u, RO: %u)\n",
                         DriveNumber, Media->MediaPresent, Media->RemovableMedia, Media->ReadOnly);
        }
        else if(LastNode->Type == EFI_MEDIA_DEVICE_PATH && LastNode->SubType == EFI_MEDIA_HARDDRIVE_DP)
        {
            /* Hard disk partition found */
            HDPath = (PEFI_HARDDRIVE_DEVICE_PATH)LastNode;
            DriveType = XTBL_BOOT_DEVICE_HARDDISK;
            DriveNumber = (HDPath->PartitionNumber == 1) ? HDCount++ : HDCount - 1;
            PartitionNumber = HDPath->PartitionNumber;
            PartitionGuid = (PEFI_GUID)HDPath->Signature;

            /* Check if this is the EFI System Partition (ESP) */
            if(BootDeviceHandle != NULLPTR)
            {
                /* Allocate memory for device path */
                DevicePath = BlpDuplicateDevicePath(BlockDeviceData->DevicePath);
                if(DevicePath != NULLPTR)
                {
                    /* Check if this is the boot device */
                    Status = EfiSystemTable->BootServices->LocateDevicePath(&BlockIoGuid, &DevicePath,
                                                                            &DeviceHandle);
                    if(Status == STATUS_EFI_SUCCESS && DeviceHandle == BootDeviceHandle)
                    {
                        /* Mark partition as ESP */
                        DriveType |= XTBL_BOOT_DEVICE_ESP;
                    }
                }
            }

            /* Print debug message */
            BlDebugPrint(L"Found Hard Disk partition (DiskNumber: %lu, PartNumber: %lu, "
                         L"MBRType: %u, GUID: {%V}, PartSize: %lluB) %S\n",
                         DriveNumber, PartitionNumber, HDPath->MBRType,
                         PartitionGuid, HDPath->PartitionSize * Media->BlockSize,
                         (DriveType & XTBL_BOOT_DEVICE_ESP) ? L"(ESP)" : L"");
        }
        else if(LastNode->Type == EFI_MEDIA_DEVICE_PATH && LastNode->SubType == EFI_MEDIA_RAMDISK_DP)
        {
            /* RAM disk found */
            DriveType = XTBL_BOOT_DEVICE_RAMDISK;
            DriveNumber = RDCount++;
            PartitionNumber = 0;

            /* Print debug message */
            BlDebugPrint(L"Found RAM Disk (DiskNumber: %lu, MediaPresent: %u)\n",
                         DriveNumber, Media->MediaPresent);
        }

        /* Make sure the device found has valid type set */
        if(DriveType != XTBL_BOOT_DEVICE_UNKNOWN)
        {
            /* Allocate memory for block device */
            Status = BlAllocateMemoryPool(sizeof(EFI_BLOCK_DEVICE), (PVOID *)&BlockDevice);
            if(Status != STATUS_EFI_SUCCESS)
            {
                BlDebugPrint(L"ERROR: Failed to allocate memory pool for block device (Status Code: 0x%zX)\n", Status);
                return STATUS_EFI_OUT_OF_RESOURCES;
            }

            /* Initialize block device */
            BlockDevice->DevicePath = BlpDuplicateDevicePath(BlockDeviceData->DevicePath);
            BlockDevice->DriveType = DriveType;
            BlockDevice->DriveNumber = DriveNumber;
            BlockDevice->PartitionNumber = PartitionNumber;
            BlockDevice->PartitionGuid = PartitionGuid;

            /* Add block device to global list */
            RtlInsertTailList(&EfiBlockDevices, &BlockDevice->ListEntry);
        }

        /* Get next entry from linked list */
        ListEntry = ListEntry->Flink;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Finds an EFI device path for a specified path on a given file system.
 *
 * @param FsHandle
 *        The handle of the corresponding file system.
 *
 * @param FileSystemPath
 *        Specifies a path on the corresponding file system.
 *
 * @param DevicePath
 *        Specifies a pointer to the memory area, where found device path will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlFindVolumeDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL FsHandle,
                       IN CONST PWCHAR FileSystemPath,
                       OUT PEFI_DEVICE_PATH_PROTOCOL* DevicePath)
{
    EFI_STATUS Status;
    SIZE_T FsPathLength, DevicePathLength = 0;
    PEFI_FILEPATH_DEVICE_PATH FilePath = NULLPTR;
    PEFI_DEVICE_PATH_PROTOCOL EndDevicePath;
    PEFI_DEVICE_PATH_PROTOCOL DevicePathHandle;

    /* Set local device path handle */
    DevicePathHandle = FsHandle;

    /* Find the end device path node */
    while(TRUE) {
        /* Make sure there is a next node */
        if(*(PUSHORT)DevicePathHandle->Length == 0)
        {
            /* End device path not found */
            return STATUS_EFI_NOT_FOUND;
        }

        /* Check if end device path node found */
        if(DevicePathHandle->Type == EFI_END_DEVICE_PATH)
        {
            /* End device path node found */
            break;
        }

        /* Get next node */
        DevicePathLength += *(PUSHORT)DevicePathHandle->Length;
        DevicePathHandle = (PEFI_DEVICE_PATH_PROTOCOL)((PUCHAR)DevicePathHandle + *(PUSHORT)DevicePathHandle->Length);
    }

    /* Check real path length */
    FsPathLength = RtlWideStringLength(FileSystemPath, 0) * sizeof(WCHAR);

    /* Allocate memory pool for device path */
    Status = BlAllocateMemoryPool(FsPathLength + DevicePathLength + sizeof(EFI_DEVICE_PATH_PROTOCOL),
                                     (PVOID *)DevicePath);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Set file path */
    RtlCopyMemory(*DevicePath, FsHandle, DevicePathLength);
    FilePath = (PEFI_FILEPATH_DEVICE_PATH)((PUCHAR)*DevicePath + DevicePathLength);
    FilePath->Header.Type = EFI_MEDIA_DEVICE_PATH;
    FilePath->Header.SubType = EFI_MEDIA_FILEPATH_DP;
    FilePath->Header.Length[0] = (UCHAR)FsPathLength + FIELD_OFFSET(EFI_FILEPATH_DEVICE_PATH, PathName) + sizeof(WCHAR);
    FilePath->Header.Length[1] = FilePath->Header.Length[0] >> 8;

    /* Set device path end node */
    RtlCopyMemory(FilePath->PathName, FileSystemPath, FsPathLength + sizeof(WCHAR));
    EndDevicePath = (PEFI_DEVICE_PATH_PROTOCOL)&FilePath->PathName[(FsPathLength / sizeof(WCHAR)) + 1];
    EndDevicePath->Type = EFI_END_DEVICE_PATH;
    EndDevicePath->SubType = EFI_END_ENTIRE_DP;
    EndDevicePath->Length[0] = sizeof(EFI_DEVICE_PATH_PROTOCOL);
    EndDevicePath->Length[1] = 0;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Creates a copy of the system path with EFI standard directory separators.
 *
 * @param SystemPath
 *        Supplies a pointer to the system path.
 *
 * @param EfiPath
 *        Supplies a pointer to the memory area, where EFI path will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlGetEfiPath(IN PWCHAR SystemPath,
             OUT PWCHAR *EfiPath)
{
    SIZE_T Index, PathLength;
    EFI_STATUS Status;

    /* Get system path length */
    PathLength = RtlWideStringLength(SystemPath, 0);

    /* Allocate memory for storing EFI path */
    Status = BlAllocateMemoryPool(sizeof(WCHAR) * (PathLength + 1), (PVOID *)EfiPath);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to allocate memory, print error message and return status code */
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
        return STATUS_EFI_OUT_OF_RESOURCES;
    }

    /* Make a copy of SystemPath string */
    RtlCopyMemory(*EfiPath, SystemPath, sizeof(WCHAR) * (PathLength + 1));

    /* Replace directory separator if needed to comply with EFI standard */
    for(Index = 0; Index < PathLength; Index++)
    {
        if((*EfiPath)[Index] == L'/')
        {
            /* Replace '/' with '\' */
            (*EfiPath)[Index] = L'\\';
        }
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Finds a volume device path based on the specified ARC name or UUID.
 *
 * @param SystemPath
 *        An input string containing ARC/UUID path.
 *
 * @param DevicePath
 *        Supplies a pointer to memory region where device path will be stored.
 *
 * @param Path
 *        Supplies a pointer to the memory area, where path on device will be saved.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlGetVolumeDevicePath(IN PWCHAR SystemPath,
                      OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePath,
                      OUT PWCHAR *ArcName,
                      OUT PWCHAR *Path)
{
    PEFI_BLOCK_DEVICE Device;
    USHORT DriveType;
    ULONG DriveNumber;
    ULONG PartNumber;
    PWCHAR Volume;
    ULONG PathLength;
    PLIST_ENTRY ListEntry;
    EFI_STATUS Status;

    /* Make sure this is not set */
    *DevicePath = NULLPTR;

    /* Find volume path and its length */
    Volume = SystemPath;
    while(*Volume != '/' && *Volume != '\\' && *Volume != '\0')
    {
        Volume++;
    }
    PathLength = Volume - SystemPath;

    /* Check if valume path specified */
    if(PathLength == 0)
    {
        /* No volume path available */
        *Path = SystemPath;
        return STATUS_EFI_NOT_FOUND;
    }

    /* Check system path format */
    if(SystemPath[0] == '{')
    {
        if(PathLength == GUID_STRING_LENGTH)
        {
            /* This is EFI GUID */
            BlDebugPrint(L"WARNING: EFI/GPT GUID in system path is not supported\n");
            return STATUS_EFI_UNSUPPORTED;
        }
        else if(PathLength == PARTUUID_STRING_LENGTH)
        {
            /* This is MBR UUID */
            BlDebugPrint(L"WARNING: MBR partition UUID in system path is not supported\n");
            return STATUS_EFI_UNSUPPORTED;
        }
        else
        {
            /* Invalid UUID format */
            return STATUS_EFI_INVALID_PARAMETER;
        }
    }
    else
    {
        /* Defaults to ARC path, dissect it */
        Status = BlpDissectVolumeArcPath(SystemPath, ArcName, Path, &DriveType, &DriveNumber, &PartNumber);
    }

    /* Check if volume path parsed successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to parse system path */
        BlDebugPrint(L"ERROR: Failed to parse system path: '%s' (Status Code: 0x%zX)\n", SystemPath, Status);
        return Status;
    }

    /* Look for block device corresponding to dissected ARC path */
    ListEntry = EfiBlockDevices.Flink;
    while(ListEntry != &EfiBlockDevices)
    {
        /* Check if this is the volume we are looking for */
        Device = CONTAIN_RECORD(ListEntry, EFI_BLOCK_DEVICE, ListEntry);
        if(DriveType == XTBL_BOOT_DEVICE_ESP)
        {
            /* ESP requested, verify if flag is set for this device */
            if((Device->DriveType & XTBL_BOOT_DEVICE_ESP) != 0)
            {
                /* Found volume */
                *DevicePath = Device->DevicePath;
                break;
            }
        }
        else
        {
            if(((Device->DriveType & DriveType) == DriveType) &&
               (Device->DriveNumber == DriveNumber) &&
               (Device->PartitionNumber == PartNumber))
            {
                /* Found volume */
                *DevicePath = Device->DevicePath;
                break;
            }
        }
        ListEntry = ListEntry->Flink;
    }

    /* Check if volume was found */
    if(*DevicePath == NULLPTR)
    {
        /* Volume not found */
        BlDebugPrint(L"ERROR: Volume (DriveType: %u, DriveNumber: %lu, PartNumber: %lu) not found\n",
                     DriveType, DriveNumber, PartNumber);
        return STATUS_EFI_NOT_FOUND;
    }

    /* return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine opens an EFI volume and corresponding filesystem.
 *
 * @param DevicePath
 *        Specifies a device path of the volume to open. If not specifies, uses image protocol by default.
 *
 * @param DiskHandle
 *        The handle of the opened disk volume.
 *
 * @param FsHandle
 *        The handle of the opened file system.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlOpenVolume(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
             OUT PEFI_HANDLE DiskHandle,
             OUT PEFI_FILE_HANDLE *FsHandle)
{
    EFI_GUID SFSGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL FileSystemProtocol;
    PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol;
    EFI_STATUS Status;

    /* Check if device path has been passed or not */
    if(DevicePath != NULLPTR)
    {
        /* Locate the device path */
        Status = EfiSystemTable->BootServices->LocateDevicePath(&SFSGuid, &DevicePath, DiskHandle);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to locate device path */
            return Status;
        }
    }
    else
    {
        /* Open the image protocol if no device path specified */
        Status = EfiSystemTable->BootServices->OpenProtocol(EfiImageHandle, &LIPGuid, (PVOID *)&ImageProtocol,
                                                            EfiImageHandle, NULLPTR, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to open image protocol */
            return Status;
        }

        /* Store disk handle */
        *DiskHandle = ImageProtocol->DeviceHandle;
    }

    /* Open the filesystem protocol */
    Status = EfiSystemTable->BootServices->OpenProtocol(*DiskHandle, &SFSGuid, (PVOID *)&FileSystemProtocol,
                                                        EfiImageHandle, NULLPTR, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    /* Check if filesystem protocol opened successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open the filesystem protocol, close volume */
        BlCloseVolume(DiskHandle);
        return Status;
    }

    /* Open the corresponding filesystem */
    Status = FileSystemProtocol->OpenVolume(FileSystemProtocol, FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open the filesystem, close volume */
        BlCloseVolume(DiskHandle);
        return Status;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Reads data from the file.
 *
 * @param DirHandle
 *        Supplies a handle of the opened filesystem directory.
 *
 * @param FileName
 *        Supplies the name of the file to read.
 *
 * @param FileData
 *        Provides a buffer to store the data read from the file.
 *
 * @param FileSize
 *        Provides a pointer to the variable to store a size of the buffer.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlReadFile(IN PEFI_FILE_HANDLE DirHandle,
           IN PCWSTR FileName,
           OUT PVOID *FileData,
           OUT PSIZE_T FileSize)
{
    EFI_GUID FileInfoGuid = EFI_FILE_INFO_PROTOCOL_GUID;
    EFI_PHYSICAL_ADDRESS Address;
    PEFI_FILE_HANDLE FileHandle;
    PEFI_FILE_INFO FileInfo;
    EFI_STATUS Status;
    UINT_PTR ReadSize;
    SIZE_T Pages;

    Status = DirHandle->Open(DirHandle, &FileHandle, (PWCHAR)FileName, EFI_FILE_MODE_READ,
                             EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open file */
        return Status;
    }

    /* Set required size for getting file information */
    ReadSize = sizeof(EFI_FILE_INFO) + 32;

    /* Allocate necessary amount of memory */
    Status = BlAllocateMemoryPool(ReadSize, (PVOID *)&FileInfo);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        FileHandle->Close(FileHandle);
        return Status;
    }

    /* First attempt to get file information */
    FileHandle->GetInfo(FileHandle, &FileInfoGuid, &ReadSize, FileInfo);
    if(Status == STATUS_EFI_BUFFER_TOO_SMALL)
    {
        /* Buffer is too small, but EFI tells the required size, so reallocate */
        BlFreeMemoryPool(&FileInfo);
        Status = BlAllocateMemoryPool(ReadSize, (PVOID *)&FileInfo);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            FileHandle->Close(FileHandle);
            return Status;
        }

        /* Second attempt to get file information */
        Status = FileHandle->GetInfo(FileHandle, &FileInfoGuid, &ReadSize, FileInfo);
    }

    /* Check if file information got successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to get file information */
        FileHandle->Close(FileHandle);
        BlFreeMemoryPool(&FileInfo);
        return Status;
    }

    /* Store file size and calculate number of pages */
    *FileSize = FileInfo->FileSize;
    Pages = EFI_SIZE_TO_PAGES(FileInfo->FileSize);

    /* Allocate pages */
    Status = BlAllocateMemoryPages(AllocateAnyPages, Pages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Pages allocation failure */
        FileHandle->Close(FileHandle);
        BlFreeMemoryPool(&FileInfo);
        return Status;
    }

    /* Calculate number of bytes to read and zero memory */
    ReadSize = Pages * EFI_PAGE_SIZE;
    *FileData = (PCHAR)(UINT_PTR)Address;
    RtlZeroMemory(*FileData, ReadSize);

    /* Read data from the file */
    Status = FileHandle->Read(FileHandle, &ReadSize, *FileData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to read data */
        FileHandle->Close(FileHandle);
        BlFreeMemoryPool(&FileInfo);
        BlFreeMemoryPages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)*FileData);
        return Status;
    }

    /* Close handle and free memory */
    FileHandle->Close(FileHandle);
    BlFreeMemoryPool(FileInfo);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Gets a list of block devices from an EFI enabled BIOS.
 *
 * @param BlockDevices
 *        Supplies a pointer to a variable to receive a list of EFI block devices.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpDiscoverEfiBlockDevices(OUT PLIST_ENTRY BlockDevices)
{
    EFI_GUID DevicePathGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;
    EFI_GUID IoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    PEFI_DEVICE_PATH_PROTOCOL DevicePath;
    PEFI_BLOCK_DEVICE_DATA BlockDevice;
    UINT_PTR HandlesCount, Index;
    PEFI_HANDLE Handles = NULLPTR;
    PEFI_BLOCK_IO_PROTOCOL Io;
    EFI_STATUS Status;

    /* Locate handles which support the disk I/O interface */
    Status = BlLocateProtocolHandles(&Handles, &HandlesCount, &IoGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to locate handles */
        BlDebugPrint(L"ERROR: Failed to locate block devices handles (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Iterate through all handles */
    for(Index = 0; Index < HandlesCount; Index++)
    {
        /* Print debug message */
        BlDebugPrint(L"Opening %lu block device from %lu discovered\n", Index + 1, HandlesCount);

        /* Open I/O protocol for given handle */
        Io = NULLPTR;
        Status = BlOpenProtocolHandle(Handles[Index], (PVOID *)&Io, &IoGuid);
        if(Status != STATUS_EFI_SUCCESS || Io == NULLPTR)
        {
            /* Failed to open I/O protocol, skip it */
            BlDebugPrint(L"WARNING: Failed to open EFI Block I/O protocol (Status Code: 0x%zX)\n", Status);
            continue;
        }

        /* Check if this is iPXE stub */
        if(Io->Media && Io->Media->BlockSize == 1 && Io->Media->MediaId == 0x69505845U)
        {
            /* Skip stub as it is non-functional */
            BlDebugPrint(L"WARNING: Skipping iPXE stub block I/O protocol");
            continue;
        }

        /* Check if DevicePath protocol is supported by this handle */
        DevicePath = NULLPTR;
        Status = EfiSystemTable->BootServices->HandleProtocol(Handles[Index], &DevicePathGuid, (PVOID *)&DevicePath);
        if(Status != STATUS_EFI_SUCCESS || DevicePath == NULLPTR)
        {
            /* Device failed to handle DP protocol */
            BlDebugPrint(L"WARNING: Unable to open DevicePath protocol (Status Code: 0x%zX)\n", Status);
            EfiSystemTable->BootServices->CloseProtocol(Handles[Index], &IoGuid, EfiImageHandle, NULLPTR);
            continue;
        }

        /* Allocate memory for block device */
        Status = BlAllocateMemoryPool(sizeof(*BlockDevice), (PVOID *)&BlockDevice);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            BlDebugPrint(L"ERROR: Failed to allocate memory pool for block device (Status Code: 0x%zX)\n", Status);
            EfiSystemTable->BootServices->CloseProtocol(Handles[Index], &DevicePathGuid, EfiImageHandle, NULLPTR);
            EfiSystemTable->BootServices->CloseProtocol(Handles[Index], &IoGuid, EfiImageHandle, NULLPTR);
            return Status;
        }

        /* Store new block device into a linked list */
        BlockDevice->BlockIo = Io;
        BlockDevice->DevicePath = DevicePath;
        RtlInsertTailList(BlockDevices, &BlockDevice->ListEntry);
    }

    /* Free handles buffer */
    BlFreeMemoryPool(Handles);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Dissects a specified ARC name and provides detailed information about corresponding device and on disk path.
 *
 * @param SystemPath
 *        Supplies an input ARC path.
 *
 * @param Path
 *        Specifies a pointer to variable, where on disk path will be saved.
 *
 * @param DriveType
 *        Supplies a pointer to the variable that receives a drive type.
 *
 * @param DriveNumber
 *        Supplies a pointer to the variable that receives a drive number.
 *
 * @param PartNumber
 *        Supplies a pointer to the variable that receives a parition number if applicable, otherwise stores 0 (zero).
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpDissectVolumeArcPath(IN PWCHAR SystemPath,
                        OUT PWCHAR *ArcName,
                        OUT PWCHAR *Path,
                        OUT PUSHORT DriveType,
                        OUT PULONG DriveNumber,
                        OUT PULONG PartNumber)
{
    PWCHAR ArcPath, LocalArcName;
    ULONG ArcLength = 0;

    /* Set default values */
    *DriveType = XTBL_BOOT_DEVICE_UNKNOWN;
    *DriveNumber = 0;
    *PartNumber = 0;

    /* Look for the ARC path */
    if(RtlCompareWideStringInsensitive(SystemPath, L"ramdisk(0)", 0) == 0)
    {
        /* This is RAM disk */
        ArcLength = 10;
        *DriveType = XTBL_BOOT_DEVICE_RAMDISK;
    }
    else if(RtlCompareWideStringInsensitive(SystemPath, L"multi(0)esp(0)", 0) == 0)
    {
        /* This is ESP */
        ArcLength = 14;
        *DriveType = XTBL_BOOT_DEVICE_ESP;
    }
    else if(RtlCompareWideStringInsensitive(SystemPath, L"multi(0)disk(0)", 0) == 0)
    {
        /* This is a multi-disk port */
        ArcLength = 15;
        ArcPath = SystemPath + ArcLength;

        /* Check for disk type */
        if(RtlCompareWideStringInsensitive(ArcPath, L"cdrom(", 0) == 0)
        {
            /* This is an optical drive */
            ArcLength += 6;

            /* Find drive number */
            while(SystemPath[ArcLength] != ')' && SystemPath[ArcLength] != '\0')
            {
                if(SystemPath[ArcLength] >= '0' && SystemPath[ArcLength] <= '9')
                {
                    /* Calculate drive number */
                    *DriveNumber *= 10;
                    *DriveNumber += SystemPath[ArcLength] - '0';
                }
                ArcLength++;
            }

            /* Set proper drive type */
            *DriveType = XTBL_BOOT_DEVICE_CDROM;
            ArcLength++;
        }
        else if(RtlCompareWideStringInsensitive(ArcPath, L"fdisk(", 0) == 0)
        {
            /* This is a floppy drive */
            ArcLength += 6;

            /* Find drive number */
            while(SystemPath[ArcLength] != ')' && SystemPath[ArcLength] != '\0')
            {
                if(SystemPath[ArcLength] >= '0' && SystemPath[ArcLength] <= '9')
                {
                    /* Calculate drive number */
                    *DriveNumber *= 10;
                    *DriveNumber += SystemPath[ArcLength] - '0';
                }
                ArcLength++;
            }

            /* Set proper drive type */
            *DriveType = XTBL_BOOT_DEVICE_FLOPPY;
            ArcLength++;
        }
        else if(RtlCompareWideStringInsensitive(ArcPath, L"rdisk(", 0) == 0)
        {
            /* This is a hard disk */
            ArcLength += 6;

            /* Find drive number */
            while(SystemPath[ArcLength] != ')' && SystemPath[ArcLength] != '\0')
            {
                if(SystemPath[ArcLength] >= '0' && SystemPath[ArcLength] <= '9')
                {
                    /* Calculate drive number */
                    *DriveNumber *= 10;
                    *DriveNumber += SystemPath[ArcLength] - '0';
                }
                ArcLength++;
            }

            /* Set proper drive type */
            *DriveType = XTBL_BOOT_DEVICE_HARDDISK;
            ArcLength++;
            ArcPath = SystemPath + ArcLength;

            /* Look for a partition */
            if(RtlCompareWideStringInsensitive(ArcPath, L"partition(", 0) == 0)
            {
                /* Partition information found */
                ArcLength += 10;

                /* Find partition number */
                while(SystemPath[ArcLength] != ')' && SystemPath[ArcLength] != '\0')
                {
                    if(SystemPath[ArcLength] >= '0' && SystemPath[ArcLength] <= '9')
                    {
                        /* Calculate partition number */
                        *PartNumber *= 10;
                        *PartNumber += SystemPath[ArcLength] - '0';
                    }
                    ArcLength++;
                }
                ArcLength++;
            }
        }
        else
        {
            /* Unsupported disk type */
            return STATUS_EFI_UNSUPPORTED;
        }
    }
    else
    {
        /* Unsupported ARC path */
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Store the path if possible */
    if(Path)
    {
        *Path = SystemPath + ArcLength;
    }

    /* Store ARC name if possible */
    if(ArcName)
    {
        BlAllocateMemoryPool(ArcLength * sizeof(WCHAR), (PVOID *)&LocalArcName);
        RtlCopyMemory(LocalArcName, SystemPath, ArcLength * sizeof(WCHAR));
        LocalArcName[ArcLength] = '\0';
        *ArcName = LocalArcName;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine duplicates a device path object.
 *
 * @param DevicePath
 *        An input device path that is going to be clonned.
 *
 * @return Returns a duplicate of input device path.
 *
 * @since XT 1.0
 */
XTCDECL
PEFI_DEVICE_PATH_PROTOCOL
BlpDuplicateDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath)
{
    PEFI_DEVICE_PATH_PROTOCOL DevicePathNode;
    PEFI_DEVICE_PATH_PROTOCOL DevicePathClone;
    EFI_STATUS Status;
    UINT Length = 0;

    /* Check if the input device path is NULL pointer */
    if(!DevicePath)
    {
        /* Nothing to duplicate */
        return NULLPTR;
    }

    /* Start iterating from the beginning of the device path */
    DevicePathNode = DevicePath;

    /* Get the device path length */
    while(TRUE)
    {
        Length += *(PUSHORT)DevicePathNode->Length;
        if(DevicePathNode->Type == EFI_END_DEVICE_PATH)
        {
            break;
        }
        DevicePathNode = (PEFI_DEVICE_PATH_PROTOCOL)((PUCHAR)DevicePathNode + *(PUSHORT)DevicePathNode->Length);
    }

    /* Check length */
    if(Length == 0)
    {
        /* Nothing to duplicate */
        return NULLPTR;
    }

    /* Allocate memory for the new device path */
    Status = BlAllocateMemoryPool(Length, (PVOID *)&DevicePathClone);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to allocate memory */
        BlDebugPrint(L"ERROR: Failed to allocate memory pool for device path duplicate (Status Code: 0x%zX)\n", Status);
        return NULLPTR;
    }

    /* Copy the device path */
    RtlCopyMemory(DevicePathClone, DevicePath, Length);

    /* Return the cloned object */
    return DevicePathClone;
}

/**
 * Attempts to find a last node of the EFI block device.
 *
 * @param DevicePath
 *        An input device path.
 *
 * @param LastNode
 *        A pointer to the buffer where last node will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpFindLastBlockDeviceNode(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                           OUT PEFI_DEVICE_PATH_PROTOCOL *LastNode)
{
    PEFI_DEVICE_PATH_PROTOCOL EndNode, NextNode;

    /* Make sure end is not reached yet */
    if(DevicePath->Type == EFI_END_DEVICE_PATH)
    {
        /* End reached, nothing to do */
        LastNode = NULLPTR;
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

/**
 * This routine attempts to find a parent device of the provided block device.
 *
 * @param BlockDevice
 *        A linked list of discovered block devices.
 *
 * @param ChildNode
 *        Block device that is looking for a parent device.
 *
 * @param ParentNode
 *        A pointer to memory region where pointer to the parent node will be provided.
 *
 * @return This routine returns TRUE if parent node has been found, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
BlpFindParentBlockDevice(IN PLIST_ENTRY BlockDevices,
                         IN PEFI_BLOCK_DEVICE_DATA ChildNode,
                         OUT PEFI_BLOCK_DEVICE_DATA *ParentNode)
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

        /* A device cannot be its own parent */
        if (BlockDeviceData == ChildNode)
        {
            /* Move to the next device */
            ListEntry = ListEntry->Flink;
            continue;
        }

        ChildDevicePath = ChildNode->DevicePath;
        ParentDevicePath = BlockDeviceData->DevicePath;

        /* Iterate nodes */
        while(TRUE)
        {
            /* Check if the parent device is a match */
            if(ParentDevicePath->Type == EFI_END_DEVICE_PATH)
            {
                /* Parent device is a match */
                *ParentNode = BlockDeviceData;
                return TRUE;
            }

            /* Get child and parent node lengths */
            ChildLength = *(PUSHORT)ChildDevicePath->Length;
            ParentLength = *(PUSHORT)ParentDevicePath->Length;

            /* Check if nodes match */
            if((ChildLength != ParentLength) ||
               (RtlCompareMemory(ChildDevicePath, ParentDevicePath, ParentLength) != ParentLength))
            {
                /* Nodes do not match, this is not a valid parent */
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
