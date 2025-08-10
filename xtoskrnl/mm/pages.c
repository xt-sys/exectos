/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/pages.c
 * DESCRIPTION:     Low level page management support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Flushes current Translation Lookaside Buffer (TLB)
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmFlushTlb(VOID)
{
    CPUID_REGISTERS CpuRegisters;
    BOOLEAN Interrupts;
    ULONG_PTR Cr4;

    /* Save interrupts state and disable them */
    Interrupts = ArInterruptsEnabled();
    ArClearInterruptFlag();

    /* Get CPU features */
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;
    ArCpuId(&CpuRegisters);

    /* Check if Paging Global Extensions (PGE) is supported */
    if(CpuRegisters.Edx & CPUID_FEATURES_EDX_PGE)
    {
        /* Read CR4 */
        Cr4 = ArReadControlRegister(4);

        /* Disable PGE */
        ArWriteControlRegister(4, Cr4 & ~CR4_PGE);

        /* Flush the TLB */
        ArFlushTlb();

        /* Restore CR4 */
        ArWriteControlRegister(4, Cr4);
    }
    else
    {
        /* Simply flush the TLB */
        ArFlushTlb();
    }

    /* Check if interrupts should be enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        ArSetInterruptFlag();
    }
}
