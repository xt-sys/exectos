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

/* XT Boot Loader protocol */
XT_BOOT_LOADER_PROTOCOL EfiLdrProtocol;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI Secure Boot status */
INT_PTR EfiSecureBoot;

/* Serial port configuration */
CPPORT EfiSerialPort;

/**
 * This routine loads XTLDR EFI modules.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlLoadEfiModules()
{
    CONST PWCHAR ModulesDirPath = L"\\EFI\\BOOT\\XTLDR\\";
    EFI_GUID DevicePathGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_DEVICE_PATH_PROTOCOL VolumeDevicePath, DevicePath;
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    PEFI_FILE_HANDLE FsHandle, ModulesDir;
    EFI_HANDLE DiskHandle, ModuleHandle;
    PEFI_HANDLE Handles;
    SIZE_T Length;
    EFI_STATUS Status;
    UINT_PTR DirSize, ModulesCount;
    CHAR Buffer[1024];
    WCHAR ModulePath[1024];
    PWCHAR ModuleName;
    UINT Index;

    /* Open EFI volume */
    Status = BlOpenVolume(NULL, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        return Status;
    }

    /* Open EFI/BOOT/XTLDR directory, which contains all the modules and close the FS immediately */
    Status = FsHandle->Open(FsHandle, &ModulesDir, ModulesDirPath, EFI_FILE_MODE_READ, 0);
    FsHandle->Close(FsHandle);

    /* Check if modules directory opened successfully */
    if(Status == STATUS_EFI_NOT_FOUND)
    {
        /* Directory not found, nothing to load */
        BlDbgPrint(L"WARNING: Boot loader directory (EFI/BOOT/XTLDR) not found\n");

        /* Close volume */
        BlCloseVolume(DiskHandle);
        return STATUS_EFI_SUCCESS;
    }
    else if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        BlDbgPrint(L"ERROR: Unable to open XTLDR directory (EFI/BOOT/XTLDR)\n");
        BlCloseVolume(DiskHandle);
        return Status;
    }

    /* Open EFI device path protocol */
    Status = EfiSystemTable->BootServices->HandleProtocol(DiskHandle, &DevicePathGuid, (PVOID *)&DevicePath);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Close volume */
        BlCloseVolume(DiskHandle);
        return Status;
    }

    /* Iterate through files inside XTLDR directory */
    while(TRUE)
    {
        /* Read directory */
        DirSize = sizeof(Buffer);
        Status = ModulesDir->Read(ModulesDir, &DirSize, Buffer);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to read directory */
            BlDbgPrint(L"\n");

            /* Close directory and volume */
            ModulesDir->Close(ModulesDir);
            BlCloseVolume(DiskHandle);
            return Status;
        }

        /* Check if read anything */
        if(DirSize == 0)
        {
            /* Already read all contents, break loop execution */
            break;
        }

        /* Take filename and its length */
        ModuleName = ((PEFI_FILE_INFO)Buffer)->FileName;
        Length = RtlWideStringLength(ModuleName, 0);

        /* Make sure we deal with .EFI executable file */
        if(Length < 4 || ModuleName[Length - 4] != '.' ||
           (ModuleName[Length - 3] != 'E' && ModuleName[Length - 3] != 'e') ||
           (ModuleName[Length - 2] != 'F' && ModuleName[Length - 2] != 'f') ||
           (ModuleName[Length - 1] != 'I' && ModuleName[Length - 1] != 'i'))
        {
            /* Skip non .EFI file */
            continue;
        }

        /* Print debug message */
        BlDbgPrint(L"Loading module '%S' ... ", ModuleName);

        /* Set correct path to the module file */
        RtlCopyMemory(ModulePath, ModulesDirPath, sizeof(ModulePath) / sizeof(WCHAR));
        RtlWideStringConcatenate(ModulePath, ModuleName, 0);

        /* Find valid device path */
        Status = BlFindVolumeDevicePath(DevicePath, ModulePath, &VolumeDevicePath);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to set path */
            BlDbgPrint(L"FAIL\n");
            BlDbgPrint(L"ERROR: Unable to set valid device path\n");

            /* Close directory and volume */
            ModulesDir->Close(ModulesDir);
            BlCloseVolume(DiskHandle);
            return Status;
        }

        /* Load the module into memory */
        Status = EfiSystemTable->BootServices->LoadImage(FALSE, EfiImageHandle, VolumeDevicePath,
                                                         NULL, 0, &ModuleHandle);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Module failed */
            BlDbgPrint(L"FAIL\n");

            /* Check if caused by secure boot */
            if(Status == STATUS_EFI_ACCESS_DENIED && EfiSecureBoot >= 1)
            {
                BlDbgPrint(L"ERROR: SecureBoot signature validation failed\n");
            }
            else
            {
                BlDbgPrint(L"ERROR: Unable to load module\n");
            }

            /* Free memory and skip module */
            BlEfiMemoryFreePool(VolumeDevicePath);
            continue;
        }

        /* Free memory */
        BlEfiMemoryFreePool(VolumeDevicePath);

        /* Access module interface for further module type check */
        Status = EfiSystemTable->BootServices->OpenProtocol(ModuleHandle, &LIPGuid, (PVOID *)&LoadedImage,
                                                            EfiImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to open protocol */
            BlDbgPrint(L"FAIL\n");
            BlDbgPrint(L"ERROR: Unable to access module interface\n");

            /* Skip to the next module */
            continue;
        }

        /* Some firmwares do not allow to start drivers which are not of 'boot system driver' type, so check it */
        if(LoadedImage->ImageCodeType != EfiBootServicesCode)
        {
            /* Different type set, probably 'runtime driver', refuse to load it */
            BlDbgPrint(L"FAIL\n");
            BlDbgPrint(L"ERROR: Loaded module is not a boot system driver\n");

            /* Close protocol and skip module */
            EfiSystemTable->BootServices->CloseProtocol(LoadedImage, &LIPGuid, LoadedImage, NULL);
            continue;
        }

        /* Close loaded image protocol */
        EfiSystemTable->BootServices->CloseProtocol(LoadedImage, &LIPGuid, LoadedImage, NULL);

        /* Start the module */
        Status = EfiSystemTable->BootServices->StartImage(ModuleHandle, NULL, NULL);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Module failed */
            BlDbgPrint(L"FAIL\n");
            BlDbgPrint(L"ERROR: Unable to start module\n");

            /* Skip module */
            continue;
        }

        /* Module loaded successfully */
        BlDbgPrint(L"OK\n");
    }

    /* Get list of all handles */
    Status = EfiSystemTable->BootServices->LocateHandleBuffer(AllHandles, NULL, NULL, &ModulesCount, &Handles);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get list of handles */
        BlDbgPrint(L"WARNING: Unable to get a list of handles, some modules might not work properly\n");
    }
    else
    {
        /* Iterate through a list of handles */
        BlDbgPrint(L"Starting services for %lu handles\n", ModulesCount);
        for(Index = 0; Index < ModulesCount; Index++)
        {
            /* Start services for all loaded modules */
            EfiSystemTable->BootServices->ConnectController(Handles[Index], NULL, NULL, TRUE);
        }
    }

    /* Free memory */
    BlEfiMemoryFreePool(Handles);

    /* Close directory and volume */
    ModulesDir->Close(ModulesDir);
    BlCloseVolume(DiskHandle);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine attempts to start XT Operating System.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS BlLoadXtSystem()
{
    EFI_GUID ProtocolGuid = XT_XTOS_BOOT_PROTOCOL_GUID;
    XT_BOOT_PROTOCOL_PARAMETERS BootParameters;
    PXT_BOOT_PROTOCOL BootProtocol;
    PUCHAR ArcName, SystemPath;
    EFI_STATUS Status;
    PCHAR ArcPath;
    SIZE_T Length;

    /* Set ARC path */
    ArcPath = "multi(0)disk(0)rdisk(0)partition(1)/ExectOS";

    /* Zero boot parameters structure to NULLify all pointers */
    RtlZeroMemory(&BootParameters, sizeof(XT_BOOT_PROTOCOL_PARAMETERS));

    /* Get boot volume path */
    Status = BlGetVolumeDevicePath((PUCHAR)ArcPath, &BootParameters.DevicePath, &ArcName, &SystemPath);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to find volume */
        BlDbgPrint(L"ERROR: Unable to find volume device path\n");
        return Status;
    }

    /* Store ARC name in boot parameters */
    Length = RtlStringLength(ArcName, 0);
    BlEfiMemoryAllocatePool(Length + 1, (PVOID *)&BootParameters.ArcName);
    RtlStringToWideString(BootParameters.ArcName, &ArcName, Length * 2);

    /* Store system path in boot parameters */
    Length = RtlStringLength(SystemPath, 0);
    BlEfiMemoryAllocatePool(Length + 1, (PVOID *)&BootParameters.SystemPath);
    RtlStringToWideString(BootParameters.SystemPath, &SystemPath, Length + 1);

    /* Open the XT boot protocol */
    Status = BlLoadXtProtocol((PVOID *)&BootProtocol, &ProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open boot protocol */
        BlDbgPrint(L"ERROR: Unable to load boot protocol\n");
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Boot operating system */
    return BootProtocol->BootSystem(&BootParameters);
}

