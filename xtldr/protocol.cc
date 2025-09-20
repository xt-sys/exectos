/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/protocol.cc
 * DESCRIPTION:     XT Boot Loader protocol support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


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
Protocol::CloseProtocol(IN PEFI_HANDLE Handle,
                        IN PEFI_GUID ProtocolGuid)
{
    return EfiSystemTable->BootServices->CloseProtocol(Handle, ProtocolGuid, EfiImageHandle, NULLPTR);
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
Protocol::FindBootProtocol(IN PCWSTR SystemType,
                           OUT PEFI_GUID BootProtocolGuid)
{
    PXTBL_KNOWN_BOOT_PROTOCOL ProtocolEntry;
    PLIST_ENTRY ProtocolListEntry;

    ProtocolListEntry = BootProtocols.Flink;
    while(ProtocolListEntry != &BootProtocols)
    {
        /* Get boot protocol entry */
        ProtocolEntry = CONTAIN_RECORD(ProtocolListEntry, XTBL_KNOWN_BOOT_PROTOCOL, Flink);

        /* Check if this boot protocol supports specified system */
        if(RTL::WideString::CompareWideStringInsensitive(ProtocolEntry->SystemType, SystemType, 0) == 0)
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
Protocol::GetModulesList()
{
    /* Return a pointer to a list of all loaded modules */
    return &LoadedModules;
}

XTCDECL
VOID
Protocol::InitializeProtocol()
{
    /* Initialize list of loaded modules and boot protocols */
    RTL::LinkedList::InitializeListHead(&BootProtocols);
    RTL::LinkedList::InitializeListHead(&LoadedModules);
}

/**
 * Installs XTLDR protocol interface.
 *
 * @param Guid
 *        Specifies a unique protocol GUID.
 *
 * @param Interface
 *        Supplies a pointer to the protocol interface, or NULLPTR if there is no structure associated.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Protocol::InstallProtocol(IN PVOID Interface,
                          IN PEFI_GUID Guid)
{
    EFI_HANDLE Handle = NULLPTR;

    /* Install protocol interface */
    return EfiSystemTable->BootServices->InstallProtocolInterface(&Handle, Guid, EFI_NATIVE_INTERFACE, Interface);
}

/**
 * Loads all necessary modules and invokes boot protocol.
 *
 * @param ShortName
 *        Supplies a pointer to a short name of the chosen boot menu entry.
 *
 * @param OptionsList
 *        Supplies a pointer to list of options associated with chosen boot menu entry.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Protocol::InvokeBootProtocol(IN PWCHAR ShortName,
                             IN PLIST_ENTRY OptionsList)
{
    EFI_GUID VendorGuid = XT_BOOT_LOADER_PROTOCOL_GUID;
    XTBL_BOOT_PARAMETERS BootParameters;
    PXTBL_BOOT_PROTOCOL BootProtocol;
    PLIST_ENTRY OptionsListEntry;
    PXTBL_CONFIG_ENTRY Option;
    EFI_GUID BootProtocolGuid;
    SIZE_T ModuleListLength;
    PWCHAR ModulesList;
    EFI_HANDLE Handle;
    EFI_STATUS Status;

    /* Initialize boot parameters and a list of modules */
    RTL::Memory::ZeroMemory(&BootParameters, sizeof(XTBL_BOOT_PARAMETERS));
    ModulesList = NULLPTR;

    /* Iterate through all options provided by boot menu entry and propagate boot parameters */
    OptionsListEntry = OptionsList->Flink;
    while(OptionsListEntry != OptionsList)
    {
        /* Get option */
        Option = CONTAIN_RECORD(OptionsListEntry, XTBL_CONFIG_ENTRY, Flink);

        /* Look for boot protocol and modules list */
        if(RTL::WideString::CompareWideStringInsensitive(Option->Name, L"BOOTMODULES", 0) == 0)
        {
            /* Check a length of modules list */
            ModuleListLength = RTL::WideString::WideStringLength(Option->Value, 0);

            Status = Memory::AllocatePool(sizeof(WCHAR) * (ModuleListLength + 1), (PVOID *)&ModulesList);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to allocate memory, print error message and return status code */
                Debug::Print(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
                return STATUS_EFI_OUT_OF_RESOURCES;
            }

            /* Make a copy of modules list */
            RTL::Memory::CopyMemory(ModulesList, Option->Value, sizeof(WCHAR) * (ModuleListLength + 1));
        }
        else if(RTL::WideString::CompareWideStringInsensitive(Option->Name, L"SYSTEMTYPE", 0) == 0)
        {
            /* Boot protocol found */
            BootParameters.SystemType = Option->Value;
        }
        else if(RTL::WideString::CompareWideStringInsensitive(Option->Name, L"SYSTEMPATH", 0) == 0)
        {
            /* System path found, get volume device path */
            Status = Volume::GetDevicePath(Option->Value, &BootParameters.DevicePath,
                                           &BootParameters.ArcName, &BootParameters.SystemPath);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to find volume */
                Debug::Print(L"ERROR: Failed to find volume device path (Status Code: 0x%zX)\n", Status);
                return Status;
            }

            /* Get EFI compatible system path */
            Status = Volume::GetEfiPath(BootParameters.SystemPath, &BootParameters.EfiPath);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get EFI path */
                Debug::Print(L"ERROR: Failed to get EFI path (Status Code: 0x%zX)\n", Status);
                return Status;
            }
        }
        else if(RTL::WideString::CompareWideStringInsensitive(Option->Name, L"KERNELFILE", 0) == 0)
        {
            /* Kernel file name found */
            BootParameters.KernelFile = Option->Value;
        }
        else if(RTL::WideString::CompareWideStringInsensitive(Option->Name, L"INITRDFILE", 0) == 0)
        {
            /* Initrd file name found */
            BootParameters.InitrdFile = Option->Value;
        }
        else if(RTL::WideString::CompareWideStringInsensitive(Option->Name, L"HALFILE", 0) == 0)
        {
            /* Hal file name found */
            BootParameters.HalFile = Option->Value;
        }
        else if(RTL::WideString::CompareWideStringInsensitive(Option->Name, L"PARAMETERS", 0) == 0)
        {
            /* Kernel parameters found */
            BootParameters.Parameters = Option->Value;
        }

        /* Move to the next option entry */
        OptionsListEntry = OptionsListEntry->Flink;
    }

    /* Load all necessary modules */
    Status = LoadModules(ModulesList);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load modules, print error message and return status code */
        Debug::Print(L"ERROR: Failed to load XTLDR modules (Status Code: 0x%zX)\n", Status);
        return STATUS_EFI_NOT_READY;
    }

    /* Attempt to get boot protocol GUID */
    Status = FindBootProtocol(BootParameters.SystemType, &BootProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get boot protocol GUID */
        Debug::Print(L"ERROR: Unable to find appropriate boot protocol (Status Code: 0x%zX)\n", Status);
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Open boot protocol */
    Status = OpenProtocol(&Handle, (PVOID *)&BootProtocol, &BootProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open boot protocol */
        Debug::Print(L"ERROR: Failed to open boot protocol (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Check if chosen operating system should be saved */
    if(Configuration::GetBooleanValue(L"KEEPLASTBOOT"))
    {
        /* Save chosen operating system in NVRAM */
        Status = EfiUtils::SetEfiVariable(&VendorGuid, L"XtLdrLastBootOS", (PVOID)ShortName, RTL::WideString::WideStringLength(ShortName, 0) * sizeof(WCHAR));
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to save chosen Operating System */
            Debug::Print(L"WARNING: Failed to save chosen Operating System in NVRAM (Status Code: 0x%zX)\n", Status);
        }
    }

    /* Boot Operating System */
    return BootProtocol->BootSystem(&BootParameters);
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
Protocol::LoadModule(IN PWCHAR ModuleName)
{
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PLIST_ENTRY DepsListEntry, ModuleListEntry;
    EFI_MEMMAP_DEVICE_PATH ModuleDevicePath[2];
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    PEFI_FILE_HANDLE DirHandle, FsHandle;
    EFI_HANDLE DiskHandle, ModuleHandle;
    PPECOFF_IMAGE_SECTION_HEADER SectionHeader;
    PPECOFF_IMAGE_DOS_HEADER DosHeader;
    PPECOFF_IMAGE_PE_HEADER PeHeader;
    PXTBL_MODULE_DEPS ModuleDependency;
    PXTBL_MODULE_INFO ModuleInfo;
    WCHAR ModuleFileName[24];
    USHORT SectionIndex;
    PWCHAR SectionData;
    SIZE_T ModuleSize;
    EFI_STATUS Status;
    PVOID ModuleData;

    ModuleListEntry = LoadedModules.Flink;
    while(ModuleListEntry != &LoadedModules)
    {
        /* Get module information */
        ModuleInfo = CONTAIN_RECORD(ModuleListEntry, XTBL_MODULE_INFO, Flink);

        if(RTL::WideString::CompareWideStringInsensitive(ModuleInfo->ModuleName, ModuleName, 0) == 0)
        {
            /* Module already loaded */
            Debug::Print(L"WARNING: Module '%S' already loaded!\n", ModuleName);
            return STATUS_EFI_SUCCESS;
        }

        /* Move to next module */
        ModuleListEntry = ModuleListEntry->Flink;
    }

    /* Print debug message */
    Debug::Print(L"Loading module '%S' ...\n", ModuleName);

    /* Set module path */
    RTL::Memory::CopyMemory(ModuleFileName, ModuleName, (RTL::WideString::WideStringLength(ModuleName, 0) + 1) * sizeof(WCHAR));
    RTL::WideString::ConcatenateWideString(ModuleFileName, (PWCHAR)L".EFI", 0);

    /* Open EFI volume */
    Status = Volume::OpenVolume(NULLPTR, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        return Status;
    }

    /* Open XTLDR modules common directory */
    Status = FsHandle->Open(FsHandle, &DirHandle, (PWCHAR)XTBL_MODULES_DIRECTORY_PATH, EFI_FILE_MODE_READ, 0);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Modules directory not found, attempt to open XTLDR architecture specific modules directory */
        Status = FsHandle->Open(FsHandle, &DirHandle, (PWCHAR)XTBL_ARCH_MODULES_DIRECTORY_PATH, EFI_FILE_MODE_READ, 0);
    }

    /* Close FS handle */
    FsHandle->Close(FsHandle);

    /* Check if modules directory opened successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        Volume::CloseVolume(&DiskHandle);
        return Status;
    }

    /* Read module file from disk and close directory and EFI volume */
    Status = Volume::ReadFile(DirHandle, ModuleFileName, &ModuleData, &ModuleSize);
    DirHandle->Close(DirHandle);
    Volume::CloseVolume(&DiskHandle);

    /* Make sure module file was read successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to read file */
        return Status;
    }

    /* Allocate memory for module information block */
    Status = Memory::AllocatePool(sizeof(XTBL_MODULE_INFO), (PVOID*)&ModuleInfo);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to allocate memory */
        return Status;
    }

    /* Zero module information block */
    RTL::Memory::ZeroMemory(ModuleInfo, sizeof(XTBL_MODULE_INFO));

    /* Setup PE/COFF EFI image headers */
    DosHeader = (PPECOFF_IMAGE_DOS_HEADER)ModuleData;
    PeHeader = (PPECOFF_IMAGE_PE_HEADER)((PUCHAR)ModuleData + DosHeader->PeHeaderOffset);

    /* Check PE/COFF image type*/
    if(PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Get PE32+ (64-bit) image section headers */
        SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&PeHeader->OptionalHeader64 +
                                                       PeHeader->FileHeader.SizeOfOptionalHeader);
    }
    else
    {
        /* Get PE32 (32-bit) image section headers */
        SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&PeHeader->OptionalHeader32 +
                                                       PeHeader->FileHeader.SizeOfOptionalHeader);
    }

    /* Look for .modinfo section */
    for(SectionIndex = 0; SectionIndex < PeHeader->FileHeader.NumberOfSections; SectionIndex++)
    {
        if(RTL::String::CompareString((PCHAR)SectionHeader[SectionIndex].Name, ".modinfo", 8) == 0)
        {
            /* Module information section found */
            SectionData = (PWCHAR)((PUCHAR)ModuleData + SectionHeader[SectionIndex].PointerToRawData);

            /* Get module information */
            Status = GetModuleInformation(SectionData, SectionHeader[SectionIndex].SizeOfRawData, ModuleInfo);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to read module information */
                return Status;
            }
        }
    }

    /* Iterate through module dependencies */
    DepsListEntry = ModuleInfo->Dependencies.Flink;
    while(DepsListEntry != &ModuleInfo->Dependencies)
    {
        /* Get module dependency information */
        ModuleDependency = CONTAIN_RECORD(DepsListEntry, XTBL_MODULE_DEPS, Flink);

        /* Make sure dependency list contains a valid module name */
        if(ModuleDependency->ModuleName == NULLPTR || ModuleDependency->ModuleName[0] == L'\0')
        {
            /* Invalid module name found, just skip this step */
            break;
        }

        /* Load dependency module */
        Debug::Print(L"Module '%S' requires '%S' ...\n", ModuleName, ModuleDependency->ModuleName);
        Status = LoadModule(ModuleDependency->ModuleName);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to load module, print error message and return status code */
            Debug::Print(L"Failed to load dependency module '%S' (Status Code: 0x%zX)\n", ModuleDependency->ModuleName, Status);
            return STATUS_EFI_UNSUPPORTED;
        }

        /* Move to the next dependency */
        DepsListEntry = DepsListEntry->Flink;
    }

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
    Debug::Print(L"Starting module '%S' ...\n", ModuleName);
    Status = EfiUtils::LoadEfiImage((PEFI_DEVICE_PATH_PROTOCOL)ModuleDevicePath, ModuleData, ModuleSize, &ModuleHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Check if caused by secure boot */
        if(Status == STATUS_EFI_ACCESS_DENIED && BlpStatus.SecureBoot >= 1)
        {
            /* SecureBoot signature validation failed */
            Debug::Print(L"ERROR: SecureBoot signature validation failed, module '%S' will not be loaded\n", ModuleName);
        }
        else
        {
            /* Failed to load module */
            Debug::Print(L"ERROR: Unable to load module '%S' (Status Code: 0x%zX)\n", ModuleName, Status);
        }

        /* Return error status code */
        return Status;
    }

    /* Access module interface for further module type check */
    Status = EfiSystemTable->BootServices->OpenProtocol(ModuleHandle, &LIPGuid, (PVOID *)&LoadedImage,
                                                        EfiImageHandle, NULLPTR, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open LoadedImage protocol */
        Debug::Print(L"ERROR: Unable to access module interface (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Some firmwares do not allow to start drivers which are not of 'boot system driver' type, so check it */
    if(LoadedImage->ImageCodeType != EfiBootServicesCode)
    {
        /* Different type set, probably 'runtime driver', refuse to load it */
        Debug::Print(L"ERROR: Loaded module is not a boot system driver\n");

        /* Close protocol and skip module */
        EfiSystemTable->BootServices->CloseProtocol(LoadedImage, &LIPGuid, LoadedImage, NULLPTR);
    }

    /* Save additional module information, not found in '.modinfo' section */
    ModuleInfo->ModuleName = ModuleName;
    ModuleInfo->ModuleBase = LoadedImage->ImageBase;
    ModuleInfo->ModuleSize = LoadedImage->ImageSize;
    ModuleInfo->Revision = LoadedImage->Revision;
    ModuleInfo->UnloadModule = LoadedImage->Unload;

    /* Close loaded image protocol */
    EfiSystemTable->BootServices->CloseProtocol(LoadedImage, &LIPGuid, LoadedImage, NULLPTR);

    /* Start EFI image */
    Status = EfiUtils::StartEfiImage(ModuleHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to start module image */
        Debug::Print(L"ERROR: Failed to start module '%S' (Status Code: 0x%zX)\n", ModuleName, Status);
        return Status;
    }

    /* Add module to the list of loaded modules */
    RTL::LinkedList::InsertTailList(&LoadedModules, &ModuleInfo->Flink);

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
Protocol::LoadModules(IN PWCHAR ModulesList)
{
    PWCHAR LastModule, Module;
    EFI_STATUS ReturnStatus, Status;

    /* Set default return value */
    ReturnStatus = STATUS_EFI_SUCCESS;

    if(ModulesList != NULLPTR)
    {
        /* Tokenize provided list of modules */
        Module = RTL::WideString::TokenizeWideString(ModulesList, L" ", &LastModule);

        /* Iterate over all arguments passed to boot loader */
        while(Module != NULLPTR)
        {
            Status = LoadModule(Module);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to load module, print error message and set new return value */
                Debug::Print(L"ERROR: Failed to load module '%S' (Status Code: 0x%zX)\n", Module, Status);
                ReturnStatus = STATUS_EFI_LOAD_ERROR;
            }

            /* Take next module from the list */
            Module = RTL::WideString::TokenizeWideString(NULLPTR, L" ", &LastModule);
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
Protocol::LocateProtocolHandles(OUT PEFI_HANDLE *Handles,
                                OUT PUINT_PTR Count,
                                IN PEFI_GUID ProtocolGuid)
{
    return EfiSystemTable->BootServices->LocateHandleBuffer(ByProtocol, ProtocolGuid, NULLPTR, Count, Handles);
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
Protocol::OpenProtocol(OUT PEFI_HANDLE Handle,
                       OUT PVOID *ProtocolHandler,
                       IN PEFI_GUID ProtocolGuid)
{
    PEFI_HANDLE Handles = NULLPTR;
    EFI_STATUS Status;
    UINT_PTR Count;
    UINT Index;

    /* Try to locate the handles */
    Status = LocateProtocolHandles(&Handles, &Count, ProtocolGuid);
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
            Status = OpenProtocolHandle(Handles[Index], ProtocolHandler, ProtocolGuid);

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
    if(*ProtocolHandler == NULLPTR)
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
Protocol::OpenProtocolHandle(IN EFI_HANDLE Handle,
                             OUT PVOID *ProtocolHandler,
                             IN PEFI_GUID ProtocolGuid)
{
    return EfiSystemTable->BootServices->OpenProtocol(Handle, ProtocolGuid, ProtocolHandler, EfiImageHandle,
                                                      NULLPTR, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
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
Protocol::RegisterBootMenu(IN PVOID BootMenuRoutine)
{
    /* Set boot menu routine */
    BlpStatus.BootMenu = (PBL_XT_BOOT_MENU)BootMenuRoutine;
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
Protocol::RegisterBootProtocol(IN PCWSTR SystemType,
                               IN PEFI_GUID BootProtocolGuid)
{
    PXTBL_KNOWN_BOOT_PROTOCOL ProtocolEntry;
    PLIST_ENTRY ProtocolListEntry;
    EFI_STATUS Status;

    ProtocolListEntry = BootProtocols.Flink;
    while(ProtocolListEntry != &BootProtocols)
    {
        /* Get boot protocol entry */
        ProtocolEntry = CONTAIN_RECORD(ProtocolListEntry, XTBL_KNOWN_BOOT_PROTOCOL, Flink);

        /* Check if boot protocol already registered for specified system */
        if(RTL::WideString::CompareWideStringInsensitive(ProtocolEntry->SystemType, SystemType, 0) == 0)
        {
            /* Boot protocol already registered */
            return STATUS_EFI_ABORTED;
        }

        /* Move to the next registered boot protocol */
        ProtocolListEntry = ProtocolListEntry->Flink;
    }

    /* Create new boot protocol entry */
    Status = Memory::AllocatePool(sizeof(XTBL_BOOT_PROTOCOL), (PVOID *)&ProtocolEntry);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return STATUS_EFI_OUT_OF_RESOURCES;
    }

    /* Set protocol properties and add it to the list */
    ProtocolEntry->SystemType = (PWCHAR)SystemType;
    ProtocolEntry->Guid = *BootProtocolGuid;
    RTL::LinkedList::InsertTailList(&BootProtocols, &ProtocolEntry->Flink);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Reads information from the '.modinfo' section and populates the module information structure.
 *
 * @param SectionData
 *        Supplies a pointer to the module's information section data.
 *
 * @param SectionSize
 *        Supplies an expected size of the section data.
 *
 * @param ModuleInfo
 *        Supplies a pointer to the module information structure that will be filled by data from module's info section.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Protocol::GetModuleInformation(IN PWCHAR SectionData,
                               IN ULONG SectionSize,
                               OUT PXTBL_MODULE_INFO ModuleInfo)
{
    PXTBL_MODULE_DEPS ModuleDependencies;
    PXTBL_MODULE_AUTHORS ModuleAuthors;
    PWCHAR Dependency, Key, LastStr;
    ULONG Index, Count;
    EFI_STATUS Status;
    PWCHAR *Strings;

    /* Initialize authors and dependencies lists */
    RTL::LinkedList::InitializeListHead(&ModuleInfo->Authors);
    RTL::LinkedList::InitializeListHead(&ModuleInfo->Dependencies);

    /* Get information strings from '.modinfo' section */
    Status = GetModuleInfoStrings(SectionData, SectionSize, &Strings, &Count);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get information strings */
        return Status;
    }

    /* Parse information strings */
    for(Index = 0; Index < Count; Index++)
    {
        /* Store the key */
        Key = Strings[Index];

        /* Find the end of the key and the beginning of the value */
        while(*Strings[Index] != L'=' && *Strings[Index] != L'\0' && *Strings[Index] != L'\n')
        {
            /* Move to the next character */
            Strings[Index]++;
        }

        /* Make sure value is NULL-terminated */
        *Strings[Index] = L'\0';
        Strings[Index]++;

        /* Parse information string key */
        if(RTL::WideString::CompareWideString(Key, L"author", 6) == 0)
        {
            /* Allocate memory for module author */
            Status = Memory::AllocatePool(sizeof(XTBL_MODULE_AUTHORS), (PVOID*)&ModuleAuthors);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure */
                return Status;
            }

            /* Store module's author */
            ModuleAuthors->AuthorName = Strings[Index];
            RTL::LinkedList::InsertTailList(&ModuleInfo->Authors, &ModuleAuthors->Flink);
        }
        else if(RTL::WideString::CompareWideString(Key, L"description", 11) == 0)
        {
            /* Store module's description */
            ModuleInfo->ModuleDescription = Strings[Index];
        }
        else if(RTL::WideString::CompareWideString(Key, L"license", 7) == 0)
        {
            /* Store module's license */
            ModuleInfo->License = Strings[Index];
        }
        else if(RTL::WideString::CompareWideString(Key, L"softdeps", 6) == 0)
        {
            /* Tokenize value to get module's single dependency */
            Dependency = RTL::WideString::TokenizeWideString(Strings[Index], L" ", &LastStr);
            while(Dependency != NULLPTR)
            {
                /* Allocate memory for module dependency */
                Status = Memory::AllocatePool(sizeof(XTBL_MODULE_DEPS), (PVOID*)&ModuleDependencies);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Memory allocation failure */
                    return Status;
                }

                /* Store module's dependency */
                ModuleDependencies->ModuleName = Dependency;
                RTL::LinkedList::InsertTailList(&ModuleInfo->Dependencies, &ModuleDependencies->Flink);

                /* Get next dependency from single value if available */
                Dependency = RTL::WideString::TokenizeWideString(NULLPTR, L" ", &LastStr);
            }
        }
        else if(RTL::WideString::CompareWideString(Key, L"version", 7) == 0)
        {
            /* Store module's version */
            ModuleInfo->Version = Strings[Index];
        }
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Reads raw data from the '.modinfo' section and populates an array of strings.
 *
 * @param SectionData
 *        Supplies a pointer to the module's information section data.
 *
 * @param SectionSize
 *        Supplies an expected size of the section data.
 *
 * @param ModInfo
 *        Supplies a pointer to memory area, where an array of strings read from the section will be stored.
 *
 * @param InfoCount
 *        Supplies a pointer to variable that will receive the number of strings found in the section.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Protocol::GetModuleInfoStrings(IN PWCHAR SectionData,
                               IN ULONG SectionSize,
                               OUT PWCHAR **ModInfo,
                               OUT PULONG InfoCount)
{
    ULONG Count, Index, ArrayIndex;
    PCWSTR InfoStrings;
    EFI_STATUS Status;
    PWCHAR *Array;
    PWCHAR String;
    ULONG DataSize;

    /* Check input parameters */
    InfoStrings = SectionData;
    if(!InfoStrings || !SectionSize)
    {
        /* Invalid input parameters */
        *ModInfo = NULLPTR;
        *InfoCount = 0;
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Calculate the size of the data based on the size of the section */
    DataSize = SectionSize / sizeof(WCHAR);

    /* Skip zero padding at the beginning */
    while(DataSize > 0 && *InfoStrings == L'\0')
    {
        InfoStrings++;
        DataSize--;
    }

    /* Make sure there is at least one string available */
    if(DataSize < 1)
    {
        /* No strings found */
        *ModInfo = NULLPTR;
        *InfoCount = 0;
        return STATUS_EFI_END_OF_FILE;
    }

    /* Count number of strings */
    Index = 0;
    Count = 0;
    while(Index < DataSize)
    {
        /* Found start of a new string */
        Count++;

        /* Go to the end of the string */
        while(Index < DataSize && InfoStrings[Index] != L'\0')
        {
            Index++;
        }
        /* Skip all NULL terminators */
        while(Index < DataSize && InfoStrings[Index] == L'\0')
        {
            Index++;
        }
    }

    /* Allocate memory for the pointer array and the string data */
    Status = Memory::AllocatePool(sizeof(PWCHAR) * (Count + 1) + (DataSize + 1) * sizeof(WCHAR), (PVOID *)&Array);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to allocate memory */
        return STATUS_EFI_OUT_OF_RESOURCES;
    }

    /* The string buffer is located right after the pointer array */
    String = (PWCHAR)(Array + Count + 1);

    /* Copy the raw string data */
    RTL::Memory::CopyMemory(String, InfoStrings, DataSize * sizeof(WCHAR));

    /* Ensure the entire buffer is NULL-terminated for safety */
    String[DataSize] = L'\0';

    /* Set the last element of the pointer array to NULLPTR */
    Array[Count] = NULLPTR;

    /* Populate the array with pointers to the strings within the buffer */
    Index = 0;
    ArrayIndex = 0;
    while(Index < DataSize && ArrayIndex < Count)
    {
        /* Set pointer to the beginning of the string */
        Array[ArrayIndex++] = &String[Index];

        /* Find the end of the current string */
        while(Index < DataSize && String[Index] != L'\0')
        {
            Index++;
        }

        /* Skip all NULL terminators to find the beginning of the next string */
        while(Index < DataSize && String[Index] == L'\0')
        {
            Index++;
        }
    }

    /* Return array of strings and its size */
    *ModInfo = Array;
    *InfoCount = Count;

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
Protocol::InstallXtLoaderProtocol()
{
    EFI_GUID Guid = XT_BOOT_LOADER_PROTOCOL_GUID;

    /* Set all routines available via loader protocol */
    LoaderProtocol.Boot.FindProtocol = FindBootProtocol;
    LoaderProtocol.Boot.InitializeMenuList = Configuration::InitializeBootMenuList;
    LoaderProtocol.Boot.InvokeProtocol = InvokeBootProtocol;
    LoaderProtocol.Boot.RegisterMenu = RegisterBootMenu;
    LoaderProtocol.Boot.RegisterProtocol = RegisterBootProtocol;
    LoaderProtocol.BootUtils.GetBooleanParameter = BootUtils::GetBooleanParameter;
    LoaderProtocol.BootUtils.GetTrampolineInformation = AR::ProcSup::GetTrampolineInformation;
    LoaderProtocol.Config.GetBooleanValue = Configuration::GetBooleanValue;
    LoaderProtocol.Config.GetBootOptionValue = Configuration::GetBootOptionValue;
    LoaderProtocol.Config.GetEditableOptions = Configuration::GetEditableOptions;
    LoaderProtocol.Config.GetValue = Configuration::GetValue;
    LoaderProtocol.Config.SetBootOptionValue = Configuration::SetBootOptionValue;
    LoaderProtocol.Console.ClearLine = Console::ClearLine;
    LoaderProtocol.Console.ClearScreen = Console::ClearScreen;
    LoaderProtocol.Console.DisableCursor = Console::DisableCursor;
    LoaderProtocol.Console.EnableCursor = Console::EnableCursor;
    LoaderProtocol.Console.Print = Console::Print;
    LoaderProtocol.Console.QueryMode = Console::QueryMode;
    LoaderProtocol.Console.ReadKeyStroke = Console::ReadKeyStroke;
    LoaderProtocol.Console.ResetInputBuffer = Console::ResetInputBuffer;
    LoaderProtocol.Console.SetAttributes = Console::SetAttributes;
    LoaderProtocol.Console.SetCursorPosition = Console::SetCursorPosition;
    LoaderProtocol.Console.Write = Console::Write;
    LoaderProtocol.Cpu.CpuId = AR::CpuFunc::CpuId;
    LoaderProtocol.Cpu.ReadControlRegister = AR::CpuFunc::ReadControlRegister;
    LoaderProtocol.Cpu.ReadModelSpecificRegister = AR::CpuFunc::ReadModelSpecificRegister;
    LoaderProtocol.Cpu.WriteControlRegister = AR::CpuFunc::WriteControlRegister;
    LoaderProtocol.Debug.Print = Debug::Print;
    LoaderProtocol.Disk.CloseVolume = Volume::CloseVolume;
    LoaderProtocol.Disk.OpenVolume = Volume::OpenVolume;
    LoaderProtocol.Disk.ReadFile = Volume::ReadFile;
    LoaderProtocol.IoPort.Read8 = HL::IoPort::ReadPort8;
    LoaderProtocol.IoPort.Read16 = HL::IoPort::ReadPort16;
    LoaderProtocol.IoPort.Read32 = HL::IoPort::ReadPort32;
    LoaderProtocol.IoPort.Write8 = HL::IoPort::WritePort8;
    LoaderProtocol.IoPort.Write16 = HL::IoPort::WritePort16;
    LoaderProtocol.IoPort.Write32 = HL::IoPort::WritePort32;
    LoaderProtocol.LinkedList.InitializeHead = RTL::LinkedList::InitializeListHead;
    LoaderProtocol.LinkedList.InsertHead = RTL::LinkedList::InsertHeadList;
    LoaderProtocol.LinkedList.InsertTail = RTL::LinkedList::InsertTailList;
    LoaderProtocol.LinkedList.RemoveEntry = RTL::LinkedList::RemoveEntryList;
    LoaderProtocol.Memory.AllocatePages = Memory::AllocatePages;
    LoaderProtocol.Memory.AllocatePool = Memory::AllocatePool;
    LoaderProtocol.Memory.BuildPageMap = Memory::BuildPageMap;
    LoaderProtocol.Memory.CompareMemory = RTL::Memory::CompareMemory;
    LoaderProtocol.Memory.CopyMemory = RTL::Memory::CopyMemory;
    LoaderProtocol.Memory.FreePages = Memory::FreePages;
    LoaderProtocol.Memory.FreePool = Memory::FreePool;
    LoaderProtocol.Memory.GetMappingsCount = Memory::GetMappingsCount;
    LoaderProtocol.Memory.GetMemoryMap = Memory::GetMemoryMap;
    LoaderProtocol.Memory.GetVirtualAddress = Memory::GetVirtualAddress;
    LoaderProtocol.Memory.InitializePageMap = Memory::InitializePageMap;
    LoaderProtocol.Memory.MapEfiMemory = Memory::MapEfiMemory;
    LoaderProtocol.Memory.MapPage = Memory::MapPage;
    LoaderProtocol.Memory.MapVirtualMemory = Memory::MapVirtualMemory;
    LoaderProtocol.Memory.MoveMemory = RTL::Memory::MoveMemory;
    LoaderProtocol.Memory.PhysicalAddressToVirtual = Memory::PhysicalAddressToVirtual;
    LoaderProtocol.Memory.PhysicalListToVirtual = Memory::PhysicalListToVirtual;
    LoaderProtocol.Memory.SetMemory = RTL::Memory::SetMemory;
    LoaderProtocol.Memory.ZeroMemory = RTL::Memory::ZeroMemory;
    LoaderProtocol.Protocol.Close = CloseProtocol;
    LoaderProtocol.Protocol.GetModulesList = GetModulesList;
    LoaderProtocol.Protocol.Install = InstallProtocol;
    LoaderProtocol.Protocol.LocateHandles = LocateProtocolHandles;
    LoaderProtocol.Protocol.Open = OpenProtocol;
    LoaderProtocol.Protocol.OpenHandle = OpenProtocolHandle;
    LoaderProtocol.String.Compare = RTL::String::CompareString;
    LoaderProtocol.String.Length = RTL::String::StringLength;
    LoaderProtocol.String.ToWideString = RTL::String::StringToWideString;
    LoaderProtocol.String.Trim = RTL::String::TrimString;
    LoaderProtocol.Tui.DisplayErrorDialog = TextUi::DisplayErrorDialog;
    LoaderProtocol.Tui.DisplayInfoDialog = TextUi::DisplayInfoDialog;
    LoaderProtocol.Tui.DisplayInputDialog = TextUi::DisplayInputDialog;
    LoaderProtocol.Tui.DisplayProgressDialog = TextUi::DisplayProgressDialog;
    LoaderProtocol.Tui.UpdateProgressBar = TextUi::UpdateProgressBar;
    LoaderProtocol.Utils.EnterFirmwareSetup = EfiUtils::EnterFirmwareSetup;
    LoaderProtocol.Utils.ExitBootServices = EfiUtils::ExitBootServices;
    LoaderProtocol.Utils.GetConfigurationTable = EfiUtils::GetConfigurationTable;
    LoaderProtocol.Utils.GetEfiVariable = EfiUtils::GetEfiVariable;
    LoaderProtocol.Utils.GetRandomValue = EfiUtils::GetRandomValue;
    LoaderProtocol.Utils.GetSecureBootStatus = EfiUtils::GetSecureBootStatus;
    LoaderProtocol.Utils.InitializeEntropy = EfiUtils::InitializeEntropy;
    LoaderProtocol.Utils.LoadEfiImage = EfiUtils::LoadEfiImage;
    LoaderProtocol.Utils.RebootSystem = EfiUtils::RebootSystem;
    LoaderProtocol.Utils.SetEfiVariable = EfiUtils::SetEfiVariable;
    LoaderProtocol.Utils.ShutdownSystem = EfiUtils::ShutdownSystem;
    LoaderProtocol.Utils.SleepExecution = EfiUtils::SleepExecution;
    LoaderProtocol.Utils.StartEfiImage = EfiUtils::StartEfiImage;
    LoaderProtocol.Utils.WaitForEfiEvent = EfiUtils::WaitForEfiEvent;
    LoaderProtocol.WideString.Compare = RTL::WideString::CompareWideString;
    LoaderProtocol.WideString.CompareInsensitive = RTL::WideString::CompareWideStringInsensitive;
    LoaderProtocol.WideString.Concatenate = RTL::WideString::ConcatenateWideString;
    LoaderProtocol.WideString.Format = RTL::WideString::FormatWideString;
    LoaderProtocol.WideString.Length = RTL::WideString::WideStringLength;
    LoaderProtocol.WideString.Tokenize = RTL::WideString::TokenizeWideString;

    /* Register XTLDR loader protocol */
    Debug::Print(L"Registering XT loader protocol\n");
    return InstallProtocol(&LoaderProtocol, &Guid);
}
