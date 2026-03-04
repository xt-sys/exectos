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
 *        Supplies a pointer to the allocated pool.
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
    PLIST_ENTRY Entry, LastHead, ListHead;
    PMMPTE PointerPte, ValidPte;
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

    /* Get a template valid PTE and loop through the allocation to map physical pages */
    ValidPte = MM::Pte::GetValidPte();
    do
    {
        /* Allocate a physical page */
        PageFrameNumber = MM::Pfn::AllocatePhysicalPage(MM::Colors::GetNextColor());

        /* Initialize the PFN entry for the allocated physical page */
        Pfn = MM::Pfn::GetPfnEntry(PageFrameNumber);
        Pfn->PteAddress = PointerPte;
        Pfn->u2.ShareCount = 1;
        Pfn->u3.e1.PageLocation = ActiveAndValid;
        Pfn->u3.e2.ReferenceCount = 1;
        Pfn->u4.VerifierAllocation = 0;

        /* Build a valid PTE pointing to the allocated page frame */
        MM::Paging::SetPte(ValidPte, PageFrameNumber, 0);

        /* Write the valid PTE into the system PTE range */
        *(MM::Paging::GetNextPte(PointerPte)) = *ValidPte;
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
 *        Supplies a pointer to the allocated pool.
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
 *        Supplies a pointer to the allocated pool.
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
    PMMFREE_POOL_ENTRY FreeEntry, SetupEntry;
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

    /* Take the first free entry from the pool and set its size */
    FreeEntry = (PMMFREE_POOL_ENTRY)MemoryLayout->NonPagedPoolStart;
    FreeEntry->Size = MemoryLayout->NonPagedPoolSize;

    /* Take number of pages in the pool */
    Index = (ULONG)(MemoryLayout->NonPagedPoolSize - 1);
    if(Index >= MM_MAX_FREE_PAGE_LIST_HEADS)
    {
        /* Number of pages exceeds the number of free page list heads */
        Index = MM_MAX_FREE_PAGE_LIST_HEADS - 1;
    }

    /* Insert the first free entry into the free page list */
    RTL::LinkedList::InsertHeadList(&NonPagedPoolFreeList[Index], &FreeEntry->List);

    /* Create a free entry for each page in the pool */
    SetupEntry = FreeEntry;
    for(Index = 0; Index < MemoryLayout->NonPagedPoolSize; Index++)
    {
        /* Initialize the owner for each entry */
        SetupEntry->Owner = FreeEntry;
        SetupEntry = (PMMFREE_POOL_ENTRY)((ULONG_PTR)SetupEntry + MM_PAGE_SIZE);
    }
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
