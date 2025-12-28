/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/pfn.cc
 * DESCRIPTION:     Physical Frame Number (PFN) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 *                  Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Allocates a block of physical pages for early kernel initialization.
 *
 * @param NumberOfPages
 *        The number of physical pages to allocate.
 *
 * @return This routine returns the base page frame number (PFN) of the allocated block.
 *
 * @since XT 1.0
 */
XTAPI
PFN_NUMBER
MM::Pfn::AllocateBootstrapPages(IN PFN_NUMBER NumberOfPages)
{
    PFN_NUMBER Pfn;

    /* Check if the largest free memory block has enough pages */
    if(NumberOfPages > FreeDescriptor->PageCount)
    {
        /* Not enough physical memory available, kernel panic */
        DebugPrint(L"Insufficient physical pages! Install additional memory\n");
        KE::Crash::Panic(0);
    }

    /* Allocate pages from the beginning of the free descriptor */
    Pfn = FreeDescriptor->BasePage;
    FreeDescriptor->BasePage += NumberOfPages;
    FreeDescriptor->PageCount -= NumberOfPages;

    /* Return the base page frame number of the allocated block */
    return Pfn;
}

/**
 * Calculates the total number of pages required for the PFN database and its associated color tables.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::ComputePfnDatabaseSize(VOID)
{
    /* Calculate the total number of pages required for the PFN database */
    PfnDatabaseSize = (HighestPhysicalPage + 1) * sizeof(MMPFN);
    PfnDatabaseSize = ROUND_UP(PfnDatabaseSize, MM_PAGE_SIZE);
    PfnDatabaseSize >>= MM_PAGE_SHIFT;
}

/**
 * Decrements the global count of available pages.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::DecrementAvailablePages(VOID)
{
    /* Decrement the global count of available pages */
    AvailablePages--;
}

/**
 * Retrieves the highest physical page number (PFN) detected in the system.
 *
 * @return This routine returns the highest physical page number.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::Pfn::GetHighestPhysicalPage()
{
    /* Return the highest physical page number */
    return HighestPhysicalPage;
}

/**
 * Retrieves the total number of physical pages managed by the system.
 *
 * @return Returns the total count of physical memory pages.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
MM::Pfn::GetNumberOfPhysicalPages(VOID)
{
    /* Return the number of physical pages */
    return NumberOfPhysicalPages;
}

/**
 * Gets the size of the PFN database and its associated structures, in pages.
 *
 * @return This routine returns the total number of pages required for the PFN database and its associated structures.
 *
 * @since XT 1.0
 */
XTAPI
PFN_NUMBER
MM::Pfn::GetPfnDatabaseSize(VOID)
{
    /* Return the pre-calculated size of the PFN database in pages */
    return PfnDatabaseSize;
}

/**
 * Retrieves a pointer to the PFN database entry for a given physical page.
 *
 * @param Pfn
 *        The Page Frame Number (PFN) to look up.
 *
 * @return This routine returns a pointer to the MMPFN structure for the given PFN, or NULLPTR if the PFN is invalid.
 *
 * @since XT 1.0
 */
XTAPI
PMMPFN
MM::Pfn::GetPfnEntry(IN PFN_NUMBER Pfn)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Validate that the PFN is within the range of managed physical memory */
    if(Pfn > HighestPhysicalPage)
    {
        /* The requested page number is outside the bounds, return NULLPTR */
        return NULLPTR;
    }

    /* Get the memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Calculate the address of the PFN entry by indexing into the PFN database array and return it */
    return &((PMMPFN)MemoryLayout->PfnDatabaseAddress)[Pfn];
}

/**
 * Increments the global count of available pages.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::IncrementAvailablePages(VOID)
{
    /* Increment the global count of available pages */
    AvailablePages++;
}

