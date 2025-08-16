/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/pmap.c
 * DESCRIPTION:     Low-level support for AMD64 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Checks if eXtended Physical Addressing (XPA) is enabled.
 *
 * @return This routine returns TRUE if LA57 is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MmpGetExtendedPhysicalAddressingStatus(VOID)
{
    /* Check if LA57 is enabled */
    return ((ArReadControlRegister(4) & CR4_LA57) != 0) ? TRUE : FALSE;
}

/**
 * Gets the address of the P5E (Page Map Level 5 Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding P5E.
 *
 * @return This routine returns the address of the P5E, or NULL if LA57 is not enabled.
 *
 * @since XT 1.0
 */
XTAPI
PMMP5E
MmpGetP5eAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = (((ULONGLONG)Address >> MM_P5I_SHIFT) << MM_PTE_SHIFT);
    return (PMMP5E)((MmpPageMapInfo.P5eBase + Offset) * MmpPageMapInfo.Xpa);
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
    ULONGLONG Offset;

    Offset = (((ULONGLONG)Address >> MM_PDI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPDE)(MmpPageMapInfo.PdeBase + Offset);
}

/**
 * Gets the address of the PPE (Page Directory Pointer Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PPE.
 *
 * @return This routine returns the address of the PPE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPPE
MmpGetPpeAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = (((ULONGLONG)Address >> MM_PPI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPPE)(MmpPageMapInfo.PpeBase + Offset);
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
    ULONGLONG Offset;

    Offset = (((ULONGLONG)Address >> MM_PTI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MmpPageMapInfo.PteBase + Offset);
}

/**
 * Gets the address of the PXE (Extended Page Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PXE.
 *
 * @return This routine returns the address of the PXE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPXE
MmpGetPxeAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = (((ULONGLONG)Address >> MM_PXI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPXE)(MmpPageMapInfo.PxeBase + Offset);
}
