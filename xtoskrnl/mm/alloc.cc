/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/alloc.cc
 * DESCRIPTION:     Memory manager pool allocation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Allocates pages from the non-paged pool.
 *
 * @param Pages
 *        Specifies the number of pages to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated pool of pages.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocateNonPagedPoolPages(IN PFN_COUNT Pages,
                                         OUT PVOID *Memory)
{
    PMMPTE CurrentPte, PointerPte, ValidPte;
    PLIST_ENTRY Entry, LastHead, ListHead;
    PMMFREE_POOL_ENTRY FreePage;
    PFN_NUMBER PageFrameNumber;
    PVOID BaseAddress;
    ULONG Index;
    PMMPFN Pfn;

    /* Calculate the free list index based on the requested page count, capped at the maximum list head index */
    Index = MIN(Pages, MM_MAX_FREE_PAGE_LIST_HEADS) - 1;

    /* Set the starting list head and the boundary for the search loop */
    ListHead = &NonPagedPoolFreeList[Index];
    LastHead = &NonPagedPoolFreeList[MM_MAX_FREE_PAGE_LIST_HEADS];

    /* Start a guarded code block */
    {
        /* Acquire the Non-Paged pool lock and raise runlevel to DISPATCH_LEVEL */
        KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
        KE::QueuedSpinLockGuard NonPagedPoolSpinLock(NonPagedPoolLock);

        /* Iterate through the free lists */
        do
        {
            /* Iterate through the free entries in the current list */
            Entry = ListHead->Flink;
            while(Entry != ListHead)
            {
                /* Get the free pool entry structure from the list entry */
                FreePage = CONTAIN_RECORD(Entry, MMFREE_POOL_ENTRY, List);

                /* Check if this block is large enough to satisfy the request */
                if(FreePage->Size >= Pages)
                {
                    /* Adjust the size of the free block to account for the allocated pages */
                    FreePage->Size -= Pages;

                    /* Calculate the base address of the allocated block */
                    BaseAddress = (PVOID)((ULONG_PTR)FreePage + (FreePage->Size  << MM_PAGE_SHIFT));

                    /* Remove the entry from the free list */
                    RTL::LinkedList::RemoveEntryList(&FreePage->List);

                    /* Check if there is remaining space in the entry */
                    if(FreePage->Size != 0)
                    {
                        /* Calculate the new list index for the remaining fragment */
                        Index = MIN(FreePage->Size, MM_MAX_FREE_PAGE_LIST_HEADS) - 1;

                        /* Insert the entry into the free list */
                        RTL::LinkedList::InsertTailList(&NonPagedPoolFreeList[Index], &FreePage->List);
                    }

                    /* Get the Page Table Entry (PTE) for the allocated address */
                    PointerPte = MM::Paging::GetPteAddress(BaseAddress);

                    /* Get the Page Frame Number (PFN) database entry for the corresponding physical page */
                    Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));

                    /* Denote allocation boundaries */
                    Pfn->u3.e1.ReadInProgress = 1;

                    /* Check if multiple pages were requested */
                    if(Pages != 1)
                    {
                        /* Advance to the PTE of the last page in the allocation */
                        PointerPte = MM::Paging::AdvancePte(PointerPte, Pages - 1);

                        /* Get the PFN entry for the last page */
                        Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));
                    }

                    /* Denote allocation boundaries */
                    Pfn->u3.e1.WriteInProgress = 1;

                    /* Set the allocated memory address and return success */
                    *Memory = BaseAddress;
                    return STATUS_SUCCESS;
                }

                /* Move to the next entry in the free list */
                Entry = FreePage->List.Flink;
            }
        }
        while(++ListHead < LastHead);
    }

    /* No suitable free block found; try to expand the pool by reserving system PTEs */
    PointerPte = MM::Pte::ReserveSystemPtes(Pages, NonPagedPoolExpansion);
    if(PointerPte == NULLPTR)
    {
        /* PTE reservation failed, return insufficient resources */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Acquire the Non-Paged pool lock and raise runlevel to DISPATCH_LEVEL */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard NonPagedPoolSpinLock(NonPagedPoolLock);

    /* Acquire the PFN database lock */
    KE::QueuedSpinLockGuard PfnSpinLock(PfnLock);

    /* Check if there are enough available physical pages to back the allocation */
    if(Pages >= MM::Pfn::GetAvailablePages())
    {
        /* Not enough physical pages, release the reserved system PTEs */
        MM::Pte::ReleaseSystemPtes(PointerPte, Pages, NonPagedPoolExpansion);

        /* Return failure due to insufficient resources */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Set the tracking pointer to iterate through the reserved PTE space */
    CurrentPte = PointerPte;

    /* Get a template valid PTE and loop through the allocation to map physical pages */
    ValidPte = MM::Pte::GetValidPte();
    do
    {
        /* Allocate a physical page */
        PageFrameNumber = MM::Pfn::AllocatePhysicalPage(MM::Colors::GetNextColor());

        /* Initialize the PFN entry for the allocated physical page */
        Pfn = MM::Pfn::GetPfnEntry(PageFrameNumber);
        Pfn->PteAddress = CurrentPte;
        Pfn->u2.ShareCount = 1;
        Pfn->u3.e1.PageLocation = ActiveAndValid;
        Pfn->u3.e2.ReferenceCount = 1;
        Pfn->u4.VerifierAllocation = 0;

        /* Build a valid PTE pointing to the allocated page frame */
        MM::Paging::SetPte(ValidPte, PageFrameNumber, 0);

        /* Write the valid PTE into the system PTE range and advance to the next PTE */
        *CurrentPte = *ValidPte;
        CurrentPte = MM::Paging::GetNextPte(CurrentPte);
    }
    while(--Pages > 0);

    /* Dnote allocation boundaries */
    Pfn->u3.e1.WriteInProgress = 1;

    /* Get the PFN entry for the first page of the allocation */
    Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));

    /* Denote allocation boundaries */
    Pfn->u3.e1.ReadInProgress = 1;

    /* Convert the PTE address to the virtual address and store in the buffer */
    *Memory = MM::Paging::GetPteVirtualAddress(PointerPte);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Allocates pages from the paged pool.
 *
 * @param Pages
 *        Specifies the number of pages to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated pool of pages.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocatePagedPoolPages(IN PFN_COUNT Pages,
                                      OUT PVOID *Memory)
{
    UNIMPLEMENTED;

    /* Return not implemented status code */
    return STATUS_NOT_IMPLEMENTED;
}

