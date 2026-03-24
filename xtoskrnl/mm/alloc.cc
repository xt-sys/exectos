/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/alloc.cc
 * DESCRIPTION:     Memory Manager pool allocator
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
        /* Acquire the Non-Paged pool lock and raise runlevel to DISPATCH level */
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

                    /* Get the PTE for the allocated address */
                    PointerPte = MM::Paging::GetPteAddress(BaseAddress);

                    /* Get the PFN database entry for the corresponding physical page */
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

    /* Acquire the Non-Paged pool lock and raise runlevel to DISPATCH level */
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
        MM::Paging::SetPte(&Pfn->OriginalPte, 0, MM_READWRITE << MM_PROTECT_FIELD_SHIFT);
        Pfn->PteAddress = CurrentPte;
        Pfn->u2.ShareCount = 1;
        Pfn->u3.e1.PageLocation = ActiveAndValid;
        Pfn->u3.e2.ReferenceCount = 1;
        Pfn->u4.PteFrame = MM::Paging::GetPageFrameNumber(MM::Paging::GetPteAddress(CurrentPte));

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
    PPOOL_HEADER PoolEntry, NextPoolEntry, PoolRemainder;
    PPOOL_DESCRIPTOR PoolDescriptor;
    USHORT BlockSize, Index;
    PLIST_ENTRY ListHead;
    XTSTATUS Status;

    /* Verify run level for the specified pool */
    VerifyRunLevel(PoolType, Bytes, NULLPTR);

    /* Check if there are any bytes to allocate */
    if(!Bytes)
    {
        /* Allocate at least a single byte */
        Bytes = 1;
    }

    /* Retrieve the specific pool descriptor based on the masked pool type */
    PoolDescriptor = PoolVector[PoolType & MM_POOL_TYPE_MASK];

    /* Determine if the requested size exceeds the maximum standard pool block capacity */
    if(Bytes > (MM_PAGE_SIZE - (sizeof(POOL_HEADER) + MM_POOL_BLOCK_SIZE)))
    {
        /* Allocate new, raw pages directly to satisfy the large allocation request */
        Status = AllocatePages(PoolType, Bytes, (PVOID*)&PoolEntry);
        if(Status != STATUS_SUCCESS || !PoolEntry)
        {
            /* Allocation failed, clear the output pointer and return the error status */
            *Memory = NULLPTR;
            return Status;
        }

        /* Update the pool descriptor statistical counters */
        RTL::Atomic::ExchangeAdd32((PLONG)&PoolDescriptor->TotalBigAllocations, (LONG)SIZE_TO_PAGES(Bytes));
        RTL::Atomic::ExchangeAdd64((PLONG_PTR)&PoolDescriptor->TotalBytes, (LONG_PTR)Bytes);
        RTL::Atomic::Increment32((PLONG)&PoolDescriptor->RunningAllocations);

        /* Attempt to register the big allocation within the tracking table */
        if(!RegisterBigAllocationTag(PoolEntry, Tag, (ULONG)SIZE_TO_PAGES(Bytes), PoolType))
        {
            /* Fallback to a default tag */
            Tag = SIGNATURE32('B', 'i', 'g', 'A');
        }

        /* Register the allocation in the tracking table */
        RegisterAllocationTag(Tag, SIZE_TO_PAGES(Bytes), PoolType);

        /* Supply the allocated address and return success */
        *Memory = PoolEntry;
        return STATUS_SUCCESS;
    }

    /* Calculate the required block index */
    Index = (USHORT)((Bytes + sizeof(POOL_HEADER) + (MM_POOL_BLOCK_SIZE - 1)) / MM_POOL_BLOCK_SIZE);

    /* Resolve the appropriate list head for the calculated block index */
    ListHead = &PoolDescriptor->ListHeads[Index];
    while(ListHead != &PoolDescriptor->ListHeads[MM_POOL_LISTS_PER_PAGE])
    {
        /* Check whether the target free list contains available blocks */
        if(!PoolListEmpty(ListHead))
        {
            /* Start a guarded code block */
            {
                /* Acquire the pool lock */
                PoolLockGuard PoolLock((MMPOOL_TYPE)(PoolDescriptor->PoolType & MM_POOL_TYPE_MASK));

                /* Re-evaluate the list emptiness to prevent race conditions */
                if(PoolListEmpty(ListHead))
                {
                    /* Proceed to evaluate the next list head */
                    continue;
                }

                /* Validate the structural integrity of the pool list */
                VerifyPoolLinks(ListHead);

                /* Extract the first available free block from the list and resolve its header */
                PoolEntry = GetPoolEntry(RemovePoolHeadList(ListHead));

                /* Re-validate the pool list and verify integrity of the extracted block */
                VerifyPoolLinks(ListHead);
                VerifyPoolBlocks(PoolEntry);

                /* Check whether the extracted block requires splitting */
                if(PoolEntry->BlockSize != Index)
                {
                    /* Check if the block is located at the absolute beginning of a page */
                    if(PoolEntry->PreviousSize == 0)
                    {
                        /* Split the block and initialize the remainder */
                        PoolRemainder = GetPoolBlock(PoolEntry, Index);
                        PoolRemainder->BlockSize = PoolEntry->BlockSize - Index;
                        PoolRemainder->PreviousSize = Index;

                        /* Resolve the subsequent block and update its previous size field */
                        NextPoolEntry = GetPoolNextBlock(PoolRemainder);
                        if(PAGE_ALIGN(NextPoolEntry) != NextPoolEntry)
                        {
                            /* Adjust the adjacent block to reflect the new size of the remainder */
                            NextPoolEntry->PreviousSize = PoolRemainder->BlockSize;
                        }
                    }
                    else
                    {
                        /* Split the extracted block */
                        PoolRemainder = PoolEntry;
                        PoolEntry->BlockSize -= Index;

                        /* Advance the pointer to the new block and update its previous size */
                        PoolEntry = GetPoolNextBlock(PoolEntry);
                        PoolEntry->PreviousSize = PoolRemainder->BlockSize;

                        /* Resolve the adjacent next block and adjust its previous size */
                        NextPoolEntry = GetPoolBlock(PoolEntry, Index);
                        if(PAGE_ALIGN(NextPoolEntry) != NextPoolEntry)
                        {
                            /* Adjust the adjacent block */
                            NextPoolEntry->PreviousSize = Index;
                        }
                    }

                    /* Finalize the structural sizing fields */
                    BlockSize = PoolRemainder->BlockSize;
                    PoolEntry->BlockSize = Index;
                    PoolRemainder->PoolType = 0;

                    /* Validate the target free list */
                    VerifyPoolLinks(&PoolDescriptor->ListHeads[BlockSize - 1]);

                    /* Ensure the remainder block is large enough to contain valid list */
                    if(BlockSize != 1)
                    {
                        /* Insert the new remainder block into the appropriate free list and verify links */
                        InsertPoolTailList(&PoolDescriptor->ListHeads[BlockSize - 1], GetPoolFreeBlock(PoolRemainder));
                        VerifyPoolLinks(GetPoolFreeBlock(PoolRemainder));
                    }
                }

                /* Update the active pool type and verify structural invariants */
                PoolEntry->PoolType = PoolType + 1;
                VerifyPoolBlocks(PoolEntry);
            }

            /* Update the pool descriptor statistical counters */
            RTL::Atomic::ExchangeAdd64((PLONG_PTR)&PoolDescriptor->TotalBytes, (LONG_PTR)(PoolEntry->BlockSize * MM_POOL_BLOCK_SIZE));
            RTL::Atomic::Increment32((PLONG)&PoolDescriptor->RunningAllocations);

            /* Register the allocation in the tracking table */
            RegisterAllocationTag(Tag, PoolEntry->BlockSize * MM_POOL_BLOCK_SIZE, PoolType);

            /* Assign the specified identification tag */
            PoolEntry->PoolTag = Tag;

            /* Clear the internal list links */
            (GetPoolFreeBlock(PoolEntry))->Flink = NULLPTR;
            (GetPoolFreeBlock(PoolEntry))->Blink = NULLPTR;

            /* Supply the allocated address and return success */
            *Memory = GetPoolFreeBlock(PoolEntry);
            return STATUS_SUCCESS;
        }

        /* Advance to the next list head */
        ListHead++;
    }

    /* Allocate a new page to fulfill the request */
    Status = AllocatePages(PoolType, MM_PAGE_SIZE, (PVOID *)&PoolEntry);
    if(Status != STATUS_SUCCESS || !PoolEntry)
    {
        /* Allocation failed, clear the output pointer and return the error status */
        *Memory = NULLPTR;
        return Status;
    }

    /* Initialize the structural header */
    PoolEntry->Long = 0;
    PoolEntry->BlockSize = Index;
    PoolEntry->PoolType = PoolType + 1;

    /* Calculate the block size of the remaining unused space */
    BlockSize = (MM_PAGE_SIZE / MM_POOL_BLOCK_SIZE) - Index;

    /* Initialize the remainder entry representing the free space */
    PoolRemainder = GetPoolBlock(PoolEntry, Index);
    PoolRemainder->Long = 0;
    PoolRemainder->BlockSize = BlockSize;
    PoolRemainder->PreviousSize = Index;

    /* Update the pool descriptor statistical counters */
    RTL::Atomic::Increment32((PLONG)&PoolDescriptor->TotalPages);
    RTL::Atomic::ExchangeAdd64((PLONG_PTR)&PoolDescriptor->TotalBytes, (LONG_PTR)(PoolEntry->BlockSize * MM_POOL_BLOCK_SIZE));

    /* Check if the remainder block is large enough */
    if(PoolRemainder->BlockSize != 1)
    {
        /* Acquire the pool lock */
        PoolLockGuard PoolLock((MMPOOL_TYPE)(PoolDescriptor->PoolType & MM_POOL_TYPE_MASK));

        /* Validate the target free list structure */
        VerifyPoolLinks(&PoolDescriptor->ListHeads[BlockSize - 1]);

        /* Insert the remainder block into the free list */
        InsertPoolTailList(&PoolDescriptor->ListHeads[BlockSize - 1], GetPoolFreeBlock(PoolRemainder));

        /* Verify the structural integrity of the remainder and the allocated blocks */
        VerifyPoolLinks(GetPoolFreeBlock(PoolRemainder));
        VerifyPoolBlocks(PoolEntry);
    }
    else
    {
        /* Verify the allocated block invariants */
        VerifyPoolBlocks(PoolEntry);
    }

    /* Increment the running allocation counter for the pool descriptor */
    RTL::Atomic::Increment32((PLONG)&PoolDescriptor->RunningAllocations);

    /* Register the allocation in the tracking table */
    RegisterAllocationTag(Tag, PoolEntry->BlockSize * MM_POOL_BLOCK_SIZE, PoolType);

    /* Perform a final structural validation of the pool block */
    VerifyPoolBlocks(PoolEntry);

    /* Apply the requested identification tag */
    PoolEntry->PoolTag = Tag;

    /* Supply the allocated address and return success */
    *Memory = GetPoolFreeBlock(PoolEntry);
    return STATUS_SUCCESS;
}

