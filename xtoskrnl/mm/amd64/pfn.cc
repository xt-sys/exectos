/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/pfn.cc
 * DESCRIPTION:     Physical Frame Number for AMD64 support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 *                  Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Allocates and initializes page directory structures for a range of PDEs.
 *
 * @param StartingPde
 *        Supplies a pointer to the first PDE in the range to initialize
 *
 * @param EndingPde
 *        Supplies a pointer to the last PDE in the range to initialize
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::InitializePageDirectory(IN PMMPDE StartingPde,
                                 IN PMMPDE EndingPde)
{
    PMMPTE ParentPte, ValidPte;
    BOOLEAN PteValidated;

    /* Get a template PTE for mapping the PFN database pages */
    ValidPte = MM::Pte::GetValidPte();

    /* Initialize validation flag */
    PteValidated = FALSE;

    /* Iterate through the range of PDEs to ensure the paging hierarchy is fully mapped */
    while(StartingPde <= EndingPde)
    {
        /* Check if there is a need to validate upper-level page table entries */
        if(!PteValidated || ((ULONG_PTR)StartingPde & (MM_PAGE_SIZE - 1)) == 0)
        {
            /* For LA57, ensure PML5 entry exists */
            if(MM::Paging::GetXpaStatus())
            {
                /* Get the P5E that maps the PXE page containing this hierarchy */
                ParentPte = MM::Paging::GetPpeAddress(StartingPde);

                /* Check if P5E entry is valid */
                if(!MM::Paging::PteValid(ParentPte))
                {
                    /* Allocate a new PML4 page and map P5E to it */
                    MM::Paging::SetPte(ValidPte, AllocateBootstrapPages(1), 0);
                    *ParentPte = *ValidPte;

                    /* Clear the newly created page */
                    RTL::Memory::ZeroMemory(MM::Paging::GetPteVirtualAddress(ParentPte), MM_PAGE_SIZE);
                }
            }

            /* Get the PXE that maps the PPE page containing PDE */
            ParentPte = MM::Paging::GetPdeAddress(StartingPde);

            /* Check if PXE entry is valid */
            if(!MM::Paging::PteValid(ParentPte))
            {
                /* Allocate a new PPE page and map PXE to it */
                MM::Paging::SetPte(ValidPte, AllocateBootstrapPages(1), 0);
                *ParentPte = *ValidPte;

                /* Clear the newly created page */
                RTL::Memory::ZeroMemory(MM::Paging::GetPteVirtualAddress(ParentPte), MM_PAGE_SIZE);
            }

            /* Get the PPE that maps the PDE page containing PTE */
            ParentPte = MM::Paging::GetPteAddress(StartingPde);

            /* Check if PPE entry is valid */
            if(!MM::Paging::PteValid(ParentPte))
            {
                /* Allocate a new PDE page and map PPE to it */
                MM::Paging::SetPte(ValidPte, AllocateBootstrapPages(1), 0);
                *ParentPte = *ValidPte;

                /* Clear the newly created page */
                RTL::Memory::ZeroMemory(MM::Paging::GetPteVirtualAddress(ParentPte), MM_PAGE_SIZE);
            }

            /* Upper levels for this PDE have been validated */
            PteValidated = TRUE;
        }

        /* Ensure the PDE has a PTE page allocated */
        if(!MM::Paging::PteValid(StartingPde))
        {
            /* Allocate a new PTE page and map PDE to it */
            MM::Paging::SetPte(ValidPte, AllocateBootstrapPages(1), 0);
            *StartingPde = *ValidPte;
        }

        /* Move to the next PDE */
        StartingPde = MM::Paging::GetNextPte(StartingPde);
    }
}

