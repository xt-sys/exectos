/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/xtos.c
 * DESCRIPTION:     XTOS boot protocol support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* XTOS module information */
XTBL_MODINFO = L"XTOS boot protocol support";
XTBL_MODDEPS = {L"fb_o", L"pecoff"};

/* EFI XT Loader Protocol */
PXTBL_LOADER_PROTOCOL XtLdrProtocol;

/* XTOS PE/COFF Image Protocol */
PXTBL_EXECUTABLE_IMAGE_PROTOCOL XtPeCoffProtocol;

/* XTOS Boot Protocol */
XTBL_BOOT_PROTOCOL XtBootProtocol;

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
XtBootSystem(IN PXTBL_BOOT_PARAMETERS Parameters)
{
    EFI_GUID PeCoffProtocolGuid = XT_PECOFF_IMAGE_PROTOCOL_GUID;
    EFI_HANDLE DiskHandle, ProtocolHandle;
    PEFI_FILE_HANDLE FsHandle, BootDir;
    PWCHAR SystemPath;
    EFI_STATUS Status;

    /* Print debug message */
    XtLdrProtocol->Debug.Print(L"XTOS boot protocol activated\n");

    /* Open the XT PE/COFF protocol */
    Status = XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID *)&XtPeCoffProtocol, &PeCoffProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to load PE/COFF image protocol\n");
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Check device path */
    if(Parameters->DevicePath == NULL)
    {
        /* No device path set */
        XtLdrProtocol->Debug.Print(L"ERROR: No device path provided, unable to boot system\n");
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
        Parameters->SystemPath = L"\\ExectOS";
    }

    /* Check if kernel file is set */
    if(Parameters->KernelFile == NULL)
    {
        /* No kernel filename set, fallback to default */
        XtLdrProtocol->Debug.Print(L"WARNING: No kernel file specified, falling back to defaults\n");
        Parameters->KernelFile = L"xtoskrnl.exe";
    }

    /* Check if provided any kernel boot arguments */
    if(Parameters->Parameters == NULL)
    {
        /* No argument supplied */
        Parameters->Parameters = L"";
    }

    /* Print a debug message */
    XtLdrProtocol->Debug.Print(L"[XTOS] ARC Path: %S\n"
                               L"[XTOS] System Path: %S\n"
                               L"[XTOS] Kernel File: %S\n"
                               L"[XTOS] Boot Arguments: %S\n",
                               Parameters->ArcName, Parameters->SystemPath,
                               Parameters->KernelFile, Parameters->Parameters);

    /* Open EFI volume */
    Status = XtLdrProtocol->Disk.OpenVolume(NULL, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to open boot volume\n");
        return Status;
    }

    /* System path has to point to the boot directory */
    RtlConcatenateWideString(Parameters->SystemPath, L"\\Boot", 0);

    /* Open XTOS system boot directory */
    Status = FsHandle->Open(FsHandle, &BootDir, Parameters->SystemPath, EFI_FILE_MODE_READ, 0);
    FsHandle->Close(FsHandle);

    /* Check if system path directory opened successfully */
    if(Status == STATUS_EFI_NOT_FOUND)
    {
        /* Directory not found, nothing to load */
        XtLdrProtocol->Debug.Print(L"ERROR: System boot directory not found\n");

        /* Close volume */
        XtLdrProtocol->Disk.CloseVolume(DiskHandle);
        return Status;
    }
    else if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        XtLdrProtocol->Debug.Print(L"ERROR: Unable to open system boot directory\n");
        XtLdrProtocol->Disk.CloseVolume(DiskHandle);
        return Status;
    }

    /* Start boot sequence */
    return XtpBootSequence(BootDir, Parameters);
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
XtpBootSequence(IN PEFI_FILE_HANDLE BootDir,
                IN PXTBL_BOOT_PARAMETERS Parameters)
{
    EFI_GUID LoadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PKERNEL_INITIALIZATION_BLOCK KernelParameters;
    PPECOFF_IMAGE_CONTEXT ImageContext = NULL;
    PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol;
    PVOID VirtualAddress, VirtualMemoryArea;
    PXT_ENTRY_POINT KernelEntryPoint;
    EFI_HANDLE ProtocolHandle;
    EFI_STATUS Status;
    XTBL_PAGE_MAPPING PageMap;

    /* Initialize XTOS startup sequence */
    XtLdrProtocol->Debug.Print(L"Initializing XTOS startup sequence\n");

    /* Set base virtual memory area for the kernel mappings */
    VirtualMemoryArea = (PVOID)KSEG0_BASE;
    VirtualAddress = (PVOID)(KSEG0_BASE + KSEG0_KERNEL_BASE);

    /* Initialize virtual memory mappings */
    XtLdrProtocol->Memory.InitializePageMap(&PageMap, 3, Size4K);

    Status = XtLdrProtocol->Memory.MapEfiMemory(&PageMap, &VirtualMemoryArea);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    /* Load the kernel */
    Status = XtpLoadModule(BootDir, Parameters->KernelFile, VirtualAddress, LoaderSystemCode, &ImageContext);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load the kernel */
        return Status;
    }

    /* Add kernel image memory mapping */
    Status = XtLdrProtocol->Memory.MapVirtualMemory(&PageMap, ImageContext->VirtualAddress,
                                                    ImageContext->PhysicalAddress, ImageContext->ImagePages, 0);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    /* Set next valid virtual address right after the kernel */
    VirtualAddress += ImageContext->ImagePages * EFI_PAGE_SIZE;

    /* Store virtual address of kernel initialization block for future kernel call */
    KernelParameters = (PKERNEL_INITIALIZATION_BLOCK)VirtualAddress;

    /* Setup and map kernel initialization block */
    Status = XtpInitializeLoaderBlock(&PageMap, &VirtualAddress, Parameters);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to setup kernel initialization block */
        XtLdrProtocol->Debug.Print(L"Failed to setup kernel initialization block (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Find and map APIC base address */
    Status = XtpInitializeApicBase(&PageMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to setup kernel initialization block */
        XtLdrProtocol->Debug.Print(L"Failed to initialize APIC (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Get kernel entry point */
    XtPeCoffProtocol->GetEntryPoint(ImageContext, (PVOID)&KernelEntryPoint);

    /* Close boot directory handle */
    BootDir->Close(BootDir);

    /* Enable paging */
    XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID*)&ImageProtocol, &LoadedImageGuid);
    Status = XtEnablePaging(&PageMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to enable paging */
        XtLdrProtocol->Debug.Print(L"Failed to enable paging (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Call XTOS kernel */
    XtLdrProtocol->Debug.Print(L"Booting the XTOS kernel\n");
    KernelEntryPoint(KernelParameters);

    /* Return success */
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
XtpInitializeApicBase(IN PXTBL_PAGE_MAPPING PageMap)
{
    PCPUID_REGISTERS CpuRegisters = NULL;
    PVOID ApicBaseAddress;

    /* Get CPU features list */
    CpuRegisters->Leaf = CPUID_GET_CPU_FEATURES;
    CpuRegisters->SubLeaf = 0;
    CpuRegisters->Eax = 0;
    CpuRegisters->Ebx = 0;
    CpuRegisters->Ecx = 0;
    CpuRegisters->Edx = 0;
    ArCpuId(CpuRegisters);

    /* Check if APIC is present */
    if((CpuRegisters->Edx & CPUID_FEATURES_EDX_APIC) == 0)
    {
        /* APIC is not supported by the CPU */
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Get APIC base address */
    ApicBaseAddress = (PVOID)((UINT_PTR)ArReadModelSpecificRegister(0x1B) & 0xFFFFF000);

    /* Map APIC base address */
    XtLdrProtocol->Memory.MapVirtualMemory(PageMap, (PVOID)APIC_BASE, ApicBaseAddress, 1, LoaderFirmwarePermanent);
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
XtpInitializeLoaderBlock(IN PXTBL_PAGE_MAPPING PageMap,
                         IN PVOID *VirtualAddress,
                         IN PXTBL_BOOT_PARAMETERS Parameters)
{
    EFI_GUID FrameBufGuid = XT_FRAMEBUFFER_PROTOCOL_GUID;
    PXT_FRAMEBUFFER_PROTOCOL FrameBufProtocol;
    PKERNEL_INITIALIZATION_BLOCK LoaderBlock;
    EFI_PHYSICAL_ADDRESS Address;
    // PVOID RuntimeServices;
    EFI_STATUS Status;
    EFI_HANDLE ProtocolHandle;
    UINT BlockPages, FrameBufferPages;

    /* Calculate number of pages needed for initialization block */
    BlockPages = EFI_SIZE_TO_PAGES(sizeof(KERNEL_INITIALIZATION_BLOCK));

    /* Allocate memory for kernel initialization block */
    Status = XtLdrProtocol->Memory.AllocatePages(BlockPages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Initialize and zero-fill kernel initialization block */
    LoaderBlock = (PKERNEL_INITIALIZATION_BLOCK)(UINT_PTR)Address;
    RtlZeroMemory(LoaderBlock, sizeof(KERNEL_INITIALIZATION_BLOCK));

    /* Set basic loader block properties */
    LoaderBlock->BlockSize = sizeof(KERNEL_INITIALIZATION_BLOCK);
    LoaderBlock->BlockVersion = INITIALIZATION_BLOCK_VERSION;
    LoaderBlock->ProtocolVersion = BOOT_PROTOCOL_VERSION;

    /* Set LoaderInformation block properties */
    LoaderBlock->LoaderInformation.DbgPrint = XtLdrProtocol->Debug.Print;

    /* Load FrameBuffer protocol */
    Status = XtLdrProtocol->Protocol.Open(&ProtocolHandle, (PVOID*)&FrameBufProtocol, &FrameBufGuid);
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Make sure FrameBuffer is initialized */
        FrameBufProtocol->Initialize();

        /* Store information about FrameBuffer device */
        FrameBufProtocol->GetDisplayInformation(&LoaderBlock->LoaderInformation.FrameBuffer);
        // FrameBufProtocol->PrintDisplayInformation();
    }
    else
    {
        /* No FrameBuffer available */
        LoaderBlock->LoaderInformation.FrameBuffer.Initialized = FALSE;
        LoaderBlock->LoaderInformation.FrameBuffer.Protocol = NONE;
    }

    /* Close FrameBuffer protocol */
    XtLdrProtocol->Protocol.Close(ProtocolHandle, &FrameBufGuid);

    /* Attempt to find virtual address of the EFI Runtime Services */
    // Status = XtLdrProtocol->GetVirtualAddress(MemoryMappings, &EfiSystemTable->RuntimeServices->Hdr, &RuntimeServices);
    // if(Status == STATUS_EFI_SUCCESS)
    // {
        /* Set FirmwareInformation block properties */
        LoaderBlock->FirmwareInformation.FirmwareType = SystemFirmwareEfi;
        LoaderBlock->FirmwareInformation.EfiFirmware.EfiVersion = 0; //EfiSystemTable->Hdr.Revision;
        LoaderBlock->FirmwareInformation.EfiFirmware.EfiRuntimeServices = NULL;
    // }
    // else
    // {
    //     /* Set invalid firmware type to indicate that kernel cannot rely on FirmwareInformation block */
    //     LoaderBlock->FirmwareInformation.FirmwareType = SystemFirmwareInvalid;
    // }

    /* Copy parameters to kernel initialization block */
    RtlCopyMemory(&LoaderBlock->KernelParameters, Parameters->Parameters, RtlWideStringLength(Parameters->Parameters, 0));

    /* Map kernel initialization block */
    XtLdrProtocol->Memory.MapVirtualMemory(PageMap, *VirtualAddress, (PVOID)LoaderBlock,
                                           BlockPages, LoaderSystemBlock);

    /* Calculate next valid virtual address */
    *VirtualAddress += (UINT_PTR)(BlockPages * EFI_PAGE_SIZE);

    /* Check if framebuffer initialized */
    if(LoaderBlock->LoaderInformation.FrameBuffer.Initialized)
    {
        /* Calculate pages needed to map framebuffer */
        FrameBufferPages = EFI_SIZE_TO_PAGES(LoaderBlock->LoaderInformation.FrameBuffer.BufferSize);

        /* Map frame buffer memory */
        XtLdrProtocol->Memory.MapVirtualMemory(PageMap, *VirtualAddress,
                                               LoaderBlock->LoaderInformation.FrameBuffer.Address,
                                               FrameBufferPages, LoaderFirmwarePermanent);

        /* Rewrite framebuffer address by using virtual address */
        LoaderBlock->LoaderInformation.FrameBuffer.Address = *VirtualAddress;

        /* Calcualate next valid virtual address */
        *VirtualAddress += (UINT_PTR)(FrameBufferPages * EFI_PAGE_SIZE);
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
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
XtpLoadModule(IN PEFI_FILE_HANDLE SystemDir,
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

    /* Load the PE/COFF image file */
    Status = XtPeCoffProtocol->LoadImage(ModuleHandle, MemoryType, VirtualAddress, (PVOID)ImageContext);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to load the file */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to load '%S'\n", FileName);
        return Status;
    }

    /* Close image file */
    ModuleHandle->Close(ModuleHandle);

    /* Check PE/COFF image machine type compatibility */
    XtPeCoffProtocol->GetMachineType(*ImageContext, &MachineType);
    if(MachineType != _ARCH_IMAGE_MACHINE_TYPE)
    {
        /* Machine type mismatch */
        XtLdrProtocol->Debug.Print(L"ERROR: Loaded incompatible PE/COFF image (machine type mismatch)\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Check PE/COFF image subsystem */
    XtPeCoffProtocol->GetSubSystem(*ImageContext, &SubSystem);
    if(SubSystem != PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_KERNEL &&
       SubSystem != PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_APPLICATION &&
       SubSystem != PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_DRIVER)
    {
        XtLdrProtocol->Debug.Print(L"WARNING: Loaded PE/COFF image with non-XT subsystem set\n");
    }

    /* Print debug message */
    XtLdrProtocol->Debug.Print(L"Loaded %S at PA: 0x%lx, VA: 0x%lx\n", FileName,
                               (*ImageContext)->PhysicalAddress, (*ImageContext)->VirtualAddress);

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
    XtBootProtocol.BootSystem = XtBootSystem;

    /* Register XTOS boot protocol */
    XtLdrProtocol->Boot.RegisterProtocol(L"XTOS", &Guid);

    /* Install XTOS protocol */
    return XtLdrProtocol->Protocol.Install(&XtBootProtocol, &Guid);
}
