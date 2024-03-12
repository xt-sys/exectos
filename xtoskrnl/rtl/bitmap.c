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
