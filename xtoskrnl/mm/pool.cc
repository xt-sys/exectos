/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/pool.cc
 * DESCRIPTION:     Memory Manager pool manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Initializes the non-paged pool for memory allocator.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::InitializeNonPagedPool(VOID)
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
MM::Pool::InitializePagedPool(VOID)
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
XTAPI
VOID
MM::Pool::VerifyRunLevel(IN MMPOOL_TYPE PoolType,
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
