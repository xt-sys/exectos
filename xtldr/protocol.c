/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/protocol.c
 * DESCRIPTION:     XT Boot Loader protocol support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Closes a protocol on a provided handle.
 *
 * @param Handle
 *        Supplies a handle for the protocol interface that was previously opened.
 *
 * @param ProtocolGuid
 *        Supplies a unique protocol GUID.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlCloseProtocol(IN PEFI_HANDLE Handle,
                IN PEFI_GUID ProtocolGuid)
{
    return EfiSystemTable->BootServices->CloseProtocol(Handle, ProtocolGuid, EfiImageHandle, NULL);
}

/**
 * Finds a boot protocol for specified system type.
 *
 * @param SystemType
 *        Specifies the system type to search for.
 *
 * @param BootProtocolGuid
 *        Receives the GUID of the registered boot protocol, that supports specified system.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlFindBootProtocol(IN PWCHAR SystemType,
                   OUT PEFI_GUID BootProtocolGuid)
{
    PXTBL_KNOWN_BOOT_PROTOCOL ProtocolEntry;
    PLIST_ENTRY ProtocolListEntry;

    ProtocolListEntry = BlpBootProtocols.Flink;
    while(ProtocolListEntry != &BlpBootProtocols)
    {
        /* Get boot protocol entry */
        ProtocolEntry = CONTAIN_RECORD(ProtocolListEntry, XTBL_KNOWN_BOOT_PROTOCOL, Flink);

        /* Check if this boot protocol supports specified system */
        if(RtlCompareWideStringInsensitive(ProtocolEntry->SystemType, SystemType, 0) == 0)
        {
            /* Boot protocol matched, return success */
            *BootProtocolGuid = ProtocolEntry->Guid;
            return STATUS_EFI_SUCCESS;
        }

        /* Move to the next registered boot protocol */
        ProtocolListEntry = ProtocolListEntry->Flink;
    }

    /* Boot protocol not found, return error */
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Returns a linked list of all loaded modules.
 *
 * @return This routine returns a pointer to a linked list of all loaded modules.
 *
 * @since XT 1.0
 *
 * @todo This is a temporary solution and it should be replaced by a complex API allowing to map modules.
 */
XTCDECL
PLIST_ENTRY
BlGetModulesList()
{
    /* Return a pointer to a list of all loaded modules */
    return &BlpLoadedModules;
}

/**
 * Installs XTLDR protocol interface.
 *
 * @param Guid
 *        Specifies a unique protocol GUID.
 *
 * @param Interface
 *        Supplies a pointer to the protocol interface, or NULL if there is no structure associated.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlInstallProtocol(IN PVOID Interface,
                  IN PEFI_GUID Guid)
{
    EFI_HANDLE Handle = NULL;

    /* Install protocol interface */
    return EfiSystemTable->BootServices->InstallProtocolInterface(&Handle, Guid, EFI_NATIVE_INTERFACE, Interface);
}