/**
 * Computes a hash for a given virtual address to be used in the big allocation tracker.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address to be hashed.
 *
 * @return This routine returns the computed hash value.
 *
 * @since XT 1.0
 */
XTINLINE
ULONG
MM::Allocator::ComputeHash(IN PVOID VirtualAddress)
{
    ULONG Result;

    /* Transform the virtual address into a page frame number representation */
    Result = (ULONG)((ULONG_PTR)VirtualAddress >> MM_PAGE_SHIFT);

    /* Fold the page number bits using XOR to distribute the entropy across the lower bits */
    return (Result >> 24) ^ (Result >> 16) ^ (Result >> 8) ^ Result;
}

/**
 * Computes a hash for a given pool tag to be used in the allocation tracker.
 *
 * @param Tag
 *        Supplies the 32-bit pool tag to be hashed.
 *
 * @param TableMask
 *        Supplies the bitmask used to bound the resulting hash index to the table size.
 *
 * @return This routine returns the computed hash value.
 *
 * @since XT 1.0
 */
XTINLINE
ULONG
MM::Allocator::ComputeHash(IN ULONG Tag,
                           IN ULONG TableMask)
{
    ULONG Result;

    /* Fold the bytes using arithmetic shifts and XORs */
    Result = ((((((Tag & 0xFF) << 2) ^ ((Tag >> 8)  & 0xFF)) << 2) ^ ((Tag >> 16) & 0xFF)) << 2) ^ ((Tag >> 24) & 0xFF);

    /* Multiply by the NT magic prime-like constant and shift down */
    return ((40543 * Result) >> 2) & TableMask;
}