/**
 * Links a physical page to the appropriate free lists.
 *
 * @param PageFrameIndex
 *        The Page Frame Number (PFN) of the page to link.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::LinkFreePage(IN PFN_NUMBER PageFrameIndex)
{
    ULONG Color;
    PMMPFNLIST ListHead;
    PFN_NUMBER LastPage;
    PMMPFN ColoredPfn, PfnEntry;
    PMMCOLOR_TABLES ColorTable;
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Get the PFN database entry for the page */
    PfnEntry = GetPfnEntry(PageFrameIndex);

    /* Check if the page is part of a ROM image */
    if(PfnEntry->u3.e1.Rom == 1)
    {
        /* Mark the page as inactive and clear its links */
        PfnEntry->u1.Flink = 0;
        PfnEntry->u3.e1.PageLocation = 0;

        /* Do not free ROM pages */
        return;
    }

    /* Check if the page is marked for removal */
    if(PfnEntry->u3.e1.RemovalRequested == 1)
    {
        /* Update cache attribute to not mapped */
        PfnEntry->u3.e1.CacheAttribute = PfnNotMapped;

        /* Do not add it to the free list */
        return;
    }

    /* Insert the page into the global free list */
    ListHead = &FreePagesList;
    ListHead->Total++;

    /* Get the current last page on the list */
    LastPage = ListHead->Blink;

    /* Check if the list is not empty */
    if(LastPage != MAXULONG_PTR)
    {
        /* Link with the previous last page */
        GetPfnEntry(LastPage)->u1.Flink = PageFrameIndex;
    }
    else
    {
        /* Put the page as the first entry */
        ListHead->Flink = PageFrameIndex;
    }

    /* Set the page as the new tail of the list */
    ListHead->Blink = PageFrameIndex;
    PfnEntry->u1.Flink = MAXULONG_PTR;
    PfnEntry->u2.Blink = LastPage;
    PfnEntry->u3.e1.CacheAttribute = PfnNotMapped;
    PfnEntry->u3.e1.PageLocation = FreePageList;
    PfnEntry->u4.AweAllocation = 0;
    PfnEntry->u4.InPageError = 0;
    PfnEntry->u4.Priority = 3;

    /* Insert the page into the colored free list */
    Color = PageFrameIndex & MM::Colors::GetPagingColorsMask();
    ColorTable = MM::Colors::GetFreePages(FreePageList, Color);

    /* Get the memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Check if the colored list is empty */
    if(ColorTable->Flink == MAXULONG_PTR)
    {
        /* Put the page as the first entry */
        ColorTable->Flink = PageFrameIndex;
        PfnEntry->u4.PteFrame = MM_PFN_PTE_FRAME;
    }
    else
    {
        /* Get the current last page on the colored list */
        ColoredPfn = (PMMPFN)ColorTable->Blink;

        /* Link with the previous last page */
        MM::Paging::SetPte(&ColoredPfn->OriginalPte, PageFrameIndex);
        PfnEntry->u4.PteFrame = ColoredPfn - (PMMPFN)MemoryLayout->PfnDatabaseAddress;
    }

    /* Set the page as the new tail of the colored list */
    ColorTable->Blink = PfnEntry;
    ColorTable->Count++;
    MM::Paging::SetPte(&PfnEntry->OriginalPte, MAXULONG_PTR);

    /* Increment number of available pages */
    IncrementAvailablePages();
}

/**
 * Initializes the PFN database entry for a physical page that is used as a page table.
 *
 * @param PageFrameIndex
 *        The page frame number of the physical page being used as a page table.
 *
 * @param PointerPte
 *        A pointer to the higher-level PTE that maps this page table page.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::LinkPfnForPageTable(PFN_NUMBER PageFrameIndex,
                             PMMPTE PointerPte)
{
    PMMPFN Pfn;
    PMMPDE PointerPde;
    PVOID EndAddress;

    /* Retrieve the PFN database entry for the physical page of the page table */
    Pfn = GetPfnEntry(PageFrameIndex);

    /* Calculate the end address of the PFN entry to ensure it's mapped */
    EndAddress = (PUCHAR)(Pfn + 1) - 1;

    /* Validate that the PFN entry corresponds to a valid, active physical page */
    if((PageFrameIndex <= HighestPhysicalPage) && (MM::Pte::AddressValid(Pfn)) &&
       (MM::Pte::AddressValid(EndAddress)) && (Pfn->u3.e1.PageLocation == ActiveAndValid))
    {
        /* Initialize the PFN entry for this page table page */
        Pfn->u1.WsIndex = 0;
        Pfn->u2.ShareCount++;
        Pfn->PteAddress = PointerPte;
        Pfn->OriginalPte = *PointerPte;
        Pfn->u3.e1.PageLocation = ActiveAndValid;
        Pfn->u3.e1.CacheAttribute = PfnNonCached;
        Pfn->u3.e2.ReferenceCount = 1;
        Pfn->u4.PteFrame = MM::Paging::GetPageFrameNumber(MM::Paging::GetPteAddress(PointerPte));
    }

    /* Increment the share count of the parent page table that contains the mapping */
    PointerPde = MM::Paging::GetPdeAddress(MM::Paging::GetPteVirtualAddress(PointerPte));
    Pfn = GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPde));
    Pfn->u2.ShareCount++;
}

