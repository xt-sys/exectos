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
RtlReadRegisterByte(IN VOLATILE PUCHAR Register)
{
    return *Register;
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
RtlReadRegisterLong(IN VOLATILE PULONG Register)
{
    return *Register;
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
RtlReadRegisterShort(IN VOLATILE PUSHORT Register)
{
    return *Register;
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
RtlWriteRegisterByte(IN PUSHORT Register,
                     IN UCHAR Value)
{
    HlIoPortOutByte((USHORT)(ULONG_PTR)Register, Value);
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
RtlWriteRegisterLong(IN PUSHORT Register,
                     IN ULONG Value)
{
    HlIoPortOutLong((USHORT)(ULONG_PTR)Register, Value);
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
RtlWriteRegisterShort(IN PUSHORT Register,
                      IN USHORT Value)
{
    HlIoPortOutShort((USHORT)(ULONG_PTR)Register, Value);
}
