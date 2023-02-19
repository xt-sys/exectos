/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/amd64/ioport.c
 * DESCRIPTION:     I/O port access routines for AMD64 platform
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Reads the 8-bit data from the specified I/O port.
 *
 * @param Port
 *        Specifies the address to read from, in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTCDECL
UCHAR
HlIoPortInByte(IN USHORT Port)
{
    UCHAR Value;
    asm volatile("inb %1, %0"
                 : "=a" (Value)
                 : "Nd" (Port));
    return Value;
}

/**
 * Reads the 16-bit data from the specified I/O port.
 *
 * @param Port
 *        Specifies the address to read from, in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTCDECL
USHORT
HlIoPortInShort(IN USHORT Port)
{
    USHORT Value;
    asm volatile("inw %1, %0"
                 : "=a" (Value)
                 : "Nd" (Port));
    return Value;
}

/**
 * Reads the 32-bit data from the specified I/O port.
 *
 * @param Port
 *        Specifies the address to read from, in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTCDECL
ULONG
HlIoPortInLong(IN USHORT Port)
{
    ULONG Value;
    asm volatile("inl %1, %0"
                 : "=a" (Value)
                 : "Nd" (Port));
    return Value;
}

/**
 * Writes the 8-bit data to the specified I/O port.
 *
 * @param Port
 *        Specifies the address to write to, in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Value)
{
    asm volatile("outb %0, %1"
                 :
                 : "a" (Value),
                   "Nd" (Port));
}

/**
 * Writes the 16-bit data to the specified I/O port.
 *
 * @param Port
 *        Specifies the address to write to, in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HlIoPortOutShort(IN USHORT Port,
                 IN USHORT Value)
{
    asm volatile("outw %0, %1"
                 :
                 : "a" (Value),
                   "Nd" (Port));
}

/**
 * Writes the 32-bit data to the specified I/O port.
 *
 * @param Port
 *        Specifies the address to write to, in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HlIoPortOutLong(IN USHORT Port,
                IN ULONG Value)
{
    asm volatile("outl %0, %1"
                 :
                 : "a" (Value),
                   "Nd" (Port));
}
