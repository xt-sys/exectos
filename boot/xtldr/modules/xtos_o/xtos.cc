/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/xtos.cc
 * DESCRIPTION:     XTOS boot protocol support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* XTOS module information */
MODULE_AUTHOR(L"Rafal Kupiec <belliash@codingworkshop.eu.org>");
MODULE_DESCRIPTION(L"XTOS boot protocol support");
MODULE_DEPENDENCY(L"acpi framebuf pecoff");
MODULE_LICENSE(L"GPLv3");
MODULE_VERSION(L"0.1");


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
XTCDECL
EFI_STATUS
Xtos::BootSystem(IN PXTBL_BOOT_PARAMETERS Parameters)
{
    EFI_GUID PeCoffProtocolGuid = XT_PECOFF_IMAGE_PROTOCOL_GUID;
    EFI_HANDLE DiskHandle, ProtocolHandle;
    PEFI_FILE_HANDLE FsHandle, BootDir;
    PWCHAR SystemPath;
    EFI_STATUS Status;

    /* Print debug message */
    XtLdrProtocol->Debug.Print(L"XTOS boot protocol activated\n");

    /* Open the XT PE/COFF protocol */
    Status = XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID *)&PeCoffProtocol, &PeCoffProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to load PE/COFF image protocol\n");
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Check device path */
    if(Parameters->DevicePath == NULLPTR)
    {
        /* No device path set */
        XtLdrProtocol->Debug.Print(L"ERROR: No device path provided, unable to boot system\n");
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Check if system path is set */
    if(Parameters->SystemPath != NULLPTR)
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
                XtLdrProtocol->Debug.Print(L"ERROR: System path does not point to the valid XTOS installation\n");
                return STATUS_EFI_INVALID_PARAMETER;
            }
            /* Check next character in the path */
            SystemPath++;
        }
    }
    else
    {
        /* Fallback to '/ExectOS' by default */
        XtLdrProtocol->Debug.Print(L"WARNING: No system path set, falling back to defaults\n");
        Parameters->SystemPath = (PWCHAR)L"\\ExectOS";
    }

    /* Check if kernel file is set */
    if(Parameters->KernelFile == NULLPTR)
    {
        /* No kernel filename set, fallback to default */
        XtLdrProtocol->Debug.Print(L"WARNING: No kernel file specified, falling back to defaults\n");
        Parameters->KernelFile = (PWCHAR)L"xtoskrnl.exe";
    }

    /* Check if provided any kernel boot arguments */
    if(Parameters->Parameters == NULLPTR)
    {
        /* No argument supplied */
        Parameters->Parameters = (PWCHAR)L"";
    }

    /* Print a debug message */
    XtLdrProtocol->Debug.Print(L"[XTOS] ARC Path: %S\n"
                               L"[XTOS] System Path: %S\n"
                               L"[XTOS] Kernel File: %S\n"
                               L"[XTOS] Boot Arguments: %S\n",
                               Parameters->ArcName, Parameters->SystemPath,
                               Parameters->KernelFile, Parameters->Parameters);

    /* Open EFI volume */
    Status = XtLdrProtocol->Disk.OpenVolume(Parameters->DevicePath, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to open boot volume\n");
        return Status;
    }

    /* System path has to point to the boot directory */
    XtLdrProtocol->WideString.Concatenate(Parameters->SystemPath, (PWCHAR)L"\\Boot", 0);

    /* Open XTOS system boot directory */
    Status = FsHandle->Open(FsHandle, &BootDir, Parameters->SystemPath, EFI_FILE_MODE_READ, 0);
    FsHandle->Close(FsHandle);

    /* Check if system path directory opened successfully */
    if(Status == STATUS_EFI_NOT_FOUND)
    {
        /* Directory not found, nothing to load */
        XtLdrProtocol->Debug.Print(L"ERROR: System boot directory not found\n");

        /* Close volume */
        XtLdrProtocol->Disk.CloseVolume(&DiskHandle);
        return Status;
    }
    else if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to open system boot directory\n");
        XtLdrProtocol->Disk.CloseVolume(&DiskHandle);
        return Status;
    }

    /* Start boot sequence */
    return RunBootSequence(BootDir, Parameters);
}

