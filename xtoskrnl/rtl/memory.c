/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/memory.c
 * DESCRIPTION:     Memory related routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtkmapi.h>


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
 * @return Returns the destination pointer.
 *
 * @since NT 3.5
 */
XTAPI
PVOID
RtlCopyMemory(IN PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length)
{
    PCHAR DestinationBytes = (PCHAR)Destination;
    PCHAR SourceBytes = (PCHAR)Source;

    /* Forward buffer copy */
    while(Length--)
    {
        *DestinationBytes++ = *SourceBytes++;
    }

    /* Return pointer to destination buffer */
    return Destination;
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
 * @return A value indicating the relationship between the content of the memory blocks.
 *         It returns zero (0) if both memory blocks are equal or a value different than
 *         zero representing which is greater if they do not match.
 *
 * @since XT 1.0
 */
XTAPI
SIZE_T
RtlSameMemory(IN PCVOID LeftBuffer,
              IN PCVOID RightBuffer,
              IN SIZE_T Length)
{
    CONST UCHAR *Left = (PUCHAR)LeftBuffer;
    CONST UCHAR *Right = (PUCHAR)RightBuffer;

    /* Check if there is anything to compare */
    if(Length)
    {
        /* Iterate through whole buffer length */
        while(Length--)
        {
            /* Compare bytes from both bufers */
            if(*Left != *Right)
            {
                /* Buffers differ */
                return (*Left - *Right);
            }

            /* Advance to next byte */
            Left++;
            Right++;
        }
    }

    /* Buffers equal */
    return 0;
}