/**
 * Expands the big allocation tracking table to accommodate additional large allocations.
 *
 * @return This routine returns TRUE if the table was successfully expanded, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Allocator::ExpandBigAllocationsTable(VOID)
{
    PPOOL_TRACKING_BIG_ALLOCATIONS NewTable, OldTable;
    SIZE_T AllocationBytes, OldSize, NewSize;
    ULONG Hash, HashMask, Index;
    PFN_NUMBER PagesFreed;
    XTSTATUS Status;
    BOOLEAN Abort;

    /* Initialize the abort flag and snapshot current table capacity */
    Abort = FALSE;
    OldSize = BigAllocationsTrackingTableSize;

    /* Check if doubling the size would cause an integer overflow */
    if(OldSize > ((~(SIZE_T)0) / 2))
    {
        /* Abort expansion to prevent integer wrap-around */
        return FALSE;
    }

    /* Calculate the target capacity by safely doubling table capacity */
    NewSize = OldSize * 2;

    /* Ensure the new capacity does not result in fractional memory pages */
    NewSize = ROUND_DOWN(NewSize, MM_PAGE_SIZE / sizeof(POOL_TRACKING_BIG_ALLOCATIONS));

    /* Check if calculating the total byte size would cause an integer overflow */
    if(NewSize > ((~(SIZE_T)0) / sizeof(POOL_TRACKING_BIG_ALLOCATIONS)))
    {
        /* Abort expansion to prevent allocating a truncated memory block */
        return FALSE;
    }

    /* Compute the size required for the newly expanded tracking table */
    AllocationBytes = NewSize * sizeof(POOL_TRACKING_BIG_ALLOCATIONS);

    /* Allocate the required memory */
    Status = AllocatePages(NonPagedPool, AllocationBytes, (PVOID*)&NewTable);
    if(Status != STATUS_SUCCESS || !NewTable)
    {
        /* Memory allocation failed, abort the table expansion */
        return FALSE;
    }

    /* Zero the newly allocated table */
    RTL::Memory::ZeroMemory(NewTable, AllocationBytes);

    /* Iterate through the allocated memory block */
    for(Index = 0; Index < NewSize; Index++)
    {
        /* Mark the tracking entry as free and available */
        NewTable[Index].VirtualAddress = (PVOID)MM_POOL_BIG_ALLOCATIONS_ENTRY_FREE;
    }

    /* Start a guarded code block */
    {
        /* Acquire the tracking table lock and raise runlevel to DISPATCH level */
        KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
        KE::SpinLockGuard TrackingTableLock(&BigAllocationsTrackingTableLock);

        /* Verify if another thread has already expanded the table concurrently */
        if(BigAllocationsTrackingTableSize >= NewSize)
        {
            /* Another thread has already expanded the table, discard changes */
            Abort = TRUE;
        }
        else
        {
            /* Cache the legacy table pointer and calculate new hash mask */
            HashMask = NewSize - 1;
            OldTable = BigAllocationsTrackingTable;

            /* Rehash and migrate all active entries from the old table */
            for(Index = 0; Index < OldSize; Index++)
            {
                /* Bypass unallocated entries in the legacy table */
                if((ULONG_PTR)OldTable[Index].VirtualAddress & MM_POOL_BIG_ALLOCATIONS_ENTRY_FREE)
                {
                    /* Skip to the next entry */
                    continue;
                }

                /* Compute the updated hash index */
                Hash = ComputeHash(OldTable[Index].VirtualAddress) & HashMask;

                /* Resolve hash collisions using linear probing */
                while(!((ULONG_PTR)NewTable[Hash].VirtualAddress & MM_POOL_BIG_ALLOCATIONS_ENTRY_FREE))
                {
                    /* Advance the bucket index and check for table boundary overflow */
                    if(++Hash == NewSize)
                    {
                        /* Wrap the probing index back to the beginning */
                        Hash = 0;
                    }
                }

                /* Migrate the active entry to its new hash bucket */
                NewTable[Hash] = OldTable[Index];
            }

            /* Activate the newly populated table globally */
            BigAllocationsTrackingTable = NewTable;
            BigAllocationsTrackingTableHash = NewSize - 1;
            BigAllocationsTrackingTableSize = NewSize;
        }
    }

    /* Check if another thread has already expanded the table concurrently */
    if(Abort)
    {
        /* Free memory allocated for the new table and return */
        FreePages(NewTable);
        return TRUE;
    }

    /* Free memory allocated for the legacy table */
    FreePages(OldTable, &PagesFreed);

    /* Update the pool tracking statistics */
    UnregisterAllocationTag(SIGNATURE32('M', 'M', 'g', 'r'), PagesFreed << MM_PAGE_SHIFT, (MMPOOL_TYPE)0);
    RegisterAllocationTag(SIGNATURE32('M', 'M', 'g', 'r'), ROUND_UP(NewSize, MM_PAGE_SIZE), (MMPOOL_TYPE)0);

    /* Return success */
    return TRUE;
}

