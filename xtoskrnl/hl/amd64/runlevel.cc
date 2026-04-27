/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/amd64/runlevel.cc
 * DESCRIPTION:     Run Level management support for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Gets the current run level from APIC for the current processor.
 *
 * @return This routine returns the current run level.
 *
 * @since XT 1.0
 */
XTFASTCALL
KRUNLEVEL
HL::RunLevel::GetRunLevel(VOID)
{
    /* Read current run level */
    return (KRUNLEVEL)AR::CpuFunc::ReadControlRegister(8);
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
HL::RunLevel::SetRunLevel(IN KRUNLEVEL RunLevel)
{
    /* Set new run level */
    AR::CpuFunc::WriteControlRegister(8, RunLevel);
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
HL::RunLevel::TransformApicTprToRunLevel(IN UCHAR Tpr)
{
    /* Transform APIC TPR to run level */
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
HL::RunLevel::TransformRunLevelToApicTpr(IN KRUNLEVEL RunLevel)
{
    /* Transform run level to APIC TPR */
    return (RunLevel << 4);
}

/**
 * Transforms a given execution run level into a corresponding hardware interrupt vector
 * suitable for software interrupts.
 *
 * @param RunLevel
 *        Supplies the run level to be translated into a software interrupt vector.
 *
 * @return This routine returns the 8-bit APIC vector corresponding to the requested software interrupt level.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
HL::RunLevel::TransformRunLevelToSoftwareVector(IN KRUNLEVEL RunLevel)
{
    /* Transform run level to APIC interrupt vector */
    return TransformRunLevelToApicTpr(RunLevel) | 0xF;
}
