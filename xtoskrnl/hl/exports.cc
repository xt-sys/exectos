/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/exports.cc
 * DESCRIPTION:     C-compatible API wrappers for exported kernel functions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Reads an 8-bit data from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns a value at the specified register.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
UCHAR
HlReadRegister8(IN PVOID Register)
{
    return HL::IoRegister::ReadRegister8(Register);
}

/**
 * Reads a 16-bit data from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns a value at the specified register.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
USHORT
HlReadRegister16(IN PVOID Register)
{
    return HL::IoRegister::ReadRegister16(Register);
}

/**
 * Reads a 32-bit data from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns a value at the specified register.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
ULONG
HlReadRegister32(IN PVOID Register)
{
    return HL::IoRegister::ReadRegister32(Register);
}

/**
 * Writes an 8-bit value into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
HlWriteRegister8(IN PVOID Register,
                 IN UCHAR Value)
{
    HL::IoRegister::WriteRegister8(Register, Value);
}

/**
 * Writes a 16-bit value into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
HlWriteRegister16(IN PVOID Register,
                  IN USHORT Value)
{
    HL::IoRegister::WriteRegister16(Register, Value);
}

/**
 * Writes a 32-bit value into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
HlWriteRegister32(IN PVOID Register,
                  IN ULONG Value)
{
    HL::IoRegister::WriteRegister32(Register, Value);
}