/**
 * Initializes the PFN database by mapping virtual memory and populating entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::InitializePfnDatabase(VOID)
{
    PKERNEL_INITIALIZATION_BLOCK InitializationBlock;
    PLIST_ENTRY ListEntry;
    PLOADER_MEMORY_DESCRIPTOR Descriptor;
    PUCHAR PfnDatabaseEnd;
    PMMMEMORY_LAYOUT MemoryLayout;
    PMMPTE ValidPte;

    /* Raise runlevel and acquire the PFN lock */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard SpinLock(SystemSpaceLock);

    /* Get the kernel initialization block */
    InitializationBlock = KE::BootInformation::GetInitializationBlock();

    /* Get the memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Get the PFN database size and calculate the end of the PFN database virtual address space */
    PfnDatabaseEnd = (PUCHAR)MemoryLayout->PfnDatabase + (MemoryLayout->PfnDatabaseSize * MM_PAGE_SIZE) - 1;

    /* Get a template PTE for mapping the PFN database pages */
    ValidPte = MM::Pte::GetValidPte();

    /* Map the Page Directory and Page Directory Pointer tables for the PFN database */
    MM::Pte::MapPPE(MemoryLayout->PfnDatabase, PfnDatabaseEnd, ValidPte);
    MM::Pte::MapPDE(MemoryLayout->PfnDatabase, PfnDatabaseEnd, ValidPte);
    MM::Pte::MapPTE(MemoryLayout->PfnDatabase, PfnDatabaseEnd, ValidPte);

    /* Zero PFN database virtual space */
    RTL::Memory::ZeroMemory(MemoryLayout->PfnDatabase, MemoryLayout->PfnDatabaseSize * MM_PAGE_SIZE);

    /* Initialize the color tables */
    MM::Colors::InitializeColorTables();

    /* Iterate over memory descriptors to map the PFN database and initialize entries */
    ListEntry = InitializationBlock->MemoryDescriptorListHead.Flink;
    while(ListEntry != &InitializationBlock->MemoryDescriptorListHead)
    {
        /* Get the descriptor */
        Descriptor = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_DESCRIPTOR, ListEntry);

        /* Skip invisible memory regions */
        if(MM::Manager::VerifyMemoryTypeInvisible(Descriptor->MemoryType))
        {
            /* Move to the next descriptor and continue */
            ListEntry = ListEntry->Flink;
            continue;
        }

        /* Check if this is the modified free descriptor */
        if(Descriptor == FreeDescriptor)
        {
            /* Switch to the original descriptor */
            Descriptor = &OriginalFreeDescriptor;
        }

        /* Check if the free memory block that was split is being processed */
        if(Descriptor == &OriginalFreeDescriptor)
        {
            /* Skip loop processing, free memory is initialized separately */
            ListEntry = ListEntry->Flink;
            continue;
        }

        /* Map PFN database entries for this physical range */
        ProcessMemoryDescriptor(Descriptor->BasePage, Descriptor->PageCount, Descriptor->MemoryType);

        /* Move to the next descriptor */
        ListEntry = ListEntry->Flink;
    }

    /* Initialize PFNs for the free memory */
    ProcessMemoryDescriptor(FreeDescriptor->BasePage, FreeDescriptor->PageCount, LoaderFree);

    /* Initialize PFNs for the physical pages backing the PFN database */
    ProcessMemoryDescriptor(OriginalFreeDescriptor.BasePage,
                            FreeDescriptor->BasePage - OriginalFreeDescriptor.BasePage,
                            LoaderMemoryData);

    /* Restore original free descriptor */
    *FreeDescriptor = OriginalFreeDescriptor;

    /* Initialize PFNs backing page tables */
    InitializePageTablePfns();
}

