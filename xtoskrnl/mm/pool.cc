/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/pool.cc
 * DESCRIPTION:     Memory Manager pool manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Decodes an obfuscated doubly-linked pool list pointer.
 *
 * @param Link
 *        Supplies the encoded list entry pointer to be decoded.
 *
 * @return This routine returns the valid, properly aligned list entry pointer.
 *
 * @since XT 1.0
 */
XTAPI
PLIST_ENTRY
MM::Pool::DecodePoolLink(IN PLIST_ENTRY PoolLink)
{
    /* XOR the obfuscated pointer with the global pool cookie to reveal the true address */
    return (PLIST_ENTRY)((ULONG_PTR)PoolLink ^ PoolSecureCookie);
}

/**
 * Determines the pool type for a given memory address.
 *
 * @param VirtualAddress
 *        Supplies a virtual address to determine the pool type for.
 *
 * @return This routine returns the determined pool type for the specified address.
 *
 * @since XT 1.0
 */
XTAPI
MMPOOL_TYPE
MM::Pool::DeterminePoolType(IN PVOID VirtualAddress)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Retrieve the memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Evaluate the virtual address against known pool boundaries */
    if((VirtualAddress >= MemoryLayout->NonPagedPoolStart) &&
       (VirtualAddress <= MemoryLayout->NonPagedPoolEnd))
    {
        /* Address belongs to the non-paged pool */
        return NonPagedPool;
    }
    else if((VirtualAddress >= MemoryLayout->NonPagedExpansionPoolStart) &&
            (VirtualAddress <= MemoryLayout->NonPagedExpansionPoolEnd))
    {
        /* Address belongs to the non-paged expansion pool */
        return NonPagedPool;
    }
    else if((VirtualAddress >= MemoryLayout->PagedPoolStart) &&
            (VirtualAddress <= MemoryLayout->PagedPoolEnd))
    {
        /* Address belongs to the paged pool */
        return PagedPool;
    }

    /* Address does not belong to any known pool, kernel panic */
    KE::Crash::Panic(0xC2, 0x42, (ULONG_PTR)VirtualAddress, 0, 0);

    /* Return an invalid pool type to satisfy the compiler */
    return (MMPOOL_TYPE)-1;
}

/**
 * Encodes a doubly-linked pool list pointer to mitigate pool corruption.
 *
 * @param Link
 *        Supplies the raw list entry pointer to be encoded.
 *
 * @return This routine returns the obfuscated list entry pointer.
 *
 * @since XT 1.0
 */
XTAPI
PLIST_ENTRY
MM::Pool::EncodePoolLink(IN PLIST_ENTRY PoolLink)
{
    /* XOR the raw pointer with the global pool cookie to securely obfuscate it */
    return (PLIST_ENTRY)((ULONG_PTR)PoolLink ^ PoolSecureCookie);
}

/**
 * Calculates the address of a pool block at a specific relative index.
 *
 * @param Header
 *        Supplies a pointer to the base pool header.
 *
 * @param Index
 *        Supplies the block index offset. This value can be negative to traverse backwards.
 *
 * @return This routine returns a pointer to the calculated pool header.
 *
 * @since XT 1.0
 */
XTAPI
PPOOL_HEADER
MM::Pool::GetPoolBlock(IN PPOOL_HEADER Header, IN SSIZE_T Index)
{
    /* The destination block is located by advancing the base address by the specified index */
    return (PPOOL_HEADER)((ULONG_PTR)Header + (Index * MM_POOL_BLOCK_SIZE));
}

/**
 * Retrieves the pool header associated with a given pool memory address.
 *
 * @param Memory
 *        Supplies a pointer to the allocated memory region of a pool block.
 *
 * @return This routine returns a pointer to the originating pool header.
 *
 * @since XT 1.0
 */
XTAPI
PPOOL_HEADER
MM::Pool::GetPoolEntry(IN PVOID Memory)
{
    /* The structural header logically precedes the allocated memory region */
    return (PPOOL_HEADER)((ULONG_PTR)Memory - sizeof(POOL_HEADER));
}

/**
 * Resolves the list entry structure embedded within a free pool block.
 *
 * @param Header
 *        Supplies a pointer to the pool header.
 *
 * @return This routine returns a pointer to the list entry directly following the header.
 *
 * @since XT 1.0
 */
XTAPI
PLIST_ENTRY
MM::Pool::GetPoolFreeBlock(IN PPOOL_HEADER Header)
{
    /* Return the list entry pointer */
    return (PLIST_ENTRY)((ULONG_PTR)Header + sizeof(POOL_HEADER));
}