/**
 * Returns information about frame buffer in XTOS compatible format.
 *
 * @param InformationBlock
 *        A pointer to memory area containing XT structure where all the information will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
Xtos::GetDisplayInformation(OUT PSYSTEM_RESOURCE_FRAMEBUFFER FrameBufferResource,
                            IN PEFI_PHYSICAL_ADDRESS FrameBufferBase,
                            IN PULONG_PTR FrameBufferSize,
                            IN PXTBL_FRAMEBUFFER_MODE_INFORMATION FrameBufferModeInfo)
{
    /* Fill in frame buffer resource */
    FrameBufferResource->Header.PhysicalAddress = (PVOID)*FrameBufferBase;
    FrameBufferResource->Header.ResourceType = SystemResourceFrameBuffer;
    FrameBufferResource->Header.ResourceSize = sizeof(SYSTEM_RESOURCE_FRAMEBUFFER);
    FrameBufferResource->BufferSize = *FrameBufferSize;
    FrameBufferResource->Width = FrameBufferModeInfo->Width;
    FrameBufferResource->Height = FrameBufferModeInfo->Height;
    FrameBufferResource->Depth = FrameBufferModeInfo->Depth;
    FrameBufferResource->BitsPerPixel = FrameBufferModeInfo->BitsPerPixel;
    FrameBufferResource->PixelsPerScanLine = FrameBufferModeInfo->PixelsPerScanLine;
    FrameBufferResource->Pitch = FrameBufferModeInfo->Pitch;
    FrameBufferResource->Pixels.BlueShift = FrameBufferModeInfo->PixelInformation.BlueShift;
    FrameBufferResource->Pixels.BlueSize = FrameBufferModeInfo->PixelInformation.BlueSize;
    FrameBufferResource->Pixels.GreenShift = FrameBufferModeInfo->PixelInformation.GreenShift;
    FrameBufferResource->Pixels.GreenSize = FrameBufferModeInfo->PixelInformation.GreenSize;
    FrameBufferResource->Pixels.RedShift = FrameBufferModeInfo->PixelInformation.RedShift;
    FrameBufferResource->Pixels.RedSize = FrameBufferModeInfo->PixelInformation.RedSize;
    FrameBufferResource->Pixels.ReservedShift = FrameBufferModeInfo->PixelInformation.ReservedShift;
    FrameBufferResource->Pixels.ReservedSize = FrameBufferModeInfo->PixelInformation.ReservedSize;
}

XTCDECL
EFI_STATUS
Xtos::GetMemoryDescriptorList(IN PXTBL_PAGE_MAPPING PageMap,
                              IN PVOID *VirtualAddress,
                              OUT PLIST_ENTRY MemoryDescriptorList)
{
    EFI_PHYSICAL_ADDRESS Address;
    EFI_STATUS Status;
    ULONGLONG Pages;

    Pages = (ULONGLONG)EFI_SIZE_TO_PAGES((PageMap->MapSize + 1) * sizeof(LOADER_MEMORY_DESCRIPTOR));

    Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, Pages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    Status = XtLdrProtocol->Memory.MapVirtualMemory(PageMap, (ULONGLONG)*VirtualAddress, Address, Pages, LoaderMemoryData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        XtLdrProtocol->Memory.FreePages(Address, Pages);
        return Status;
    }

    PVOID PhysicalBase = (PVOID)Address;

    PLIST_ENTRY ListEntry;
    ListEntry = PageMap->MemoryMap.Flink;
    while(ListEntry != &PageMap->MemoryMap)
    {
        PXTBL_MEMORY_MAPPING MemoryMapping = CONTAIN_RECORD(ListEntry, XTBL_MEMORY_MAPPING, ListEntry);
        PLOADER_MEMORY_DESCRIPTOR MemoryDescriptor = (PLOADER_MEMORY_DESCRIPTOR)Address;

        MemoryDescriptor->MemoryType = MemoryMapping->MemoryType;
        MemoryDescriptor->BasePage = (UINT_PTR)MemoryMapping->PhysicalAddress / EFI_PAGE_SIZE;
        MemoryDescriptor->PageCount = MemoryMapping->NumberOfPages;

        XtLdrProtocol->LinkedList.InsertTail(MemoryDescriptorList, &MemoryDescriptor->ListEntry);

        Address = Address + sizeof(LOADER_MEMORY_DESCRIPTOR);
        ListEntry = ListEntry->Flink;
    }

    XtLdrProtocol->Memory.PhysicalListToVirtual(PageMap, MemoryDescriptorList, PhysicalBase, *VirtualAddress);

    return STATUS_EFI_SUCCESS;
}