/**
 * This routine registers XTLDR protocol for further usage by modules.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlRegisterXtLoaderProtocol()
{
    EFI_GUID Guid = XT_BOOT_LOADER_PROTOCOL_GUID;
    EFI_HANDLE Handle = NULL;

    /* Set all routines available via loader protocol */
    EfiLdrProtocol.AllocatePages = BlEfiMemoryAllocatePages;
    EfiLdrProtocol.AllocatePool = BlEfiMemoryAllocatePool;
    EfiLdrProtocol.FreePages = BlEfiMemoryFreePages;
    EfiLdrProtocol.FreePool = BlEfiMemoryFreePool;
    EfiLdrProtocol.DbgPrint = BlDbgPrint;
    EfiLdrProtocol.EfiPrint = BlEfiPrint;
    EfiLdrProtocol.CloseVolume = BlCloseVolume;
    EfiLdrProtocol.OpenVolume = BlOpenVolume;

    /* Register loader protocol */
    BlDbgPrint(L"Registering XT loader protocol\n");
    return EfiSystemTable->BootServices->InstallProtocolInterface(&Handle, &Guid, EFI_NATIVE_INTERFACE,
                                                                  &EfiLdrProtocol);
}

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
    BlEfiPrint(L"XTLDR boot loader v%s\n", XTOS_VERSION);

    /* Early initialize COM port for debugging */
    Status = BlComPortInitialize();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Initialization failed, try printing error to stdout and serial console */
        BlEfiPrint(L"ERROR: Failed to initialize serial console\n");
    }

    /* Check SecureBoot status */
    EfiSecureBoot = BlEfiGetSecureBootStatus();

    /* Print firmware information */
    BlDbgPrint(L"UEFI v%d.%d (%S 0x%08x), SecureBoot %S\n", EfiSystemTable->Hdr.Revision >> 16,
               EfiSystemTable->Hdr.Revision & 0xFFFF, EfiSystemTable->FirmwareVendor, EfiSystemTable->FirmwareRevision,
               EfiSecureBoot == 0 ? L"DISABLED" : EfiSecureBoot > 0 ? L"ENABLED" : L"SETUP");

    /* Disable watchdog timer */
    Status = EfiSystemTable->BootServices->SetWatchdogTimer(0, 0x10000, 0, NULL);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to disable the timer, print message */
        BlDbgPrint(L"WARNING: Failed to disable watchdog timer\n");
    }

    /* Register loader protocol */
    Status = BlRegisterXtLoaderProtocol();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to register loader protocol */
        BlDbgPrint(L"ERROR: Failed to register XTLDR loader protocol\n");
    }

    /* Load XTLDR modules */
    Status = BlLoadEfiModules();
    if(Status != STATUS_EFI_SUCCESS)
    {
        BlDbgPrint(L"ERROR: Failed to load XTLDR modules\n");
    }

    /* Discover and enumerate EFI block devices */
    BlEnumerateEfiBlockDevices();

    /* Boot XTOS */
    Status = BlLoadXtSystem();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Boot process failed */
        BlEfiPrint(L"Failed to start XT OS (Status code: %lx)!\n", Status);
    }

    /* Infinite bootloader loop */
    BlEfiPrint(L"System halted!");
    for(;;);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