/**
 * Frees a previously allocated block of pages from the non-paged pool.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the non-paged pool pages allocation to free.
 *
 * @param PagesFreed
 *        Supplies a pointer to a variable that will receive the number of pages freed.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreeNonPagedPoolPages(IN PVOID VirtualAddress,
                                     OUT PPFN_NUMBER PagesFreed)
{
    PMMFREE_POOL_ENTRY FreePage, NextPage, LastPage;
    PMMMEMORY_LAYOUT MemoryLayout;
    PFN_COUNT FreePages, Pages;
    PFN_NUMBER PageFrameNumber;
    PMMPFN Pfn, FirstPfn;
    PMMPTE PointerPte;
    ULONG Index;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Get the first PTE of the allocation */
    PointerPte = MM::Paging::GetPteAddress(VirtualAddress);
    Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));

    /* Verify that the address is within the non-paged pool */
    if((VirtualAddress < MemoryLayout->NonPagedPoolStart ||
        VirtualAddress >= MemoryLayout->NonPagedPoolEnd) &&
       (VirtualAddress < MemoryLayout->NonPagedExpansionPoolStart ||
        VirtualAddress >= MemoryLayout->NonPagedExpansionPoolEnd))
    {
        /* Address does not belong to the non-paged pool, raise kernel panic */
        KE::Crash::Panic(0xC2, 0x43, (ULONG_PTR)VirtualAddress, 0, 0);
    }

    /* Basic sanity check to prevent double-frees or freeing unallocated memory */
    if(Pfn->u3.e1.ReadInProgress == 0)
    {
        /* Address is not an allocation head, raise kernel panic */
        KE::Crash::Panic(0xC2, 0x41, (ULONG_PTR)VirtualAddress, (ULONG_PTR)Pfn, 0);
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

    /* Acquire the Non-Paged pool lock and raise runlevel to DISPATCH level */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard NonPagedPoolSpinLock(NonPagedPoolLock);

    /* Denote allocation boundaries */
    FirstPfn->u3.e1.ReadInProgress = 0;
    Pfn->u3.e1.WriteInProgress = 0;

    /* Check if the address belongs to the non-paged expansion pool */
    if(VirtualAddress >= MemoryLayout->NonPagedExpansionPoolStart)
    {
        /* Check if the allocation spans more than 3 pages and should be reclaimed */
        if(Pages > 3)
        {
            /* Get the first PTE of the allocation and iterate over all pages */
            PointerPte = MM::Paging::GetPteAddress(VirtualAddress);
            for(Index = 0; Index < Pages; Index++)
            {
                /* Get the page frame number from the PTE */
                PageFrameNumber = MM::Paging::GetPageFrameNumber(PointerPte);
                Pfn = MM::Pfn::GetPfnEntry(PageFrameNumber);

                /* Clear PFN shared count */
                Pfn->u2.ShareCount = 0;

                /* Decrement the reference count of the page table */
                MM::Pfn::DecrementReferenceCount(Pfn, PageFrameNumber, FALSE);

                /* Clear the PTE address and invalidate the PTE */
                Pfn->PteAddress = NULLPTR;
                MM::Paging::ClearPte(PointerPte);

                /* Get the next PTE */
                PointerPte = MM::Paging::GetNextPte(PointerPte);
            }

            /* Release reserved system PTEs back to the pool */
            MM::Pte::ReleaseSystemPtes(MM::Paging::GetPteAddress(VirtualAddress), Pages, NonPagedPoolExpansion);

            /* Check if a page count was requested */
            if(PagesFreed != NULLPTR)
            {
                /* Return the number of pages freed */
                *PagesFreed = FreePages;
            }

            /* Return success */
            return STATUS_SUCCESS;
        }
    }

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
            /* Get the PFN entry for the page laying in either the expansion or initial non-paged pool */
            Pfn = MM::Pfn::GetPfnEntry(MM::Paging::GetPageFrameNumber(PointerPte));
        }
        else
        {
            /* Ignore the last page of the non-paged expansion pool */
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

    /* Check if a page count was requested */
    if(PagesFreed != NULLPTR)
    {
        /* Return the number of pages freed */
        *PagesFreed = FreePages;
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
 * @param PagesFreed
 *        Supplies a pointer to a variable that will receive the number of pages freed.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreePagedPoolPages(IN PVOID VirtualAddress,
                                  OUT PPFN_NUMBER PagesFreed)
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
    return FreePages(VirtualAddress, NULLPTR);
}

/**
 * Frees a previously allocated block of pages.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the pages allocation to free.
 *
 * @param PagesFreed
 *        Supplies a pointer to a variable that will receive the number of pages freed.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::FreePages(IN PVOID VirtualAddress,
                         OUT PPFN_NUMBER PagesFreed)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Check if the address is in the paged pool */
    if(VirtualAddress >= MemoryLayout->PagedPoolStart && VirtualAddress < MemoryLayout->PagedPoolEnd)
    {
        /* Free pages from the paged pool */
        return FreePagedPoolPages(VirtualAddress, PagesFreed);
    }
    else
    {
        /* Free pages from the non-paged pool */
        return FreeNonPagedPoolPages(VirtualAddress, PagesFreed);
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
    PPOOL_HEADER PoolEntry, NextPoolEntry;
    PFN_NUMBER PageCount, RealPageCount;
    PPOOL_DESCRIPTOR PoolDescriptor;
    MMPOOL_TYPE PoolType;
    USHORT BlockSize;
    BOOLEAN Combined;
    XTSTATUS Status;

    /* Determine if the allocation is page-aligned */
    if(PAGE_ALIGN(VirtualAddress) == VirtualAddress)
    {
        /* Determine and the memory pool type from the VA mapping */
        PoolType = DeterminePoolType(VirtualAddress);

        /* Verify run level for the specified pool */
        VerifyRunLevel(PoolType, 0, VirtualAddress);

        /* Retrieve original metadata while removing the allocation from the tracking table */
        Tag = UnregisterBigAllocationTag(VirtualAddress, &PageCount, PoolType);
        if(Tag & MM_POOL_PROTECTED)
        {
            /* Strip the protected pool bit */
            Tag &= ~MM_POOL_PROTECTED;
        }
        else if(!Tag)
        {
            /* Fallback to a default tag */
            Tag = SIGNATURE32('B', 'i', 'g', 'A');
            PageCount = 1;
        }

        /* Remove the allocation from the tracking table */
        UnregisterAllocationTag(Tag, PageCount << MM_PAGE_SHIFT, PoolType);

        /* Retrieve the specific pool descriptor based on the masked pool type */
        PoolDescriptor = PoolVector[PoolType];

        /* Update the pool descriptor statistical counters */
        RTL::Atomic::Increment32((PLONG)&PoolDescriptor->RunningFrees);
        RTL::Atomic::ExchangeAdd64((PLONG_PTR)&PoolDescriptor->TotalBytes, -(LONG_PTR)(PageCount << MM_PAGE_SHIFT));

        /* Release the underlying physical pages */
        Status = FreePages(VirtualAddress, &RealPageCount);
        if(Status == STATUS_SUCCESS)
        {
            /* Adjust the big allocation counter */
            RTL::Atomic::ExchangeAdd32((PLONG)&PoolDescriptor->TotalBigAllocations, -(LONG)RealPageCount);
        }

        /* Return status code */
        return Status;
    }

    /* Resolve the pool header */
    PoolEntry = (PPOOL_HEADER)VirtualAddress;
    PoolEntry--;

    /* Extract the structural block size from the pool header */
    BlockSize = PoolEntry->BlockSize;

    /* Determine the underlying pool type and resolve its corresponding pool descriptor */
    PoolType = (MMPOOL_TYPE)((PoolEntry->PoolType - 1) & MM_POOL_TYPE_MASK);
    PoolDescriptor = PoolVector[PoolType];

    /* Verify run level for the specified pool */
    VerifyRunLevel(PoolType, 0, VirtualAddress);

    /* Extract the allocation identifying tag and initialize the consolidation flag */
    Tag = PoolEntry->PoolTag;
    Combined = FALSE;

    /* Check if the allocation tag carries the protected pool modifier */
    if(Tag & MM_POOL_PROTECTED)
    {
        /* Strip the protected pool bit */
        Tag &= ~MM_POOL_PROTECTED;
    }

    /* Remove the allocation from the tracking table */
    UnregisterAllocationTag(Tag, BlockSize * MM_POOL_BLOCK_SIZE, (MMPOOL_TYPE)(PoolEntry->PoolType - 1));

    /* Locate the adjacent forward pool block */
    NextPoolEntry = GetPoolBlock(PoolEntry, BlockSize);

    /* Update the pool descriptor statistical counters */
    RTL::Atomic::Increment32((PLONG)&PoolDescriptor->RunningFrees);
    RTL::Atomic::ExchangeAdd64((PLONG_PTR)&PoolDescriptor->TotalBytes, (LONG_PTR)(-BlockSize * MM_POOL_BLOCK_SIZE));

    /* Acquire the pool lock */
    PoolLockGuard PoolLock((MMPOOL_TYPE)(PoolDescriptor->PoolType & MM_POOL_TYPE_MASK));

    /* Validate the structural integrity of the base block */
    VerifyPoolBlocks(PoolEntry);

    /* Ensure the adjacent forward block does not cross a page boundary */
    if(PAGE_ALIGN(NextPoolEntry) != NextPoolEntry)
    {
        /* Check if the adjacent forward block is currently marked as free */
        if(NextPoolEntry->PoolType == 0)
        {
            /* Flag the deallocation as having triggered a forward block merge */
            Combined = TRUE;

            /* Check if the forward block is large enough */
            if(NextPoolEntry->BlockSize != 1)
            {
                /* Validate the list links */
                VerifyPoolLinks(GetPoolFreeBlock(NextPoolEntry));

                /* Unlink the forward block from its respective free list */
                RemovePoolEntryList(GetPoolFreeBlock(NextPoolEntry));

                /* Re-validate the surrounding list links */
                VerifyPoolLinks(DecodePoolLink((GetPoolFreeBlock(NextPoolEntry))->Flink));
                VerifyPoolLinks(DecodePoolLink((GetPoolFreeBlock(NextPoolEntry))->Blink));
            }

            /* Expand the size of the current block to include the forward free block */
            PoolEntry->BlockSize += NextPoolEntry->BlockSize;
        }
    }

    /* Check if a valid adjacent backward block exists */
    if(PoolEntry->PreviousSize)
    {
        /* Resolve the adjacent backward block and check if it is free */
        NextPoolEntry = GetPoolPreviousBlock(PoolEntry);
        if(NextPoolEntry->PoolType == 0)
        {
            /* Flag the deallocation as having triggered a backward block merge */
            Combined = TRUE;

            /* Check if the backward free block contains embedded list links */
            if(NextPoolEntry->BlockSize != 1)
            {
                /* Validate the backward block list links */
                VerifyPoolLinks(GetPoolFreeBlock(NextPoolEntry));

                /* Extract the backward block from the free list */
                RemovePoolEntryList(GetPoolFreeBlock(NextPoolEntry));

                /* Re-validate the adjacent free list */
                VerifyPoolLinks(DecodePoolLink((GetPoolFreeBlock(NextPoolEntry))->Flink));
                VerifyPoolLinks(DecodePoolLink((GetPoolFreeBlock(NextPoolEntry))->Blink));
            }

            /* Expand the backward block to include the freed base block */
            NextPoolEntry->BlockSize += PoolEntry->BlockSize;

            /* Shift the base entry pointer */
            PoolEntry = NextPoolEntry;
        }
    }

    /* Check whether the consolidated block spans an entire page */
    if((PAGE_ALIGN(PoolEntry) == PoolEntry) &&
       (PAGE_ALIGN(GetPoolNextBlock(PoolEntry)) == GetPoolNextBlock(PoolEntry)))
    {
        /* Release the pool lock */
        PoolLock.Release();

        /* Decrement the total page count and return the entire page back */
        RTL::Atomic::ExchangeAdd32((PLONG)&PoolDescriptor->TotalPages, -1);
        return FreePages(PoolEntry);
    }

    /* Finalize the consolidated block size and mark the primary header as free */
    BlockSize = PoolEntry->BlockSize;
    PoolEntry->PoolType = 0;

    /* Check if any coalescing occurred */
    if(Combined)
    {
        /* Resolve the new adjacent forward block and verify it resides on the same page */
        NextPoolEntry = GetPoolNextBlock(PoolEntry);
        if(PAGE_ALIGN(NextPoolEntry) != NextPoolEntry)
        {
            /* Adjust the backward reference of the forward block */
            NextPoolEntry->PreviousSize = BlockSize;
        }
    }

    /* Insert the freed and consolidated block into the pool free list */
    InsertPoolHeadList(&PoolDescriptor->ListHeads[BlockSize - 1], GetPoolFreeBlock(PoolEntry));

    /* Perform a final linkvalidation and return success */
    VerifyPoolLinks(GetPoolFreeBlock(PoolEntry));
    return STATUS_SUCCESS;
}

/**
 * Initializes the allocations tracking table during early system boot.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::InitializeAllocationsTracking(VOID)
{
    SIZE_T TableSize;
    ULONG Index;
    XTSTATUS Status;
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Not fully implemented yet, HIVE support needed */
    UNIMPLEMENTED;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* TODO: Retrieve tracking table size from the HIVE */
    AllocationsTrackingTableSize = 0;

    /* Calculate the target table size */
    TableSize = MIN(AllocationsTrackingTableSize, (MemoryLayout->NonPagedPoolSize * MM_PAGE_SIZE) >> 8);

    /* Perform a bit-scan to determine the highest set bit */
    for(Index = 0; Index < 32; Index++)
    {
        /* Check if the lowest bit is currently set */
        if(TableSize & 1)
        {
            /* Verify if this is the only remaining set bit */
            if(!(TableSize & ~1))
            {
                /* Exit the loop as the highest bit has been found */
                break;
            }
        }

        /* Shift the size down by one bit to evaluate higher bits */
        TableSize >>= 1;
    }

    /* Check if the bit-scan completed without finding any set bits */
    if(Index == 32)
    {
        /* Apply the default size of 1024 entries */
        AllocationsTrackingTableSize = 1024;
    }
    else
    {
        /* Calculate the aligned power of two size, enforcing a minimum of 64 entries */
        AllocationsTrackingTableSize = MAX(1 << Index, 64);
    }

    /* Iteratively attempt to allocate the tracking table */
    while(TRUE)
    {
        /* Prevent integer overflow when calculating the required byte size for the table */
        if(AllocationsTrackingTableSize + 1 > (MAXULONG_PTR / sizeof(POOL_TRACKING_TABLE)))
        {
            /* Halve the requested entry count and restart the evaluation */
            AllocationsTrackingTableSize >>= 1;
            continue;
        }

        /* Attempt to allocate physical memory for the table */
        Status = MM::Allocator::AllocatePages(NonPagedPool,
                                              (AllocationsTrackingTableSize + 1) *
                                              sizeof(POOL_TRACKING_TABLE), (PVOID *)&AllocationsTrackingTable);

        /* Check if the allocation succeeded */
        if(Status != STATUS_SUCCESS || !AllocationsTrackingTable)
        {
            /* Check if the allocation failed duefor a single entry */
            if(AllocationsTrackingTableSize == 1)
            {
                /* Failed to initialize the pool tracker, raise kernel panic */
                KE::Crash::Panic(0x41, TableSize, (ULONG_PTR)~0, (ULONG_PTR)~0, (ULONG_PTR)~0);
            }

            /* Halve the requested entry count */
            AllocationsTrackingTableSize >>= 1;
        }
        else
        {
            /* Allocation succeeded */
            break;
        }
    }

    /* Increment the table size to account for the overflow bucket entry */
    AllocationsTrackingTableSize++;

    /* Zero the entire memory used by the table */
    RtlZeroMemory(AllocationsTrackingTable, AllocationsTrackingTableSize * sizeof(POOL_TRACKING_TABLE));

    /* Assign the global tracking table as the local table for the bootstrap processor */
    TagTables[0] = AllocationsTrackingTable;

    /* Calculate and store the hash mask */
    AllocationsTrackingTableMask = AllocationsTrackingTableSize - 2;

    /* Initialize the spinlock used to synchronize concurrent modifications to the tracking table */
    KE::SpinLock::InitializeSpinLock(&AllocationsTrackingTableLock);
}

