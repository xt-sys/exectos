/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/pic.c
 * DESCRIPTION:     Programmable Interrupt Controller (PIC) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Reads from the local APIC register.
 *
 * @param Register
 *        Supplies the APIC register to read from.
 *
 * @return This routine returns the value read from the APIC register.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONG
HlReadApicRegister(IN APIC_REGISTER Register)
{
    return RtlReadRegisterLong((PULONG)APIC_BASE + (Register << 4));
}

/**
 * Writes to the local APIC register.
 *
 * @param Register
 *        Supplies the APIC register to write to.
 *
 * @param Value
 *        Supplies the value to write to the APIC register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
HlWriteApicRegister(IN APIC_REGISTER Register,
                    IN ULONG Value)
{
    RtlWriteRegisterLong((PULONG)APIC_BASE + (Register << 4), Value);
}
