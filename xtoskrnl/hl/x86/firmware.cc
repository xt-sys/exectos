/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/firmware.cc
 * DESCRIPTION:     UEFI/BIOS Firmware support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Reads a byte from the specified CMOS register.
 *
 * @param Register
 *        Supplies the CMOS register index to read from.
 *
 * @return This routine returns the data read from the register.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
HL::Firmware::ReadCmosRegister(IN UCHAR Register)
{
    /* Select the register (Setting the highest bit disables NMI) */
    HL::IoPort::WritePort8(CMOS_ADDRESS_PORT, Register | 0x80);

    /* Read value from the data port */
    return HL::IoPort::ReadPort8(CMOS_DATA_PORT);
}

/**
 * Writes a byte to the specified CMOS register.
 *
 * @param Register
 *        Supplies the CMOS register index to write to.
 *
 * @param Value
 *        Supplies the value to write to the register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
HL::Firmware::WriteCmosRegister(IN UCHAR Register,
                                IN UCHAR Value)
{
    /* Select the register (Setting the highest bit disables NMI) */
    HL::IoPort::WritePort8(CMOS_ADDRESS_PORT, Register | 0x80);

    /* Write the provided value to the data port */
    HL::IoPort::WritePort8(CMOS_DATA_PORT, Value);
}
