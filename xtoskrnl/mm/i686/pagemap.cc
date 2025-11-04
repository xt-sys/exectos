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
MM::PageMap::ClearPte(IN PMMPTE PtePointer)
{
    /* Clear PTE */
    PtePointer->Long = 0;
}

/**
 * Gets the value representing an empty PTE list.
 *
 * @return This routine returns the value representing an empty PTE list.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetEmptyPteList(VOID)
{
    /* Return empty PTE list mask */
    return PageMapInfo.EmptyPteList;
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
MM::PageMap::GetPdeAddress(IN PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PDE address */
    Offset = ((((ULONG_PTR)(Address)) >> PageMapInfo.PdiShift) << PageMapInfo.PteShift);
    return (PMMPDE)(PageMapInfo.PdeBase + Offset);
}

/**
 * Gets the index of the PDE (Page Directory Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PDE.
 *
 * @return This routine returns the index of the PDE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPdeIndex(IN PVOID Address)
{
    /* Return PDE index */
    return ((((ULONG_PTR)(Address)) >> PageMapInfo.PdiShift) & (PageMapInfo.Xpa ? 0x1FF : 0x3FF));
}

/**
 * Gets the virtual address that is mapped by a given Page Directory Entry.
 *
 * @param PdePointer
 *        Specifies the address of the PDE.
 *
 * @return This routine returns the virtual address mapped by the PDE.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
MM::PageMap::GetPdeVirtualAddress(IN PMMPDE PdePointer)
{
    /* Return PDE virtual address */
    return ((PVOID)((ULONG)(PdePointer) << 20));
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
MM::PageMap::GetPpeAddress(IN PVOID Address)
{
    /* Return zero */
    return (PMMPPE)0;
}

/**
 * Gets the index of the PPE (Page Directory Pointer Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PPE.
 *
 * @return This routine returns the index of the PPE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPpeIndex(IN PVOID Address)
{
    /* Return PPE index */
    return ((((ULONG_PTR)(Address)) >> MM_PPI_SHIFT) & 0x3) * PageMapInfo.Xpa;
}

/**
 * Gets the virtual address that is mapped by a given Page Directory Pointer Table Entry.
 *
 * @param PpePointer
 *        Specifies the virtual address of the PPE.
 *
 * @return This routine returns the virtual address mapped by the PPE.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
MM::PageMap::GetPpeVirtualAddress(IN PMMPPE PpePointer)
{
    /* Return PPE virtual address */
    return (PVOID)((ULONG)(PpePointer) << 30);
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
MM::PageMap::GetPteAddress(IN PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONG_PTR)(Address)) >> MM_PTI_SHIFT) << PageMapInfo.PteShift);
    return (PMMPTE)(PageMapInfo.PteBase + Offset);
}

/**
 * Gets the index of the PTE (Page Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PTE.
 *
 * @return This routine returns the index of the PTE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPteIndex(IN PVOID Address)
{
    /* Return PTE index */
    return ((((ULONG_PTR)(Address)) >> MM_PTI_SHIFT) & (PageMapInfo.Xpa ? 0x1FF : 0x3FF));
}

/**
 * Gets the virtual address that is mapped by a given Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the virtual address of the PTE.
 *
 * @return This routine returns the virtual address mapped by the PTE.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
MM::PageMap::GetPteVirtualAddress(IN PMMPTE PtePointer)
{
    /* Return PTE virtual address */
    return ((PVOID)((ULONG)(PtePointer) << 10));
}

/**
 * Advances a PTE pointer by a given number of entries, considering the actual PTE size for PML2.
 *
 * @param Pte
 *        The PTE pointer to advance.
 *
 * @param Count
 *        The number of PTE entries to advance by.
 *
 * @return This routine returns the advanced PTE pointer.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::PageMapBasic::AdvancePte(IN PMMPTE Pte,
                             IN ULONG Count)
{
    /* Return advanced PTE pointer */
    return (PMMPTE)((ULONG_PTR)Pte + (Count * sizeof(MMPML2_PTE)));
}

/**
 * Gets the next entry in a PTE list.
 *
 * @param Pte
 *        The PTE pointer to get the next entry from.
 *
 * @return This routine returns the next entry in the PTE list.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::PageMapBasic::GetNextEntry(IN PMMPTE Pte)
{
    /* Return next entry in PTE list */
    return Pte->Pml2.List.NextEntry;
}

/**
 * Advances a PTE pointer, considering the actual PTE size for PML2.
 *
 * @param Pte
 *        The PTE pointer to advance.
 *
 * @return The advanced PTE pointer.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::PageMapBasic::GetNextPte(IN PMMPTE Pte)
{
    /* Return advanced PTE pointer */
    return AdvancePte(Pte, 1);
}

/**
 * Checks if a PTE list contains only one entry.
 *
 * @param Pte
 *        The PTE pointer to check.
 *
 * @return This routine returns TRUE if the PTE list has only one entry, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::PageMapBasic::GetOneEntry(IN PMMPTE Pte)
{
    /* Return one entry status */
    return Pte->Pml2.List.OneEntry;
}

/**
 * Gets the size of a PTE for basic paging (PML2).
 *
 * @return This routine returns the size of a PTE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapBasic::GetPteSize(VOID)
{
    /* Return the size of MMPTE */
    return sizeof(MMPML2_PTE);
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

    /* Set PML2 empty PTE list mask */
    PageMapInfo.EmptyPteList = (ULONG)0xFFFFF;

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
MM::PageMapBasic::PteValid(IN PMMPTE PtePointer)
{
    /* Check if PTE is valid */
    return (BOOLEAN)PtePointer->Pml2.Hardware.Valid;
}