/**
 * Retrieves a pointer to the adjacent contiguous pool block following the specified header.
 *
 * @param Header
 *        Supplies a pointer to the current pool header.
 *
 * @return This routine returns a pointer to the next pool header in memory.
 *
 * @since XT 1.0
 */
XTAPI
PPOOL_HEADER
MM::Pool::GetPoolNextBlock(IN PPOOL_HEADER Header)
{
    /* The adjacent forward header is located exactly 'BlockSize' units ahead of the current block */
    return (PPOOL_HEADER)((ULONG_PTR)Header + (Header->BlockSize * MM_POOL_BLOCK_SIZE));
}

/**
 * Retrieves a pointer to the adjacent contiguous pool block preceding the specified header.
 *
 * @param Header
 *        Supplies a pointer to the current pool header.
 *
 * @return This routine returns a pointer to the previous pool header in memory.
 *
 * @since XT 1.0
 */
XTAPI
PPOOL_HEADER
MM::Pool::GetPoolPreviousBlock(IN PPOOL_HEADER Header)
{
    /* The adjacent backward header is located exactly 'PreviousSize' units behind the current block */
    return (PPOOL_HEADER)((ULONG_PTR)Header - (Header->PreviousSize * MM_POOL_BLOCK_SIZE));
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

    /* Store non-paged pool descriptor in the pool vector and initialize it */
    PoolVector[NonPagedPool] = &NonPagedPoolDescriptor;
    InitializePoolDescriptor(PoolVector[NonPagedPool], NonPagedPool, 0, 0, NULLPTR);
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
 * Initializes a pool descriptor used by the memory manager.
 *
 * @param Descriptor
 *        Supplies a pointer to the pool descriptor structure to be initialized.
 *
 * @param PoolType
 *        Specifies the type of memory pool that will be managed by the descriptor.
 *
 * @param Index
 *        Supplies the zero-based index of the descriptor within the pool vector.
 *
 * @param Threshold
 *        Specifies the allocation threshold that dictates when the pool should expand.
 *
 * @param LockAddress
 *        Supplies a pointer to the synchronization primitive that will serialize access to this descriptor. *
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::InitializePoolDescriptor(IN PPOOL_DESCRIPTOR Descriptor,
                                   IN MMPOOL_TYPE PoolType,
                                   IN ULONG Index,
                                   IN ULONG Threshold,
                                   IN PVOID LockAddress)
{
    PLIST_ENTRY LastEntry, ListEntry;

    /* Populate the core attributes of the descriptor */
    Descriptor->LockAddress = LockAddress;
    Descriptor->PoolIndex = Index;
    Descriptor->PoolType = PoolType;
    Descriptor->Threshold = Threshold;

    /* Clear the deferred free list */
    Descriptor->PendingFrees = NULLPTR;
    Descriptor->PendingFreeDepth = 0;

    /* Zero out the runtime accounting and statistical tracking counters */
    Descriptor->RunningAllocations = 0;
    Descriptor->RunningFrees = 0;
    Descriptor->TotalBigAllocations = 0;
    Descriptor->TotalBytes = 0;
    Descriptor->TotalPages = 0;

    /* Establish the iteration boundaries */
    ListEntry = Descriptor->ListHeads;
    LastEntry = ListEntry + MM_POOL_LISTS_PER_PAGE;

    /* Traverse and initialize all block list heads */
    while(ListEntry < LastEntry)
    {
        /* Initialize the empty list head */
        InitializePoolListHead(ListEntry);
        ListEntry++;
    }
}

/**
 * Initializes a doubly-linked pool list head with encoded pointers.
 *
 * @param ListHead
 *        Supplies a pointer to the pool list head that is to be initialized.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::InitializePoolListHead(IN PLIST_ENTRY ListHead)
{
    PLIST_ENTRY ListEntry;

    /* Obfuscate the list head address and establish the empty circular linkage */
    ListEntry = EncodePoolLink(ListHead);
    ListHead->Flink = ListEntry;
    ListHead->Blink = ListEntry;
}

/**
 * Initializes the memory pool security mechanisms.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::InitializePoolSecurity(VOID)
{
    UNIMPLEMENTED;

    /* Initialize the global pool cookie using a hard-coded value */
    PoolSecureCookie = 0xDEADC0DE;
}

