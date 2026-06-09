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
    /* Atomically set the target CPU bit */
    RTL::Atomic::Or64((PLONG_PTR)&AffinityMap->Bitmap[CpuNumber / 64], ((KAFFINITY)1 << (CpuNumber % 64)));
}

/**
 * Computes the memory size required to allocate an affinity map.
 *
 * @param CpuCount
 *        Supplies the total number of logical processors the map needs to support.
 *
 * @return This routine returns the required allocation bytes.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONG
KE::Affinity::CalculateAffinityMapSize(IN ULONG CpuCount)
{
    ULONG AffinitySize, MapSize;

    /* Calculate the required number of blocks and the total structure size */
    AffinitySize = (CpuCount + 63) / 64;
    MapSize = sizeof(KAFFINITY_MAP) + (AffinitySize * sizeof(KAFFINITY));

    /* Align the calculated size to an 8-byte boundary */
    return (MapSize + 7) & ~7;
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
    /* Isolate and test the specific bit corresponding to the target CPU */
    return (AffinityMap->Bitmap[CpuNumber / 64] & ((KAFFINITY)1 << (CpuNumber % 64))) != 0;
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
    /* Clear the target CPU bit in the affinity map */
    AffinityMap->Bitmap[CpuNumber / 64] &= ~((KAFFINITY)1 << (CpuNumber % 64));
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
VOID
KE::Affinity::CopyAffinity(OUT PKAFFINITY_MAP Destination,
                           IN PKAFFINITY_MAP Source)
{
    USHORT Index;

    /* Copy map metadata */
    Destination->Count = Source->Count;
    Destination->Size = Source->Size;
    Destination->Reserved = Source->Reserved;

    /* Copy the active affinity bitmasks */
    for(Index = 0; Index < Source->Size; Index++)
    {
        /* Replicate the hardware topology bindings across all active array elements */
        Destination->Bitmap[Index] = Source->Bitmap[Index];
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
 * @return Returns the absolute topological index of the selected processor.
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
 * @return Returns the absolute topological index of the selected processor.
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
    /* Set the target CPU bit in the affinity map */
    AffinityMap->Bitmap[CpuNumber / 64] |= ((KAFFINITY)1 << (CpuNumber % 64));
}
