/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/bitmap.c
 * DESCRIPTION:     Bit maps support related routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Clears all bits in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RtlClearAllBits(IN PRTL_BITMAP BitMap)
{
    /* Clear all bits */
    RtlSetMemory(BitMap->Buffer, 0, ((BitMap->Size + BITS_PER_LONG - 1) / BITS_PER_LONG) * sizeof(ULONG_PTR));
}

/**
 * Clears a single bit in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Bit
 *        Specifies the number of the bit to be cleared.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTAPI
VOID
RtlClearBit(IN PRTL_BITMAP BitMap,
            IN ULONG_PTR Bit)
{
    /* Check if bit is in range */
    if(Bit >= BitMap->Size)
    {
        /* Supplied bit exceeds bit map size */
        return;
    }

    /* Clear specified bit */
    BitMap->Buffer[Bit / BITS_PER_LONG] &= ~(1 << (Bit & (BITS_PER_LONG - 1)));
}

/**
 * Clears a specified set of bits within a bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param StartingIndex
 *        Supplies the starting index of the first bit to clear.
 *
 * @param Length
 *        Supplies the length (number of bits) to clear.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RtlClearBits(IN PRTL_BITMAP BitMap,
             IN ULONG_PTR StartingIndex,
             IN ULONG_PTR Length)
{
    ULONG_PTR BitOffset, Mask;
    PULONG_PTR Buffer;

    /* Make sure there is anything to clear */
    if(!Length)
    {
        /* No bits to clear */
        return;
    }

    /* Get pointer to first byte to clear and calculate bit offset */
    Buffer = &BitMap->Buffer[StartingIndex / BITS_PER_LONG];
    BitOffset = StartingIndex & (BITS_PER_LONG - 1);

    /* Check if bit offset is not zero */
    if(BitOffset)
    {
        /* Get mask and calculate new bit offset */
        Mask = MAXULONG_PTR << BitOffset;
        BitOffset = BITS_PER_LONG - BitOffset;

        /* Check if there are enough bits to clear */
        if(Length < BitOffset)
        {
            /* Recalculate bit offset and fixup the mask */
            BitOffset -= Length;
            Mask = Mask << BitOffset >> BitOffset;

            /* Clear bits and return */
            *Buffer &= ~Mask;
            return;
        }

        /* Clear bits, recalculate length and advance buffer pointer */
        *Buffer &= ~Mask;
        Length -= BitOffset;
        Buffer++;
    }

    /* Clear remaining bits */
    RtlSetMemory(Buffer, 0, Length >> 3);

    /* Look for any remaining bits to clear */
    Buffer += Length / BITS_PER_LONG;
    Length &= BITS_PER_LONG - 1;
    if(Length)
    {
        /* Clear what's left */
        *Buffer &= MAXULONG_PTR << Length;
    }
}

/**
 * Dumps the contents of the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RtlDumpBitMap(IN PRTL_BITMAP BitMap)
{
    ULONG_PTR Index;

    /* Dump bit map buffer information */
    DebugPrint(L"BitMap Buffer: 0x%zX (%lu bytes)\n", BitMap->Buffer, BitMap->Size);

    /* Dump bit map buffer content */
    for(Index = 0; Index < (BitMap->Size + BITS_PER_LONG - 1) / BITS_PER_LONG; Index++)
    {
        DebugPrint(L"  %8zu: %08lx\n", Index, BitMap->Buffer[Index]);
    }
}

/**
 * Initializes a bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map to initialize.
 *
 * @param Buffer
 *        Supplies a pointer to the buffer that will be used as a bit map.
 *
 * @param Size
 *        Supplies a size of the bit map.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RtlInitializeBitMap(IN PRTL_BITMAP BitMap,
                    IN PULONG_PTR Buffer,
                    IN ULONG Size)
{
    /* Initialize bit map */
    BitMap->Buffer = Buffer;
    BitMap->Size = Size;
}

/**
 * Sets all bits in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RtlSetAllBits(IN PRTL_BITMAP BitMap)
{
    /* Set all bits */
    RtlSetMemory(BitMap->Buffer, 0xFF, ((BitMap->Size + BITS_PER_LONG - 1) / BITS_PER_LONG) * sizeof(ULONG_PTR));
}

/**
 * Sets a single bit in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Bit
 *        Specifies the number of the bit to be set.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTAPI
VOID
RtlSetBit(IN PRTL_BITMAP BitMap,
          IN ULONG_PTR Bit)
{
    /* Check if bit is in range */
    if(Bit >= BitMap->Size)
    {
        /* Supplied bit exceeds bit map size */
        return;
    }

    /* Set specified bit */
    BitMap->Buffer[Bit / BITS_PER_LONG] |= 1 << (Bit & (BITS_PER_LONG - 1));
}

/**
 * Sets a specified set of bits within a bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param StartingIndex
 *        Supplies the starting index of the first bit to set.
 *
 * @param Length
 *        Supplies the length (number of bits) to set.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RtlSetBits(IN PRTL_BITMAP BitMap,
           IN ULONG_PTR StartingIndex,
           IN ULONG_PTR Length)
{
    ULONG_PTR BitOffset, Mask;
    PULONG_PTR Buffer;

    /* Make sure there is anything to set */
    if(!Length)
    {
        /* No bits to set */
        return;
    }

    /* Get pointer to first byte to clear and calculate bit offset */
    Buffer = &BitMap->Buffer[StartingIndex / BITS_PER_LONG];
    BitOffset = StartingIndex & (BITS_PER_LONG - 1);

    /* Check if bit offset is not zero */
    if(BitOffset)
    {
        /* Get mask and calculate new bit offset */
        Mask = MAXULONG_PTR << BitOffset;
        BitOffset = BITS_PER_LONG - BitOffset;

        /* Check if there are enough bits to set */
        if(Length < BitOffset)
        {
            /* Recalculate bit offset and fixup the mask */
            BitOffset -= Length;
            Mask = Mask << BitOffset >> BitOffset;

            /* Set bits and return */
            *Buffer |= Mask;
            return;
        }

        /* Set bits, recalculate length and advance buffer pointer */
        *Buffer |= Mask;
        Buffer++;
        Length -= BitOffset;
    }

    /* Set remaining bits */
    RtlSetMemory(Buffer, 0xFF, Length >> 3);

    /* Look for any remaining bits to set */
    Buffer += Length / BITS_PER_LONG;
    Length &= BITS_PER_LONG - 1;
    if(Length)
    {
        /* Set what's left */
        *Buffer |= ~(MAXULONG_PTR << Length);
    }
}

/**
 * Tests a state of a single bit in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Bit
 *        Specifies the number of the bit to be tested.
 *
 * @return This routine returns TRUE when bit is set, or FALSE otherwise.
 *
 * @since NT 5.1
 */
XTAPI
BOOLEAN
RtlTestBit(IN PRTL_BITMAP BitMap,
           IN ULONG_PTR Bit)
{
    /* Check if bit is in range */
    if(Bit >= BitMap->Size)
    {
        /* Supplied bit exceeds bit map size, return FALSE */
        return FALSE;
    }

    /* Test specified bit and return result */
    return ((BitMap->Buffer[Bit / BITS_PER_LONG] >> (Bit & (BITS_PER_LONG - 1))) & 1) ? TRUE : FALSE;
}
