/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/affinity.cc
 * DESCRIPTION:     XT kernel processor affinity management support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Atomically sets the target processor's affinity bit within the affinity map.
 *
 * @param AffinityMap
 *        Supplies a pointer to the affinity map to be modified.
 *
 * @param CpuNumber
 *        Supplies the logical processor number to include in the affinity map.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Affinity::AtomicSetProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                         IN ULONG CpuNumber)
{
    /* Verify that the target processor falls within the allocated map boundaries */
    if((CpuNumber / 64) < AffinityMap->Size)
    {
        /* Atomically set the target CPU bit */
        RTL::Atomic::Or64((PLONG_PTR)&AffinityMap->Bitmap[CpuNumber / 64], ((KAFFINITY)1 << (CpuNumber % 64)));
    }
}

/**
 * Computes the memory size required to allocate an affinity map.
 *
 * @param CpuCount
 *        Supplies the total number of logical processors the map needs to support.
 *
 * @param RequiredMapSize
 *        Supplies a pointer to a variable that receives the 8-byte aligned allocation size in bytes.
 *
 * @param RequiredBlockCount
 *        Supplies a pointer to a variable that receives the number of KAFFINITY blocks required to hold all CPUs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Affinity::CalculateAffinityMapSize(IN ULONG CpuCount,
                                       OUT PULONG RequiredMapSize,
                                       OUT PULONG RequiredBlockCount)
{
    ULONG AffinitySize, MapSize;

    /* Calculate the required number of blocks and the total structure size */
    AffinitySize = (CpuCount + 63) / 64;
    MapSize = sizeof(KAFFINITY_MAP) + (AffinitySize * sizeof(KAFFINITY));

    /* Check if an allocation size is required */
    if(RequiredMapSize != NULLPTR)
    {
        /* Return the required allocation bytes aligned to an 8-byte boundary */
        *RequiredMapSize = (MapSize + 7) & ~7;
    }

    /* Check if a block count is required */
    if(RequiredBlockCount != NULLPTR)
    {
        /* Return the required logical block count */
        *RequiredBlockCount = AffinitySize;
    }
}

/**
 * Checks whether a specific processor is included in the affinity map.
 *
 * @param AffinityMap
 *        Supplies a pointer to the affinity map to query.
 *
 * @param CpuNumber
 *        Supplies the logical processor number to test.
 *
 * @return This routine returns TRUE if the processor's bit is set in the map, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
KE::Affinity::CheckProcessorAffinity(IN PKAFFINITY_MAP AffinityMap,
                                     IN ULONG CpuNumber)
{
    /* Verify that the target processor falls within the allocated map boundaries */
    if((CpuNumber / 64) < AffinityMap->Size)
    {
        /* Isolate and test the specific bit corresponding to the target CPU */
        return (AffinityMap->Bitmap[CpuNumber / 64] & ((KAFFINITY)1 << (CpuNumber % 64))) != 0;
    }

    /* Return FALSE if the requested CPU exceeds the map capacity */
    return FALSE;
}

/**
 * Clears all processor bindings from the given affinity map.
 *
 * @param AffinityMap
 *        Supplies a pointer to the affinity map to be cleared.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Affinity::ClearAffinityMap(IN OUT PKAFFINITY_MAP AffinityMap)
{
    /* Zero out the entire bitmap */
    RTL::Memory::ZeroMemory(AffinityMap->Bitmap, AffinityMap->Size * sizeof(KAFFINITY));
}

/**
 * Clears the affinity bit for a specified processor. This is a non-atomic operation.
 *
 * @param AffinityMap
 *        Supplies a pointer to the affinity map to be modified.
 *
 * @param CpuNumber
 *        Supplies the logical processor number to exclude from the affinity map.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Affinity::ClearProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                     IN ULONG CpuNumber)
{
    /* Verify that the target processor falls within the allocated map boundaries */
    if((CpuNumber / 64) < AffinityMap->Size)
    {
        /* Clear the target CPU bit in the affinity map */
        AffinityMap->Bitmap[CpuNumber / 64] &= ~((KAFFINITY)1 << (CpuNumber % 64));
    }
}

