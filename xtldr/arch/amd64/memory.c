/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/arch/amd64/memory.c
 * DESCRIPTION:     XT Boot Loader AMD64 specific memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Maps boot loader related code and builds page map.
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
BlBuildPageMap(IN PXTBL_PAGE_MAPPING PageMap,
               IN ULONG_PTR SelfMapAddress)
{
    PLIST_ENTRY ListEntry, ModulesList, ModulesListEntry;
    PXTBL_MEMORY_MAPPING Mapping;
    PXTBL_MODULE_INFO ModuleInfo;
    EFI_PHYSICAL_ADDRESS Address;
    EFI_STATUS Status;

    /* Allocate pages for the Page Map */
    Status = BlAllocateMemoryPages(1, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Assign and zero-fill memory used by page mappings */
    PageMap->PtePointer = (PVOID)(UINT_PTR)Address;
    RtlZeroMemory(PageMap->PtePointer, EFI_PAGE_SIZE);

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
            BlDebugPrint(L"   Type=%02lu, PhysicalBase=%.16P, VirtualBase=%.16P, Pages=%llu\n", Mapping->MemoryType,
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
    SIZE_T Pml1Entry, Pml2Entry, Pml3Entry, Pml4Entry, Pml5Entry;
    PHARDWARE_PTE Pml1, Pml2, Pml3, Pml4, Pml5;
    SIZE_T PageFrameNumber;
    EFI_STATUS Status;

    /* Set the Page Frame Number (PFN) */
    PageFrameNumber = PhysicalAddress >> EFI_PAGE_SHIFT;

    /* Do the recursive mapping */
    while(NumberOfPages > 0)
    {
        /* Calculate the indices in the various Page Tables from the virtual address */
        Pml5Entry = (VirtualAddress & ((ULONGLONG)0x1FF << MM_LA57_SHIFT)) >> MM_LA57_SHIFT;
        Pml4Entry = (VirtualAddress & ((ULONGLONG)0x1FF << MM_PXI_SHIFT)) >> MM_PXI_SHIFT;
        Pml3Entry = (VirtualAddress & ((ULONGLONG)0x1FF << MM_PPI_SHIFT)) >> MM_PPI_SHIFT;
        Pml2Entry = (VirtualAddress & ((ULONGLONG)0x1FF << MM_PDI_SHIFT)) >> MM_PDI_SHIFT;
        Pml1Entry = (VirtualAddress & ((ULONGLONG)0x1FF << MM_PTI_SHIFT)) >> MM_PTI_SHIFT;

        /* Check page map level */
        if(PageMap->PageMapLevel == 5)
        {
            /* Five level Page Map */
            Pml5 = ((PHARDWARE_PTE)(PageMap->PtePointer));

            /* Get PML4 */
            Status = BlpGetNextPageTable(PageMap, Pml5, Pml5Entry, &Pml4);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory mapping failure */
                return Status;
            }
        }
        else
        {
            /* Four level Page Map */
            Pml4 = ((PHARDWARE_PTE)(PageMap->PtePointer));
        }

        /* Get PML3 */
        Status = BlpGetNextPageTable(PageMap, Pml4, Pml4Entry, &Pml3);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory mapping failure */
            return Status;
        }

        /* Get PML 2 */
        Status = BlpGetNextPageTable(PageMap, Pml3, Pml3Entry, &Pml2);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory mapping failure */
            return Status;
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
    PHARDWARE_PTE PmlBase;
    ULONGLONG PmlIndex;

    /* Initialize PML base pointer */
    PmlBase = (PHARDWARE_PTE)PageMap->PtePointer;

    /* Check page map level */
    if(PageMap->PageMapLevel == 5)
    {
        /* Self-mapping for PML5 is not supported */
        BlDebugPrint(L"PML5 self-mapping not supported yet!\n");
        return STATUS_EFI_UNSUPPORTED;
    }
    else
    {
        /* Calculate PML index based on provided self map address */
        PmlIndex = (SelfMapAddress >> MM_PXI_SHIFT) & 0x1FF;

        /* Add self-mapping for PML4 */
        RtlZeroMemory(&PmlBase[PmlIndex], sizeof(HARDWARE_PTE));
        PmlBase[PmlIndex].PageFrameNumber = PmlBasePfn;
        PmlBase[PmlIndex].Valid = 1;
        PmlBase[PmlIndex].Writable = 1;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
