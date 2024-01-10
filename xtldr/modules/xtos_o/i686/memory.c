/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/i686/memory.c
 * DESCRIPTION:     EFI memory management for i686 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Builds the actual memory mapping page table and enables paging. This routine exits EFI boot services as well.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a pointer to the next valid, free and available virtual address.
 *
 * @param ImageProtocol
 *        A pointer to the EFI loaded image protocol with information about where in memory the loader code was placed.
 *
 * @param PtePointer
 *        Supplies a pointer to memory area containing a Page Table Entries (PTE).
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtEnablePaging(IN PLIST_ENTRY MemoryMappings,
               IN PVOID VirtualAddress,
               IN PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol,
               IN PVOID *PtePointer)
{
    UINT_PTR PhysicalAddress, DescriptorCount;
    EFI_PHYSICAL_ADDRESS Address, PDPTAddress = 0;
    PCPUID_REGISTERS CpuRegisters = NULL;
    PEFI_MEMORY_DESCRIPTOR Descriptor;
    PLOADER_MEMORY_MAPPING Mapping;
    PEFI_MEMORY_MAP MemoryMap;
    PLIST_ENTRY ListEntry, ModulesList, ModulesListEntry;
    PXTBL_MODULE_INFO ModuleInfo;
    EFI_STATUS Status;
    UINT Index;

    /* Prepare CPUID registers */
    CpuRegisters->Leaf = CPUID_GET_CPU_FEATURES;
    CpuRegisters->SubLeaf = 0;
    CpuRegisters->Eax = 0;
    CpuRegisters->Ebx = 0;
    CpuRegisters->Ecx = 0;
    CpuRegisters->Edx = 0;

    /* Get CPUID */
    ArCpuId(CpuRegisters);

    /* Store PAE status from the CPUID results */
    if(!(CpuRegisters->Edx & CPUID_FEATURES_EDX_PAE))
    {
        /* No PAE support */
        XtLdrProtocol->Debug.Print(L"ERROR: PAE extension not supported by the CPU\n");
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Allocate and zero-fill buffer for EFI memory map */
    XtLdrProtocol->Memory.AllocatePool(sizeof(EFI_MEMORY_MAP), (PVOID*)&MemoryMap);
    RtlZeroMemory(MemoryMap, sizeof(EFI_MEMORY_MAP));

    /* Get EFI memory map */
    Status = XtLdrProtocol->Memory.GetMemoryMap(MemoryMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to get memory map */
        return Status;
    }

    /* Calculate descriptors count and get first one */
    Descriptor = MemoryMap->Map;
    DescriptorCount = MemoryMap->MapSize / MemoryMap->DescriptorSize;

    /* Calculate physical address based on KSEG0 base */
    PhysicalAddress = (UINT_PTR)VirtualAddress - KSEG0_BASE;

    /* Iterate over all descriptors from memory map to find satisfying address for PDPT */
    for(Index = 0; Index < DescriptorCount; Index++)
    {
        /* Check descriptor if it can be used to store PDPT */
        if((Descriptor->PhysicalStart + ((Descriptor->NumberOfPages - 1) * EFI_PAGE_SIZE) >= PhysicalAddress) &&
           (Descriptor->Type == EfiConventionalMemory))
        {
            /* Use highest address possible */
            if(PhysicalAddress >= Descriptor->PhysicalStart)
            {
                /* Use physical address */
                PDPTAddress = PhysicalAddress;
            }
            else
            {
                /* Use descriptor physical start as PDPT address */
                PDPTAddress = Descriptor->PhysicalStart;
            }

            /* Allocate pages for the PDPT address */
            Status = XtLdrProtocol->Memory.AllocatePages(1, &PDPTAddress);
            if(Status != STATUS_EFI_SUCCESS) {
                return Status;
            }
            break;
        }

        /* Get next descriptor */
        Descriptor = (EFI_MEMORY_DESCRIPTOR*)((UINT8*)Descriptor + MemoryMap->DescriptorSize);
    }

    /* Make sure PDPT address found */
    if(PDPTAddress == 0)
    {
        /* No suitable area for PDPT found in EFI memory map */
        return STATUS_EFI_NOT_FOUND;
    }

    /* Set virtual address based on new PDPT address mapped to KSEG0 base */
    VirtualAddress = (PVOID)(UINT_PTR)(PDPTAddress + EFI_PAGE_SIZE + KSEG0_BASE);

    /* Set base page frame number */
    Address = 0x100000;

    /* Allocate pages for the PFN */
    Status = XtLdrProtocol->Memory.AllocatePages(4, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Set and zero memory used by page mappings and CR3 */
    *PtePointer = (PVOID)(UINT_PTR)PDPTAddress;
    RtlZeroMemory(*PtePointer, EFI_PAGE_SIZE);
    RtlZeroMemory((PVOID)Address, EFI_PAGE_SIZE * 4);

    /* Set the page directory into the PDPT and mark it present */
    for(Index = 0; Index < 4; Index++)
    {
        /* Set paging entry settings */
        ((PHARDWARE_PTE)*PtePointer)[Index].PageFrameNumber = Address / EFI_PAGE_SIZE;
        ((PHARDWARE_PTE)*PtePointer)[Index].Valid = 1;

        /* Next valid PFN address */
        Address += EFI_PAGE_SIZE;
    }

    /* Get list of XTLDR modules */
    ModulesList = XtLdrProtocol->Protocol.GetModulesList();
    ModulesListEntry = ModulesList->Flink;
    while(ModulesListEntry != ModulesList)
    {
        /* Get module info */
        ModuleInfo = CONTAIN_RECORD(ModulesListEntry, XTBL_MODULE_INFO, Flink);

        /* Map module code */
        XtAddVirtualMemoryMapping(MemoryMappings, ModuleInfo->ModuleBase, ModuleInfo->ModuleBase,
                                  EFI_SIZE_TO_PAGES(ModuleInfo->ModuleSize), LoaderFirmwareTemporary);

        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Mapping module code failed */
            return Status;
        }

        /* Get next module */
        ModulesListEntry = ModulesListEntry->Flink;
    }

    /* Map XTLDR code */
    XtAddVirtualMemoryMapping(MemoryMappings, ImageProtocol->ImageBase, ImageProtocol->ImageBase,
                                       EFI_SIZE_TO_PAGES(ImageProtocol->ImageSize), LoaderFirmwareTemporary);

    /* Add page mapping itself to memory mapping */
    Status = XtAddVirtualMemoryMapping(MemoryMappings, NULL, *PtePointer, 1, LoaderMemoryData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping PD failed */
        return Status;
    }

    /* Iterate through and map all the mappings */
    XtLdrProtocol->Debug.Print(L"Mapping and dumping EFI memory:\n");
    ListEntry = MemoryMappings->Flink;
    while(ListEntry != MemoryMappings)
    {
        /* Take mapping from the list */
        Mapping = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_MAPPING, ListEntry);

        /* Check if virtual address is set */
        if(Mapping->VirtualAddress)
        {
            /* Dump memory mapping */
            XtLdrProtocol->Debug.Print(L"   Type=%02lu, PhysicalBase=0x%08lx, VirtualBase=0x%08lx, Pages=%lu\n", Mapping->MemoryType,
                                       Mapping->PhysicalAddress, Mapping->VirtualAddress, Mapping->NumberOfPages);

            /* Map memory */
            Status = XtMapVirtualMemory(MemoryMappings, (UINT_PTR)Mapping->VirtualAddress,
                                        (UINT_PTR)Mapping->PhysicalAddress, Mapping->NumberOfPages, PtePointer);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory mapping failed */
                return Status;
            }
        }

        /* Take next element */
        ListEntry = ListEntry->Flink;
    }

    /* Map zero page as well */
    XtMapVirtualMemory(MemoryMappings, 0, 0, 1, PtePointer);

    /* Exit EFI Boot Services */
    XtLdrProtocol->Debug.Print(L"Exiting EFI boot services\n");
    Status = XtLdrProtocol->Util.ExitBootServices();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to exit boot services */
        XtLdrProtocol->Debug.Print(L"Failed to exit boot services (Status code: %lx)\n", Status);
        return STATUS_EFI_ABORTED;
    }

    /* Enable Physical Address Extension (PAE) */
    ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_PAE);

    /* Write page mappings to CR3 */
    ArWriteControlRegister(3, (UINT_PTR)*PtePointer);

    /* Enable paging */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_PG);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine does the actual virtual memory mapping.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a virtual address of the mapping.
 *
 * @param PhysicalAddress
 *        Supplies a physical address of the mapping.
 *
 * @param NumberOfPages
 *        Supplies a number of the pages of the mapping.
 *
 * @param PaeExtension
 *        Specifies whether Physical Address Extension (PAE) is supported by the hardware.
 *
 * @param PtePointer
 *        Supplies a pointer to an array of pointers to page table entries.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtMapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                   IN UINT_PTR VirtualAddress,
                   IN UINT_PTR PhysicalAddress,
                   IN UINT NumberOfPages,
                   IN OUT PVOID *PtePointer)
{
    EFI_PHYSICAL_ADDRESS Address;
    UINT_PTR PageFrameNumber;
    PHARDWARE_PTE PageTable, PageDirectory;
    EFI_STATUS Status;
    unsigned int PdIndex, PtIndex;

    /* Set the PFN */
    PageFrameNumber = PhysicalAddress >> EFI_PAGE_SHIFT;

    /* Do the recursive mapping */
    while(NumberOfPages > 0)
    {
        /* Find Page Directory and calculate indices from a virtual address */
        PageDirectory = (PHARDWARE_PTE)(UINT_PTR)(((PHARDWARE_PTE)(*PtePointer))[VirtualAddress >> 30].PageFrameNumber * EFI_PAGE_SIZE);
        PdIndex = (VirtualAddress >> 21) & 0x1FF;
        PtIndex = (VirtualAddress & 0x1FF000) >> 12;

        /* Validate Page Directory */
        if(!PageDirectory[PdIndex].Valid) {
            /* Allocate pages for new page table */
            Status = XtLdrProtocol->Memory.AllocatePages(1, &Address);
            if(Status != STATUS_EFI_SUCCESS) {
                /* Memory allocation failure */
                return Status;
            }

            /* Fill allocated memory with zeros */
            RtlZeroMemory((PVOID)(UINT_PTR)Address, EFI_PAGE_SIZE);

            /* Set paging entry settings */
            PageDirectory[PdIndex].PageFrameNumber = Address / EFI_PAGE_SIZE;
            PageDirectory[PdIndex].Valid = 1;
            PageDirectory[PdIndex].Write = 1;

            /* Set page table */
            PageTable = (PHARDWARE_PTE)(UINT_PTR)Address;
        }
        else
        {
            /* Set page table */
            PageTable = (PHARDWARE_PTE)(UINT_PTR)(PageDirectory[PdIndex].PageFrameNumber * EFI_PAGE_SIZE);
        }
        /* Set page table settings */
        PageTable[PtIndex].PageFrameNumber = PageFrameNumber;
        PageTable[PtIndex].Valid = 1;
        PageTable[PtIndex].Write = 1;

        /* Take next virtual address and PFN */
        VirtualAddress += EFI_PAGE_SIZE;
        PageFrameNumber++;

        /* Decrease number of pages left */
        NumberOfPages--;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
