/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/memory.cc
 * DESCRIPTION:     Memory related routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * This routine compares the first bytes of the specified memory buffers.
 *
 * @param LeftBuffer
 *        Supplies a pointer to the first block of memory to compare.
 *
 * @param RightBuffer
 *        Supplies a pointer to the second block of memory to compare.
 *
 * @param Length
 *        Specifies a number of bytes to compare.
 *
 * @return This routine returns a number of bytes that are equal in both memory blocks.
 *
 * @since NT 3.5
 */
XTAPI
SIZE_T
RTL::Memory::CompareMemory(IN PCVOID LeftBuffer,
                           IN PCVOID RightBuffer,
                           IN SIZE_T Length)
{
    SIZE_T Bytes = 0;

    /* Check if there is anything to compare */
    if(Length)
    {
        /* Iterate through whole buffer length */
        while(Bytes < Length)
        {
            /* Compare bytes from both bufers */
            if(((CONST PBYTE)LeftBuffer)[Bytes] != ((CONST PBYTE)RightBuffer)[Bytes])
            {
                /* Buffers are different */
                break;
            }

            /* Increase number of common bytes */
            Bytes++;
        }
    }

    /* Return number of equal characters */
    return Bytes;
}

/**
 * This routine copies a block of memory.
 *
 * @param Destination
 *        Supplies a pointer to the buffer where data will be copied to.
 *
 * @param Source
 *        Supplies a pointer to the source buffer that will be copied.
 *
 * @param Length
 *        Specifies the number of bytes to copy.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RTL::Memory::CopyMemory(OUT PVOID Destination,
                        IN PCVOID Source,
                        IN SIZE_T Length)
{
    PCHAR DestinationBytes = (PCHAR)Destination;
    PCCHAR SourceBytes = (PCHAR)Source;

    /* Forward buffer copy */
    while(Length--)
    {
        *DestinationBytes++ = *SourceBytes++;
    }
}

/**
 * This routine copies a block of memory either forward of backward, depeding
 * if source and destination buffers overlap or not.
 *
 * @param Destination
 *        Supplies a pointer to the buffer where data will be copied to.
 *
 * @param Source
 *        Supplies a pointer to the source buffer that will be copied.
 *
 * @param Length
 *        Specifies the number of bytes to copy.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RTL::Memory::MoveMemory(OUT PVOID Destination,
                        IN PCVOID Source,
                        IN SIZE_T Length)
{
    PCHAR DestinationBytes = (PCHAR)Destination;
    PCHAR SourceBytes = (PCHAR)Source;

    /* Make sure there is anything to copy */
    if((!SourceBytes) && (!DestinationBytes))
    {
        return;
    }

    /* Check if source and destination overlaps */
    if((DestinationBytes > SourceBytes) && (SourceBytes + Length > DestinationBytes))
    {
        /* Backward buffer copy */
        while(Length)
        {
            DestinationBytes[Length - 1] = SourceBytes[Length - 1];
            Length--;
        }
    }
    else
    {
        /* Forward buffer copy */
        RtlCopyMemory(Destination, Source, Length);
    }
}

/**
 * This routine compares the first bytes of the specified memory buffers.
 *
 * @param LeftBuffer
 *        Supplies a pointer to the first block of memory to compare.
 *
 * @param RightBuffer
 *        Supplies a pointer to the second block of memory to compare.
 *
 * @param Length
 *        Specifies a number of bytes to compare.
 *
 * @return Returns TRUE if both buffers are equal up to the specified length, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
RTL::Memory::SameMemory(IN PCVOID LeftBuffer,
                        IN PCVOID RightBuffer,
                        IN SIZE_T Length)
{
    return (CompareMemory(LeftBuffer, RightBuffer, Length) == Length) ? TRUE : FALSE;
}

/**
 * This routine fills a section of memory with a specified byte.
 *
 * @param Destination
 *        Supplies a pointer to the buffer to fill.
 *
 * @param Byte
 *        Supplies a pattern to fill memory.
 *
 * @param Length
 *        Specifies a number of bytes to store in memory.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Memory::SetMemory(OUT PVOID Destination,
                       IN UCHAR Byte,
                       IN SIZE_T Length)
{
    PCHAR DestinationBytes = (PCHAR)Destination;

    /* Fill the buffer with specified byte */
    while(Length--)
    {
        *DestinationBytes++ = Byte;
    }
}

/**
 * This routine fills a section of memory with zeroes.
 *
 * @param Destination
 *        Supplies a pointer to the buffer to fill.
 *
 * @param Length
 *        Specifies a number of bytes to be filled with zeroes.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
RTL::Memory::ZeroMemory(OUT PVOID Destination,
                        IN SIZE_T Length)
{
    /* Fill the buffer with zeroes */
    SetMemory(Destination, 0, Length);
}
