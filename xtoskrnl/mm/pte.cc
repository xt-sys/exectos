/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/pte.cc
 * DESCRIPTION:     Page Table Entry (PTE) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Finds a free cluster of system PTEs that can satisfy a given size.
 *
 * @param NumberOfPtes
 *        The number of contiguous PTEs required.
 *
 * @param SystemPtePoolType
 *        Specifies the system PTE pool to search within.
 *
 * @param FoundCluster
 *        On success, receives a pointer to the first PTE of the found cluster.
 *
 * @param PreviousClusterNode
 *        On success, receives a pointer to the list node that precedes the found cluster.
 *
 * @return This routine returns TRUE if a suitable cluster was found, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Pte::FindFreeCluster(IN PFN_COUNT NumberOfPtes,
                         IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType,
                         OUT PMMPTE *FoundCluster,
                         OUT PMMPTE *PreviousClusterNode)
{
    PMMPTE CurrentCluster;
    PMMPTE PreviousNode = &FirstSystemFreePte[SystemPtePoolType];
    ULONG ClusterSize;

    /* Find a free PTE cluster large enough for the request */
    while(MM::Paging::GetNextEntry(PreviousNode) != MAXULONG)
    {
        /* Retrieve the cluster and its size */
        CurrentCluster = MM::Paging::AdvancePte(SystemPteBase, MM::Paging::GetNextEntry(PreviousNode));
        ClusterSize = GetClusterSize(CurrentCluster);

        /* Check if this cluster is large enough */
        if(NumberOfPtes <= ClusterSize)
        {
            /* Found a suitable cluster */
            *FoundCluster = CurrentCluster;
            *PreviousClusterNode = PreviousNode;
            return TRUE;
        }

        /* This cluster is too small, check the next one */
        PreviousNode = CurrentCluster;
    }

    /* No suitable cluster was found */
    return FALSE;
}

/**
 * Decodes and returns the size of a free PTE cluster.
 *
 * @param Pte
 *        Supplies a pointer to the first PTE of the free cluster to inspect.
 *
 * @return This routine returns the total number of contiguous PTEs in the free cluster.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::Pte::GetClusterSize(IN PMMPTE Pte)
{
    /* A special flag in the first PTE indicates a free cluster of size one */
    if(MM::Paging::GetOneEntry(Pte))
    {
        /* Flag is set, so the cluster size is 1 by definition */
        return 1;
    }

    /* For larger clusters, the size is encoded in the second PTE of the block */
    Pte = MM::Paging::GetNextPte(Pte);
    return MM::Paging::GetNextEntry(Pte);
}

/**
 * Calculates the number of Page Table Entries (PTEs) that fit within a single page.
 *
 * @return This routine returns the number of PTEs per page.
 *
 * @since XT 1.0
 */
XTAPI
PFN_COUNT
MM::Pte::GetPtesPerPage(VOID)
{
    /* Calculate and return the number of PTEs per page */
    return MM_PAGE_SIZE / MM::Paging::GetPteSize();
}

/**
 * Returns a pointer to the valid PTE.
 *
 * @return This routine returns a pointer to the valid PTE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::Pte::GetValidPte()
{
    /* Return a pointer to the valid PTE */
    return &ValidPte;
}

/**
 * Formats a range of PTEs into a freelist-based pool for system allocations.
 *
 * @param StartingPte
 *        Supplies a pointer to the start of the PTE range to be formatted.
 *
 * @param NumberOfPtes
 *        Supplies the total number of PTEs in the contiguous range.
 *
 * @param PoolType
 *        The system PTE pool type that this range will be used for.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::InitializeSystemPtePool(IN PMMPTE StartingPte,
                                 IN PFN_COUNT NumberOfPtes,
                                 IN MMSYSTEM_PTE_POOL_TYPE PoolType)
{
    /* Set the system PTE base address */
    SystemPteBase = GetSystemPteBaseAddress();

    /* Record the boundaries of this new PTE pool */
    SystemPtesStart[PoolType] = StartingPte;
    SystemPtesEnd[PoolType] = MM::Paging::AdvancePte(StartingPte, NumberOfPtes - 1);

    /* Zero the memory for the new PTE pool before use */
    RTL::Memory::ZeroMemory(StartingPte, NumberOfPtes * MM::Paging::GetPteSize());

    /* Build the free list head to point to the start of the pool */
    MM::Paging::SetNextEntry(StartingPte, MAXULONG);
    MM::Paging::ClearPte(&FirstSystemFreePte[PoolType]);
    MM::Paging::SetNextEntry(&FirstSystemFreePte[PoolType], MM::Paging::GetPteDistance(StartingPte, SystemPteBase));

    /* Use the second PTE slot to store the total size of this pool */
    StartingPte = MM::Paging::GetNextPte(StartingPte);
    MM::Paging::ClearPte(StartingPte);
    MM::Paging::SetNextEntry(StartingPte, NumberOfPtes);

    /* Record the total number of free PTEs in this pool */
    TotalSystemFreePtes[PoolType] = NumberOfPtes;
}

