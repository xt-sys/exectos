/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/init.c
 * DESCRIPTION:     Architecture specific Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Detects if eXtended Physical Addressing (XPA) is enabled and initializes page map support.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmInitializePageMapSupport(VOID)
{
    /* Check if XPA is enabled */
    if(MmpGetExtendedPhysicalAddressingStatus())
    {
        /* XPA enabled, use modern PAE paging (PML3) */
        MmpPageMapRoutines = &MmpPml3Routines;

        /* Set PML3 page map information */
        MmpPageMapInfo.Xpa = TRUE;

        /* Set PML3 base addresses */
        MmpPageMapInfo.PteBase = MM_PTE_BASE;
        MmpPageMapInfo.PdeBase = MM_PDE_BASE;

        /* Set PML3 shift values */
        MmpPageMapInfo.PdiShift = MM_PDI_SHIFT;
        MmpPageMapInfo.PteShift = MM_PTE_SHIFT;
    }
    else
    {
        /* XPA disabled, use legacy i386 paging (PML2) */
        MmpPageMapRoutines = &MmpPml2Routines;

        /* Set PML2 page map information */
        MmpPageMapInfo.Xpa = FALSE;

        /* Set PML2 base addresses */
        MmpPageMapInfo.PteBase = MM_PTE_BASE;
        MmpPageMapInfo.PdeBase = MM_PDE_LEGACY_BASE;

        /* Set PML2 shift values */
        MmpPageMapInfo.PdiShift = MM_PDI_LEGACY_SHIFT;
        MmpPageMapInfo.PteShift = MM_PTE_LEGACY_SHIFT;
    }
}

/**
 * Performs architecture specific initialization of the XTOS Memory Manager.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpInitializeArchitecture(VOID)
{
    UNIMPLEMENTED;
}
