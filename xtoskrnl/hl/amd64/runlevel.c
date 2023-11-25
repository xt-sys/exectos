/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/amd64/runlevel.c
 * DESCRIPTION:     Run Level management support for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Gets the current run level from APIC for the current processor.
 *
 * @return This routine returns the current run level.
 *
 * @since XT 1.0
 */
XTFASTCALL
KRUNLEVEL
HlGetRunLevel(VOID)
{
    return (KRUNLEVEL)ArReadControlRegister(8);
}

/**
 * Sets new run level for the current processor.
 *
 * @param RunLevel
 *        Supplies the new run level to store into APIC.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
HlSetRunLevel(IN KRUNLEVEL RunLevel)
{
    ArWriteControlRegister(8, RunLevel);
}

/**
 * Maps APIC interrupt vector to XT run level.
 *
 * @param Tpr
 *        Supplies the interrupt vector rad from APIC Task Priority Register.
 *
 * @return This routine returns the XT run level corresponding to the specified APIC interrupt vector.
 *
 * @since XT 1.0
 */
XTFASTCALL
KRUNLEVEL
HlpTransformApicTprToRunLevel(IN UCHAR Tpr)
{
    return (KRUNLEVEL)(Tpr >> 4);
}

/**
 * Maps XT run level to interrupt vector suitable for the APIC Task Priority Register.
 *
 * @param RunLevel
 *        Supplies the XT run level.
 *
 * @return This routine returns the APIC interrupt vector corresponding to the specified XT run level.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
HlpTransformRunLevelToApicTpr(IN KRUNLEVEL RunLevel)
{
    return (RunLevel << 4);
}
