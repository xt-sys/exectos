/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/i686/ioport.cc
 * DESCRIPTION:     I/O port access routines for i686 platform
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


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
HL::IoPort::ReadPort8(IN USHORT Port)
{
    UCHAR Value;
    __asm__ volatile("inb %1, %0"
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
HL::IoPort::ReadPort16(IN USHORT Port)
{
    USHORT Value;
    __asm__ volatile("inw %1, %0"
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
HL::IoPort::ReadPort32(IN USHORT Port)
{
    ULONG Value;
    __asm__ volatile("inl %1, %0"
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
HL::IoPort::WritePort8(IN USHORT Port,
                       IN UCHAR Value)
{
    __asm__ volatile("outb %0, %1"
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
HL::IoPort::WritePort16(IN USHORT Port,
                        IN USHORT Value)
{
    __asm__ volatile("outw %0, %1"
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
HL::IoPort::WritePort32(IN USHORT Port,
                        IN ULONG Value)
{
    __asm__ volatile("outl %0, %1"
                     :
                     : "a" (Value),
                       "Nd" (Port));
}
