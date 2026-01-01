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
 * Allocates a physical page frame (PFN) from one of the system's free page lists.
 *
 * @param Color
 *        The preferred page color, used to optimize CPU cache alignment and reduce cache contention.
 *
 * @return This routine returns the Page Frame Number (PFN) of the allocated page.
 *
 * @since XT 1.0
 */
XTAPI
PFN_NUMBER
MM::Pfn::AllocatePhysicalPage(IN ULONG Color)
{
    PFN_NUMBER PageNumber;
    ULONG PagingColorsMask;

    /* Check if any physical pages are available in the system */
    if(!AvailablePages)
    {
        /* No physical pages are available in the system, return 0 */
        return 0;
    }

    /* Retrieve the bitmask used for calculating a page's color */
    PagingColorsMask = MM::Colors::GetPagingColorsMask();

    /* Attempt to retrieve a page from the colored free page list */
    PageNumber = MM::Colors::GetFreePages(FreePageList, Color)->Flink;
    if(PageNumber == MAXULONG_PTR)
    {
        /* No page was found in the colored free page list, check the colored zero page list */
        PageNumber = MM::Colors::GetFreePages(ZeroedPageList, Color)->Flink;
    }

    /* Attempt to retrieve a page from the colored zero list */
    if(PageNumber == MAXULONG_PTR)
    {
        /* No page was found in the colored zero page list, check the global free page list */
        PageNumber = FreePagesList.Flink;
    }

    /* Attempt to retrieve a page from the global free page list */
    if(PageNumber == MAXULONG_PTR)
    {
        /* No page was found in the global free page list, check the global zero page list */
        PageNumber = ZeroedPagesList.Flink;
    }

    /* Remove the page from its list and return its PFN */
    return UnlinkFreePage(PageNumber, PageNumber & PagingColorsMask);
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
    PfnDatabaseSize += (MM::Colors::GetPagingColors() * sizeof(MMCOLOR_TABLES) * 2);
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
MM::Pfn::GetHighestPhysicalPage(VOID)
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
 * Links a physical page to the appropriate list.
 *
 * @param ListHead
 *        Pointer to the list head.
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
MM::Pfn::LinkPage(IN PMMPFNLIST ListHead,
                  IN PFN_NUMBER PageFrameIndex)
{
    PMMCOLOR_TABLES ColorHead;
    MMPAGELISTS ListName;
    PMMPFN PageFrame;

    /* Get the memory layout */
    PMMMEMORY_LAYOUT MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Get the PFN database entry for the target page */
    PageFrame = &((PMMPFN)MemoryLayout->PfnDatabaseAddress)[PageFrameIndex];

    /* Get the list name */
    ListName = ListHead->ListName;

    /* Handle pages being linked to the modified or standby lists */
    if(ListName == ModifiedPageList || ListName == StandbyPageList)
    {
        /* Detect an invalid prototype/transition PTE state */
        if(!MM::Paging::GetPteSoftwarePrototype(&PageFrame->OriginalPte) &&
           MM::Paging::GetPteSoftwareTransition(&PageFrame->OriginalPte))
        {
            /* Crash system due to corrupted PFN/PTE state */
            KE::Crash::PanicEx(0x71, 0x8888, 0, 0, 0);
        }
    }

    /* Check if the page is ROM */
    if(PageFrame->u3.e1.Rom == 1)
    {
        /* Link the page to the ROM list */
        ListHead = &RomPagesList;
        ListHead->Total++;

        /* Append the page to the end of the ROM list */
        if(ListHead->Blink != MAXULONG_PTR)
        {
            /* Update the previous tail to point to this page */
            (&((PMMPFN)MemoryLayout->PfnDatabaseAddress)[ListHead->Blink])->u1.Flink = PageFrameIndex;
        }
        else
        {
            /* Initialize the list if it was empty */
            ListHead->Flink = PageFrameIndex;
        }

        /* Update list tail and PFN linkage */
        ListHead->Blink = PageFrameIndex;
        PageFrame->u1.Flink = MAXULONG_PTR;
        PageFrame->u2.Blink = ListHead->Blink;
        PageFrame->u3.e1.PageLocation = ListName;

        /* ROM pages require no further processing */
        return;
    }

    /* Account for the page being inserted into the target list */
    ListHead->Total++;

    /* Redirect modified pages to the per-color modified list */
    if(ListHead == &ModifiedPagesList)
    {
        /* Select the modified list matching the page color */
        ListHead = MM::Colors::GetModifiedPages(PageFrame->u3.e1.PageColor);
        ListHead->Total++;
    }
    else if((PageFrame->u3.e1.RemovalRequested == 1) && (ListName <= StandbyPageList))
    {
        /* Undo the insertion into the current list */
        ListHead->Total--;

        /* Preserve the standby location for removed pages */
        if(ListName == StandbyPageList)
        {
            /* Keep the page marked as standby */
            PageFrame->u3.e1.PageLocation = StandbyPageList;
        }

        /* Mark the page as no longer cache-mapped */
        PageFrame->u3.e1.CacheAttribute = PfnNotMapped;

        /* Move the page to the bad page list */
        ListHead = &BadPagesList;
        ListHead->Total++;
        ListName = BadPageList;
    }

    /* Insert zeroed pages at the head of the list */
    if(ListName == ZeroedPageList)
    {
        /* Link the page as the new list head */
        ListHead->Flink = PageFrameIndex;

        /* Initialize PFN forward and backward links */
        PageFrame->u1.Flink = ListHead->Flink;
        PageFrame->u2.Blink = MAXULONG_PTR;

        /* Update the previous head if it exists */
        if(ListHead->Flink != MAXULONG_PTR)
        {
            /* Fix up the backward link of the old head */
            (&((PMMPFN)MemoryLayout->PfnDatabaseAddress)[ListHead->Flink])->u2.Blink = PageFrameIndex;
        }
        else
        {
            /* Set the tail if the list was empty */
            ListHead->Blink = PageFrameIndex;
        }
    }
    else
    {
        /* Append the page to the tail of the list */
        if(ListHead->Blink != MAXULONG_PTR)
        {
            /* Link the current tail to the new page */
            (&((PMMPFN)MemoryLayout->PfnDatabaseAddress)[ListHead->Blink])->u1.Flink = PageFrameIndex;
        }
        else
        {
            /* Initialize the list if it was empty */
            ListHead->Flink = PageFrameIndex;
        }

        /* Update list tail */
        ListHead->Blink = PageFrameIndex;

        /* Terminate PFN forward link and set backward link */
        PageFrame->u1.Flink = MAXULONG_PTR;
        PageFrame->u2.Blink = ListHead->Blink;
    }

    /* Record the page’s current location */
    PageFrame->u3.e1.PageLocation = ListName;

    /* Handle pages that contribute to the available page count */
    if(ListName <= StandbyPageList)
    {
        /* Increment the system-wide available page counter */
        MM::Pfn::IncrementAvailablePages();

        /* Select the free list matching the page color */
        ColorHead = MM::Colors::GetFreePages(ZeroedPageList, PageFrameIndex & MM::Colors::GetPagingColorsMask());

        /* Store the color list linkage in the original PTE */
        MM::Paging::SetPte(&PageFrame->OriginalPte, ColorHead->Flink);
        PageFrame->u4.PteFrame = MM_PFN_PTE_FRAME;

        /* Insert the page into the color free list */
        ColorHead->Flink = PageFrameIndex;

        /* Update the previous head or initialize the tail */
        if(ColorHead->Flink != MAXULONG_PTR)
        {
            /* Fix up the PTE frame of the previous entry */
            (&((PMMPFN)MemoryLayout->PfnDatabaseAddress)[ColorHead->Flink])->u4.PteFrame = PageFrameIndex;
        }
        else
        {
            /* Set the tail for an empty color list */
            ColorHead->Blink = (PVOID)PageFrame;
        }

        /* Increment the color list page count */
        ColorHead->Count++;
    }
    else if(ListName == ModifiedPageList)
    {
        /* Modified page insertion logic not implemented yet */
        UNIMPLEMENTED;
    }
    else if(ListName == ModifiedReadOnlyPageList)
    {
        /* Modified read-only page handling not implemented yet */
        UNIMPLEMENTED;
    }
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
MM::Pfn::LinkPfnForPageTable(IN PFN_NUMBER PageFrameIndex,
                             IN PMMPTE PointerPte)
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
        Pfn->OriginalPte = *PointerPte;
        Pfn->PteAddress = PointerPte;
        Pfn->u1.WsIndex = 0;
        Pfn->u2.ShareCount++;
        Pfn->u3.e1.CacheAttribute = PfnNonCached;
        Pfn->u3.e1.PageLocation = ActiveAndValid;
        Pfn->u3.e2.ReferenceCount = 1;
        Pfn->u4.PteFrame = MM::Paging::GetPageFrameNumber(MM::Paging::GetPteAddress(PointerPte));
    }

    /* Increment the share count of the parent page table that contains the mapping */
    PointerPde = MM::Paging::GetPdeAddress(MM::Paging::GetPteVirtualAddress(PointerPte));
    Pfn = GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPde));
    Pfn->u2.ShareCount++;
}

