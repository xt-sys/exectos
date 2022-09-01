/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/amd64/cpufunc.c
 * DESCRIPTION:     Routines to provide access to special AMD64 CPU instructions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtkmapi.h"


/**
 * Reads the data from the specified I/O port.
 *
 * @param Port
 *        Specifies the port number in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTAPI
UCHAR
HlIoPortInByte(IN USHORT Port)
{
    UCHAR Value;
    asm volatile("inb %1, %0"
                 : "=a"(Value)
                 : "Nd"(Port));
    return Value;
}

/**
 * Writes the data to the specified I/O port.
 *
 * @param Port
 *        Specifies the port number in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Value)
{
    asm volatile("outb %0, %1"
                 :
                 :
                 "a"(Value),
                 "Nd"(Port));
}
