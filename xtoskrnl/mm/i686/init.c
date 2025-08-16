/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/init.c
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
 * Gets the address of the PDE (Page Directory Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PDE for.
 *
 * @return This routine returns the address of the PDE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPdeAddress(PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONG)(Address)) >> MM_PDI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MM_PDE_BASE + Offset);
}

/**
 * Gets the address of the PTE (Page Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PTE for.
 *
 * @return This routine returns the address of the PTE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPteAddress(PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONG)(Address)) >> MM_PTI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MM_PTE_BASE + Offset);
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