/**
 * Initializes the big allocations tracking table during early system boot.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::InitializeBigAllocationsTracking(VOID)
{
    SIZE_T TableSize;
    ULONG Index;
    XTSTATUS Status;
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Not fully implemented yet, HIVE support needed */
    UNIMPLEMENTED;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* TODO: Retrieve initial big allocation table size from the HIVE */
    BigAllocationsTrackingTableSize = 0;

    /* Calculate the target table size */
    TableSize = MIN(BigAllocationsTrackingTableSize, (MemoryLayout->NonPagedPoolSize * MM_PAGE_SIZE) >> 12);

    /* Perform a bit-scan to determine the highest set bit */
    for(Index = 0; Index < 32; Index++)
    {
        /* Check if the lowest bit is currently set */
        if(TableSize & 1)
        {
            /* Verify if this is the only remaining set bit */
            if(!(TableSize & ~1))
            {
                /* Exit the loop as the highest bit has been found */
                break;
            }
        }

        /* Shift the size down by one bit to evaluate higher bits */
        TableSize >>= 1;
    }

    /* Check if the bit-scan completed without finding any set bits */
    if(Index == 32)
    {
        /* Apply the default size of 4096 entries */
        BigAllocationsTrackingTableSize = 4096;
    }
    else
    {
        /* Calculate the aligned power of two size, enforcing a minimum of 64 entries */
        BigAllocationsTrackingTableSize = MAX(1 << Index, 64);
    }

    /* Iteratively attempt to allocate the tracking table */
    while(TRUE)
    {
        /* Prevent integer overflow when calculating the required byte size for the table */
        if((BigAllocationsTrackingTableSize + 1) > (MAXULONG_PTR / sizeof(POOL_TRACKING_BIG_ALLOCATIONS)))
        {
            /* Halve the requested entry count and restart the evaluation */
            BigAllocationsTrackingTableSize >>= 1;
            continue;
        }

        /* Attempt to allocate physical memory for the table */
        Status = AllocatePages(NonPagedPool,
                               BigAllocationsTrackingTableSize * sizeof(POOL_TRACKING_BIG_ALLOCATIONS),
                               (PVOID*)&BigAllocationsTrackingTable);

        /* Check if the allocation succeeded */
        if(Status != STATUS_SUCCESS || !BigAllocationsTrackingTable)
        {
            /* Check if the allocation failed duefor a single entry */
            if(BigAllocationsTrackingTableSize == 1)
            {
                /* Failed to initialize the pool tracker, raise kernel panic */
                KE::Crash::Panic(0x41, TableSize, (ULONG_PTR)~0, (ULONG_PTR)~0, (ULONG_PTR)~0);
            }

            /* Halve the requested entry count */
            BigAllocationsTrackingTableSize >>= 1;
        }
        else
        {
            /* Allocation succeeded */
            break;
        }
    }

    /* Zero the entire memory used by the table */
    RtlZeroMemory(BigAllocationsTrackingTable, BigAllocationsTrackingTableSize * sizeof(POOL_TRACKING_BIG_ALLOCATIONS));

    /* Iterate through the newly allocated table */
    for(Index = 0; Index < BigAllocationsTrackingTableSize; Index++)
    {
        /* Mark the individual pool tracker entry as free and available */
        BigAllocationsTrackingTable[Index].VirtualAddress = (PVOID)MM_POOL_BIG_ALLOCATIONS_ENTRY_FREE;
    }

    /* Calculate and store the hash mask */
    BigAllocationsTrackingTableHash = BigAllocationsTrackingTableSize - 1;

    /* Initialize the spinlock used to synchronize concurrent modifications to the tracking table */
    KE::SpinLock::InitializeSpinLock(&BigAllocationsTrackingTableLock);

    /* Register the allocation in the tracking table */
    RegisterAllocationTag(SIGNATURE32('M', 'M', 'g', 'r'),
                          SIZE_TO_PAGES(BigAllocationsTrackingTableSize * sizeof(POOL_TRACKING_BIG_ALLOCATIONS)),
                          NonPagedPool);
}