/**
 * Sets up the entire system PTE address space.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::InitializeSystemPteSpace(VOID)
{
    PMMPTE PointerPte;
    PMMPTE FirstZeroingPte;
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Retrieve the system's memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Map the page table hierarchy for the entire system PTE space */
    MM::Pte::MapPPE(MemoryLayout->SystemSpaceStart, MemoryLayout->SystemSpaceEnd, &ValidPte);
    MM::Pte::MapPDE(MemoryLayout->SystemSpaceStart, MemoryLayout->SystemSpaceEnd, &ValidPte);

    /* Format the main block of system PTEs into a free list pool */
    PointerPte = MM::Paging::GetPteAddress(MemoryLayout->SystemSpaceStart);
    InitializeSystemPtePool(PointerPte, MM::Manager::GetNumberOfSystemPtes(), SystemPteSpace);

    /* Reserve and zero a dedicated block of system PTEs */
    FirstZeroingPte = ReserveSystemPtes(MM_RESERVED_ZERO_PTES + 1, SystemPteSpace);
    RTL::Memory::ZeroMemory(FirstZeroingPte, (MM_RESERVED_ZERO_PTES + 1) * MM::Paging::GetPteSize());

    /* Use the first PTE of this block as a counter for available zeroing PTEs */
    MM::Paging::SetPte(FirstZeroingPte, MM_RESERVED_ZERO_PTES, 0);
}

/**
 * Maps a range of virtual addresses at the PDE (Page Directory Entry) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplatePde
 *        A template PDE to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapPDE(PVOID StartAddress,
                PVOID EndAddress,
                PMMPDE TemplatePde)
{
    PMMPDE EndSpace, PointerPde;

    /* Get PDE addresses */
    PointerPde = MM::Paging::GetPdeAddress(StartAddress);
    EndSpace = MM::Paging::GetPdeAddress(EndAddress);

    /* Iterate over all PDEs */
    while(PointerPde <= EndSpace)
    {
        /* Check if PDE is already mapped */
        if(!MM::Paging::PteValid(PointerPde))
        {
            /* Map PDE */
            MM::Paging::SetPte(TemplatePde, MM::Pfn::AllocateBootstrapPages(1), 0);
            MM::Paging::WritePte(PointerPde, *TemplatePde);

            /* Clear the page table */
            RTL::Memory::ZeroMemory(MM::Paging::GetPteVirtualAddress(PointerPde), MM_PAGE_SIZE);
        }

        /* Get next table entry */
        PointerPde = MM::Paging::GetNextPte(PointerPde);
    }
}

/**
 * Maps a range of virtual addresses at the PTE (Page Table Entry) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplatePte
 *        A template PTE to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapPTE(PVOID StartAddress,
                PVOID EndAddress,
                PMMPTE TemplatePte)
{
    PMMPTE EndSpace, PointerPte;

    /* Get PTE addresses */
    PointerPte = MM::Paging::GetPteAddress(StartAddress);
    EndSpace = MM::Paging::GetPteAddress(EndAddress);

    /* Iterate over all PTEs */
    while(PointerPte <= EndSpace)
    {
        /* Check if PTE is already mapped */
        if(!MM::Paging::PteValid(PointerPte))
        {
            /* Map PTE */
            MM::Paging::SetPte(TemplatePte, MM::Pfn::AllocateBootstrapPages(1), 0);
            MM::Paging::WritePte(PointerPte, *TemplatePte);

            /* Clear the page table */
            RTL::Memory::ZeroMemory(MM::Paging::GetPteVirtualAddress(PointerPte), MM_PAGE_SIZE);
        }

        /* Get next table entry */
        PointerPte = MM::Paging::GetNextPte(PointerPte);
    }
}


