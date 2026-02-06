/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pfn.cc
 * DESCRIPTION:     Physical Frame Number for i686 support
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
    /* Nothing to do */
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
    PLOADER_MEMORY_DESCRIPTOR Descriptor;
    PKERNEL_INITIALIZATION_BLOCK InitializationBlock;
    PLIST_ENTRY ListEntry;
    PMMMEMORY_LAYOUT MemoryLayout;
    PUCHAR PfnDatabaseEnd;
    PMMPTE ValidPte;

    /* Raise runlevel and acquire PFN lock */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard SpinLock(SystemSpaceLock);

    /* Get the kernel initialization block */
    InitializationBlock = KE::BootInformation::GetInitializationBlock();

    /* Get memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Get the PFN database size and calculate the end of the PFN database virtual address space */
    PfnDatabaseEnd = (PUCHAR)MemoryLayout->PfnDatabase + (MemoryLayout->PfnDatabaseSize * MM_PAGE_SIZE) - 1;

    /* Get a template PTE for mapping the PFN database pages */
    ValidPte = MM::Pte::GetValidPte();

    /* Map the Page Directory and Page Directory Pointer tables for the PFN database */
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

        /* Initialize PFNs for this memory range */
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
        /* XPA enabled, 3-level paging (PAE) */
        RootLevel = 3;

        /* Retrieve the PFN of the PML3 table and its virtual base address */
        PageFrameIndex = AR::CpuFunc::ReadControlRegister(3) >> MM_PAGE_SHIFT;
        RootPte = (PMMPTE)MM::Paging::GetPpeAddress(NULLPTR);
    }
    else
    {
        /* XPA disabled, 2-level paging */
        RootLevel = 2;

        /* Retrieve the PFN of the PML2 table and its virtual base address */
        PageFrameIndex = AR::CpuFunc::ReadControlRegister(3) >> MM_PAGE_SHIFT;
        RootPte = (PMMPTE)MM::Paging::GetPdeAddress(NULLPTR);
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
        Pfn->u3.e1.PageLocation = ActiveAndValid;
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

    /* Check if PML3 is enabled and current level is PDPT */
    if(Level == 3)
    {
        /* PAE PDPT has only 4 entries */
        PtesPerPage = 4;
    }

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
                    case 3:
                        /* Calculate PDE */
                        Address = MM::Paging::GetPpeVirtualAddress(PointerPte);
                        NextLevelPte = (PMMPTE)MM::Paging::GetPdeAddress(Address);
                        break;
                    case 2:
                        /* Calculate PTE */
                        Address = MM::Paging::GetPdeVirtualAddress(PointerPte);
                        NextLevelPte = (PMMPTE)MM::Paging::GetPteAddress(Address);
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
