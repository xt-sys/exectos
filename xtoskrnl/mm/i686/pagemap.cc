/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pagemap.cc
 * DESCRIPTION:     Low-level support for i686 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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
MM::PageMap::ClearPte(PHARDWARE_PTE PtePointer)
{
    PtePointer->Long = 0;
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
MM::PageMap::GetPdeAddress(PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PDE address */
    Offset = ((((ULONG_PTR)(Address)) >> PageMapInfo.PdiShift) << PageMapInfo.PteShift);
    return (PMMPDE)(PageMapInfo.PdeBase + Offset);
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
MM::PageMap::GetPpeAddress(PVOID Address)
{
    /* Return zero */
    return (PMMPPE)0;
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
MM::PageMap::GetPteAddress(PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONG_PTR)(Address)) >> MM_PTI_SHIFT) << PageMapInfo.PteShift);
    return (PMMPTE)(MM_PTE_BASE + Offset);
}

/**
 * Initializes page map information for basic paging (PML2).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::InitializePageMapInfo(VOID)
{
    /* Set PML2 page map information */
    PageMapInfo.Xpa = FALSE;

    /* Set PML2 base addresses */
    PageMapInfo.PteBase = MM_PTE_BASE;
    PageMapInfo.PdeBase = MM_PDE_LEGACY_BASE;

    /* Set PML2 shift values */
    PageMapInfo.PdiShift = MM_PDI_LEGACY_SHIFT;
    PageMapInfo.PteShift = MM_PTE_LEGACY_SHIFT;
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
MM::PageMapBasic::PteValid(PHARDWARE_PTE PtePointer)
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
MM::PageMapBasic::SetPte(PHARDWARE_PTE PtePointer,
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
MM::PageMapBasic::SetPteCaching(PHARDWARE_PTE PtePointer,
                                BOOLEAN CacheDisable,
                                BOOLEAN WriteThrough)
{
    PtePointer->Pml2.CacheDisable = CacheDisable;
    PtePointer->Pml2.WriteThrough = WriteThrough;
}

/**
 * Initializes page map information for basic paging (PML3).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::InitializePageMapInfo(VOID)
{
    /* Set PML2 page map information */
    PageMapInfo.Xpa = TRUE;

    /* Set PML2 base addresses */
    PageMapInfo.PteBase = MM_PTE_BASE;
    PageMapInfo.PdeBase = MM_PDE_BASE;

    /* Set PML2 shift values */
    PageMapInfo.PdiShift = MM_PDI_SHIFT;
    PageMapInfo.PteShift = MM_PTE_SHIFT;
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
MM::PageMapXpa::PteValid(PHARDWARE_PTE PtePointer)
{
    return (BOOLEAN)PtePointer->Pml3.Valid;
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
MM::PageMapXpa::SetPte(PHARDWARE_PTE PtePointer,
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
MM::PageMapXpa::SetPteCaching(PHARDWARE_PTE PtePointer,
                              BOOLEAN CacheDisable,
                              BOOLEAN WriteThrough)
{
    PtePointer->Pml3.CacheDisable = CacheDisable;
    PtePointer->Pml3.WriteThrough = WriteThrough;
}