/**
 * Processes a memory descriptor and initializes the corresponding PFN database entries
 *
 * @param BasePage
 *        The starting physical page number of the memory run
 *
 * @param PageCount
 *        The number of pages in the memory run
 *
 * @param MemoryType
 *        The type of memory as reported by the bootloader (e.g., free, ROM, in-use)
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::ProcessMemoryDescriptor(IN PFN_NUMBER BasePage,
                                 IN PFN_NUMBER PageCount,
                                 IN LOADER_MEMORY_TYPE MemoryType)
{
    PFN_NUMBER CurrentPage, PageNumber;
    PMMPFN Pfn;

    /* Check if the memory descriptor describes a free memory region */
    if(MM::Manager::VerifyMemoryTypeFree(MemoryType))
    {
        /* Iterate over each page in this free memory run */
        for(PageNumber = 0; PageNumber < PageCount; PageNumber++)
        {
            /* Get the PFN entry for the current page and set its initial cache attribute */
            CurrentPage = BasePage + PageNumber;
            Pfn = GetPfnEntry(CurrentPage);
            Pfn->u3.e1.CacheAttribute = PfnNonCached;

            /* Add the page to the free list to make it available for allocation */
            LinkFreePage(CurrentPage);
        }
    }
    else
    {
        /* Handle all other (non-free) memory types */
        switch(MemoryType)
        {
            case LoaderBad:
                /* This memory is marked as bad and should not be used */
                UNIMPLEMENTED;
                break;
            case LoaderXIPRom:
                /* This memory range contains Read-Only Memory (ROM) */
                for(PageNumber = 0; PageNumber < PageCount; PageNumber++)
                {
                    /* Get the PFN entry for the current ROM page */
                    Pfn = GetPfnEntry(BasePage + PageNumber);

                    /* Initialize the PFN entry to represent a ROM page */
                    Pfn->PteAddress = 0;
                    Pfn->u1.Flink = 0;
                    Pfn->u2.ShareCount = 0;
                    Pfn->u3.e1.CacheAttribute = PfnNonCached;
                    Pfn->u3.e1.PageLocation = 0;
                    Pfn->u3.e1.PrototypePte = 1;
                    Pfn->u3.e2.ReferenceCount = 0;
                    Pfn->u3.e1.Rom = 1;
                    Pfn->u4.InPageError = 0;
                    Pfn->u4.PteFrame = 0;
                }
                break;
            default:
                /* All other types are considered in-use (ie, by the kernel, ACPI, etc) */
                for(PageNumber = 0; PageNumber < PageCount; PageNumber++)
                {
                    /* Get the PFN entry for the current in-use page */
                    Pfn = GetPfnEntry(BasePage + PageNumber);

                    /* Mark the PFN as active and valid to prevent it from being allocated */
                    Pfn->u3.e1.PageLocation = ActiveAndValid;
                }
                break;
        }
    }
}

/**
 * Scans memory descriptors provided by the boot loader.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::ScanMemoryDescriptors(VOID)
{
    PLIST_ENTRY LoaderMemoryDescriptors, MemoryMappings;
    PLOADER_MEMORY_DESCRIPTOR MemoryDescriptor;
    PFN_NUMBER FreePages;

    /* Initially, set number of free pages to 0 */
    FreePages = 0;

    /* Get the list head of memory descriptors */
    LoaderMemoryDescriptors = KE::BootInformation::GetMemoryDescriptors();

    /* Iterate through the memory descriptors */
    MemoryMappings = LoaderMemoryDescriptors->Flink;
    while(MemoryMappings != LoaderMemoryDescriptors)
    {
        /* Get the memory descriptor */
        MemoryDescriptor = CONTAIN_RECORD(MemoryMappings, LOADER_MEMORY_DESCRIPTOR, ListEntry);

        /* Skip invisible or hardware cached memory regions */
        if(MM::Manager::VerifyMemoryTypeInvisible(MemoryDescriptor->MemoryType) ||
            (MemoryDescriptor->MemoryType == LoaderHardwareCachedMemory))
        {
            /* Move to the next descriptor and skip further processing */
            MemoryMappings = MemoryMappings->Flink;
            continue;
        }

        /* Count the number of physical pages, excluding bad memory */
        if(MemoryDescriptor->MemoryType != LoaderBad)
        {
            /* Add the pages from this descriptor to the total count */
            NumberOfPhysicalPages += MemoryDescriptor->PageCount;
        }

        /* Check if this physical page is the lowest one yet */
        if(MemoryDescriptor->BasePage < LowestPhysicalPage)
        {
            /* Update the lowest physical page number found so far */
            LowestPhysicalPage = MemoryDescriptor->BasePage;
        }

        /* Check if this physical page is the highest one yet */
        if((MemoryDescriptor->BasePage + MemoryDescriptor->PageCount) > HighestPhysicalPage)
        {
            /* Update the highest physical page number found so far */
            HighestPhysicalPage = (MemoryDescriptor->BasePage + MemoryDescriptor->PageCount) - 1;
        }

        /* Identify the largest block of free memory */
        if(MM::Manager::VerifyMemoryTypeFree(MemoryDescriptor->MemoryType))
        {
            /* Check if this free memory block is the largest one yet */
            if(MemoryDescriptor->PageCount >= FreePages)
            {
                /* Update the largest free block size and save the descriptor */
                FreePages = MemoryDescriptor->PageCount;
                FreeDescriptor = MemoryDescriptor;
            }
        }

        /* Get next memory descriptor */
        MemoryMappings = MemoryMappings->Flink;
    }

    /* Ensure a free memory descriptor was found */
    if(!FreeDescriptor)
    {
        /* No free memory available to bootstrap the system */
        KE::Crash::Panic(0);
    }

    /* Save a copy of the original free descriptor before it gets modified */
    RTL::Memory::CopyMemory(&OriginalFreeDescriptor, FreeDescriptor, sizeof(LOADER_MEMORY_DESCRIPTOR));
}