/**
 * Releases a block of system PTEs into a specified pool.
 *
 * @param StartingPte
 *        A pointer to the first PTE to release.
 *
 * @param NumberOfPtes
 *        The number of contiguous PTEs to release.
 *
 * @param SystemPtePoolType
 *        Specifies the system PTE pool to release into.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::ReleaseSystemPtes(IN PMMPTE StartingPte,
                           IN PFN_COUNT NumberOfPtes,
                           IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType)
{
    PMMPTE NextPte, PreviousPte, ReleasedPte;
    ULONG ClusterSize;

    /* Raise runlevel and acquire lock to protect the PTE pool */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard SpinLock(SystemSpaceLock);

    /* Increment the total number of available PTEs in this pool */
    TotalSystemFreePtes[SystemPtePoolType] += NumberOfPtes;

    /* Start at the head of the free list for this pool */
    PreviousPte = &FirstSystemFreePte[SystemPtePoolType];
    ReleasedPte = NULLPTR;

    /* Iterate through the free list to find adjacent blocks */
    while(MM::Paging::GetNextEntry(PreviousPte) != MAXULONG)
    {
        /* Get the next free cluster to check its size */
        NextPte = MM::Paging::AdvancePte(SystemPteBase, MM::Paging::GetNextEntry(PreviousPte));
        ClusterSize = GetClusterSize(NextPte);

        /* Check if the released block is adjacent to the current free block */
        if((MM::Paging::AdvancePte(NextPte, ClusterSize) == StartingPte) ||
           (MM::Paging::AdvancePte(StartingPte, NumberOfPtes) == NextPte))
        {
            /* Merge the blocks by adding their sizes */
            NumberOfPtes += ClusterSize;

            /* Check if the current free block is before the released block */
            if(NextPte < StartingPte)
            {
                /* The new merged block starts at the current free block's address */
                StartingPte = NextPte;
            }

            /* Unlink the current free block as it is being merged */
            MM::Paging::SetNextEntry(PreviousPte, MM::Paging::GetNextEntry(NextPte));

            /* Check if the block represents more than one PTE */
            if(!MM::Paging::GetOneEntry(NextPte))
            {
                /* Clear block header and move to the size PTE */
                MM::Paging::ClearPte(NextPte);
                NextPte = MM::Paging::GetNextPte(NextPte);
            }

            /* Clear the merged block */
            MM::Paging::ClearPte(NextPte);

            /* Reset insertion point since block size/address changed due to merge */
            ReleasedPte = NULLPTR;
        }
        else
        {
            /* Select the first free block large enough as insertion point */
            if((ReleasedPte == NULLPTR) && (NumberOfPtes <= ClusterSize))
            {
                /* Mark this as the insertion point */
                ReleasedPte = PreviousPte;
            }

            /* Advance to the next free block */
            PreviousPte = NextPte;
        }
    }

    /* Check if there is only one PTE to release */
    if(NumberOfPtes == 1)
    {
        /* Mark it as a single-PTE block */
        MM::Paging::SetOneEntry(StartingPte, 1);
    }
    else
    {
        /* Otherwise, mark it as a multi-PTE block */
        MM::Paging::SetOneEntry(StartingPte, 0);

        /* The next PTE stores the size of the block */
        NextPte = MM::Paging::GetNextPte(StartingPte);
        MM::Paging::SetNextEntry(NextPte, NumberOfPtes);
    }

    /* Check if no suitable insertion point was found */
    if(ReleasedPte == NULLPTR)
    {
        /* Insert at the end of the list */
        ReleasedPte = PreviousPte;
    }

    /* Link the new block into the free list */
    MM::Paging::SetNextEntry(StartingPte, MM::Paging::GetNextEntry(ReleasedPte));
    MM::Paging::SetNextEntry(ReleasedPte, MM::Paging::GetPteDistance(StartingPte, SystemPteBase));

    /* Flush the TLB to ensure address translation consistency */
    AR::CpuFunc::FlushTlb();
}

