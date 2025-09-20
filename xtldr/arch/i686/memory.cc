/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/arch/i686/memory.cc
 * DESCRIPTION:     XT Boot Loader i686 specific memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.hh>


/**
 * Maps boot loader related code and builds page map.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Memory::BuildPageMap(IN PXTBL_PAGE_MAPPING PageMap,
                     IN ULONG_PTR SelfMapAddress)
{
    PLIST_ENTRY ListEntry, ModulesList, ModulesListEntry;
    EFI_PHYSICAL_ADDRESS Address, DirectoryAddress;
    PXTBL_MODULE_INFO ModuleInfo;
    PXTBL_MEMORY_MAPPING Mapping;
    EFI_STATUS Status;
    ULONG Index;

    /* Check the page map level to determine which paging structure to create */
    if(PageMap->PageMapLevel == 3)
    {
        /* Allocate a page for the 3-level page map structure (PAE enabled) */
        Status = AllocatePages(AllocateAnyPages, 1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failed, cannot proceed with page map creation */
            return Status;
        }

        /* Assign the allocated page to the page map and zero it out */
        PageMap->PtePointer = (PVOID)(UINT_PTR)Address;
        RTL::Memory::ZeroMemory(PageMap->PtePointer, EFI_PAGE_SIZE);

        /* Allocate 4 pages for the Page Directories (PDs) */
        Status = AllocatePages(AllocateAnyPages, 4, &DirectoryAddress);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failed, cannot proceed with page map creation */
            return Status;
        }

        /* Zero-fill the allocated memory for the Page Directories */
        RTL::Memory::ZeroMemory((PVOID)DirectoryAddress, EFI_PAGE_SIZE * 4);

        /* Fill the PDPT with pointers to the Page Directories */
        for(Index = 0; Index < 4; Index++)
        {
            RTL::Memory::ZeroMemory(&((PHARDWARE_MODERN_PTE)PageMap->PtePointer)[Index], sizeof(HARDWARE_MODERN_PTE));
            ((PHARDWARE_MODERN_PTE)PageMap->PtePointer)[Index].PageFrameNumber = DirectoryAddress / EFI_PAGE_SIZE;
            ((PHARDWARE_MODERN_PTE)PageMap->PtePointer)[Index].Valid = 1;
            DirectoryAddress += EFI_PAGE_SIZE;
        }
    }
    else
    {
        /* Allocate a page for the 2-level page map structure (PAE disabled) */
        Status = AllocatePages(AllocateAnyPages, 1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failed, cannot proceed with page map creation */
            return Status;
        }

        /* Assign the allocated page to the page map and zero it out */
        PageMap->PtePointer = (PVOID)(UINT_PTR)Address;
        RTL::Memory::ZeroMemory(PageMap->PtePointer, EFI_PAGE_SIZE);
    }

    /* Add page mapping itself to memory mapping */
    Status = SelfMapPml(PageMap, SelfMapAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* PML mapping failed */
        return Status;
    }

    /* Map the trampoline code area */
    Status = MapVirtualMemory(PageMap, (PVOID)MM_TRAMPOLINE_ADDRESS,(PVOID)MM_TRAMPOLINE_ADDRESS,
                              1, LoaderFirmwareTemporary);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping trampoline code failed */
        return Status;
    }

    /* Get list of XTLDR modules */
    ModulesList = Protocol::GetModulesList();
    ModulesListEntry = ModulesList->Flink;
    while(ModulesListEntry != ModulesList)
    {
        /* Get module info */
        ModuleInfo = CONTAIN_RECORD(ModulesListEntry, XTBL_MODULE_INFO, Flink);

        /* Map module code */
        Status = MapVirtualMemory(PageMap, ModuleInfo->ModuleBase, ModuleInfo->ModuleBase,
                                  EFI_SIZE_TO_PAGES(ModuleInfo->ModuleSize), LoaderFirmwareTemporary);

        /* Check if mapping succeeded */
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Mapping module code failed */
            return Status;
        }

        /* Get next module */
        ModulesListEntry = ModulesListEntry->Flink;
    }

    /* Make sure boot loader image base and size are set */
    if(BlpStatus.LoaderBase && BlpStatus.LoaderSize)
    {
        /* Map boot loader code as well */
        Status = MapVirtualMemory(PageMap, BlpStatus.LoaderBase, BlpStatus.LoaderBase,
                                  EFI_SIZE_TO_PAGES(BlpStatus.LoaderSize), LoaderFirmwareTemporary);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Mapping boot loader code failed */
            return Status;
        }
    }
    else
    {
        /* Boot loader image information re not available */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Iterate through and map all the mappings*/
    Debug::Print(L"Mapping and dumping EFI memory:\n");
    ListEntry = PageMap->MemoryMap.Flink;
    while(ListEntry != &PageMap->MemoryMap)
    {
        /* Take mapping from the list */
        Mapping = CONTAIN_RECORD(ListEntry, XTBL_MEMORY_MAPPING, ListEntry);

        /* Check if virtual address is set */
        if(Mapping->VirtualAddress)
        {
            /* Dump memory mapping */
            Debug::Print(L"   Type=%02lu, PhysicalBase=%.8P, VirtualBase=%.8P, Pages=%llu\n", Mapping->MemoryType,
                         Mapping->PhysicalAddress, Mapping->VirtualAddress, Mapping->NumberOfPages);

            /* Map memory */
            Status = MapPage(PageMap, (UINT_PTR)Mapping->VirtualAddress,
                             (UINT_PTR)Mapping->PhysicalAddress, Mapping->NumberOfPages);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory mapping failed */
                return Status;
            }
        }

        /* Take next element */
        ListEntry = ListEntry->Flink;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Returns next level of the Page Table.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param PageTable
 *        Supplies a pointer to the current Page Table.
 *
 * @param Entry
 *        Supplies an index of the current Page Table entry.
 *
 * @param NextPageTable
 *        Supplies a pointer to the memory area where the next Page Table level is returned.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Memory::GetNextPageTable(IN PXTBL_PAGE_MAPPING PageMap,
                         IN PVOID PageTable,
                         IN SIZE_T Entry,
                         OUT PVOID *NextPageTable)
{
    EFI_PHYSICAL_ADDRESS Address;
    ULONGLONG PmlPointer = 0;
    EFI_STATUS Status;
    PHARDWARE_LEGACY_PTE LegacyPmlTable;
    PHARDWARE_MODERN_PTE PmlTable;
    BOOLEAN ValidPte = FALSE;

    /* Check page map level to determine PTE size */
    if(PageMap->PageMapLevel >= 3)
    {
        /* 64-bit PTE for PML3 (PAE enabled) */
        PmlTable = (PHARDWARE_MODERN_PTE)PageTable;
        if(PmlTable[Entry].Valid)
        {
            /* Get page frame number from page table entry */
            PmlPointer = PmlTable[Entry].PageFrameNumber;
            ValidPte = TRUE;
        }
    }
    else
    {
        /* 32-bit PTE for PML2 (PAE disabled) */
        LegacyPmlTable = (PHARDWARE_LEGACY_PTE)PageTable;
        if(LegacyPmlTable[Entry].Valid)
        {
            /* Get page frame number from page table entry */
            PmlPointer = LegacyPmlTable[Entry].PageFrameNumber;
            ValidPte = TRUE;
        }
    }

    /* Check if page table entry is valid */
    if(ValidPte)
    {
        /* Calculate the base address of the next page table */
        PmlPointer <<= EFI_PAGE_SHIFT;
    }
    else
    {
        /* Allocate pages for new PML entry */
        Status = AllocatePages(AllocateAnyPages, 1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            return Status;
        }

        /* Add new memory mapping */
        Status = MapVirtualMemory(PageMap, NULLPTR, (PVOID)(UINT_PTR)Address, 1, LoaderMemoryData);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory mapping failure */
            return Status;
        }

        /* Fill allocated memory with zeros */
        RTL::Memory::ZeroMemory((PVOID)(ULONGLONG)Address, EFI_PAGE_SIZE);

        /* Set paging entry settings based on level */
        if(PageMap->PageMapLevel >= 3)
        {
            /* 64-bit PTE for PML3 (PAE enabled) */
            PmlTable = (PHARDWARE_MODERN_PTE)PageTable;
            PmlTable[Entry].PageFrameNumber = Address / EFI_PAGE_SIZE;
            PmlTable[Entry].Valid = 1;
            PmlTable[Entry].Writable = 1;
        }
        else
        {
            /* 32-bit PTE for PML2 (PAE disabled) */
            LegacyPmlTable = (PHARDWARE_LEGACY_PTE)PageTable;
            LegacyPmlTable[Entry].PageFrameNumber = (UINT32)(Address / EFI_PAGE_SIZE);
            LegacyPmlTable[Entry].Valid = 1;
            LegacyPmlTable[Entry].Writable = 1;
        }

        /* Return the address of the new page table */
        PmlPointer = (ULONGLONG)Address;
    }

    /* Set next Page Map Level (PML) */
    *NextPageTable = (PVOID)(ULONGLONG)PmlPointer;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Does the actual virtual memory mapping.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
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
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Memory::MapPage(IN PXTBL_PAGE_MAPPING PageMap,
                IN ULONG_PTR VirtualAddress,
                IN ULONG_PTR PhysicalAddress,
                IN ULONG NumberOfPages)
{
    SIZE_T PageFrameNumber;
    PVOID Pml1, Pml2, Pml3;
    SIZE_T Pml1Entry, Pml2Entry, Pml3Entry;
    PHARDWARE_LEGACY_PTE LegacyPmlTable;
    PHARDWARE_MODERN_PTE PmlTable;
    EFI_STATUS Status;

    /* Set the Page Frame Number (PFN) */
    PageFrameNumber = PhysicalAddress >> EFI_PAGE_SHIFT;

    /* Map all requested pages */
    while(NumberOfPages > 0)
    {
        /* Check the paging mode to use the correct page table structure */
        if(PageMap->PageMapLevel == 3)
        {
            /* Calculate the indices for PAE page tables */
            Pml3Entry = (VirtualAddress >> 30) & 0x3;
            Pml2Entry = (VirtualAddress >> 21) & 0x1FF;
            Pml1Entry = (VirtualAddress >> 12) & 0x1FF;

            /* Get Page Directory Pointer Table (PML3) */
            Pml3 = PageMap->PtePointer;

            /* Get Page Directory (PML2) */
            Status = GetNextPageTable(PageMap, Pml3, Pml3Entry, &Pml2);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get the Page Table, abort mapping */
                return Status;
            }

            /* Get Page Table (PML1) */
            Status = GetNextPageTable(PageMap, Pml2, Pml2Entry, &Pml1);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get the Page Table, abort mapping */
                return Status;
            }

            /* Set the 64-bit PTE entry */
            PmlTable = (PHARDWARE_MODERN_PTE)Pml1;
            RTL::Memory::ZeroMemory(&PmlTable[Pml1Entry], sizeof(HARDWARE_MODERN_PTE));
            PmlTable[Pml1Entry].PageFrameNumber = PageFrameNumber;
            PmlTable[Pml1Entry].Valid = 1;
            PmlTable[Pml1Entry].Writable = 1;
        }
        else
        {
            /* Calculate the indices for non-PAE page tables */
            Pml2Entry = (VirtualAddress >> 22) & 0x3FF;
            Pml1Entry = (VirtualAddress >> 12) & 0x3FF;

            /* Get Page Directory (PML2) */
            Pml2 = PageMap->PtePointer;

            /* Get Page Table (PML1) */
            Status = GetNextPageTable(PageMap, Pml2, Pml2Entry, &Pml1);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get the Page Table, abort mapping */
                return Status;
            }

            /* Set the 32-bit PTE entry */
            LegacyPmlTable = (PHARDWARE_LEGACY_PTE)Pml1;
            RTL::Memory::ZeroMemory(&LegacyPmlTable[Pml1Entry], sizeof(HARDWARE_LEGACY_PTE));
            LegacyPmlTable[Pml1Entry].PageFrameNumber = (UINT32)PageFrameNumber;
            LegacyPmlTable[Pml1Entry].Valid = 1;
            LegacyPmlTable[Pml1Entry].Writable = 1;
        }

        /* Take next virtual address and PFN */
        VirtualAddress += EFI_PAGE_SIZE;
        PageFrameNumber++;

        /* Decrease number of pages left */
        NumberOfPages--;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Creates a recursive self mapping for all PML levels.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param SelfMapAddress
 *        Supplies a virtual address of the page tables.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Memory::SelfMapPml(IN PXTBL_PAGE_MAPPING PageMap,
                   IN ULONG_PTR SelfMapAddress)
{
    PHARDWARE_LEGACY_PTE LegacyPml;
    PHARDWARE_MODERN_PTE Pml;
    ULONGLONG PmlIndex;
    ULONG Index;

    /* Check page map level */
    if(PageMap->PageMapLevel == 3)
    {
        /* Calculate PML index based on provided self map address */
        PmlIndex = (SelfMapAddress >> MM_PDI_SHIFT) & 0x1FF;

        /* Get Page Directory */
        Pml = (PHARDWARE_MODERN_PTE)(((PHARDWARE_MODERN_PTE)PageMap->PtePointer)[SelfMapAddress >> MM_PPI_SHIFT].PageFrameNumber * EFI_PAGE_SIZE);

        /* Add self-mapping for PML3 (PAE enabled) */
        for(Index = 0; Index < 4; Index++)
        {
            RTL::Memory::ZeroMemory(&Pml[PmlIndex + Index], sizeof(HARDWARE_MODERN_PTE));
            Pml[PmlIndex + Index].PageFrameNumber = ((PHARDWARE_MODERN_PTE)PageMap->PtePointer)[Index].PageFrameNumber;
            Pml[PmlIndex + Index].Valid = 1;
            Pml[PmlIndex + Index].Writable = 1;
        }
    }
    else
    {
        LegacyPml = (PHARDWARE_LEGACY_PTE)PageMap->PtePointer;

        /* Calculate PML index based on provided self map address */
        PmlIndex = (SelfMapAddress >> MM_PDI_LEGACY_SHIFT);

        /* Add self-mapping for PML2 (PAE disabled) */
        RTL::Memory::ZeroMemory(&LegacyPml[PmlIndex], sizeof(HARDWARE_LEGACY_PTE));
        LegacyPml[PmlIndex].PageFrameNumber = (UINT_PTR)PageMap->PtePointer / EFI_PAGE_SIZE;
        LegacyPml[PmlIndex].Valid = 1;
        LegacyPml[PmlIndex].Writable = 1;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