/**
 * Sets the next entry in a PTE list.
 *
 * @param Pte
 *        The PTE pointer to modify.
 *
 * @param Value
 *        The value to set as the next entry.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::SetNextEntry(IN PMMPTE Pte,
                               IN ULONG_PTR Value)
{
    /* Set next entry in PTE list */
    Pte->Pml2.List.NextEntry = Value;
}

/**
 * Sets the flag indicating whether a PTE list contains only one entry.
 *
 * @param Pte
 *        The PTE pointer to modify.
 *
 * @param Value
 *        The value to set. TRUE if the list has only one entry, FALSE otherwise.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::SetOneEntry(IN PMMPTE Pte,
                              IN BOOLEAN Value)
{
    /* Set one entry status */
    Pte->Pml2.List.OneEntry = Value;
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
 * @param AttributesMask
 *        Specifies the attributes mask to apply to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::SetPte(IN PMMPTE PtePointer,
                         IN PFN_NUMBER PageFrameNumber,
                         IN ULONG_PTR AttributesMask)
{
    /* Set PTE */
    PtePointer->Pml2.Hardware.PageFrameNumber = PageFrameNumber;
    PtePointer->Pml2.Hardware.Valid = 1;
    PtePointer->Long |= AttributesMask;
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
MM::PageMapBasic::SetPteCaching(IN PMMPTE PtePointer,
                                IN BOOLEAN CacheDisable,
                                IN BOOLEAN WriteThrough)
{
    /* Set caching attributes */
    PtePointer->Pml2.Hardware.CacheDisable = CacheDisable;
    PtePointer->Pml2.Hardware.WriteThrough = WriteThrough;
}

/**
 * Advances a PTE pointer by a given number of entries, considering the actual PTE size for PML3.
 *
 * @param Pte
 *        The PTE pointer to advance.
 *
 * @param Count
 *        The number of PTE entries to advance by.
 *
 * @return The advanced PTE pointer.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::PageMapXpa::AdvancePte(IN PMMPTE Pte,
                           IN ULONG Count)
{
    /* Return advanced PTE pointer */
    return (PMMPTE)((ULONG_PTR)Pte + (Count * sizeof(MMPML3_PTE)));
}

/**
 * Gets the next entry in a PTE list.
 *
 * @param Pte
 *        The PTE pointer to get the next entry from.
 *
 * @return This routine returns the next entry in the PTE list.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::PageMapXpa::GetNextEntry(IN PMMPTE Pte)
{
    /* Return next entry in PTE list */
    return Pte->Pml3.List.NextEntry;
}

/**
 * Advances a PTE pointer, considering the actual PTE size for PML3.
 *
 * @param Pte
 *        The PTE pointer to advance.
 *
 * @return The advanced PTE pointer.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::PageMapXpa::GetNextPte(IN PMMPTE Pte)
{
    /* Return advanced PTE pointer */
    return AdvancePte(Pte, 1);
}

/**
 * Checks if a PTE list contains only one entry.
 *
 * @param Pte
 *        The PTE pointer to check.
 *
 * @return This routine returns TRUE if the PTE list has only one entry, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::PageMapXpa::GetOneEntry(IN PMMPTE Pte)
{
    /* Return one entry status */
    return Pte->Pml3.List.OneEntry;
}

/**
 * Gets the size of a PTE for XPA paging (PML3).
 *
 * @return This routine returns the size of a PTE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapXpa::GetPteSize(VOID)
{
    /* Return the size of MMPTE */
    return sizeof(MMPML3_PTE);
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
    /* Set PML3 page map information */
    PageMapInfo.Xpa = TRUE;

    /* Set PML3 empty PTE list mask */
    PageMapInfo.EmptyPteList = (ULONG)0xFFFFFFFF;

    /* Set PML3 base addresses */
    PageMapInfo.PteBase = MM_PTE_BASE;
    PageMapInfo.PdeBase = MM_PDE_BASE;

    /* Set PML3 shift values */
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
MM::PageMapXpa::PteValid(IN PMMPTE PtePointer)
{
    return (BOOLEAN)PtePointer->Pml3.Hardware.Valid;
}

/**
 * Sets the next entry in a PTE list.
 *
 * @param Pte
 *        The PTE pointer to modify.
 *
 * @param Value
 *        The value to set as the next entry.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::SetNextEntry(IN PMMPTE Pte,
                             IN ULONG_PTR Value)
{
    /* Set next entry in PTE list */
    Pte->Pml3.List.NextEntry = Value;
}

/**
 * Sets the flag indicating whether a PTE list contains only one entry.
 *
 * @param Pte
 *        The PTE pointer to modify.
 *
 * @param Value
 *        The value to set. TRUE if the list has only one entry, FALSE otherwise.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::SetOneEntry(IN PMMPTE Pte,
                            IN BOOLEAN Value)
{
    /* Set one entry status */
    Pte->Pml3.List.OneEntry = Value;
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
 * @param AttributesMask
 *        Specifies the attributes mask to apply to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::SetPte(IN PMMPTE PtePointer,
                       IN PFN_NUMBER PageFrameNumber,
                       IN ULONG_PTR AttributesMask)
{
    /* Set PTE */
    PtePointer->Pml3.Hardware.PageFrameNumber = PageFrameNumber;
    PtePointer->Pml3.Hardware.Valid = 1;
    PtePointer->Long |= AttributesMask;
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
MM::PageMapXpa::SetPteCaching(IN PMMPTE PtePointer,
                              IN BOOLEAN CacheDisable,
                              IN BOOLEAN WriteThrough)
{
    /* Set caching attributes */
    PtePointer->Pml3.Hardware.CacheDisable = CacheDisable;
    PtePointer->Pml3.Hardware.WriteThrough = WriteThrough;
}