/**
 * Initializes PFN database entries for the system page tables.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::InitializePageTablePfns(VOID)
{
    PFN_NUMBER PageFrameIndex;
    PMMPFN Pfn;
    ULONG RootLevel;
    PMMPTE RootPte;

    /* Determine root structure based on paging mode */
    if(MM::Paging::GetXpaStatus())
    {
        /* XPA enabled, 5-level paging (LA57) */
        RootLevel = 5;

        /* Retrieve the PFN of the PML5 table and its virtual base address */
        PageFrameIndex = MM::Paging::GetPageFrameNumber(MM::Paging::GetPteAddress((PVOID)MM_P5E_LA57_BASE));
        RootPte = (PMMPTE)MM::Paging::GetP5eAddress(NULLPTR);
    }
    else
    {
        /* XPA disabled, 4-level paging */
        RootLevel = 4;

        /* Retrieve the PFN of the PML4 table and its virtual base address */
        PageFrameIndex = MM::Paging::GetPageFrameNumber(MM::Paging::GetPteAddress((PVOID)MM_PXE_BASE));
        RootPte = (PMMPTE)MM::Paging::GetPxeAddress(NULLPTR);
    }

    /* Initialize the PFN entry for the root page table itself */
    Pfn = GetPfnEntry(PageFrameIndex);
    if(Pfn)
    {
        /* Initialize the PFN entry */
        Pfn->PteAddress = NULLPTR;
        Pfn->u1.WsIndex = 0;
        Pfn->u2.ShareCount = 1;
        Pfn->u3.e1.CacheAttribute = PfnNonCached;
        Pfn->u3.e2.ReferenceCount = 1;
        Pfn->u4.PteFrame = 0;
    }

    /* Start recursive scan from the top level */
    if(RootPte)
    {
        /* Scan the root page table */
        ScanPageTable(RootPte, RootLevel);
    }
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
    PVOID VirtualRangeStart, VirtualRangeEnd;
    PFN_NUMBER PageNumber;
    PMMPDE PointerPde;
    PMMPFN Pfn;

    /* Check if the memory descriptor describes a free memory region */
    if(MM::Manager::VerifyMemoryTypeFree(MemoryType))
    {
        /* Iterate over each page in this free memory run */
        for(PageNumber = 0; PageNumber < PageCount; PageNumber++)
        {
            /* Get the PFN entry for the current page and ensure it is not referenced */
            Pfn = GetPfnEntry(BasePage + PageNumber);
            if(Pfn->u3.e2.ReferenceCount == 0)
            {
                /* Add the page to the free list to make it available for allocation */
                LinkFreePage(BasePage + PageNumber);
            }
        }
    }
    else
    {
        /* Calculate the virtual address range for this physical memory region */
        VirtualRangeStart = (PVOID)(KSEG0_BASE + (BasePage << MM_PAGE_SHIFT));
        VirtualRangeEnd = (PVOID)(KSEG0_BASE + ((BasePage + PageCount) << MM_PAGE_SHIFT));

        /* Handle all other (non-free) memory types */
        switch(MemoryType)
        {
            case LoaderBad:
                /* This memory is marked as bad and should not be used */
                for(PageNumber = 0; PageNumber < PageCount; PageNumber++)
                {
                    /* Link the page to the bad pages list */
                    LinkPage(&BadPagesList, BasePage + PageNumber);
                }
                break;
            case LoaderXIPRom:
                /* Get the page directory entry for the current page */
                PointerPde = MM::Paging::GetPdeAddress(VirtualRangeStart);

                /* Initialize the page directory entries covering this memory range */
                InitializePageDirectory(PointerPde, MM::Paging::GetPdeAddress(VirtualRangeEnd));

                /* This memory range contains Read-Only Memory (ROM) */
                for(PageNumber = 0; PageNumber < PageCount; PageNumber++)
                {
                    /* Get the PFN entry for the current ROM page */
                    Pfn = GetPfnEntry(BasePage + PageNumber);

                    /* Ensure that the page is not already in-use */
                    if(Pfn->u3.e2.ReferenceCount == 0)
                    {
                        /* Initialize the PFN entry to represent a ROM page */
                        Pfn->PteAddress = MM::Paging::GetPteAddress(VirtualRangeStart);
                        Pfn->u1.Flink = 0;
                        Pfn->u2.ShareCount = 0;
                        Pfn->u3.e1.CacheAttribute = PfnCached;
                        Pfn->u3.e1.PageLocation = 0;
                        Pfn->u3.e1.PrototypePte = 1;
                        Pfn->u3.e1.Rom = 1;
                        Pfn->u3.e2.ReferenceCount = 0;
                        Pfn->u4.InPageError = 0;
                        Pfn->u4.PteFrame = MM::Paging::GetPageFrameNumber(PointerPde);
                    }
                }
                break;
            default:
                /* Get the page directory entry for the current page */
                PointerPde = MM::Paging::GetPdeAddress(VirtualRangeStart);

                /* Initialize the page directory entries covering this memory range */
                InitializePageDirectory(PointerPde, MM::Paging::GetPdeAddress(VirtualRangeEnd));

                /* All other types are considered in-use (ie, by the kernel, ACPI, etc) */
                for(PageNumber = 0; PageNumber < PageCount; PageNumber++)
                {
                    /* Get the PFN entry for the current in-use page */
                    Pfn = GetPfnEntry(BasePage + PageNumber);

                    /* Ensure that the page is not already in-use */
                    if(Pfn->u3.e2.ReferenceCount == 0)
                    {
                        /* Initialize the PFN entry to represent an in-use page and prevent it from being allocated */
                        Pfn->PteAddress = MM::Paging::GetPteAddress(VirtualRangeStart);
                        Pfn->u2.ShareCount++;
                        Pfn->u3.e1.CacheAttribute = PfnCached;
                        Pfn->u3.e1.PageLocation = ActiveAndValid;
                        Pfn->u3.e2.ReferenceCount = 1;
                        Pfn->u4.PteFrame = MM::Paging::GetPageFrameNumber(PointerPde);
                    }
                }
                break;
        }
    }
}