XTCDECL
EFI_STATUS
Xtos::GetSystemResourcesList(IN PXTBL_PAGE_MAPPING PageMap,
                             IN PVOID *VirtualAddress,
                             OUT PLIST_ENTRY SystemResourcesList)
{
    XTSTATUS Status;
    EFI_HANDLE ProtocolHandle;
    EFI_GUID AcpiGuid = XT_ACPI_PROTOCOL_GUID;
    EFI_GUID FrameBufGuid = XT_FRAMEBUFFER_PROTOCOL_GUID;
    PXTBL_ACPI_PROTOCOL AcpiProtocol;
    PXTBL_FRAMEBUFFER_PROTOCOL FrameBufProtocol;
    XTBL_FRAMEBUFFER_MODE_INFORMATION FbModeInfo;
    EFI_PHYSICAL_ADDRESS FbAddress;
    ULONG_PTR FbSize;
    UINT FrameBufferPages;
    PSYSTEM_RESOURCE_FRAMEBUFFER FrameBufferResource;
    PSYSTEM_RESOURCE_ACPI AcpiResource;
    ULONGLONG Pages;
    EFI_PHYSICAL_ADDRESS Address;
    PVOID PhysicalBase, VirtualBase;

    Pages = (ULONGLONG)EFI_SIZE_TO_PAGES(sizeof(SYSTEM_RESOURCE_ACPI) + sizeof(SYSTEM_RESOURCE_FRAMEBUFFER));

    Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, Pages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    Status = XtLdrProtocol->Memory.MapVirtualMemory(PageMap, (ULONGLONG)*VirtualAddress, Address, Pages, LoaderFirmwarePermanent);
    if(Status != STATUS_EFI_SUCCESS)
    {
        XtLdrProtocol->Memory.FreePages(Address, Pages);
        return Status;
    }

    PhysicalBase = (PVOID)Address;
    VirtualBase = *VirtualAddress;

    /* Calculate next valid virtual address */
    *VirtualAddress = (PUINT8)*VirtualAddress + (Pages * EFI_PAGE_SIZE);

    AcpiResource = (PSYSTEM_RESOURCE_ACPI)Address;

    XtLdrProtocol->Memory.ZeroMemory(AcpiResource, sizeof(SYSTEM_RESOURCE_ACPI));

    /* Load FrameBuffer protocol */
    Status = XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID*)&AcpiProtocol, &AcpiGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    AcpiResource->Header.ResourceType = SystemResourceAcpi;
    AcpiResource->Header.ResourceSize = sizeof(SYSTEM_RESOURCE_ACPI);

    /* Get APIC and XSDP/RSDP addresses */
    AcpiProtocol->GetApicBase(&AcpiResource->ApicBase);
    AcpiProtocol->GetAcpiDescriptionPointer(&AcpiResource->Header.PhysicalAddress);

    /* No need to map ACPI */
    AcpiResource->Header.VirtualAddress = 0;

    XtLdrProtocol->LinkedList.InsertTail(SystemResourcesList, &AcpiResource->Header.ListEntry);

    /* Close FrameBuffer protocol */
    XtLdrProtocol->Protocol.Close(&ProtocolHandle, &FrameBufGuid);

    Address = Address + sizeof(SYSTEM_RESOURCE_ACPI);

    FrameBufferResource = (PSYSTEM_RESOURCE_FRAMEBUFFER)Address;

    XtLdrProtocol->Memory.ZeroMemory(FrameBufferResource, sizeof(SYSTEM_RESOURCE_FRAMEBUFFER));

    /* Load FrameBuffer protocol */
    Status = XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID*)&FrameBufProtocol, &FrameBufGuid);
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Get FrameBuffer information */
        Status = FrameBufProtocol->GetDisplayInformation(&FbAddress, &FbSize, &FbModeInfo);
        if(Status == STATUS_EFI_SUCCESS)
        {

            /* Store information about FrameBuffer device */
            GetDisplayInformation(FrameBufferResource, &FbAddress, &FbSize, &FbModeInfo);
        }
    }
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    /* Calculate pages needed to map framebuffer */
    FrameBufferPages = EFI_SIZE_TO_PAGES(FbSize);

    /* Rewrite framebuffer address by using virtual address */
    FrameBufferResource->Header.VirtualAddress = *VirtualAddress;

    /* Map frame buffer memory */
    XtLdrProtocol->Memory.MapVirtualMemory(PageMap, (ULONGLONG)FrameBufferResource->Header.VirtualAddress,
                                           (ULONGLONG)FrameBufferResource->Header.PhysicalAddress,
                                           FrameBufferPages, LoaderFirmwarePermanent);

    /* Close FrameBuffer protocol */
    XtLdrProtocol->Protocol.Close(&ProtocolHandle, &FrameBufGuid);

    *VirtualAddress = (PUINT8)*VirtualAddress + (FrameBufferPages * EFI_PAGE_SIZE);

    XtLdrProtocol->LinkedList.InsertTail(SystemResourcesList, &FrameBufferResource->Header.ListEntry);

    XtLdrProtocol->Memory.PhysicalListToVirtual(PageMap, SystemResourcesList, PhysicalBase, VirtualBase);

    return STATUS_EFI_SUCCESS;
}

