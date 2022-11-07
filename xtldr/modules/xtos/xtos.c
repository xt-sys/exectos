/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/xtos.c
 * DESCRIPTION:     XTOS boot protocol support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI XT Loader Protocol */
PXT_BOOT_LOADER_PROTOCOL XtLdrProtocol;

/* XTOS PE/COFF Image Protocol */
PXT_PECOFF_IMAGE_PROTOCOL XtPeCoffProtocol;

/* XTOS Boot Protocol */
XT_BOOT_PROTOCOL XtBootProtocol;

/**
 * Starts the operating system according to the provided parameters using XTOS boot protocol.
 *
 * @param Parameters
 *        Input parameters with detailed system configuration like boot device or kernel path.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
XtBootSystem(IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters)
{
    EFI_GUID PeCoffProtocolGuid = XT_PECOFF_IMAGE_PROTOCOL_GUID;
    EFI_HANDLE DiskHandle;
    PEFI_FILE_HANDLE FsHandle, BootDir;
    PWCHAR SystemPath;
    EFI_STATUS Status;

    /* Print debug message */
    XtLdrProtocol->DbgPrint(L"XTOS boot protocol activated\n");

    /* Open the XT PE/COFF protocol */
    Status = BlLoadXtProtocol((PVOID *)&XtPeCoffProtocol, &PeCoffProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        XtLdrProtocol->DbgPrint(L"ERROR: Unable to load PE/COFF image protocol\n");
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Check device path */
    if(Parameters->DevicePath == NULL)
    {
        /* No device path set */
        XtLdrProtocol->DbgPrint(L"ERROR: No device path provided, unable to boot system\n");
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Check if system path is set */
    if(Parameters->SystemPath != NULL)
    {
        /* Make sure system path begins with backslash, the only separator supported by EFI */
        if(Parameters->SystemPath[0] == '/')
        {
            /* Replace directory separator if needed */
            Parameters->SystemPath[0] = '\\';
        }

        /* Validate system path */
        SystemPath = &Parameters->SystemPath[1];
        while(*SystemPath)
        {
            /* Make sure it does not point to any subdirectory and not contains special characters */
            if(((*SystemPath | 32) - 'a' >= 26) && ((*SystemPath - '0') >= 10))
            {
                /* Invalid path specified */
                XtLdrProtocol->DbgPrint(L"ERROR: System path does not point to the valid XTOS installation\n");
                return STATUS_EFI_INVALID_PARAMETER;
            }
            /* Check next character in the path */
            SystemPath++;
        }
    }
    else
    {
        /* Fallback to /ExectOS by default */
        XtLdrProtocol->DbgPrint(L"WARNING: No system path set, falling back to defaults\n");
        Parameters->SystemPath = L"\\ExectOS";
    }

    if(Parameters->KernelFile == NULL)
    {
        /* No kernel filename set, fallback to default */
        XtLdrProtocol->DbgPrint(L"WARNING: No kernel file specified, falling back to defaults\n");
        Parameters->KernelFile = L"xtoskrnl.exe";
    }

    /* Print a debug message */
    XtLdrProtocol->DbgPrint(L"ARC Path: %S\nSystem Path: %S\nKernel File: %S\nBoot Arguments: %S\n",
                               Parameters->ArcName, Parameters->SystemPath,
                               Parameters->KernelFile, Parameters->Arguments);

    /* Open EFI volume */
    Status = XtLdrProtocol->OpenVolume(NULL, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        XtLdrProtocol->DbgPrint(L"ERROR: Unable to open boot volume\n");
        return Status;
    }

    /* System path has to point to the boot directory */
    RtlWideStringConcatenate(Parameters->SystemPath, L"\\Boot", 0);

    /* Open XTOS system boot directory */
    Status = FsHandle->Open(FsHandle, &BootDir, Parameters->SystemPath, EFI_FILE_MODE_READ, 0);
    FsHandle->Close(FsHandle);

    /* Check if system path directory opened successfully */
    if(Status == STATUS_EFI_NOT_FOUND)
    {
        /* Directory not found, nothing to load */
        XtLdrProtocol->DbgPrint(L"ERROR: System boot directory not found\n");

        /* Close volume */
        XtLdrProtocol->CloseVolume(DiskHandle);
        return Status;
    }
    else if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        XtLdrProtocol->DbgPrint(L"ERROR: Unable to open system boot directory\n");
        XtLdrProtocol->CloseVolume(DiskHandle);
        return Status;
    }

    /* Start boot sequence */
    return XtpBootSequence(BootDir, Parameters);
}

/**
 * This routine initiates an XTOS boot sequence.
 *
 * @param Parameters
 *        Input parameters with detailed system configuration like boot device or kernel path.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
XtpBootSequence(IN PEFI_FILE_HANDLE BootDir,
                IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters)
{
    PEFI_FILE_HANDLE KernelHandle;
    EFI_STATUS Status;

    XtLdrProtocol->DbgPrint(L"Issuing XT startup sequence\n");

    /* Open kernel file */
    Status = BootDir->Open(BootDir, &KernelHandle, Parameters->KernelFile, EFI_FILE_MODE_READ, 0);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to open the file */
        XtLdrProtocol->DbgPrint(L"ERROR: Failed to open the XTOS kernel\n");
        return Status;
    }

    /* Load the PE/COFF kernel file */
    Status = XtPeCoffProtocol->Load(KernelHandle, (PVOID)XTOS_KERNEL_ADDRESS, NULL);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to load the file */
        XtLdrProtocol->DbgPrint(L"ERROR: Failed to load the XTOS kernel\n");
        return Status;
    }

    /* Close kernel file */
    KernelHandle->Close(KernelHandle);

    /* Return success */
    return STATUS_EFI_SUCCESS;
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
EFI_STATUS
BlXtLdrModuleMain(EFI_HANDLE ImageHandle,
                  PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_XTOS_BOOT_PROTOCOL_GUID;
    EFI_HANDLE Handle = NULL;
    EFI_STATUS Status;

    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Open the XTLDR protocol */
    Status = BlGetXtLoaderProtocol(&XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set routines available via XTOS boot protocol */
    XtBootProtocol.BootSystem = XtBootSystem;

    /* Register XTOS boot protocol */
    return EfiSystemTable->BootServices->InstallProtocolInterface(&Handle, &Guid, EFI_NATIVE_INTERFACE, &XtBootProtocol);
}