/**
 * Allocates pages from the specified pool type.
 *
 * @param PoolType
 *        Specifies the type of pool to allocate pages from.
 *
 * @param Bytes
 *        Specifies the number of bytes to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated pool of pages.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocatePages(IN MMPOOL_TYPE PoolType,
                             IN SIZE_T Bytes,
                             OUT PVOID *Memory)
{
    PFN_COUNT Pages;

    /* Initialize the output parameter */
    *Memory = NULLPTR;

    /* Convert bytes to pages */
    Pages = SIZE_TO_PAGES(Bytes);

    /* Check if there are any pages to allocate */
    if(!Pages)
    {
        /* Nothing to allocate, return NULLPTR */
        return STATUS_INVALID_PARAMETER;
    }

    /* Switch on pool type */
    switch(PoolType & MM_POOL_TYPE_MASK)
    {
        case NonPagedPool:
            /* Allocate non-paged pool */
            return AllocateNonPagedPoolPages(Pages, Memory);
        case PagedPool:
            /* Allocate paged pool */
            return AllocatePagedPoolPages(Pages, Memory);
    }

    /* Invalid pool type specified, return error */
    return STATUS_INVALID_PARAMETER;
}

/**
 * Allocates a block of memory from the specified pool type.
 *
 * @param PoolType
 *        Specifies the type of pool to allocate from.
 *
 * @param Bytes
 *        Specifies the number of bytes to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated memory.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocatePool(IN MMPOOL_TYPE PoolType,
                            IN SIZE_T Bytes,
                            OUT PVOID *Memory)
{
    /* Allocate pool */
    return AllocatePool(PoolType, Bytes, Memory, SIGNATURE32('N', 'o', 'n', 'e'));
}

