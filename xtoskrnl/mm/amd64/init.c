/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/init.c
 * DESCRIPTION:     Architecture specific Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
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
        /* Set PML5 page map information */
        MmpPageMapInfo.Xpa = TRUE;

        /* Set PML5 base addresses */
        MmpPageMapInfo.PteBase = MM_PTE_LA57_BASE;
        MmpPageMapInfo.PdeBase = MM_PDE_LA57_BASE;
        MmpPageMapInfo.PpeBase = MM_PPE_LA57_BASE;
        MmpPageMapInfo.PxeBase = MM_PXE_LA57_BASE;
        MmpPageMapInfo.P5eBase = MM_P5E_LA57_BASE;
    }
    else
    {
        /* Set PML4 page map information */
        MmpPageMapInfo.Xpa = FALSE;

        /* Set PML4 base addresses */
        MmpPageMapInfo.PteBase = MM_PTE_BASE;
        MmpPageMapInfo.PdeBase = MM_PDE_BASE;
        MmpPageMapInfo.PpeBase = MM_PPE_BASE;
        MmpPageMapInfo.PxeBase = MM_PXE_BASE;
        MmpPageMapInfo.P5eBase = 0x0;
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