/**
 * Registers a pool memory allocation in the tracking table.
 *
 * @param Tag
 *        Supplies the tag used to identify the allocation.
 *
 * @param Bytes
 *        Supplies the size of the allocation.
 *
 * @param PoolType
 *        Specifies the type of pool from which the memory was allocated.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::RegisterAllocationTag(IN ULONG Tag,
                                     IN SIZE_T Bytes,
                                     IN MMPOOL_TYPE PoolType)
{
    PPOOL_TRACKING_TABLE CpuTable, TableEntry;
    ULONG Hash, Index, Processor;

    /* Retrieve the local tracking table for the current processor */
    Processor = KE::Processor::GetCurrentProcessorNumber();
    CpuTable = TagTables[Processor];

    /* Strip the protected pool bit */
    Tag &= ~MM_POOL_PROTECTED;

    /* Compute the initial hash index */
    Hash = ComputeHash(Tag, AllocationsTrackingTableMask);
    Index = Hash;

    /* Probe the tracking table until a match or an empty slot is found */
    do
    {
        /* Fetch the tracker entry from the CPU table */
        TableEntry = &CpuTable[Hash];

        /* Check if the current entry tracks the requested pool tag */
        if(TableEntry->Tag == Tag)
        {
            /* Update the appropriate statistics based on the pool type */
            if((PoolType & MM_POOL_TYPE_MASK) == NonPagedPool)
            {
                /* Update the non-paged allocation statistics */
                RTL::Atomic::Increment32(&TableEntry->NonPagedAllocations);
                RTL::Atomic::ExchangeAdd64((PLONG_PTR)&TableEntry->NonPagedBytes, Bytes);
            }
            else
            {
                /* Update the paged allocation statistics */
                RTL::Atomic::Increment32(&TableEntry->PagedAllocations);
                RTL::Atomic::ExchangeAdd64((PLONG_PTR)&TableEntry->PagedBytes, Bytes);
            }

            /* The allocation has been successfully tracked, return */
            return;
        }

        /* Check if the CPU table is entirely empty */
        if(TableEntry->Tag == 0)
        {
            /* Check if another processor has claimed this slot in the global table */
            if(AllocationsTrackingTable[Hash].Tag != 0)
            {
                /* Synchronize the local table with the global table */
                TableEntry->Tag = AllocationsTrackingTable[Hash].Tag;

                /* Restart the loop to evaluation */
                continue;
            }

            /* Check if this is not the designated overflow bucket */
            if(Hash != (AllocationsTrackingTableSize - 1))
            {
                /* Start a guarded code block */
                {
                    /* Acquire the tracking table lock */
                    KE::SpinLockGuard TrackingTableLock(&AllocationsTrackingTableLock);

                    /* Perform a double-checked lock */
                    if(AllocationsTrackingTable[Hash].Tag == 0)
                    {
                        /* Claim the slot in both, local and global tracking tables */
                        AllocationsTrackingTable[Hash].Tag = Tag;
                        TableEntry->Tag = Tag;
                    }
                }

                /* Restart the loop */
                continue;
            }
        }

        /* Advance to the next index as hash collision occurred */
        Hash = (Hash + 1) & AllocationsTrackingTableMask;
    }
    while(Hash != Index);

    /* Fallback to the expansion path */
    RegisterAllocationTagExpansion(Tag, Bytes, PoolType);
}

/**
 * Registers a pool memory allocation in the tracking expansion table.
 *
 * @param Tag
 *        Supplies the tag used to identify the allocation.
 *
 * @param Bytes
 *        Supplies the size of the allocation.
 *
 * @param PoolType
 *        Specifies the type of pool from which the memory was allocated.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::RegisterAllocationTagExpansion(IN ULONG Tag,
                                              IN SIZE_T Bytes,
                                              IN MMPOOL_TYPE PoolType)
{
    PPOOL_TRACKING_TABLE NewTrackingTable, OldTrackingTable;
    SIZE_T Footprint, NewSize, Size;
    BOOLEAN UseOverflowBucket;
    PFN_NUMBER FreedPages;
    XTSTATUS Status;
    ULONG Hash;

    /* Initialize local state */
    NewTrackingTable = NULLPTR;
    OldTrackingTable = NULLPTR;
    UseOverflowBucket = FALSE;

    /* Start a guarded code block */
    {
        /* Acquire the tracking table lock */
        KE::SpinLockGuard TrackingTableLock(&AllocationsTrackingTableLock);

        /* Scan the expansion table to locate the requested tag */
        for(Hash = 0; Hash < AllocationsTrackingExpansionTableSize; Hash++)
        {
            /* Check if the current entry already tracks the requested pool tag */
            if(AllocationsTrackingExpansionTable[Hash].Tag == Tag)
            {
                /* Target entry found, break out */
                break;
            }

            /* Check if an unassigned slot has been reached */
            if(AllocationsTrackingExpansionTable[Hash].Tag == 0)
            {
                /* Claim the empty slot for the new pool tag and stop searching */
                AllocationsTrackingExpansionTable[Hash].Tag = Tag;
                break;
            }
        }

        /* Check if the tag was successfully located or a new slot was successfully claimed */
        if(Hash != AllocationsTrackingExpansionTableSize)
        {
            /* Update the appropriate statistics based on the pool type */
            if((PoolType & MM_POOL_TYPE_MASK) == NonPagedPool)
            {
                /* Update the non-paged allocation statistics */
                AllocationsTrackingExpansionTable[Hash].NonPagedAllocations++;
                AllocationsTrackingExpansionTable[Hash].NonPagedBytes += Bytes;
            }
            else
            {
                /* Update the paged allocation statistics */
                AllocationsTrackingExpansionTable[Hash].PagedAllocations++;
                AllocationsTrackingExpansionTable[Hash].PagedBytes += Bytes;
            }

            /* Nothing more to do */
            return;
        }

        /* Check if the global overflow bucket has been activated */
        if(AllocationsTrackingTable[AllocationsTrackingTableSize - 1].Tag != 0)
        {
            /* Use the overflow bucket */
            UseOverflowBucket = TRUE;
        }
        else
        {
            /* Calculate the exact bytes of the expansion table */
            Size = AllocationsTrackingExpansionTableSize * sizeof(POOL_TRACKING_TABLE);

            /* Determine the required physical memory */
            Footprint = ROUND_UP(Size, MM_PAGE_SIZE) + MM_PAGE_SIZE;

            /* Calculate the usable byte size */
            NewSize = ((Footprint / sizeof(POOL_TRACKING_TABLE)) * sizeof(POOL_TRACKING_TABLE));

            /* Allocate memory for the expanded tracking table */
            Status = AllocatePages(NonPagedPool, NewSize, (PVOID *)&NewTrackingTable);
            if(Status != STATUS_SUCCESS || !NewTrackingTable)
            {
                /* Activate the global overflow bucket */
                AllocationsTrackingTable[AllocationsTrackingTableSize - 1].Tag = SIGNATURE32('O', 'v', 'f', 'l');
                UseOverflowBucket = TRUE;
            }
            else
            {
                /* Check if a previous expansion table exists */
                if(AllocationsTrackingExpansionTable != NULLPTR)
                {
                    /* Migrate the existing tracking entries into the new expansion table */
                    RtlCopyMemory(NewTrackingTable, AllocationsTrackingExpansionTable, Size);
                }

                /* Zero out the added usable space */
                RtlZeroMemory((PVOID)(NewTrackingTable + AllocationsTrackingExpansionTableSize), NewSize - Size);

                /* Swap the active expansion table pointers and update the entry count */
                OldTrackingTable = AllocationsTrackingExpansionTable;
                AllocationsTrackingExpansionTable = NewTrackingTable;
                AllocationsTrackingExpansionTableSize = Footprint / sizeof(POOL_TRACKING_TABLE);;

                /* Register the new allocation tag */
                RegisterAllocationTag(SIGNATURE32('M', 'M', 'g', 'r'), Footprint, NonPagedPool);
            }
        }
    }

    /* Handle the fallback scenario */
    if(UseOverflowBucket)
    {
        /* Target the overflow bucket at the end of the tracking table */
        Hash = (ULONG)AllocationsTrackingTableSize - 1;

        /* Update the appropriate statistics based on the pool type */
        if((PoolType & MM_POOL_TYPE_MASK) == NonPagedPool)
        {
            /* Update the non-paged allocation statistics */
            RTL::Atomic::Increment32((PLONG)&AllocationsTrackingTable[Hash].NonPagedAllocations);
            RTL::Atomic::ExchangeAdd64((PLONG_PTR)&AllocationsTrackingTable[Hash].NonPagedBytes, Bytes);
        }
        else
        {
            /* Update the paged allocation statistics */
            RTL::Atomic::Increment32((PLONG)&AllocationsTrackingTable[Hash].PagedAllocations);
            RTL::Atomic::ExchangeAdd64((PLONG_PTR)&AllocationsTrackingTable[Hash].PagedBytes, Bytes);
        }

        /* Nothing more to do */
        return;
    }

    /* Check if an older expansion table needs to be freed */
    if(OldTrackingTable != NULLPTR)
    {
        /* Free the old tracking table and unregister the allocation tag */
        FreePages(OldTrackingTable, &FreedPages);
        UnregisterAllocationTag(SIGNATURE32('M', 'M', 'g', 'r'), (SIZE_T)FreedPages * MM_PAGE_SIZE, NonPagedPool);
    }

    /* Register the caller's original allocation */
    RegisterAllocationTagExpansion(Tag, Bytes, PoolType);
}