/**
 * Inserts a pool entry at the head of a doubly-linked pool list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the pool list.
 *
 * @param Entry
 *        Supplies a pointer to the pool list entry to be inserted.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::InsertPoolHeadList(IN PLIST_ENTRY ListHead,
                             IN PLIST_ENTRY Entry)
{
    PLIST_ENTRY Flink;

    /* Validate the pool list structure */
    VerifyPoolLinks(ListHead);

    /* Resolve the current forward link of the list head */
    Flink = DecodePoolLink(ListHead->Flink);

    /* Securely insert the new entry at the beginning of the pool list */
    Entry->Blink = EncodePoolLink(ListHead);
    Entry->Flink = EncodePoolLink(Flink);
    Flink->Blink = EncodePoolLink(Entry);
    ListHead->Flink = EncodePoolLink(Entry);

    /* Re-validate the pool list structure */
    VerifyPoolLinks(ListHead);
}

/**
 * Inserts a pool entry at the tail of a doubly-linked pool list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the pool list.
 *
 * @param Entry
 *        Supplies a pointer to the pool list entry to be inserted.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::InsertPoolTailList(IN PLIST_ENTRY ListHead,
                             IN PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink;

    /* Validate the pool list structure */
    VerifyPoolLinks(ListHead);

    /* Securely link the new entry at the end of the pool list */
    Blink = DecodePoolLink(ListHead->Blink);
    Blink->Flink = EncodePoolLink(Entry);
    Entry->Blink = EncodePoolLink(Blink);
    Entry->Flink = EncodePoolLink(ListHead);
    ListHead->Blink = EncodePoolLink(Entry);

    /* Re-validate the pool list structure */
    VerifyPoolLinks(ListHead);
}

/**
 * Determines whether a given doubly-linked pool list is empty.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the pool list to be evaluated.
 *
 * @return This routine returns TRUE if the pool list is empty, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Pool::PoolListEmpty(IN PLIST_ENTRY ListHead)
{
    /* Evaluate whether the pool list contains no valid entries */
    return (DecodePoolLink(ListHead->Flink) == ListHead);
}

/**
 * Removes a specific pool entry from a doubly-linked pool list.
 *
 * @param Entry
 *        Supplies a pointer to the pool list entry to be removed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::RemovePoolEntryList(IN PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink, Flink;

    /* Resolve the adjacent forward and backward links */
    Blink = DecodePoolLink(Entry->Blink);
    Flink = DecodePoolLink(Entry->Flink);

    /* Securely link the adjacent nodes together */
    Blink->Flink = EncodePoolLink(Flink);
    Flink->Blink = EncodePoolLink(Blink);
}

/**
 * Removes the first entry from a doubly-linked pool list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the pool list.
 *
 * @return This routine returns a pointer to the removed pool list entry.
 *
 * @since XT 1.0
 */
XTAPI
PLIST_ENTRY
MM::Pool::RemovePoolHeadList(IN PLIST_ENTRY ListHead)
{
    PLIST_ENTRY Entry, Flink;

    /* Securely unlink the first entry from the pool list */
    Entry = DecodePoolLink(ListHead->Flink);
    Flink = DecodePoolLink(Entry->Flink);
    Flink->Blink = EncodePoolLink(ListHead);
    ListHead->Flink = EncodePoolLink(Flink);

    /* Return the removed pool list entry */
    return Entry;
}

/**
 * Removes the last entry from a doubly-linked pool list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the pool list.
 *
 * @return This routine returns a pointer to the removed pool list entry.
 *
 * @since XT 1.0
 */
PLIST_ENTRY
XTAPI
MM::Pool::RemovePoolTailList(IN PLIST_ENTRY ListHead)
{
    PLIST_ENTRY Blink, Entry;

    /* Securely unlink the last entry from the pool list */
    Entry = DecodePoolLink(ListHead->Blink);
    Blink = DecodePoolLink(Entry->Blink);
    Blink->Flink = EncodePoolLink(ListHead);
    ListHead->Blink = EncodePoolLink(Blink);

    /* Return the removed pool list entry */
    return Entry;
}

/**
 * Verifies the structural integrity of all pool blocks residing on a specific page.
 *
 * @param Block
 *        Supplies a pointer to the specific pool block.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::VerifyPoolBlocks(IN PVOID Block)
{
    PPOOL_HEADER Entry;
    BOOLEAN FoundBlock;
    SIZE_T Size;

    /* Initialize tracking variables */
    FoundBlock = FALSE;
    Size = 0;

    /* Resolve the first pool header */
    Entry = (PPOOL_HEADER)PAGE_ALIGN(Block);

    /* Iterate through all contiguous pool allocations */
    do
    {
        /* Validate the current pool header */
        VerifyPoolHeader(Entry);

        /* Check if the current header corresponds to the target block */
        if(Entry == Block)
        {
            /* Mark the block as found */
            FoundBlock = TRUE;
        }

        /* Accumulate the total block size and advance to the next entry */
        Size += Entry->BlockSize;
        Entry = GetPoolNextBlock(Entry);
    }
    while((Size < (MM_PAGE_SIZE / MM_POOL_BLOCK_SIZE)) && (PAGE_ALIGN(Entry) != Entry));

    /* Ensure the block was found and the total size is aligned with the page */
    if(!FoundBlock || (PAGE_ALIGN(Entry) != Entry) || (Size != (MM_PAGE_SIZE / MM_POOL_BLOCK_SIZE)))
    {
        /* Pool blocks corruption detected, kernel panic */
        KE::Crash::Panic(0x19, 10, (ULONG_PTR)Block, (ULONG_PTR)Entry, FoundBlock);
    }
}