/**
 * Links a PFN entry to its corresponding PTE and ensures the page table that contains the PTE is resident in memory.
 *
 * @param PageFrameIndex
 *        Supplies the index into the PFN database for the page being initialized.
 *
 * @param PointerPte
 *        Supplies the pointer to the PTE which maps the physical page.
 *
 * @param Modified
 *        Supplies a flag indicating if the page's initial state is modified.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::LinkPfnToPte(IN PFN_NUMBER PageFrameIndex,
                      IN PMMPTE PointerPte,
                      IN BOOLEAN Modified)
{
    PMMMEMORY_LAYOUT MemoryLayout;
    XTSTATUS Status;
    PMMPFN Pfn;
    PMMPTE Pte;

    /* Get the memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Point the PFN to its PTE */
    Pfn = &((PMMPFN)MemoryLayout->PfnDatabaseAddress)[PageFrameIndex];
    Pfn->PteAddress = PointerPte;

    /* Check if the page is already mapped and in use */
    if(MM::Paging::PteValid(PointerPte))
    {
        /* Clear the original PTE information */
        MM::Paging::SetPte(&Pfn->OriginalPte, 0, MM_PTE_READWRITE | MM_PTE_CACHE_ENABLE);
    }
    else
    {
        /* Page is not resident, so save the PTE contents for later use */
        Pfn->OriginalPte = *PointerPte;
    }

    /* Initialize the PFN database entry for this page */
    Pfn->u2.ShareCount = 1;
    Pfn->u3.e1.Modified = Modified;
    Pfn->u3.e1.PageLocation = ActiveAndValid;
    Pfn->u3.e2.ReferenceCount = 1;

    /* Get the PDE that maps the page table containing this PTE */
    Pte = MM::Paging::GetPteAddress(PointerPte);
    if(!MM::Paging::PteValid(Pte))
    {
        /* Check if page table is resident */
        Status = MM::PageFault::CheckPdeForPagedPool(PointerPte);
        if(Status != STATUS_SUCCESS)
        {
            /* Could not make the page table resident, crash system */
            KE::Crash::PanicEx(0x1,
                              (ULONG_PTR)0x61940,
                              (ULONG_PTR)PointerPte,
                              MM::Paging::GetPageFrameNumber(PointerPte),
                              (ULONG_PTR)MM::Paging::GetPteVirtualAddress(PointerPte));
        }
    }

    /* Record the page frame of the page table itself */
    PageFrameIndex = MM::Paging::GetPageFrameNumber(Pte);
    Pfn->u4.PteFrame = PageFrameIndex;

    /* Pin the page table in memory by incrementing its PFN share count */
    Pfn = &((PMMPFN)MemoryLayout->PfnDatabaseAddress)[PageFrameIndex];
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
                    Pfn->u3.e1.Rom = 1;
                    Pfn->u3.e2.ReferenceCount = 0;
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

