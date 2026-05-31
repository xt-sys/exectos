/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/affinity.cc
 * DESCRIPTION:     XT kernel processor affinity management support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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