/**
 * Registers a big allocation within the tracking table.
 *
 * @param VirtualAddress
 *        Supplies the virtual address of the big allocation.
 *
 * @param Tag
 *        Supplies the tag used to identify the allocation.
 *
 * @param Pages
 *        Supplies the number of physical pages backing the allocation.
 *
 * @param PoolType
 *        Specifies the type of pool from which the memory was allocated.
 *
 * @return This routine returns TRUE on successful insertion, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Allocator::RegisterBigAllocationTag(IN PVOID VirtualAddress,
                                        IN ULONG Tag,
                                        IN ULONG Pages,
                                        IN MMPOOL_TYPE PoolType)
{
    PPOOL_TRACKING_BIG_ALLOCATIONS Entry;
    BOOLEAN Inserted, RequiresExpansion;
    ULONG Hash, StartHash;

    /* Wrap the insertion logic in a retry loop */
    while(TRUE)
    {
        /* Initialize local variables */
        Inserted = FALSE;
        RequiresExpansion = FALSE;

        /* Calculate the initial hash bucket index */
        Hash = ComputeHash(VirtualAddress);

        /* Start a guarded code block */
        {
            /* Acquire the tracking table lock and raise runlevel to DISPATCH level */
            KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
            KE::SpinLockGuard TrackingTableLock(&BigAllocationsTrackingTableLock);

            /* Retrieve the tracker entry */
            Hash &= BigAllocationsTrackingTableHash;
            StartHash = Hash;

            /* Traverse the hash table */
            do
            {
                /* Retrieve the tracker entry */
                Entry = &BigAllocationsTrackingTable[Hash];

                /* Check if the current bucket is marked as free */
                if((ULONG_PTR)Entry->VirtualAddress & MM_POOL_BIG_ALLOCATIONS_ENTRY_FREE)
                {
                    /* Populate the available bucket with the allocation metadata */
                    Entry->NumberOfPages = Pages;
                    Entry->Tag = Tag;
                    Entry->VirtualAddress = VirtualAddress;

                    /* Increment the global usage counter */
                    BigAllocationsInUse++;

                    /* Determine if the table capacity has reached the critical 75% threshold */
                    if(BigAllocationsInUse > (BigAllocationsTrackingTableSize * 3 / 4))
                    {
                        /* Flag the table for expansion */
                        RequiresExpansion = TRUE;
                    }

                    /* Mark insertion as successful and break out of the probing loop */
                    Inserted = TRUE;
                    break;
                }

                /* Advance to the next bucket */
                if(++Hash >= BigAllocationsTrackingTableSize)
                {
                    /* Wrap the index back to the beginning of the table */
                    Hash = 0;
                }

                /* If the traversal has wrapped entirely back to the starting index, the table is saturated */
                if(Hash == StartHash)
                {
                    /* Break out of the probing loop */
                    break;
                }
            }
            while(Hash != StartHash);
        }

        /* Check if the insertion succeeded */
        if(Inserted)
        {
            /* Check if a table expansion is required */
            if(RequiresExpansion)
            {
                /* Trigger a table expansion asynchronously */
                ExpandBigAllocationsTable();
            }

            /* Return success */
            return TRUE;
        }

        /* The table is completely saturated, attempt to expand the table */
        if(ExpandBigAllocationsTable())
        {
            /* The table was successfully expanded, retry the insertion */
            continue;
        }

        /* Table expansion failed, break out of the retry loop */
        break;
    }

    /* Return failure */
    return FALSE;
}

