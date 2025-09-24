/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/endian.cc
 * DESCRIPTION:     Endian conversion routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * This routine converts endianness on 16bit value.
 *
 * @param Source
 *        Supplies a value to swap bytes.
 *
 * @return Swapped 16bit value.
 *
 * @since XT 1.0
 */
XTFASTCALL
USHORT
RTL::Endianness::SwapByte16(IN USHORT Source)
{
    return (USHORT)(((Source >> 8) & 0x00FF) |
                    ((Source << 8) & 0xFF00));
}

/**
 * This routine converts endianness on 32bit value.
 *
 * @param Source
 *        Supplies a value to swap bytes.
 *
 * @return Swapped 32bit value.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONG
RTL::Endianness::SwapByte32(IN ULONG Source)
{
    return (ULONG)(((Source >> 24) & 0x000000FF) |
                   ((Source >> 8)  & 0x0000FF00) |
                   ((Source << 8)  & 0x00FF0000) |
                   ((Source << 24) & 0xFF000000));
}

/**
 * This routine converts endianness on 64bit value.
 *
 * @param Source
 *        Supplies a value to swap bytes.
 *
 * @return Swapped 64bit value.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONGLONG
RTL::Endianness::SwapByte64(IN ULONGLONG Source)
{
    return (ULONGLONG)(((Source >> 56) & 0x00000000000000FF) |
                       ((Source >> 40) & 0x000000000000FF00) |
                       ((Source >> 24) & 0x0000000000FF0000) |
                       ((Source >> 8)  & 0x00000000FF000000) |
                       ((Source << 8)  & 0x000000FF00000000) |
                       ((Source << 24) & 0x0000FF0000000000) |
                       ((Source << 40) & 0x00FF000000000000) |
                       ((Source << 56) & 0xFF00000000000000));
}