/**
 * Reserves a contiguous block of system PTEs from a specified pool.
 *
 * @param NumberOfPtes
 *        The number of contiguous PTEs to reserve.
 *
 * @param SystemPtePoolType
 *        Specifies the system PTE pool from which to allocate.
 *
 * @return This routine returns a pointer to the beginning of the reserved block,
 *         or NULLPTR if not enough contiguous PTEs are available.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::Pte::ReserveSystemPtes(IN PFN_COUNT NumberOfPtes,
                           IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType)
{
    PMMPTE NextPte, PreviousPte, ReservedPte;
    ULONG ClusterSize;

    /* Raise runlevel and acquire lock to protect the PTE pool */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard SpinLock(SystemSpaceLock);

    /* Find a free PTE cluster large enough for the request */
    if(!FindFreeCluster(NumberOfPtes, SystemPtePoolType, &NextPte, &PreviousPte))
    {
        /* Out of system PTEs for this pool, return NULLPTR */
        return NULLPTR;
    }

    /* We have the cluster, now get its size for the allocation logic below */
    ClusterSize = GetClusterSize(NextPte);

    /* Unlink the found cluster from the free list for processing */
    MM::Paging::SetNextEntry(PreviousPte, MM::Paging::GetNextEntry(NextPte));

    /* Handle the allocation based on whether the cluster size is an exact match */
    if(ClusterSize == NumberOfPtes)
    {
        /* Exact match, allocate the entire cluster */
        ReservedPte = NextPte;

        /* Handle metadata cleanup for a single-PTE cluster */
        if(MM::Paging::GetOneEntry(NextPte))
        {
            /* Clear the PTE that held the list metadata */
            MM::Paging::ClearPte(NextPte);
            NextPte = MM::Paging::GetNextPte(NextPte);
        }

        /* Clear the PTE that held the cluster size */
        MM::Paging::ClearPte(NextPte);
    }
    else
    {
        /* Cluster is larger than needed, so it will be split */
        ClusterSize -= NumberOfPtes;
        ReservedPte = MM::Paging::AdvancePte(NextPte, ClusterSize);

        /* Update metadata for the new, smaller leftover cluster */
        if(ClusterSize == 1)
        {
            /* The leftover fragment is a single PTE */
            MM::Paging::SetOneEntry(NextPte, 1);
            MM::Paging::ClearPte(ReservedPte);
        }
        else
        {
            /* The leftover fragment is larger than one PTE */
            NextPte = MM::Paging::GetNextPte(NextPte);
            MM::Paging::SetNextEntry(NextPte, ClusterSize);
        }

        /* Find the correct sorted position to re-insert the leftover fragment */
        PreviousPte = &FirstSystemFreePte[SystemPtePoolType];
        while(MM::Paging::GetNextEntry(PreviousPte) != MAXULONG)
        {
            /* Get the next free cluster to check its size */
            NextPte = MM::Paging::AdvancePte(SystemPteBase, MM::Paging::GetNextEntry(PreviousPte));

            /* Check if the leftover fragment should be inserted here */
            if(ClusterSize <= GetClusterSize(NextPte))
            {
                /* Found the correct sorted position */
                break;
            }

            /* Advance to the next entry */
            PreviousPte = NextPte;
        }

        /* Get a pointer to the start of the leftover fragment */
        NextPte = MM::Paging::AdvancePte(ReservedPte, -ClusterSize);

        /* Insert the leftover fragment back into the free list at its sorted position */
        MM::Paging::SetNextEntry(NextPte, MM::Paging::GetNextEntry(PreviousPte));
        MM::Paging::SetNextEntry(PreviousPte, MM::Paging::GetPteDistance(NextPte, SystemPteBase));
    }

    /* Decrement the total number of available PTEs in this pool */
    TotalSystemFreePtes[SystemPtePoolType] -= NumberOfPtes;

    /* Flush the TLB to ensure address translation consistency */
    AR::CpuFunc::FlushTlb();

    /* Return a pointer to the start of the reserved PTE block */
    return ReservedPte;
}