/**
 * Loads a specified XTLDR module from disk.
 *
 * @param ModuleName
 *        Specifies the name of the module to load.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlLoadModule(IN PWCHAR ModuleName)
{
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_MEMMAP_DEVICE_PATH ModuleDevicePath[2];
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    PEFI_FILE_HANDLE DirHandle, FsHandle;
    EFI_HANDLE DiskHandle, ModuleHandle;
    PPECOFF_IMAGE_SECTION_HEADER SectionHeader;
    PPECOFF_IMAGE_DOS_HEADER DosHeader;
    PPECOFF_IMAGE_PE_HEADER PeHeader;
    PXTBL_MODULE_DEPS ModuleDependencies;
    PXTBL_MODULE_INFO ModuleInfo;
    PLIST_ENTRY ModuleListEntry;
    WCHAR ModuleFileName[24];
    USHORT SectionIndex;
    SIZE_T ModuleSize;
    EFI_STATUS Status;
    PVOID ModuleData;
    PWCHAR DepsData;

    ModuleListEntry = BlpLoadedModules.Flink;
    while(ModuleListEntry != &BlpLoadedModules)
    {
        /* Get module information */
        ModuleInfo = CONTAIN_RECORD(ModuleListEntry, XTBL_MODULE_INFO, Flink);

        if(RtlCompareWideStringInsensitive(ModuleInfo->ModuleName, ModuleName, 0) == 0)
        {
            /* Module already loaded */
            BlDebugPrint(L"WARNING: Module '%S' already loaded!\n", ModuleName);
            return STATUS_EFI_SUCCESS;
        }

        /* Move to the module */
        ModuleListEntry = ModuleListEntry->Flink;
    }

    /* Print debug message */
    BlDebugPrint(L"Loading module '%S' ...\n", ModuleName);

    /* Set module path */
    RtlCopyMemory(ModuleFileName, ModuleName, sizeof(ModuleFileName) / sizeof(WCHAR));
    RtlConcatenateWideString(ModuleFileName, L".EFI", 0);

    /* Open EFI volume */
    Status = BlOpenVolume(NULL, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        return Status;
    }

    /* Open XTLDR modules common directory */
    Status = FsHandle->Open(FsHandle, &DirHandle, XTBL_MODULES_DIRECTORY_PATH, EFI_FILE_MODE_READ, 0);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Modules directory not found, attempt to open XTLDR architecture specific modules directory */
        Status = FsHandle->Open(FsHandle, &DirHandle, XTBL_ARCH_MODULES_DIRECTORY_PATH, EFI_FILE_MODE_READ, 0);
    }

    /* Close FS handle */
    FsHandle->Close(FsHandle);

    /* Check if modules directory opened successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        BlCloseVolume(DiskHandle);
        return Status;
    }

    /* Read module file from disk and close directory and EFI volume */
    Status = BlReadFile(DirHandle, ModuleFileName, &ModuleData, &ModuleSize);
    DirHandle->Close(DirHandle);
    BlCloseVolume(DiskHandle);

    /* Make sure module file was read successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to read file */
        return Status;
    }

    /* Allocate memory for module information block */
    Status = BlAllocateMemoryPool(sizeof(XTBL_MODULE_INFO), (PVOID*)&ModuleInfo);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to allocate memory */
        return Status;
    }

    /* Zero module information block and initialize dependencies list */
    RtlZeroMemory(ModuleInfo, sizeof(XTBL_MODULE_INFO));
    RtlInitializeListHead(&ModuleInfo->Dependencies);

    /* Setup PE/COFF EFI image headers */
    DosHeader = (PPECOFF_IMAGE_DOS_HEADER)ModuleData;
    PeHeader = (PPECOFF_IMAGE_PE_HEADER)(ModuleData + DosHeader->PeHeaderOffset);
    SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&PeHeader->OptionalHeader +
                                                   PeHeader->FileHeader.SizeOfOptionalHeader);

    /* Look for .moddeps and .modinfo sections */
    for(SectionIndex = 0; SectionIndex < PeHeader->FileHeader.NumberOfSections; SectionIndex++)
    {
        /* Check section name */
        if(RtlCompareString((PCHAR)SectionHeader[SectionIndex].Name, ".moddeps", 8) == 0)
        {
            /* Store address of .moddeps data segment */
            DepsData = ModuleData + SectionHeader[SectionIndex].PointerToRawData;

            /* Iterate over all dependencies stored */
            while(*DepsData != 0)
            {
                /* Load dependency module */
                BlDebugPrint(L"Module '%S' requires '%S' ...\n", ModuleName, DepsData);
                Status = BlLoadModule(DepsData);

                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Failed to load module, print error message and return status code */
                    BlDebugPrint(L"Failed to load dependency module '%S', (Status Code: 0x%lx)\n", DepsData, Status);
                    return STATUS_EFI_UNSUPPORTED;
                }

                /* Allocate memory for module dependency */
                Status = BlAllocateMemoryPool(sizeof(XTBL_MODULE_DEPS), (PVOID*)&ModuleDependencies);
                if(Status == STATUS_EFI_SUCCESS)
                {
                    /* Memory allocated successfully, store module's dependency */
                    ModuleDependencies->ModuleName = DepsData;
                    RtlInsertTailList(&ModuleInfo->Dependencies, &ModuleDependencies->Flink);
                }

                /* Get next dependency module name */
                DepsData += 8;
            }
        }
        else if(RtlCompareString((PCHAR)SectionHeader[SectionIndex].Name, ".modinfo", 8) == 0)
        {
            /* Store module description */
            ModuleInfo->ModuleDescription = ModuleData + SectionHeader[SectionIndex].PointerToRawData;
        }
    }

    /* Finally, store module name */
    ModuleInfo->ModuleName = ModuleName;

    /* Setup module device path */
    ModuleDevicePath[0].Header.Length[0] = sizeof(EFI_MEMMAP_DEVICE_PATH);
    ModuleDevicePath[0].Header.Length[1] = sizeof(EFI_MEMMAP_DEVICE_PATH) >> 8;
    ModuleDevicePath[0].Header.Type = EFI_HARDWARE_DEVICE_PATH;
    ModuleDevicePath[0].Header.SubType = EFI_HARDWARE_MEMMAP_DP;
    ModuleDevicePath[0].MemoryType = EfiLoaderData;
    ModuleDevicePath[0].StartingAddress = (UINT_PTR)ModuleData;
    ModuleDevicePath[0].EndingAddress = (UINT_PTR)ModuleData + ModuleSize;
    ModuleDevicePath[1].Header.Length[0] = sizeof(EFI_DEVICE_PATH_PROTOCOL);
    ModuleDevicePath[1].Header.Length[1] = sizeof(EFI_DEVICE_PATH_PROTOCOL) >> 8;
    ModuleDevicePath[1].Header.Type = EFI_END_DEVICE_PATH;
    ModuleDevicePath[1].Header.SubType = EFI_END_ENTIRE_DP;

    /* Load EFI image */
    Status = EfiSystemTable->BootServices->LoadImage(FALSE, EfiImageHandle, (PEFI_DEVICE_PATH_PROTOCOL)ModuleDevicePath,
                                                     ModuleData, ModuleSize, &ModuleHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Check if caused by secure boot */
        if(Status == STATUS_EFI_ACCESS_DENIED && BlpStatus.SecureBoot >= 1)
        {
            /* SecureBoot signature validation failed */
            BlDebugPrint(L"ERROR: SecureBoot signature validation failed, module '%S' will not be loaded\n", ModuleName);
        }
        else
        {
            /* Failed to load module */
            BlDebugPrint(L"ERROR: Unable to load module '%S' (Status Code: 0x%lx)\n", ModuleName, Status);
        }

        /* Return error status code */
        return Status;
    }

    /* Access module interface for further module type check */
    Status = EfiSystemTable->BootServices->OpenProtocol(ModuleHandle, &LIPGuid, (PVOID *)&LoadedImage,
                                                        EfiImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open LoadedImage protocol */
        BlDebugPrint(L"ERROR: Unable to access module interface (Status Code: 0x%lx)\n", Status);
        return Status;
    }

    /* Some firmwares do not allow to start drivers which are not of 'boot system driver' type, so check it */
    if(LoadedImage->ImageCodeType != EfiBootServicesCode)
    {
        /* Different type set, probably 'runtime driver', refuse to load it */
        BlDebugPrint(L"ERROR: Loaded module is not a boot system driver\n");

        /* Close protocol and skip module */
        EfiSystemTable->BootServices->CloseProtocol(LoadedImage, &LIPGuid, LoadedImage, NULL);
    }

    /* Save module information */
    ModuleInfo->ModuleBase = LoadedImage->ImageBase;
    ModuleInfo->ModuleSize = LoadedImage->ImageSize;
    ModuleInfo->Revision = LoadedImage->Revision;
    ModuleInfo->UnloadModule = LoadedImage->Unload;

    /* Close loaded image protocol */
    EfiSystemTable->BootServices->CloseProtocol(LoadedImage, &LIPGuid, LoadedImage, NULL);

    /* Start EFI image */
    Status = EfiSystemTable->BootServices->StartImage(ModuleHandle, NULL, NULL);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to start module image */
        BlDebugPrint(L"ERROR: Failed to start module '%S' (Status Code: 0x%lx)\n", ModuleName, Status);
        return Status;
    }

    /* Add module to the list of loaded modules */
    RtlInsertTailList(&BlpLoadedModules, &ModuleInfo->Flink);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Helper routine to load all modules supplied in the configuration file.
 *
 * @param ModulesList
 *        Supplies a space separated list of XTLDR modules to load (mostly read from configuration file).
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlLoadModules(IN PWCHAR ModulesList)
{
    PWCHAR LastModule, Module;
    EFI_STATUS ReturnStatus, Status;

    /* Set default return value */
    ReturnStatus = STATUS_EFI_SUCCESS;

    if(ModulesList != NULL)
    {
        /* Tokenize provided list of modules */
        Module = RtlTokenizeWideString(ModulesList, L" ", &LastModule);

        /* Iterate over all arguments passed to boot loader */
        while(Module != NULL)
        {
            Status = BlLoadModule(Module);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to load module, print error message and set new return value */
                BlDebugPrint(L"ERROR: Failed to load module '%S', (Status Code: 0x%lx)\n", Module, Status);
                ReturnStatus = STATUS_EFI_LOAD_ERROR;
            }

            /* Take next module from the list */
            Module = RtlTokenizeWideString(NULL, L" ", &LastModule);
        }
    }

    /* Return success */
    return ReturnStatus;
}

