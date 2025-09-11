/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/ioreg.cc
 * DESCRIPTION:     Basic I/O registers access functionality
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
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
XTAPI
UCHAR
HL::IoRegister::ReadRegister8(IN PVOID Register)
{
    return *((VOLATILE PUCHAR)Register);
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
XTAPI
USHORT
HL::IoRegister::ReadRegister16(IN PVOID Register)
{
    return *((VOLATILE PUSHORT)Register);
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
XTAPI
ULONG
HL::IoRegister::ReadRegister32(IN PVOID Register)
{
    return *((VOLATILE PULONG)Register);
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
XTAPI
VOID
HL::IoRegister::WriteRegister8(IN PVOID Register,
                               IN UCHAR Value)
{
    *((VOLATILE PUCHAR)Register) = Value;
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
XTAPI
VOID
HL::IoRegister::WriteRegister16(IN PVOID Register,
                                IN USHORT Value)
{
    *((VOLATILE PUSHORT)Register) = Value;
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
XTAPI
VOID
HL::IoRegister::WriteRegister32(IN PVOID Register,
                                IN ULONG Value)
{
    *((VOLATILE PULONG)Register) = Value;
}