/**
 * Unlinks a physical page from its corresponding list.
 *
 * @param PageIndex
 *        The Page Frame Number (PFN) of the page to unlink.
 *
 * @param Color
 *        The color of the page, used to find the correct colored list.
 *
 * @return This routine returns the PFN of the page that was unlinked.
 */
XTAPI
PFN_NUMBER
MM::Pfn::UnlinkFreePage(IN PFN_NUMBER PageFrameIndex,
                        IN ULONG Color)
{
    PMMPFN Pfn;
    PMMPFNLIST PfnList;
    MMPAGELISTS PageList;
    ULONG NodeColor;
    PMMCOLOR_TABLES ColorTable;
    PFN_NUMBER NextPage, PrevPage;

    /* Get the PFN database entry for the target page */
    Pfn = GetPfnEntry(PageFrameIndex);

    /* Identify which list the page belongs to (FreePageList or ZeroedPageList) */
    PfnList = PageLocationList[Pfn->u3.e1.PageLocation];
    PageList = PfnList->ListName;

    /* Update the forward link of the previous page */
    if(Pfn->u2.Blink != MAXULONG_PTR)
    {
        /* The page is not the head of the list; update the previous page's Flink */
        GetPfnEntry(Pfn->u2.Blink)->u1.Flink = Pfn->u1.Flink;
    }
    else
    {
        /* This is the first page in the list; update the list head's Flink */
        PfnList->Flink = Pfn->u1.Flink;
    }

    /* Update the backward link of the next page */
    if(Pfn->u1.Flink != MAXULONG_PTR)
    {
        /* The page is not the tail of the list; update the next page's Blink */
        GetPfnEntry(Pfn->u1.Flink)->u2.Blink = Pfn->u2.Blink;
    }
    else
    {
        /* This is the last page in the list; update the list head's Blink */
        PfnList->Blink = Pfn->u2.Blink;
    }

    /* Get the first page on the color list */
    ColorTable = MM::Colors::GetFreePages(PageList, Color);
    NodeColor = Pfn->u3.e1.PageColor;
    PrevPage = Pfn->u4.PteFrame;
    NextPage = MM::Paging::GetPte(&Pfn->OriginalPte);

    /* Decrement the count of pages for this specific color and total page count for this list */
    ColorTable->Count--;
    PfnList->Total--;

    /* Update the forward link of the previous colored page */
    if(PrevPage != MM_PFN_PTE_FRAME)
    {
        /* This is not the first page; update the previous page's Flink */
        MM::Paging::SetPte(&GetPfnEntry(PrevPage)->OriginalPte, NextPage);
    }
    else
    {
        /* This was the first page; update the color table's Flink */
        ColorTable->Flink = NextPage;
    }

    /* Update the backward link of the next colored page */
    if (NextPage != MAXULONG_PTR)
    {
        /* This is not the last page; update the next page's Blink */
        GetPfnEntry(NextPage)->u4.PteFrame = PrevPage;
    }
    else
    {
        /* This was the last page; update the color table's Blink */
        ColorTable->Blink = (PVOID)PrevPage;
    }

    /* Clear the list pointers and flags, but preserve the color and cache attributes */
    Pfn->u1.Flink = 0;
    Pfn->u2.Blink = 0;
    Pfn->u3.e1.CacheAttribute = PfnNotMapped;
    Pfn->u3.e1.PageColor = NodeColor;
    Pfn->u3.e2.ShortFlags = 0;

    /* Decrement the global count of available pages */
    DecrementAvailablePages();

    /* Return the page that was just unlinked */
    return PageFrameIndex;
}
