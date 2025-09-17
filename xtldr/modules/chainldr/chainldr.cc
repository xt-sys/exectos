/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/chainldr/chainldr.cc
 * DESCRIPTION:     XTLDR Chain Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <chainldr.hh>


/* ChainLoader module information */
MODULE_AUTHOR(L"Rafal Kupiec <belliash@codingworkshop.eu.org>");
MODULE_DESCRIPTION(L"XTLDR Chain Loader");
MODULE_LICENSE(L"GPLv3");
MODULE_VERSION(L"0.1");


/**
 * Chainloads another boot loader.
 *
 * @param Parameters
 *        Input parameters with detailed system configuration.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
ChainLoader::BootSystem(IN PXTBL_BOOT_PARAMETERS Parameters)
{
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_MEMMAP_DEVICE_PATH MemoryDevicePath[2];
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    EFI_HANDLE DiskHandle, LoaderHandle;
    PEFI_FILE_HANDLE FsHandle, BootDir;
    EFI_STATUS Status;
    SIZE_T LoaderSize;
    PVOID LoaderData;

    /* Check if image file is provided */
    if(Parameters->KernelFile == NULLPTR)
    {
        /* No image filename provided, return error code */
        XtLdrProtocol->Debug.Print(L"ERROR: No EFI image filename provided\n");
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Open EFI volume */
    Status = XtLdrProtocol->Disk.OpenVolume(Parameters->DevicePath, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume, return error code */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to open boot volume (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Open boot directory and close FS handle */
    Status = FsHandle->Open(FsHandle, &BootDir, Parameters->EfiPath, EFI_FILE_MODE_READ, 0);
    FsHandle->Close(FsHandle);

    /* Check if system path directory opened successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to open system boot directory (Status Code: 0x%zX)\n", Status);

        /* Close volume and return error code */
        XtLdrProtocol->Disk.CloseVolume(&DiskHandle);
        return Status;
    }

    /* Read EFI image file from disk and close both directory and EFI volume */
    Status = XtLdrProtocol->Disk.ReadFile(BootDir, Parameters->KernelFile, &LoaderData, &LoaderSize);
    BootDir->Close(BootDir);
    XtLdrProtocol->Disk.CloseVolume(&DiskHandle);

    /* Setup device path for EFI image */
    MemoryDevicePath[0].Header.Length[0] = sizeof(EFI_MEMMAP_DEVICE_PATH);
    MemoryDevicePath[0].Header.Length[1] = sizeof(EFI_MEMMAP_DEVICE_PATH) >> 8;
    MemoryDevicePath[0].Header.Type = EFI_HARDWARE_DEVICE_PATH;
    MemoryDevicePath[0].Header.SubType = EFI_HARDWARE_MEMMAP_DP;
    MemoryDevicePath[0].MemoryType = EfiLoaderData;
    MemoryDevicePath[0].StartingAddress = (UINT_PTR)LoaderData;
    MemoryDevicePath[0].EndingAddress = (UINT_PTR)LoaderData + LoaderSize;
    MemoryDevicePath[1].Header.Length[0] = sizeof(EFI_DEVICE_PATH_PROTOCOL);
    MemoryDevicePath[1].Header.Length[1] = sizeof(EFI_DEVICE_PATH_PROTOCOL) >> 8;
    MemoryDevicePath[1].Header.Type = EFI_END_DEVICE_PATH;
    MemoryDevicePath[1].Header.SubType = EFI_END_ENTIRE_DP;

    /* Load EFI image */
    Status = XtLdrProtocol->Util.LoadEfiImage((PEFI_DEVICE_PATH_PROTOCOL)MemoryDevicePath,
                                              LoaderData, LoaderSize, &LoaderHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to chainload EFI binary, return error code */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to chainload '%S' (Status Code: 0x%zX)\n",
                                   Parameters->KernelFile, Status);
        return Status;
    }

    /* Access loaded image protocol */
    Status = XtLdrProtocol->Protocol.OpenHandle(LoaderHandle, (PVOID *)&LoadedImage, &LIPGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open EFI_LOADED_IMAGE_PROTOCOL, return error code */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to access binary interface (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Check if parameters provided */
    if(Parameters->Parameters)
    {
        /* Pass arguments to chainloaded image */
        LoadedImage->LoadOptionsSize = RtlWideStringLength(Parameters->Parameters, 0) * sizeof(WCHAR);
        LoadedImage->LoadOptions = Parameters->Parameters;
    }

    /* Set device handle as LoadImage() is not going to do it */
    LoadedImage->DeviceHandle = DiskHandle;

    /* Chainload EFI image */
    return XtLdrProtocol->Util.StartEfiImage(LoaderHandle);
}

/**
 * Initializes CHAINLDR module by opening XTLDR protocol and installing CHAINLOADER protocol.
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
XTCDECL
EFI_STATUS
ChainLoader::InitializeModule(IN EFI_HANDLE ImageHandle,
                              IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_CHAIN_BOOT_PROTOCOL_GUID;
    EFI_STATUS Status;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open the protocol, return error */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set routines available via ChainLoader boot protocol */
    BootProtocol.BootSystem = BootSystem;

    /* Register XTOS boot protocol */
    XtLdrProtocol->Boot.RegisterProtocol(L"CHAINLOADER", &Guid);

    /* Install XTOS protocol */
    return XtLdrProtocol->Protocol.Install(&BootProtocol, &Guid);
}

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
XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    /* Initialize CHAINLDR module */
    return ChainLoader::InitializeModule(ImageHandle, SystemTable);
}