/**
 * Checks if APIC is present in the system and finds its base address.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @return This routine returns an EFI status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Xtos::InitializeApicBase(IN PXTBL_PAGE_MAPPING PageMap)
{
    EFI_GUID AcpiGuid = XT_ACPI_PROTOCOL_GUID;
    PXTBL_ACPI_PROTOCOL AcpiProtocol;
    EFI_HANDLE ProtocolHandle;
    PVOID ApicBaseAddress;
    EFI_STATUS Status;

    /* Open ACPI protocol */
    Status = XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID*)&AcpiProtocol, &AcpiGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* ACPI protocol not found */
        return Status;
    }

    /* Get APIC base address */
    Status = AcpiProtocol->GetApicBase(&ApicBaseAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get APIC base address */
        return Status;
    }

    /* Map APIC base address */
    XtLdrProtocol->Memory.MapVirtualMemory(PageMap, APIC_BASE, (ULONGLONG)ApicBaseAddress, 1, LoaderFirmwarePermanent);
    return STATUS_EFI_SUCCESS;
}

/**
 * Initializes and maps the kernel initialization block.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a pointer to the next valid, free and available virtual address.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Xtos::InitializeLoaderBlock(IN PXTBL_PAGE_MAPPING PageMap,
                            IN PVOID *VirtualAddress,
                            IN PXTBL_BOOT_PARAMETERS Parameters)
{
    PKERNEL_INITIALIZATION_BLOCK LoaderBlock;
    EFI_PHYSICAL_ADDRESS Address;
    EFI_STATUS Status;
    UINT BlockPages;
    UINT ParametersSize;

    /* Calculate size of parameters */
    ParametersSize = (XtLdrProtocol->WideString.Length(Parameters->Parameters, 0) + 1) * sizeof(WCHAR);

    /* Calculate number of pages needed for initialization block */
    BlockPages = EFI_SIZE_TO_PAGES(sizeof(KERNEL_INITIALIZATION_BLOCK) + ParametersSize);

    /* Allocate memory for kernel initialization block */
    Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, BlockPages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Initialize and zero-fill kernel initialization block */
    LoaderBlock = (PKERNEL_INITIALIZATION_BLOCK)(UINT_PTR)Address;
    XtLdrProtocol->Memory.ZeroMemory(LoaderBlock, sizeof(KERNEL_INITIALIZATION_BLOCK) + ParametersSize);

    /* Set basic loader block properties */
    LoaderBlock->BlockSize = sizeof(KERNEL_INITIALIZATION_BLOCK);
    LoaderBlock->BlockVersion = INITIALIZATION_BLOCK_VERSION;
    LoaderBlock->ProtocolVersion = BOOT_PROTOCOL_VERSION;

    /* Set LoaderInformation block properties */
    LoaderBlock->LoaderInformation.DbgPrint = (PVOID)XtLdrProtocol->Debug.Print;

    /* Set FirmwareInformation block properties */
    LoaderBlock->FirmwareInformation.FirmwareType = SystemFirmwareEfi;
    // LoaderBlock->FirmwareInformation.EfiFirmware.EfiVersion = EfiSystemTable->Hdr.Revision;
    LoaderBlock->FirmwareInformation.EfiFirmware.EfiRuntimeServices = NULLPTR;

    /* Copy parameters to kernel initialization block */
    LoaderBlock->KernelParameters = (PWCHAR)((UINT_PTR)*VirtualAddress + sizeof(KERNEL_INITIALIZATION_BLOCK));
    XtLdrProtocol->Memory.CopyMemory((PVOID)((UINT_PTR)LoaderBlock + sizeof(KERNEL_INITIALIZATION_BLOCK)),
                  Parameters->Parameters,
                  ParametersSize);

    /* Map kernel initialization block */
    XtLdrProtocol->Memory.MapVirtualMemory(PageMap, (ULONGLONG)*VirtualAddress, (ULONGLONG)LoaderBlock,
                                           BlockPages, LoaderSystemBlock);

    /* Calculate next valid virtual address */
    *VirtualAddress = (PUINT8)*VirtualAddress + (BlockPages * EFI_PAGE_SIZE);

    XtLdrProtocol->LinkedList.InitializeHead(&LoaderBlock->SystemResourcesListHead);
    GetSystemResourcesList(PageMap, VirtualAddress, &LoaderBlock->SystemResourcesListHead);

    /* Initialize memory descriptor list */
    XtLdrProtocol->LinkedList.InitializeHead(&LoaderBlock->MemoryDescriptorListHead);
    GetMemoryDescriptorList(PageMap, VirtualAddress, &LoaderBlock->MemoryDescriptorListHead);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