/**
 * Copies the topological layout and processor bindings from a source affinity map to a destination map.
 *
 * @param Destination
 *        Supplies a pointer to the target affinity map that will receive the copied data.
 *
 * @param Source
 *        Supplies a pointer to the source affinity map containing the active processor bindings.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::Affinity::CopyAffinity(OUT PKAFFINITY_MAP Destination,
                           IN PKAFFINITY_MAP Source)
{
    ULONG Index;

    /* Validate that the destination map can accommodate the source topology */
    if(Destination->Size < Source->Size)
    {
        /* Buffer overrun prevention */
        return STATUS_BUFFER_TOO_SMALL;
    }

    /* Copy map metadata */
    Destination->Reserved = Source->Reserved;

    /* Copy the active affinity bitmasks */
    for(Index = 0; Index < Source->Size; Index++)
    {
        /* Replicate the hardware topology bindings across all active array elements */
        Destination->Bitmap[Index] = Source->Bitmap[Index];
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Allocates and initializes a new affinity map for the specified number of processors.
 *
 * @param CpuCount
 *        Supplies the total number of logical processors the map needs to support.
 *
 * @param AffinityMap
 *        Supplies a pointer to a variable that receives the address of the newly allocated and initialized map.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::Affinity::CreateAffinityMap(IN ULONG CpuCount,
                                OUT PKAFFINITY_MAP* AffinityMap)
{
    PKAFFINITY_MAP AllocatedMap;
    ULONG BlockCount, MapSize;
    XTSTATUS Status;

    /* Query the required allocation size and internal block count */
    KE::Affinity::CalculateAffinityMapSize(CpuCount, &MapSize, &BlockCount);

    /* Allocate the memory block from the specified pool */
    Status = MM::Allocator::AllocatePool(NonPagedPool, MapSize, (PVOID*)&AllocatedMap);
    if(Status != STATUS_SUCCESS)
    {
        /* Allocation failed, return status code */
        return Status;
    }

    /* Zero the memory to ensure all processor bits are initially cleared */
    RTL::Memory::ZeroMemory(AllocatedMap, MapSize);

    /* Initialize the internal metadata required by iteration and validation routines */
    AllocatedMap->Size = BlockCount;

    /* Return the constructed map to the caller */
    *AffinityMap = AllocatedMap;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Frees a previously allocated affinity map and returns its memory to the pool.
 *
 * @param AffinityMap
 *        Supplies a pointer to the affinity map to be freed and destroyed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Affinity::DestroyAffinityMap(IN PKAFFINITY_MAP AffinityMap)
{
    /* Ensure the map pointer is valid */
    if(AffinityMap != NULLPTR)
    {
        /* Free the memory block back to the kernel pool */
        MM::Allocator::FreePool((PVOID)AffinityMap);
    }
}

/**
 * Locates the next available logical processor to the left (higher topological index) of a specified seed.
 *
 * @param ThreadSeed
 *        Supplies the logical processor index used as the starting point for the upward search.
 *
 * @param AffinityMap
 *        Supplies a pointer to the extended affinity map defining the permitted processors.
 *
 * @return This routine returns the absolute topological index of the selected processor.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
KE::Affinity::FindNextLeftSetProcessor(IN ULONG ThreadSeed,
                                       IN PKAFFINITY_MAP AffinityMap)
{
    ULONG BitIndex, BitsPerMask, Index, StartBit, StartIndex;
    KAFFINITY Mask;

    /* Define the architectural bit width of a single affinity mask */
    BitsPerMask = sizeof(KAFFINITY) * 8;

    /* Prevent division by zero and out-of-bounds access if the topology map is uninitialized or empty */
    if(AffinityMap->Size == 0)
    {
        /* Fallback to the bootstrap processor */
        return 0;
    }

    /* Calculate the target array index and bit offset based on the thread seed */
    StartIndex = (ThreadSeed / BitsPerMask) % AffinityMap->Size;
    StartBit = ThreadSeed % BitsPerMask;

    /* Isolate the segment of the current affinity mask strictly to the left */
    if(StartBit == (BitsPerMask - 1))
    {
        /* Prevent undefined behavior when shifting by the total architectural bit width */
        Mask = 0;
    }
    else
    {
        /* Mask out the seed bit and all bits below it */
        Mask = AffinityMap->Bitmap[StartIndex] & (~((KAFFINITY)0) << (StartBit + 1));
    }

    /* Evaluate if any allowed processors exist in the higher portion of the current mask */
    if(Mask != 0)
    {
        /* Locate the rightmost set bit within this masked subset */
        if(AR::CpuFunctions::ScanForwardBit(&BitIndex, Mask))
        {
            /* Return the absolute topological index of the located processor */
            return (StartIndex * BitsPerMask) + BitIndex;
        }
    }

    /* Ascend through the subsequent array elements in the map */
    for(Index = StartIndex + 1; Index < AffinityMap->Size; Index++)
    {
        /* Load the complete processor mask for the current array boundary */
        Mask = AffinityMap->Bitmap[Index];

        /* Check if this segment contains any active processor bindings */
        if(Mask != 0)
        {
            /* Find the lowest available processor within this array element */
            if(AR::CpuFunctions::ScanForwardBit(&BitIndex, Mask))
            {
                /* Return the absolute topological index of the located processor */
                return (Index * BitsPerMask) + BitIndex;
            }
        }
    }

    /* Wrap around and scan the entire map from the bottom to find the lowest globally permitted processor */
    for(Index = 0; Index < AffinityMap->Size; Index++)
    {
        /* Load the complete processor mask for the current array boundary */
        Mask = AffinityMap->Bitmap[Index];

        /* Check if this segment contains any active processor bindings */
        if(Mask != 0)
        {
            /* Find the lowest available processor within this array element */
            if(AR::CpuFunctions::ScanForwardBit(&BitIndex, Mask))
            {
                /* Return the absolute topological index of the located processor */
                return (Index * BitsPerMask) + BitIndex;
            }
        }
    }

    /* Fallback to the bootstrap processor */
    return 0;
}

/**
 * Locates the next available logical processor to the right (lower topological index) of a specified seed.
 *
 * @param ThreadSeed
 *        Supplies the logical processor index used as the starting point for the downward search.
 *
 * @param AffinityMap
 *        Supplies a pointer to the extended affinity map defining the permitted processors.
 *
 * @return This routine returns the absolute topological index of the selected processor.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
KE::Affinity::FindNextRightSetProcessor(IN ULONG ThreadSeed,
                                        IN PKAFFINITY_MAP AffinityMap)
{
    ULONG BitIndex, BitsPerMask, StartBit, StartIndex;
    KAFFINITY Mask;
    LONG Index;

    /* Define the architectural bit width of a single affinity mask */
    BitsPerMask = sizeof(KAFFINITY) * 8;

    /* Prevent division by zero and out-of-bounds access if the topology map is uninitialized or empty */
    if(AffinityMap->Size == 0)
    {
        /* Fallback to the bootstrap processor */
        return 0;
    }

    /* Calculate the target array index and bit offset based on the seed */
    StartIndex = (ThreadSeed / BitsPerMask) % AffinityMap->Size;
    StartBit = ThreadSeed % BitsPerMask;

    /* Isolate the segment of the current affinity mask strictly to the right */
    Mask = AffinityMap->Bitmap[StartIndex] & (((KAFFINITY)1 << StartBit) - 1);

    /* Evaluate if any allowed processors exist in the lower portion of the current mask */
    if(Mask != 0)
    {
        /* Locate the leftmost set bit within this masked subset */
        if(AR::CpuFunctions::ScanReverseBit(&BitIndex, Mask))
        {
            /* Return the absolute topological index of the located processor */
            return (StartIndex * BitsPerMask) + BitIndex;
        }
    }

    /* Descend through the preceding array elements in the map */
    for(Index = (LONG)StartIndex - 1; Index >= 0; Index--)
    {
        /* Load the complete processor mask for the current array boundary */
        Mask = AffinityMap->Bitmap[Index];

        /* Check if this segment contains any active processor bindings */
        if(Mask != 0)
        {
            /* Find the highest available processor within this array element */
            if(AR::CpuFunctions::ScanReverseBit(&BitIndex, Mask))
            {
                /* Return the absolute topological index of the located processor */
                return ((ULONG)Index * BitsPerMask) + BitIndex;
            }
        }
    }

    /* Wrap around and scan the entire map from the top to find the highest globally permitted processor */
    for(Index = (LONG)AffinityMap->Size - 1; Index >= 0; Index--)
    {
        /* Load the complete processor mask for the current array boundary */
        Mask = AffinityMap->Bitmap[Index];

        /* Check if this segment contains any active processor bindings */
        if(Mask != 0)
        {
            /* Find the highest available processor within this array element */
            if(AR::CpuFunctions::ScanReverseBit(&BitIndex, Mask))
            {
                /* Return the absolute topological index of the located processor */
                return ((ULONG)Index * BitsPerMask) + BitIndex;
            }
        }
    }

    /* Fallback to the bootstrap processor */
    return 0;
}