/**
 * Allocates a block of memory from the specified pool type.
 *
 * @param PoolType
 *        Specifies the type of pool to allocate from.
 *
 * @param Bytes
 *        Specifies the number of bytes to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated memory.
 *
 * @param Tag
 *        Specifies the allocation identifying tag.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocatePool(IN MMPOOL_TYPE PoolType,
                            IN SIZE_T Bytes,
                            OUT PVOID *Memory,
                            IN ULONG Tag)
{
    UNIMPLEMENTED;

    /* Verify run level for the specified pool */
    VerifyRunLevel(PoolType, Bytes, NULLPTR);

    /* Check if there are any bytes to allocate */
    if(!Bytes)
    {
        /* Allocate at least a single byte */
        Bytes = 1;
    }

    /* Allocate pages */
    return AllocatePages(PoolType, Bytes, Memory);
}

/**
 * Frees a previously allocated block of pages from the non-paged pool.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the non-paged pool pages allocation to free.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreeNonPagedPoolPages(IN PVOID VirtualAddress)
{
    PMMFREE_POOL_ENTRY FreePage, NextPage, LastPage;
    PFN_COUNT FreePages, Pages;
    PMMMEMORY_LAYOUT MemoryLayout;
    PMMPFN Pfn, FirstPfn;
    PMMPTE PointerPte;
    ULONG Index;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Get the first PTE of the allocation */
    PointerPte = MM::Paging::GetPteAddress(VirtualAddress);
    Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));

    /* Basic sanity check to prevent double-frees or freeing unallocated memory */
    if(Pfn->u3.e1.ReadInProgress == 0)
    {
        /* Memory is not marked as the start of an allocation, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Save the first PFN entry and initialize the allocation page counter */
    FirstPfn = Pfn;
    Pages = 1;

    /* Seek to the end of the allocation */
    while(Pfn->u3.e1.WriteInProgress == 0)
    {
        /* Get the next PTE and its PFN */
        PointerPte = MM::Paging::GetNextPte(PointerPte);
        Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));

        /* Increment the page count */
        Pages++;
    }

    /* Save the total free page count */
    FreePages = Pages;

    /* Acquire the Non-Paged pool lock and raise runlevel to DISPATCH_LEVEL */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard NonPagedPoolSpinLock(NonPagedPoolLock);

    /* Denote allocation boundaries */
    FirstPfn->u3.e1.ReadInProgress = 0;
    Pfn->u3.e1.WriteInProgress = 0;

    /* Get the next PTE */
    PointerPte = MM::Paging::GetNextPte(PointerPte);

    /* Check if the end of the initial nonpaged pool has been reached */
    if(Pfn - MemoryLayout->PfnDatabase == NonPagedPoolFrameEnd)
    {
        /* Ignore the last page of the initial nonpaged pool */
        Pfn = NULLPTR;
    }
    else
    {
        /* Check if the PTE is valid */
        if(MM::Paging::PteValid(PointerPte))
        {
            /* Get the PFN entry for the page laying in either the expansion or initial nonpaged pool */
            Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));
        }
        else
        {
            /* Ignore the last page of the expansion nonpaged pool */
            Pfn = NULLPTR;
        }
    }

    /* Check if the adjacent physical page following the allocation is free */
    if((Pfn) && (Pfn->u3.e1.ReadInProgress == 0))
    {
        /* Calculate the virtual address of the adjacent forward free pool entry */
        FreePage = (PMMFREE_POOL_ENTRY)((ULONG_PTR)VirtualAddress + (Pages << MM_PAGE_SHIFT));

        /* Absorb the adjacent free block's pages into the current free page count */
        FreePages += FreePage->Size;

        /* Unlink the adjacent free block from its current segregated free list */
        RTL::LinkedList::RemoveEntryList(&FreePage->List);
    }

    /* Get the free pool entry structure from the list entry */
    FreePage = (PMMFREE_POOL_ENTRY)VirtualAddress;

    /* Check if the beginning of the initial nonpaged pool has been reached */
    if(FirstPfn - MemoryLayout->PfnDatabase == NonPagedPoolFrameStart)
    {
        /* Ignore the first page of the initial nonpaged pool */
        Pfn = NULLPTR;
    }
    else
    {
        /* Calculate the PTE address for the page immediately preceding the allocation */
        PointerPte = MM::Paging::AdvancePte(PointerPte, -Pages - 1);

        /* Check if the PTE is valid */
        if(MM::Paging::PteValid(PointerPte))
        {
            /* Get the PFN entry for the page laying in either the expansion or initial nonpaged pool */
            Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));
        }
        else
        {
            /* Ignore the first page of the expansion nonpaged pool */
            Pfn = NULLPTR;
        }
    }

    /* Check if the adjacent physical page preceding the allocation is free */
    if((Pfn) && (Pfn->u3.e1.WriteInProgress == 0))
    {
        /* Retrieve the owner header of the preceding free block for backward coalescing */
        FreePage = (PMMFREE_POOL_ENTRY)((ULONG_PTR)VirtualAddress - MM_PAGE_SIZE);
        FreePage = FreePage->Owner;

        /* Check if the allocation is small enough */
        if(FreePage->Size < MM_MAX_FREE_PAGE_LIST_HEADS)
        {
            /* Remove the entry from the list */
            RTL::LinkedList::RemoveEntryList(&FreePage->List);

            /* Adjust the size of the free block to account for the allocated pages */
            FreePage->Size += FreePages;

            /* Calculate the new list index */
            Index = MIN(FreePage->Size, MM_MAX_FREE_PAGE_LIST_HEADS) - 1;

            /* Insert the entry into the head of the list */
            RTL::LinkedList::InsertHeadList(&NonPagedPoolFreeList[Index], &FreePage->List);
        }
        else
        {
            /* Adjust the size of the free block to account for the allocated pages */
            FreePage->Size += FreePages;
        }
    }

    /* Check if backward coalescing failed, requiring the freed block to become a new list head */
    if(FreePage == VirtualAddress)
    {
        /* Adjust the size of the free block to account for the allocated pages */
        FreePage->Size = FreePages;

        /* Calculate the new list index */
        Index = MIN(FreePage->Size, MM_MAX_FREE_PAGE_LIST_HEADS) - 1;

        /* Insert the entry into the head of the list */
        RTL::LinkedList::InsertHeadList(&NonPagedPoolFreeList[Index], &FreePage->List);
    }

    /* Calculate the start and end boundaries for updating the owner pointers */
    NextPage = (PMMFREE_POOL_ENTRY)VirtualAddress;
    LastPage = (PMMFREE_POOL_ENTRY)((ULONG_PTR)NextPage + (FreePages << MM_PAGE_SHIFT));

    /* Iterate through all freed and coalesced pages to update their owner reference */
    while(NextPage != LastPage)
    {
        /* Link the page to the owner */
        NextPage->Owner = FreePage;
        NextPage = (PMMFREE_POOL_ENTRY)((ULONG_PTR)NextPage + MM_PAGE_SIZE);
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Frees a previously allocated block of pages from the paged pool.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the paged pool pages allocation to free.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreePagedPoolPages(IN PVOID VirtualAddress)
{
    UNIMPLEMENTED;

    /* Return not implemented status code */
    return STATUS_NOT_IMPLEMENTED;
}

/**
 * Frees a previously allocated block of pages.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the pages allocation to free.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreePages(IN PVOID VirtualAddress)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Check if the address is in the paged pool */
    if(VirtualAddress >= MemoryLayout->PagedPoolStart && VirtualAddress < MemoryLayout->PagedPoolEnd)
    {
        /* Free pages from the paged pool */
        return FreePagedPoolPages(VirtualAddress);
    }
    else
    {
        /* Free pages from the non-paged pool */
        return FreeNonPagedPoolPages(VirtualAddress);
    }
}

