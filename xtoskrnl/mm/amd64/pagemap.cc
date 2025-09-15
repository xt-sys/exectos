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
    PtePointer->CacheDisable = 0;
    PtePointer->PageFrameNumber = 0;
    PtePointer->Valid = 0;
    PtePointer->Writable = 0;
    PtePointer->WriteThrough = 0;
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
MM::PageMap::GetP5eAddress(PVOID Address)
{
    ULONGLONG Offset;

    /* Calculate offset and return P5E address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_P5I_SHIFT) << MM_PTE_SHIFT);
    return (PMMP5E)((PageMapInfo.P5eBase + Offset) * PageMapInfo.Xpa);
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
    ULONGLONG Offset;

    /* Calculate offset and return PDE address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PDI_SHIFT) << MM_PTE_SHIFT);
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
    ULONGLONG Offset;

    /* Calculate offset and return PPE address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PPI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPPE)(PageMapInfo.PpeBase + Offset);
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
    ULONGLONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PTI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(PageMapInfo.PteBase + Offset);
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
MM::PageMap::GetPxeAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PXI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPXE)(PageMapInfo.PxeBase + Offset);
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
MM::PageMap::PteValid(PHARDWARE_PTE PtePointer)
{
    return (BOOLEAN)PtePointer->Valid;
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
MM::PageMap::SetPte(PHARDWARE_PTE PtePointer,
                    PFN_NUMBER PageFrameNumber,
                    BOOLEAN Writable)
{
    PtePointer->PageFrameNumber = PageFrameNumber;
    PtePointer->Valid = 1;
    PtePointer->Writable = Writable;
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
MM::PageMap::SetPteCaching(PHARDWARE_PTE PtePointer,
                           BOOLEAN CacheDisable,
                           BOOLEAN WriteThrough)
{
    PtePointer->CacheDisable = CacheDisable;
    PtePointer->WriteThrough = WriteThrough;
}

/**
 * Initializes page map information for basic paging (PML4).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::InitializePageMapInfo(VOID)
{
    /* Set PML4 page map information */
    PageMapInfo.Xpa = FALSE;

    /* Set PML4 base addresses */
    PageMapInfo.PteBase = MM_PTE_BASE;
    PageMapInfo.PdeBase = MM_PDE_BASE;
    PageMapInfo.PpeBase = MM_PPE_BASE;
    PageMapInfo.PxeBase = MM_PXE_BASE;
    PageMapInfo.P5eBase = 0x0;

    /* PML use 48-bit virtual addresses */
    PageMapInfo.VaBits = 48;
}

/**
 * Initializes page map information for XPA paging (PML5).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::InitializePageMapInfo(VOID)
{
    /* Set PML5 page map information */
    PageMapInfo.Xpa = TRUE;

    /* Set PML5 base addresses */
    PageMapInfo.PteBase = MM_PTE_LA57_BASE;
    PageMapInfo.PdeBase = MM_PDE_LA57_BASE;
    PageMapInfo.PpeBase = MM_PPE_LA57_BASE;
    PageMapInfo.PxeBase = MM_PXE_LA57_BASE;
    PageMapInfo.P5eBase = MM_P5E_LA57_BASE;

    /* PML5 use 57-bit virtual addresses */
    PageMapInfo.VaBits = 57;
}
