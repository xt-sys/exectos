/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            drivers/ntosdrv/rtl.cc
 * DESCRIPTION:     NTOS compatibility driver runtime library
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtkmapi.h>


/**
 * This routine fills a section of memory with a specified byte.
 *
 * @param Destination
 *        Supplies a pointer to the buffer to fill.
 *
 * @param Length
 *        Specifies a number of bytes to store in memory.
 *
 * @param Byte
 *        Supplies a pattern to fill memory.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlFillMemory(OUT PVOID Destination,
              IN SIZE_T Length,
              IN UCHAR Byte)
{
    /* Fill the buffer with specified byte */
    RtlSetMemory(Destination, Byte, Length);
}