/**
 * Returns an array of handles that support the requested protocol.
 *
 * @param Handles
 *        Supplies the address where a pointer to all handles found for the protocol interface.
 *
 * @param Count
 *        Provides a number of the returned handles.
 *
 * @param ProtocolGuid
 *        Supplies a pointer to the unique protocol GUID.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlLocateProtocolHandles(OUT PEFI_HANDLE *Handles,
                        OUT PUINT_PTR Count,
                        IN PEFI_GUID ProtocolGuid)
{
    return EfiSystemTable->BootServices->LocateHandleBuffer(ByProtocol, ProtocolGuid, NULL, Count, Handles);
}

/**
 * Locates and opens the requested XT Boot Loader or EFI protocol.
 *
 * @param Handle
 *        Supplies the address where a pointer to the handle for the protocol interface.
 *
 * @param ProtocolHandler
 *        Supplies the address where a pointer to the opened protocol is returned.
 *
 * @param ProtocolGuid
 *        Supplies a pointer to the unique protocol GUID.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlOpenProtocol(OUT PEFI_HANDLE Handle,
               OUT PVOID *ProtocolHandler,
               IN PEFI_GUID ProtocolGuid)
{
    PEFI_HANDLE Handles = NULL;
    EFI_STATUS Status;
    UINT_PTR Count;
    UINT Index;

    /* Try to locate the handles */
    Status = BlLocateProtocolHandles(&Handles, &Count, ProtocolGuid);
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
            Status = BlOpenProtocolHandle(Handles[Index], ProtocolHandler, ProtocolGuid);

            /* Check if successfully opened the loader protocol */
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Protocol found and successfully opened */
                *Handle = Handles[Index];
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