/**
 * Frees a previously allocated memory pool.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the pool allocation to free.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreePool(IN PVOID VirtualAddress)
{
    /* Free pool */
    return FreePool(VirtualAddress, SIGNATURE32('N', 'o', 'n', 'e'));
}

/**
 * Frees a previously allocated memory pool.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the pool allocation to free.
 *
 * @param Tag
 *        Specifies the allocation identifying tag.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreePool(IN PVOID VirtualAddress,
                        IN ULONG Tag)
{
    UNIMPLEMENTED;

    /* Free pages */
    return FreePages(VirtualAddress);
}

/**
 * Initializes the non-paged pool for memory allocator.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::InitializeNonPagedPool(VOID)
{
    PMMFREE_POOL_ENTRY FreePage, SetupPage;
    PMMMEMORY_LAYOUT MemoryLayout;
    ULONG Index;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Map PTEs for the non-paged pool */
    MapNonPagedPool();

    /* Iterate over the free page list heads */
    for(Index = 0; Index < MM_MAX_FREE_PAGE_LIST_HEADS; Index++)
    {
        /* Initialize a free page list head */
        RTL::LinkedList::InitializeListHead(&NonPagedPoolFreeList[Index]);
    }

    /* Take the first free page from the pool and set its size */
    FreePage = (PMMFREE_POOL_ENTRY)MemoryLayout->NonPagedPoolStart;
    FreePage->Size = MemoryLayout->NonPagedPoolSize;

    /* Take number of pages in the pool */
    Index = (ULONG)(MemoryLayout->NonPagedPoolSize - 1);
    if(Index >= MM_MAX_FREE_PAGE_LIST_HEADS)
    {
        /* Number of pages exceeds the number of free page list heads */
        Index = MM_MAX_FREE_PAGE_LIST_HEADS - 1;
    }

    /* Insert the first free page into the free page list */
    RTL::LinkedList::InsertHeadList(&NonPagedPoolFreeList[Index], &FreePage->List);

    /* Create a free page for each page in the pool */
    SetupPage = FreePage;
    for(Index = 0; Index < MemoryLayout->NonPagedPoolSize; Index++)
    {
        /* Initialize the owner for each page */
        SetupPage->Owner = FreePage;
        SetupPage = (PMMFREE_POOL_ENTRY)((ULONG_PTR)SetupPage + MM_PAGE_SIZE);
    }

    /* Store first and last allocated non-paged pool page */
    NonPagedPoolFrameStart = MM::Paging::GetPageFrameNumber(MM::Paging::GetPteAddress(MemoryLayout->NonPagedPoolStart));
    NonPagedPoolFrameEnd = MM::Paging::GetPageFrameNumber(MM::Paging::GetPteAddress(MemoryLayout->NonPagedPoolEnd));

    /* Initialize system PTE pool for the non-paged expansion pool */
    Pte::InitializeSystemPtePool(Paging::GetNextPte(Paging::GetPteAddress(MemoryLayout->NonPagedExpansionPoolStart)),
                                                                          MemoryLayout->NonPagedExpansionPoolSize - 2,
                                                                          NonPagedPoolExpansion);
}