XTCDECL
EFI_STATUS
Xtos::InitializeModule(IN EFI_HANDLE ImageHandle,
                       IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_XTOS_BOOT_PROTOCOL_GUID;
    EFI_STATUS Status;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set routines available via XTOS boot protocol */
    BootProtocol.BootSystem = Xtos::BootSystem;

    /* Register XTOS boot protocol */
    XtLdrProtocol->Boot.RegisterProtocol(L"XTOS", &Guid);

    /* Install XTOS protocol */
    return XtLdrProtocol->Protocol.Install(&BootProtocol, &Guid);
}

/**
 * Loads XTOS PE/COFF module.
 *
 * @param SystemDir
 *        An EFI handle to the opened system directory containing a module that will be loaded.
 *
 * @param FileName
 *        An on disk filename of the module that will be loaded.
 *
 * @param VirtualAddress
 *        Optional virtual address pointing to the memory area where PE/COFF file will be loaded.
 *
 * @param MemoryType
 *        Supplies the type of memory to be assigned to the memory descriptor.
 *
 * @param ImageContext
 *        Supplies pointer to the memory area where loaded PE/COFF image context will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Xtos::LoadModule(IN PEFI_FILE_HANDLE SystemDir,
                 IN PWCHAR FileName,
                 IN PVOID VirtualAddress,
                 IN LOADER_MEMORY_TYPE MemoryType,
                 OUT PPECOFF_IMAGE_CONTEXT *ImageContext)
{
    PEFI_FILE_HANDLE ModuleHandle;
    USHORT MachineType, SubSystem;
    EFI_STATUS Status;

    /* Print debug message */
    XtLdrProtocol->Debug.Print(L"Loading %S ... \n", FileName);

    /* Open module file */
    Status = SystemDir->Open(SystemDir, &ModuleHandle, FileName, EFI_FILE_MODE_READ, 0);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to open the file */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to open '%S'\n", FileName);
        return Status;
    }

    /* Load the PE/COFF image file into memory */
    Status = PeCoffProtocol->LoadImage(ModuleHandle, MemoryType, NULLPTR, (PVOID*)ImageContext);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to load the file */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to load '%S'\n", FileName);
        return Status;
    }

    /* Relocate the PE/COFF image file */
    Status = PeCoffProtocol->RelocateImage(*ImageContext, KSEG0_BASE + (ULONGLONG)(*ImageContext)->PhysicalAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to relocate the file */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to relocate '%S'\n", FileName);
        return Status;
    }

    /* Close image file */
    ModuleHandle->Close(ModuleHandle);

    /* Check PE/COFF image machine type compatibility */
    PeCoffProtocol->GetMachineType(*ImageContext, &MachineType);
    if(MachineType != _ARCH_IMAGE_MACHINE_TYPE)
    {
        /* Machine type mismatch */
        XtLdrProtocol->Debug.Print(L"ERROR: Loaded incompatible PE/COFF image (machine type mismatch)\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Check PE/COFF image subsystem */
    PeCoffProtocol->GetSubSystem(*ImageContext, &SubSystem);
    if(SubSystem != PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_KERNEL &&
       SubSystem != PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_APPLICATION &&
       SubSystem != PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_DRIVER)
    {
        XtLdrProtocol->Debug.Print(L"WARNING: Loaded PE/COFF image with non-XT subsystem set\n");
    }

    /* Print debug message */
    XtLdrProtocol->Debug.Print(L"Loaded %S at PA: %P, VA: %P\n", FileName,
                               (*ImageContext)->PhysicalAddress, KSEG0_BASE + (ULONGLONG)(*ImageContext)->VirtualAddress);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Adds a physical to virtual address mapping.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param PhysicalAddress
 *        Supplies a physical address which will be mapped.
 *
 * @param NumberOfPages
 *        Supplies a number of pages that will be mapped.
 *
 * @param MemoryType
 *        Supplies the type of mapped memory that will be assigned to the memory descriptor.
 *
 * @param KernelMapping
 *        Supplies a flag that indicates if mapping should be done in kernel mode or if identity mapping should be used.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Xtos::MapMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                IN ULONGLONG Address,
                IN ULONG NumberOfPages,
                IN LOADER_MEMORY_TYPE MemoryType,
                IN BOOLEAN KernelMapping)
{
    ULONGLONG BaseAddress;

    /* Check if kernel mode mapping */
    if(KernelMapping)
    {
        /* Map memory based on kernel base address */
        BaseAddress = KSEG0_BASE;
    }
    else
    {
        /* Use identity mapping */
        BaseAddress = (ULONGLONG)NULLPTR;
    }

    /* Map memory and return status code */
    return XtLdrProtocol->Memory.MapVirtualMemory(PageMap, BaseAddress + Address, Address, NumberOfPages, MemoryType);
}

/**
 * This routine initiates an XTOS boot sequence.
 *
 * @param BootDir
 *        An EFI handle to the XTOS boot directory.
 *
 * @param Parameters
 *        Input parameters with detailed system configuration like boot device or kernel path.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Xtos::RunBootSequence(IN PEFI_FILE_HANDLE BootDir,
                      IN PXTBL_BOOT_PARAMETERS Parameters)
{
    EFI_GUID LoadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID FrameBufGuid = XT_FRAMEBUFFER_PROTOCOL_GUID;
    PKERNEL_INITIALIZATION_BLOCK KernelParameters;
    PXTBL_FRAMEBUFFER_PROTOCOL FrameBufProtocol;
    PPECOFF_IMAGE_CONTEXT ImageContext = NULLPTR;
    PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol;
    PVOID VirtualAddress, VirtualMemoryArea;
    PXT_ENTRY_POINT KernelEntryPoint;
    EFI_HANDLE ProtocolHandle;
    EFI_STATUS Status;
    XTBL_PAGE_MAPPING PageMap;

    /* Initialize XTOS startup sequence */
    XtLdrProtocol->Debug.Print(L"Initializing XTOS startup sequence\n");

    /* Load FrameBuffer protocol */
    Status = XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID*)&FrameBufProtocol, &FrameBufGuid);
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Make sure FrameBuffer is initialized */
        FrameBufProtocol->Initialize();
        FrameBufProtocol->SetScreenResolution(0, 0);
    }

    /* Close FrameBuffer protocol */
    XtLdrProtocol->Protocol.Close(&ProtocolHandle, &FrameBufGuid);

    /* Set base virtual memory area for the kernel mappings */
    VirtualMemoryArea = (PVOID)KSEG0_BASE;
    VirtualAddress = (PVOID)(KSEG0_BASE + KSEG0_KERNEL_BASE);

    /* Initialize virtual memory mappings */
    XtLdrProtocol->Memory.InitializePageMap(&PageMap, DeterminePagingLevel(Parameters->Parameters), Size4K);

    Status = XtLdrProtocol->Memory.MapEfiMemory(&PageMap, &VirtualMemoryArea, NULLPTR);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    /* Load the kernel */
    Status = LoadModule(BootDir, Parameters->KernelFile, VirtualAddress, LoaderSystemCode, &ImageContext);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load the kernel */
        return Status;
    }

    /* Add kernel image memory mapping */
    Status = MapMemory(&PageMap, (ULONGLONG)ImageContext->PhysicalAddress,
                       ImageContext->ImagePages, LoaderSystemCode, TRUE);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to map kernel image memory */
        return Status;
    }

    /* Set next valid virtual address right after the kernel */
    VirtualAddress = (PUINT8)VirtualAddress + (ImageContext->ImagePages * EFI_PAGE_SIZE);

    /* Find and map APIC base address */
    Status = InitializeApicBase(&PageMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to setup kernel initialization block */
        XtLdrProtocol->Debug.Print(L"Failed to initialize APIC (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Store virtual address of kernel initialization block for future kernel call */
    KernelParameters = (PKERNEL_INITIALIZATION_BLOCK)VirtualAddress;

    /* Setup and map kernel initialization block */
    Status = InitializeLoaderBlock(&PageMap, &VirtualAddress, Parameters);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to setup kernel initialization block */
        XtLdrProtocol->Debug.Print(L"Failed to setup kernel initialization block (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Get kernel entry point */
    PeCoffProtocol->GetEntryPoint(ImageContext, (PVOID*)&KernelEntryPoint);

    /* Close boot directory handle */
    BootDir->Close(BootDir);

    /* Enable paging */
    XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID*)&ImageProtocol, &LoadedImageGuid);
    Status = EnablePaging(&PageMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to enable paging */
        XtLdrProtocol->Debug.Print(L"Failed to enable paging (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Call XTOS kernel */
    XtLdrProtocol->Debug.Print(L"Booting the XTOS kernel\n");
    KernelEntryPoint(KernelParameters);

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
XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    /* Initialize XTOS module */
    return Xtos::InitializeModule(ImageHandle, SystemTable);
}
