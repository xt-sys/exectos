/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/ioreg.c
 * DESCRIPTION:     I/O registers related routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Reads a byte from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns UCHAR byte read from the register.
 *
 * @since XT 1.0
 */
XTAPI
UCHAR
RtlReadRegisterByte(IN VOLATILE PVOID Register)
{
    return *((VOLATILE PUCHAR)Register);
}

/**
 * Reads a byte from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns ULONG byte read from the register.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
RtlReadRegisterLong(IN VOLATILE PVOID Register)
{
    return *((VOLATILE PULONG)Register);
}

/**
 * Reads a byte from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns USHORT byte read from the register.
 *
 * @since XT 1.0
 */
XTAPI
USHORT
RtlReadRegisterShort(IN VOLATILE PVOID Register)
{
    return *((VOLATILE PUSHORT)Register);
}

/**
 * Writes a byte into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new UCHAR value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RtlWriteRegisterByte(IN VOLATILE PVOID Register,
                     IN UCHAR Value)
{
    *((VOLATILE PUCHAR)Register) = Value;
}

/**
 * Writes a byte into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new ULONG value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RtlWriteRegisterLong(IN VOLATILE PVOID Register,
                     IN ULONG Value)
{
    *((VOLATILE PULONG)Register) = Value;
}

/**
 * Writes a byte into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new USHORT value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RtlWriteRegisterShort(IN VOLATILE PVOID Register,
                      IN USHORT Value)
{
    *((VOLATILE PUSHORT)Register) = Value;
}