/**
 * Initializes a caller-allocated affinity map structure.
 *
 * @param AffinityMap
 *        Supplies a pointer to the caller-allocated buffer to be initialized.
 *
 * @param BufferSize
 *        Supplies the size, in bytes, of the provided buffer.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::Affinity::InitializeAffinityMap(IN OUT PKAFFINITY_MAP AffinityMap,
                                    IN ULONG BufferSize)
{
    ULONG Cpus, RequiredBlocks, RequiredSize;

    /* Get the number of available CPUs */
    Cpus = KE::Processor::GetInstalledCpus();

    /* Query the required size in bytes and the architectural block count */
    KE::Affinity::CalculateAffinityMapSize(Cpus, &RequiredSize, &RequiredBlocks);

    /* Validate that the provided buffer is large enough to hold the structure */
    if(BufferSize < RequiredSize)
    {
        /* Buffer overrun prevention */
        return STATUS_BUFFER_TOO_SMALL;
    }

    /* Zero only the required portion of the memory to clear any garbage data */
    RTL::Memory::ZeroMemory(AffinityMap, RequiredSize);

    /* Initialize the internal metadata required by iteration routines */
    AffinityMap->Reserved = 0;
    AffinityMap->Size = RequiredBlocks;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Populates the affinity map with all available processors.
 *
 * @param AffinityMap
 *        Supplies a pointer to the affinity map to be fully populated.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Affinity::SetAllProcessorsAffinity(IN OUT PKAFFINITY_MAP AffinityMap)
{
    ULONG Cpus, Index;

    /* Get the number of available CPUs */
    Cpus = KE::Processor::GetInstalledCpus();

    /* Iterate through all allocated blocks in the map */
    for(Index = 0; Index < AffinityMap->Size; Index++)
    {
        /* Evaluate if the remaining logical processors fully saturate the current block */
        if(Cpus >= 64)
        {
            /* Set all 64 bits and decrement the remaining count */
            AffinityMap->Bitmap[Index] = ~((KAFFINITY)0);
            Cpus -= 64;
        }
        else if(Cpus > 0)
        {
            /* Generate a partial bitmask for the tail end of the processors */
            AffinityMap->Bitmap[Index] = (((KAFFINITY)1 << Cpus) - 1);
            Cpus = 0;
        }
        else
        {
            /* Ensure any remaining blocks are safely zeroed */
            AffinityMap->Bitmap[Index] = 0;
        }
    }
}

/**
 * Sets the affinity bit for a specified processor. This is a non-atomic operation.
 *
 * @param AffinityMap
 *        Supplies a pointer to the affinity map to be modified.
 *
 * @param CpuNumber
 *        Supplies the logical processor number to include in the affinity map.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Affinity::SetProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                   IN ULONG CpuNumber)
{
    /* Verify that the target processor falls within the allocated map boundaries */
    if((CpuNumber / 64) < AffinityMap->Size)
    {
        /* Set the target CPU bit in the affinity map */
        AffinityMap->Bitmap[CpuNumber / 64] |= ((KAFFINITY)1 << (CpuNumber % 64));
    }
}
