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
BlBuildPageMap(IN PXTBL_PAGE_MAPPING PageMap)
{
    PLIST_ENTRY ListEntry, ModulesList, ModulesListEntry;
    EFI_PHYSICAL_ADDRESS Address, DirectoryAddress;
    PXTBL_MODULE_INFO ModuleInfo;
    PXTBL_MEMORY_MAPPING Mapping;
    EFI_STATUS Status;
    ULONG Index;

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

    /* Allocate pages for the Page Directory */
    Status = BlAllocateMemoryPages(4, &DirectoryAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Zero fill memory used by Page Directory */
    RtlZeroMemory((PVOID)DirectoryAddress, EFI_PAGE_SIZE * 4);

    /* Set the page directory into the PDPT and mark it present */
    for(Index = 0; Index < 4; Index++)
    {
        /* Set paging entry settings */
        ((PHARDWARE_PTE)PageMap->PtePointer)[Index].PageFrameNumber = DirectoryAddress / EFI_PAGE_SIZE;
        ((PHARDWARE_PTE)PageMap->PtePointer)[Index].Valid = 1;

        /* Next valid PFN address */
        DirectoryAddress += EFI_PAGE_SIZE;
    }

    /* Add page mapping itself to memory mapping */
    Status = BlMapVirtualMemory(PageMap, NULL, PageMap->PtePointer, 1, LoaderMemoryData);
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
            BlDebugPrint(L"   Type=%02lu, PhysicalBase=%.8P, VirtualBase=%.8P, Pages=%lu\n", Mapping->MemoryType,
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
          IN UINT_PTR VirtualAddress,
          IN UINT_PTR PhysicalAddress,
          IN UINT NumberOfPages)
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