/**
 * Verifies the structural and spatial invariants of a specific pool header.
 *
 * @param Entry
 *        Supplies a pointer to the pool header to be verified.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::VerifyPoolHeader(IN PPOOL_HEADER Entry)
{
    PPOOL_HEADER PreviousEntry, NextEntry;

    /* Verify that the current block header is valid */
    if(!Entry->BlockSize)
    {
        /* Invalid block header size, kernel panic */
        KE::Crash::Panic(0x19, 8, (ULONG_PTR)Entry->PreviousSize, Entry->BlockSize, (ULONG_PTR)Entry);
    }

    /* Verify that the previous block header is valid */
    if(Entry->PreviousSize)
    {
        /* Resolve the previous block header */
        PreviousEntry = GetPoolPreviousBlock(Entry);

        /* Check if both adjacent blocks are within the same memory page */
        if(PAGE_ALIGN(Entry) != PAGE_ALIGN(PreviousEntry))
        {
            /* Adjacent blocks are not on the same page, kernel panic */
            KE::Crash::Panic(0x19, 6, (ULONG_PTR)PreviousEntry, (ULONG_PTR)PAGE_ALIGN(Entry), (ULONG_PTR)Entry);
        }

        /* Check the actual size of the previous block */
        if(PreviousEntry->BlockSize != Entry->PreviousSize)
        {
            /* Block size mismatch, kernel panic */
            KE::Crash::Panic(0x19, 5, (ULONG_PTR)PreviousEntry, (ULONG_PTR)Entry->PreviousSize, (ULONG_PTR)Entry);
        }
    }
    else if(PAGE_ALIGN(Entry) != Entry)
    {
        /* Not aligned to a page boundary, kernel panic */
        KE::Crash::Panic(0x19, 7, 0, (ULONG_PTR)PAGE_ALIGN(Entry), (ULONG_PTR)Entry);
    }

    /* Resolve the next block header */
    NextEntry = GetPoolNextBlock(Entry);

    /* Verify the next block header */
    if(PAGE_ALIGN(NextEntry) != NextEntry)
    {
        /* Check if both adjacent blocks are within the same memory page */
        if(PAGE_ALIGN(Entry) != PAGE_ALIGN(NextEntry))
        {
            /* Adjacent blocks are not on the same page, kernel panic */
            KE::Crash::Panic(0x19, 9, (ULONG_PTR)NextEntry, (ULONG_PTR)PAGE_ALIGN(Entry), (ULONG_PTR)Entry);
        }

        /* Check the previous block size */
        if(NextEntry->PreviousSize != Entry->BlockSize)
        {
            /* Block size mismatch, kernel panic */
            KE::Crash::Panic(0x19, 5, (ULONG_PTR)NextEntry, NextEntry->PreviousSize, (ULONG_PTR)Entry);
        }
    }
}

/**
 * Validates the structural integrity of a doubly-linked pool list.
 *
 * @param ListHead
 *        Supplies a pointer to the pool list head that is to be validated.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pool::VerifyPoolLinks(IN PLIST_ENTRY ListHead)
{
    /* Validate the doubly-linked list invariants */
    if((DecodePoolLink(DecodePoolLink(ListHead->Blink)->Flink) != ListHead) ||
       (DecodePoolLink(DecodePoolLink(ListHead->Flink)->Blink) != ListHead))
    {
        /* Pool corruption detected, raise kernel panic */
        KE::Crash::Panic(0x19,
                         3,
                         (ULONG_PTR)ListHead,
                         (ULONG_PTR)DecodePoolLink(DecodePoolLink(ListHead->Blink)->Flink),
                         (ULONG_PTR)DecodePoolLink(DecodePoolLink(ListHead->Flink)->Blink));
    }
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

    /* Invalid run level for specified pool, raise kernel panic */
    KE::Crash::Panic(0xC2,
                     (Entry ? MM_POOL_INVALID_FREE_RUNLEVEL : MM_POOL_INVALID_ALLOC_RUNLEVEL),
                     RunLevel,
                     PoolType,
                     (Entry ? (ULONG_PTR)Entry : Bytes));
}
