/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pmap.c
 * DESCRIPTION:     Low-level support for i686 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Checks if eXtended Physical Addressing (XPA) is enabled.
 *
 * @return This routine returns TRUE if PAE is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MmpGetExtendedPhysicalAddressingStatus(VOID)
{
    /* Check if PAE is enabled */
    return ((ArReadControlRegister(4) & CR4_PAE) != 0) ? TRUE : FALSE;
}

/**
 * Gets the address of the PDE (Page Directory Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PDE.
 *
 * @return This routine returns the address of the PDE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPDE
MmpGetPdeAddress(PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONG)(Address)) >> MmpPageMapInfo.PdiShift) << MmpPageMapInfo.PteShift);
    return (PMMPTE)(MmpPageMapInfo.PdeBase + Offset);
}

/**
 * Gets the address of the PPE (Page Directory Pointer Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PDE.
 *
 * @return This routine returns the address of the PPE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPPE
MmpGetPpeAddress(PVOID Address)
{
    /* Return zero */
    return 0;
}

/**
 * Gets the address of the PTE (Page Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PTE.
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
    Offset = ((((ULONG)(Address)) >> MM_PTI_SHIFT) << MmpPageMapInfo.PteShift);
    return (PMMPTE)(MM_PTE_BASE + Offset);
}
