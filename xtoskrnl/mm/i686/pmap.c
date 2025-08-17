/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pmap.c
 * DESCRIPTION:     Low-level support for i686 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Clears the contents of a page table entry (PTE).
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to be cleared.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpClearPte(PHARDWARE_PTE PtePointer)
{
    PtePointer->Long = 0;
}

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

/**
 * Checks whether the given PML2 page table entry (PTE) is valid.
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to check.
 *
 * @return Returns TRUE if the entry is valid, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MmpPml2PteValid(PHARDWARE_PTE PtePointer)
{
    return (BOOLEAN)PtePointer->Pml2.Valid;
}

/**
 * Sets a PML2 page table entry (PTE) with the specified physical page and access flags.
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to set.
 *
 * @param PageFrameNumber
 *        Physical frame number to map.
 *
 * @param Writable
 *        Indicates whether the page should be writable.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpSetPml2Pte(PHARDWARE_PTE PtePointer,
              PFN_NUMBER PageFrameNumber,
              BOOLEAN Writable)
{
    PtePointer->Pml2.PageFrameNumber = PageFrameNumber;
    PtePointer->Pml2.Valid = 1;
    PtePointer->Pml2.Writable = Writable;
}

/**
 * Sets caching attributes for a PML2 page table entry (PTE).
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to modify.
 *
 * @param CacheDisable
 *        Indicates whether caching should be disabled for this page.
 *
 * @param WriteThrough
 *        Indicates whether write-through caching should be enabled.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpSetPml2PteCaching(PHARDWARE_PTE PtePointer,
                     BOOLEAN CacheDisable,
                     BOOLEAN WriteThrough)
{
    PtePointer->Pml2.CacheDisable = CacheDisable;
    PtePointer->Pml2.WriteThrough = WriteThrough;
}

/**
 * Checks whether the given PML3 page table entry (PTE) is valid.
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to check.
 *
 * @return Returns TRUE if the entry is valid, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MmpPml3PteValid(PHARDWARE_PTE PtePointer)
{
    return PtePointer->Pml3.Valid;
}

/**
 * Sets a PML3 page table entry (PTE) with the specified physical page and access flags.
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to set.
 *
 * @param PageFrameNumber
 *        Physical frame number to map.
 *
 * @param Writable
 *        Indicates whether the page should be writable.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpSetPml3Pte(PHARDWARE_PTE PtePointer,
              PFN_NUMBER PageFrameNumber,
              BOOLEAN Writable)
{
    PtePointer->Pml3.PageFrameNumber = PageFrameNumber;
    PtePointer->Pml3.Valid = 1;
    PtePointer->Pml3.Writable = Writable;
}

/**
 * Sets caching attributes for a PML3 page table entry (PTE).
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to modify.
 *
 * @param CacheDisable
 *        Indicates whether caching should be disabled for this page.
 *
 * @param WriteThrough
 *        Indicates whether write-through caching should be enabled.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpSetPml3PteCaching(PHARDWARE_PTE PtePointer,
                     BOOLEAN CacheDisable,
                     BOOLEAN WriteThrough)
{
    PtePointer->Pml3.CacheDisable = CacheDisable;
    PtePointer->Pml3.WriteThrough = WriteThrough;
}