/**
 * Recursively scans a page table to initialize PFN database entries for active pages.
 *
 * @param PointerPte
 *        Pointer to the base of the page table to scan.
 *
 * @param Level
 *        The paging level of the table being scanned.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::ScanPageTable(IN PMMPTE PointerPte,
                       IN ULONG Level)
{
    PVOID Address;
    ULONG Index;
    PMMPTE NextLevelPte;
    ULONG PtesPerPage;

    /* Get the number of PTEs per page */
    PtesPerPage = MM::Pte::GetPtesPerPage();

    /* Iterate through all entries in the current page table */
    for(Index = 0; Index < PtesPerPage; Index++)
    {
        /* Check if the page table entry is present */
        if(MM::Paging::PteValid(PointerPte))
        {
            /* Mark the PFN pointed to by this entry as active */
            LinkPfnForPageTable(MM::Paging::GetPageFrameNumber(PointerPte), PointerPte);

            /* Recurse to the next level, if this is not a leaf node (PTE) */
            if(Level > 1)
            {
                /* Calculate the virtual address mapped by this entry to find the next table */
                switch(Level)
                {
                    case 5:
                        /* Calculate PXE */
                        Address = MM::Paging::GetP5eVirtualAddress((PMMP5E)PointerPte);
                        NextLevelPte = (PMMPTE)MM::Paging::GetPxeAddress(Address);
                        break;
                    case 4:
                        /* Calculate PPE */
                        Address = MM::Paging::GetPxeVirtualAddress((PMMPXE)PointerPte);
                        NextLevelPte = (PMMPTE)MM::Paging::GetPpeAddress(Address);
                        break;
                    case 3:
                        /* Calculate PDE */
                        Address = MM::Paging::GetPpeVirtualAddress((PMMPPE)PointerPte);
                        NextLevelPte = (PMMPTE)MM::Paging::GetPdeAddress(Address);
                        break;
                    case 2:
                        /* Calculate PTE */
                        Address = MM::Paging::GetPdeVirtualAddress((PMMPDE)PointerPte);
                        NextLevelPte = MM::Paging::GetPteAddress(Address);
                        break;
                    default:
                        /* Nothing to calculate, return NULLPTR */
                        NextLevelPte = NULLPTR;
                        break;
                }

                /* Recurse deeper if not at the bottom level (PTE) already */
                if(NextLevelPte)
                {
                    /* Recursively scan the next level page table */
                    ScanPageTable(NextLevelPte, Level - 1);
                }
            }
        }

        /* Move to the next entry in the current table */
        PointerPte = MM::Paging::GetNextPte(PointerPte);
    }
}