/**
 * Opens the requested XT Boot Loader or EFI protocol, if it is supported by the handle.
 *
 * @param Handle
 *        Supplies a handle for the protocol interface that is being opened.
 *
 * @param ProtocolHandler
 *        Supplies the address where a pointer to the opened protocol is returned.
 *
 * @param ProtocolGuid
 *        Supplies a pointer to the unique protocol GUID.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlOpenProtocolHandle(IN EFI_HANDLE Handle,
                     OUT PVOID *ProtocolHandler,
                     IN PEFI_GUID ProtocolGuid)
{
    return EfiSystemTable->BootServices->OpenProtocol(Handle, ProtocolGuid, ProtocolHandler, EfiImageHandle,
                                                      NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
}

/**
 * Registers a boot menu callback routine, that will be used to display alternative boot menu.
 *
 * @param BootMenuRoutine
 *        Supplies a pointer to the boot menu callback routine.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlRegisterBootMenu(IN PVOID BootMenuRoutine)
{
    /* Set boot menu routine */
    BlpStatus.BootMenu = BootMenuRoutine;
}

/**
 * Registers a known boot protocol for a specified OS.
 *
 * @param SystemType
 *        Supplies the type of the OS, such as "LINUX", "XTOS", etc. that is supported by the boot protocol.
 *
 * @param BootProtocolGuid
 *        Supplies a pointer to the unique protocol GUID.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlRegisterBootProtocol(IN PWCHAR SystemType,
                       IN PEFI_GUID BootProtocolGuid)
{
    PXTBL_KNOWN_BOOT_PROTOCOL ProtocolEntry;
    PLIST_ENTRY ProtocolListEntry;
    EFI_STATUS Status;

    ProtocolListEntry = BlpBootProtocols.Flink;
    while(ProtocolListEntry != &BlpBootProtocols)
    {
        /* Get boot protocol entry */
        ProtocolEntry = CONTAIN_RECORD(ProtocolListEntry, XTBL_KNOWN_BOOT_PROTOCOL, Flink);

        /* Check if boot protocol already registered for specified system */
        if(RtlCompareWideStringInsensitive(ProtocolEntry->SystemType, SystemType, 0) == 0)
        {
            /* Boot protocol already registered */
            return STATUS_EFI_ABORTED;
        }

        /* Move to the next registered boot protocol */
        ProtocolListEntry = ProtocolListEntry->Flink;
    }

    /* Create new boot protocol entry */
    Status = BlAllocateMemoryPool(sizeof(XTBL_BOOT_PROTOCOL), (PVOID *)&ProtocolEntry);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return STATUS_EFI_OUT_OF_RESOURCES;
    }

    /* Set protocol properties and add it to the list */
    ProtocolEntry->SystemType = SystemType;
    ProtocolEntry->Guid = *BootProtocolGuid;
    RtlInsertTailList(&BlpBootProtocols, &ProtocolEntry->Flink);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine installs XTLDR protocol for further usage by modules.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpInstallXtLoaderProtocol()
{
    EFI_GUID Guid = XT_BOOT_LOADER_PROTOCOL_GUID;

    /* Set all routines available via loader protocol */
    BlpLdrProtocol.Boot.FindProtocol = BlFindBootProtocol;
    BlpLdrProtocol.Boot.InitializeMenuList = BlInitializeBootMenuList;
    BlpLdrProtocol.Boot.InvokeProtocol = BlInvokeBootProtocol;
    BlpLdrProtocol.Boot.RegisterMenu = BlRegisterBootMenu;
    BlpLdrProtocol.Boot.RegisterProtocol = BlRegisterBootProtocol;
    BlpLdrProtocol.Config.GetValue = BlGetConfigValue;
    BlpLdrProtocol.Console.ClearLine = BlClearConsoleLine;
    BlpLdrProtocol.Console.ClearScreen = BlClearConsoleScreen;
    BlpLdrProtocol.Console.DisableCursor = BlDisableConsoleCursor;
    BlpLdrProtocol.Console.EnableCursor = BlEnableConsoleCursor;
    BlpLdrProtocol.Console.Print = BlConsolePrint;
    BlpLdrProtocol.Console.QueryMode = BlQueryConsoleMode;
    BlpLdrProtocol.Console.ReadKeyStroke = BlReadKeyStroke;
    BlpLdrProtocol.Console.ResetInputBuffer = BlResetConsoleInputBuffer;
    BlpLdrProtocol.Console.SetAttributes = BlSetConsoleAttributes;
    BlpLdrProtocol.Console.SetCursorPosition = BlSetCursorPosition;
    BlpLdrProtocol.Console.Write = BlConsoleWrite;
    BlpLdrProtocol.Debug.Print = BlDebugPrint;
    BlpLdrProtocol.Disk.CloseVolume = BlCloseVolume;
    BlpLdrProtocol.Disk.OpenVolume = BlOpenVolume;
    BlpLdrProtocol.Disk.ReadFile = BlReadFile;
    BlpLdrProtocol.Memory.AllocatePages = BlAllocateMemoryPages;
    BlpLdrProtocol.Memory.AllocatePool = BlAllocateMemoryPool;
    BlpLdrProtocol.Memory.CopyMemory = RtlCopyMemory;
    BlpLdrProtocol.Memory.FreePages = BlFreeMemoryPages;
    BlpLdrProtocol.Memory.FreePool = BlFreeMemoryPool;
    BlpLdrProtocol.Memory.GetMemoryMap = BlGetMemoryMap;
    BlpLdrProtocol.Memory.InitializePageMap = BlInitializePageMap;
    BlpLdrProtocol.Memory.MapEfiMemory = BlMapEfiMemory;
    BlpLdrProtocol.Memory.MapPage = BlMapPage;
    BlpLdrProtocol.Memory.MapVirtualMemory = BlMapVirtualMemory;
    BlpLdrProtocol.Memory.SetMemory = RtlSetMemory;
    BlpLdrProtocol.Memory.ZeroMemory = RtlZeroMemory;
    BlpLdrProtocol.Protocol.Close = BlCloseProtocol;
    BlpLdrProtocol.Protocol.GetModulesList = BlGetModulesList;
    BlpLdrProtocol.Protocol.Install = BlInstallProtocol;
    BlpLdrProtocol.Protocol.LocateHandles = BlLocateProtocolHandles;
    BlpLdrProtocol.Protocol.Open = BlOpenProtocol;
    BlpLdrProtocol.Protocol.OpenHandle = BlOpenProtocolHandle;
    BlpLdrProtocol.Tui.DisplayErrorDialog = BlDisplayErrorDialog;
    BlpLdrProtocol.Tui.DisplayInfoDialog = BlDisplayInfoDialog;
    BlpLdrProtocol.Tui.DisplayInputDialog = BlDisplayInputDialog;
    BlpLdrProtocol.Tui.DisplayProgressDialog = BlDisplayProgressDialog;
    BlpLdrProtocol.Tui.UpdateProgressBar = BlUpdateProgressBar;
    BlpLdrProtocol.Util.ExitBootServices = BlExitBootServices;
    BlpLdrProtocol.Util.GetSecureBootStatus = BlGetSecureBootStatus;
    BlpLdrProtocol.Util.RebootSystem = BlRebootSystem;
    BlpLdrProtocol.Util.ShutdownSystem = BlShutdownSystem;
    BlpLdrProtocol.Util.SleepExecution = BlSleepExecution;
    BlpLdrProtocol.Util.WaitForEfiEvent = BlWaitForEfiEvent;

    /* Register XTLDR loader protocol */
    BlDebugPrint(L"Registering XT loader protocol\n");
    return BlInstallProtocol(&BlpLdrProtocol, &Guid);
}