/**
 * Unregisters a pool memory allocation in the tracking table.
 *
 * @param Tag
 *        Supplies the tag used to identify the allocation.
 *
 * @param Bytes
 *        Supplies the size of the allocation.
 *
 * @param PoolType
 *        Specifies the type of pool from which the memory was allocated.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::UnregisterAllocationTag(IN ULONG Tag,
                                       IN SIZE_T Bytes,
                                       IN MMPOOL_TYPE PoolType)
{
    ULONG Hash, Index;
    PPOOL_TRACKING_TABLE CpuTable;
    PPOOL_TRACKING_TABLE TableEntry;
    ULONG Processor;

    /* Retrieve the local tracking table for the current processor */
    Processor = KE::Processor::GetCurrentProcessorNumber();
    CpuTable = TagTables[Processor];

    /* Strip the protected pool bit */
    Tag &= ~MM_POOL_PROTECTED;

    /* Compute the initial hash index */
    Hash = ComputeHash(Tag, AllocationsTrackingTableMask);
    Index = Hash;

    /* Probe the tracking table until a match or an empty slot is found */
    do
    {
        /* Fetch the tracker entry from the CPU table */
        TableEntry = &CpuTable[Hash];

        /* Check if the current entry tracks the requested pool tag */
        if(TableEntry->Tag == Tag)
        {
            /* Update the appropriate statistics based on the pool type */
            if((PoolType & MM_POOL_TYPE_MASK) == NonPagedPool)
            {
                /* Update the non-paged allocation statistics */
                RTL::Atomic::Increment32(&TableEntry->NonPagedFrees);
                RTL::Atomic::ExchangeAdd64((PLONG_PTR)&TableEntry->NonPagedBytes, 0 - Bytes);
            }
            else
            {
                /* Update the paged allocation statistics */
                RTL::Atomic::Increment32(&TableEntry->PagedFrees);
                RTL::Atomic::ExchangeAdd64((PLONG_PTR)&TableEntry->PagedBytes, 0 - Bytes);
            }

            /* The allocation has been successfully tracked, return */
            return;
        }

        /* Check if the CPU table is entirely empty */
        if(TableEntry->Tag == 0)
        {
            /* Check if another processor has claimed this slot in the global table */
            if(AllocationsTrackingTable[Hash].Tag != 0)
            {
                /* Synchronize the local table with the global table */
                TableEntry->Tag = AllocationsTrackingTable[Hash].Tag;

                /* Restart the loop to evaluation */
                continue;
            }
        }

        /* Advance to the next index as hash collision occurred */
        Hash = (Hash + 1) & AllocationsTrackingTableMask;
    }
    while(Hash != Index);

    /* Fallback to the expansion path */
    UnregisterAllocationTagExpansion(Tag, Bytes, PoolType);
}

/**
 * Unregisters a pool memory allocation in the tracking expansion table.
 *
 * @param Tag
 *        Supplies the tag used to identify the allocation.
 *
 * @param Bytes
 *        Supplies the size of the allocation.
 *
 * @param PoolType
 *        Specifies the type of pool from which the memory was allocated.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::UnregisterAllocationTagExpansion(IN ULONG Tag,
                                                IN SIZE_T Bytes,
                                                IN MMPOOL_TYPE PoolType)
{
    PPOOL_TRACKING_TABLE CpuTable;
    ULONG Hash;
    ULONG Processor;

    /* Start a guarded code block */
    {
        /* Acquire the tracking table lock */
        KE::SpinLockGuard TrackingTableLock(&AllocationsTrackingTableLock);

        /* Scan the expansion table */
        for(Hash = 0; Hash < AllocationsTrackingExpansionTableSize; Hash++)
        {
            /* Check if the current entry matches the tag */
            if(AllocationsTrackingExpansionTable[Hash].Tag == Tag)
            {
                /* Update the appropriate statistics based on the pool type */
                if((PoolType & MM_POOL_TYPE_MASK) == NonPagedPool)
                {
                    /* Update the non-paged allocation statistics */
                    AllocationsTrackingExpansionTable[Hash].NonPagedFrees++;
                    AllocationsTrackingExpansionTable[Hash].NonPagedBytes -= Bytes;
                }
                else
                {
                    /* Update the paged allocation statistics */
                    AllocationsTrackingExpansionTable[Hash].PagedFrees++;
                    AllocationsTrackingExpansionTable[Hash].PagedBytes -= Bytes;
                }

                /* Nothing more to do */
                return;
            }

            /* Check if an empty slot is encountered */
            if(AllocationsTrackingExpansionTable[Hash].Tag == 0)
            {
                /* Stop scanning as all active tags are contiguous */
                break;
            }
        }
    }

    /* Target the index of the overflow bucket */
    Hash = (ULONG)AllocationsTrackingTableSize - 1;

    /* Retrieve the local tracking table for the current processor */
    Processor = KE::Processor::GetCurrentProcessorNumber();
    CpuTable = TagTables[Processor];

    /* Update the appropriate statistics based on the pool type */
    if((PoolType & MM_POOL_TYPE_MASK) == NonPagedPool)
    {
        /* Update the non-paged allocation statistics */
        RTL::Atomic::Increment32((PLONG)&CpuTable[Hash].NonPagedFrees);
        RTL::Atomic::ExchangeAdd64((PLONG_PTR)&CpuTable[Hash].NonPagedBytes, 0 - Bytes);
    }
    else
    {
        /* Update the paged allocation statistics */
        RTL::Atomic::Increment32((PLONG)&CpuTable[Hash].PagedFrees);
        RTL::Atomic::ExchangeAdd64((PLONG_PTR)&CpuTable[Hash].PagedBytes, 0 - Bytes);
    }
}

/**
 * Unregisters a big allocation from the tracking table and retrieves its metadata.
 *
 * @param VirtualAddress
 *        Supplies the virtual address of the big allocation to be removed.
 *
 * @param Pages
 *        Supplies the number of physical pages backing the allocation.
 *
 * @param PoolType
 *        Specifies the pool type of the allocation.
 *
 * @return This routine returns the allocation pool tag if found, or a default signature otherwise.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::Allocator::UnregisterBigAllocationTag(IN PVOID VirtualAddress,
                                          OUT PULONG_PTR Pages,
                                          IN MMPOOL_TYPE PoolType)
{
    ULONG Hash, StartHash;
    ULONG PoolTag;
    BOOLEAN Found;
    PPOOL_TRACKING_BIG_ALLOCATIONS Entry;

    /* Initialize default state */
    Found = FALSE;

    /* Calculate the initial hash bucket index */
    Hash = ComputeHash(VirtualAddress);

    /* Start a guarded code block */
    {
        /* Acquire the tracking table lock and raise runlevel to DISPATCH level */
        KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
        KE::SpinLockGuard TrackingTableLock(&BigAllocationsTrackingTableLock);

        /* Mask the computed hash and record the starting bucket */
        Hash &= BigAllocationsTrackingTableHash;
        StartHash = Hash;

        /* Traverse the hash table using linear probing to pinpoint the exact allocation address */
        while(TRUE)
        {
            /* Retrieve the tracker entry */
            Entry = &BigAllocationsTrackingTable[Hash];

            /* Check if the bucket contains the target virtual address */
            if(Entry->VirtualAddress == VirtualAddress)
            {
                /* Capture the allocation metadata */
                *Pages = Entry->NumberOfPages;
                PoolTag = Entry->Tag;

                /* Invalidate the entry */
                Entry->VirtualAddress = (PVOID)MM_POOL_BIG_ALLOCATIONS_ENTRY_FREE;

                /* Decrement the global usage counter */
                BigAllocationsInUse--;

                /* Update the found flag and break out of the probing loop */
                Found = TRUE;
                break;
            }

            /* Advance to the next bucket */
            if(++Hash >= BigAllocationsTrackingTableSize)
            {
                /* Wrap the hash index back to zero */
                Hash = 0;
            }

            /* Check if the traversal has wrapped entirely back to the starting index */
            if(Hash == StartHash)
            {
                /* Abort the search */
                break;
            }
        }
    }

    /* Evaluate the result of the table traversal */
    if(Found)
    {
        /* Return the original tag captured from the tracker */
        return PoolTag;
    }

    /* Return an empty page count and a fallback tag */
    *Pages = 0;
    return SIGNATURE32('B', 'i', 'g', 'A');
}