/**
 * Initializes the non-paged pool for memory allocator.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::InitializePagedPool(VOID)
{
    UNIMPLEMENTED;
}

/**
 * Validates the run level for the specified pool. If the run level is invalid, the kernel panics.
 *
 * @param PoolType
 *        Supplies the pool type.
 *
 * @param Bytes
 *        Supplies the size of the allocation.
 *
 * @param Entry
 *        Supplies a pointer to the allocation entry.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTINLINE
VOID
MM::Allocator::VerifyRunLevel(IN MMPOOL_TYPE PoolType,
                              IN SIZE_T Bytes,
                              IN PVOID Entry)
{
    KRUNLEVEL RunLevel;

    /* Get current run level */
    RunLevel = KE::RunLevel::GetCurrentRunLevel();

    /* Validate run level */
    if((PoolType & MM_POOL_TYPE_MASK) == PagedPool)
    {
        /* Paged pool runs up to APC level */
        if(RunLevel <= APC_LEVEL)
        {
            /* Run level is valid */
            return;
        }
    }
    else
    {
        /* Non-paged pool runs up to DISPATCH_LEVEL */
        if(RunLevel <= DISPATCH_LEVEL)
        {
            /* Run level is valid */
            return;
        }
    }

    /* Invalid run level for specified pool, kernel panic */
    KE::Crash::Panic(0xC2,
                     (Entry ? MM_POOL_INVALID_FREE_RUNLEVEL : MM_POOL_INVALID_ALLOC_RUNLEVEL),
                     RunLevel,
                     PoolType,
                     (Entry ? (ULONG_PTR)Entry : Bytes));
}
