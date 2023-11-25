/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/i686/runlevel.c
 * DESCRIPTION:     Run Level management support for i686 architecture
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
HlGetRunLevel()
{
    return HlpTransformApicTprToRunLevel(HlReadApicRegister(APIC_TPR));
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
    HlWriteApicRegister(APIC_TPR, HlpTransformRunLevelToApicTpr(RunLevel));
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
    KRUNLEVEL TransformationTable[16] =
    {
        PASSIVE_LEVEL,
        PASSIVE_LEVEL,
        PASSIVE_LEVEL,
        APC_LEVEL,
        DISPATCH_LEVEL,
        DEVICE1_LEVEL,
        DEVICE2_LEVEL,
        DEVICE3_LEVEL,
        DEVICE4_LEVEL,
        DEVICE5_LEVEL,
        DEVICE6_LEVEL,
        DEVICE7_LEVEL,
        PROFILE_LEVEL,
        CLOCK_LEVEL,
        IPI_LEVEL,
        HIGH_LEVEL
    };

    /* Return the run level corresponding to the TPR from the transformation table. */
    return TransformationTable[Tpr / 16];
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
    UCHAR TransformationTable[32] =
    {
        APIC_VECTOR_ZERO,
        APIC_VECTOR_APC,
        APIC_VECTOR_DPC,
        APIC_VECTOR_DPC,
        APIC_VECTOR_DEVICE1,
        APIC_VECTOR_DEVICE2,
        APIC_VECTOR_DEVICE3,
        APIC_VECTOR_DEVICE4,
        APIC_VECTOR_DEVICE5,
        APIC_VECTOR_DEVICE6,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_DEVICE7,
        APIC_VECTOR_GENERIC,
        APIC_VECTOR_CLOCK,
        APIC_VECTOR_IPI,
        APIC_VECTOR_POWERFAIL,
        APIC_VECTOR_NMI
    };

    /* Return the TPR corresponding to the run level from the transformation table. */
    return TransformationTable[RunLevel];
}
