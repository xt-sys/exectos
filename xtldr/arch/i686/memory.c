/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/arch/i686/memory.c
 * DESCRIPTION:     XT Boot Loader i686 specific memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


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
BlBuildPageMap(IN PXTBL_PAGE_MAPPING PageMap,
               IN ULONG_PTR SelfMapAddress)
{
    PLIST_ENTRY ListEntry, ModulesList, ModulesListEntry;
    EFI_PHYSICAL_ADDRESS Address, DirectoryAddress;
    PXTBL_MODULE_INFO ModuleInfo;
    PXTBL_MEMORY_MAPPING Mapping;
    EFI_STATUS Status;
    ULONG Index;

    /* Check the page map level to determine which paging structure to create. */
    if(PageMap->PageMapLevel == 3)
    {
        /* Allocate a page for the 3-level page map structure (PAE enabled) */
        Status = BlAllocateMemoryPages(1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failed, cannot proceed with page map creation */
            return Status;
        }

        /* Assign the allocated page to the page map and zero it out */
        PageMap->PtePointer = (PVOID)(UINT_PTR)Address;
        RtlZeroMemory(PageMap->PtePointer, EFI_PAGE_SIZE);

        /* Allocate 4 pages for the Page Directories (PDs) */
        Status = BlAllocateMemoryPages(4, &DirectoryAddress);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failed, cannot proceed with page map creation */
            return Status;
        }

        /* Zero-fill the allocated memory for the Page Directories */
        RtlZeroMemory((PVOID)DirectoryAddress, EFI_PAGE_SIZE * 4);

        /* Fill the PDPT with pointers to the Page Directories */
        for(Index = 0; Index < 4; Index++)
        {
            RtlZeroMemory(&((PHARDWARE_PTE)PageMap->PtePointer)[Index], sizeof(HARDWARE_PTE));
            ((PHARDWARE_PTE)PageMap->PtePointer)[Index].PageFrameNumber = DirectoryAddress / EFI_PAGE_SIZE;
            ((PHARDWARE_PTE)PageMap->PtePointer)[Index].Valid = 1;
            DirectoryAddress += EFI_PAGE_SIZE;
        }
    }
    else
    {
        /* Allocate a page for the 2-level page map structure (PAE disabled) */
        Status = BlAllocateMemoryPages(1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failed, cannot proceed with page map creation */
            return Status;
        }

        /* Assign the allocated page to the page map and zero it out */
        PageMap->PtePointer = (PVOID)(UINT_PTR)Address;
        RtlZeroMemory(PageMap->PtePointer, EFI_PAGE_SIZE);
    }

    /* Add page mapping itself to memory mapping */
    Status = BlpSelfMapPml(PageMap, SelfMapAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* PML mapping failed */
        return Status;
    }

    /* Get list of XTLDR modules */
    ModulesList = BlGetModulesList();
    ModulesListEntry = ModulesList->Flink;
    while(ModulesListEntry != ModulesList)
    {
        /* Get module info */
        ModuleInfo = CONTAIN_RECORD(ModulesListEntry, XTBL_MODULE_INFO, Flink);

        /* Map module code */
        Status = BlMapVirtualMemory(PageMap, ModuleInfo->ModuleBase, ModuleInfo->ModuleBase,
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
        Status = BlMapVirtualMemory(PageMap, BlpStatus.LoaderBase, BlpStatus.LoaderBase,
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
    BlDebugPrint(L"Mapping and dumping EFI memory:\n");
    ListEntry = PageMap->MemoryMap.Flink;
    while(ListEntry != &PageMap->MemoryMap)
    {
        /* Take mapping from the list */
        Mapping = CONTAIN_RECORD(ListEntry, XTBL_MEMORY_MAPPING, ListEntry);

        /* Check if virtual address is set */
        if(Mapping->VirtualAddress)
        {
            /* Dump memory mapping */
            BlDebugPrint(L"   Type=%02lu, PhysicalBase=%.8P, VirtualBase=%.8P, Pages=%llu\n", Mapping->MemoryType,
                         Mapping->PhysicalAddress, Mapping->VirtualAddress, Mapping->NumberOfPages);

            /* Map memory */
            Status = BlMapPage(PageMap, (UINT_PTR)Mapping->VirtualAddress,
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
BlMapPage(IN PXTBL_PAGE_MAPPING PageMap,
          IN ULONG_PTR VirtualAddress,
          IN ULONG_PTR PhysicalAddress,
          IN ULONG NumberOfPages)
{
    SIZE_T Pml1Entry, Pml2Entry, Pml3Entry;
    PHARDWARE_PTE Pml1, Pml2, Pml3;
    SIZE_T PageFrameNumber;
    EFI_STATUS Status;

    /* Set the Page Frame Number (PFN) */
    PageFrameNumber = PhysicalAddress >> EFI_PAGE_SHIFT;

    /* Do the recursive mapping */
    while(NumberOfPages > 0)
    {
        /* Calculate the indices in the various Page Tables from the virtual address */
        Pml3Entry = (VirtualAddress & ((ULONGLONG)0x1FF << 30)) >> 30;
        Pml2Entry = (VirtualAddress & ((ULONGLONG)0x1FF << 21)) >> 21;
        Pml1Entry = (VirtualAddress & ((ULONGLONG)0x1FF << 12)) >> 12;

        /* Check page map level */
        if(PageMap->PageMapLevel == 3)
        {
            /* Three level Page Map */
            Pml3 = ((PHARDWARE_PTE)(PageMap->PtePointer));

            /* Get PML2 */
            Status = BlpGetNextPageTable(PageMap, Pml3, Pml3Entry, &Pml2);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory mapping failure */
                return Status;
            }
        }
        else
        {
            /* Two level Page Map */
            Pml2 = ((PHARDWARE_PTE)(PageMap->PtePointer));
        }

        /* Get PML1 */
        Status = BlpGetNextPageTable(PageMap, Pml2, Pml2Entry, &Pml1);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory mapping failure */
            return Status;
        }

        /* Set paging entry settings */
        RtlZeroMemory(&Pml1[Pml1Entry], sizeof(HARDWARE_PTE));
        Pml1[Pml1Entry].PageFrameNumber = PageFrameNumber;
        Pml1[Pml1Entry].Valid = 1;
        Pml1[Pml1Entry].Writable = 1;

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
BlpSelfMapPml(IN PXTBL_PAGE_MAPPING PageMap,
              IN ULONG_PTR SelfMapAddress)
{
    PHARDWARE_LEGACY_PTE LegacyPml;
    PHARDWARE_PTE Pml;
    ULONGLONG PmlIndex;
    ULONG Index;

    /* Check page map level */
    if(PageMap->PageMapLevel == 3)
    {
        /* Calculate PML index based on provided self map address */
        PmlIndex = (SelfMapAddress >> MM_PDI_SHIFT) & 0x1FF;

        /* Get Page Directory */
        Pml = (PHARDWARE_PTE)(((PHARDWARE_PTE)PageMap->PtePointer)[SelfMapAddress >> MM_PPI_SHIFT].PageFrameNumber * EFI_PAGE_SIZE);

        /* Add self-mapping for PML3 (PAE enabled) */
        for(Index = 0; Index < 4; Index++)
        {
            RtlZeroMemory(&Pml[PmlIndex + Index], sizeof(HARDWARE_PTE));
            Pml[PmlIndex + Index].PageFrameNumber = ((PHARDWARE_PTE)PageMap->PtePointer)[Index].PageFrameNumber;
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
        RtlZeroMemory(&LegacyPml[PmlIndex], sizeof(HARDWARE_LEGACY_PTE));
        LegacyPml[PmlIndex].PageFrameNumber = (UINT_PTR)PageMap->PtePointer / EFI_PAGE_SIZE;
        LegacyPml[PmlIndex].Valid = 1;
        LegacyPml[PmlIndex].Writable = 1;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
