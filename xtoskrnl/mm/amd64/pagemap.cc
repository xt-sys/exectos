/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pagemap.cc
 * DESCRIPTION:     Low-level support for i686 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Advances a PTE pointer by a given number of entries, considering the actual PTE size.
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
MM::PageMap::AdvancePte(IN PMMPTE Pte,
                        IN ULONG Count)
{
    /* Return advanced PTE pointer */
    return (PMMPTE)((ULONG_PTR)Pte + (Count * sizeof(MMPTE)));
}

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
ULONGLONG
MM::PageMap::GetEmptyPteList(VOID)
{
    /* Return empty PTE list mask */
    return PageMapInfo.EmptyPteList;
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
MM::PageMap::GetNextEntry(IN PMMPTE Pte)
{
    /* Return next entry in PTE list */
    return Pte->List.NextEntry;
}

/**
 * Advances a PTE pointer, considering the actual PTE size.
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
MM::PageMap::GetNextPte(IN PMMPTE Pte)
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
MM::PageMap::GetOneEntry(IN PMMPTE Pte)
{
    /* Return one entry status */
    return Pte->List.OneEntry;
}

/**
 * Gets the address of the P5E (Page Map Level 5 Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding P5E.
 *
 * @return This routine returns the address of the P5E, or NULLPTR if LA57 is not enabled.
 *
 * @since XT 1.0
 */
XTAPI
PMMP5E
MM::PageMap::GetP5eAddress(IN PVOID Address)
{
    ULONGLONG Offset;

    /* Calculate offset and return P5E address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_P5I_SHIFT) << MM_PTE_SHIFT);
    return (PMMP5E)((PageMapInfo.P5eBase + Offset) * PageMapInfo.Xpa);
}

/**
 * Gets the index of the P5E (Page Map Level 5 Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding P5E.
 *
 * @return This routine returns the index of the P5E, or NULLPTR if LA57 is not enabled.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetP5eIndex(IN PVOID Address)
{
    return (((((ULONGLONG)Address) >> MM_P5I_SHIFT) & 0x1FF) * PageMapInfo.Xpa);
}

/**
 * Gets the virtual address that is mapped by a given Page Map Level 5 Entry.
 *
 * @param P5ePointer
 *        Specifies the address of the P5E.
 *
 * @return This routine returns the virtual address mapped by the P5E, or NULLPTR if LA57 is not enabled.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
MM::PageMap::GetP5eVirtualAddress(IN PMMP5E P5ePointer)
{
    return (PVOID)((((LONGLONG)P5ePointer << 61) >> 16) * PageMapInfo.Xpa);
}

/**
 * Gets the page frame number from a corresponding PTE.
 *
 * @param Pte
 *        The PTE pointer to get the page frame number from.
 *
 * @return This routine returns the page frame number.
 *
 * @since XT 1.0
 */
XTAPI
PFN_NUMBER
MM::PageMap::GetPageFrameNumber(IN PMMPTE Pte)
{
    return Pte->Hardware.PageFrameNumber;
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
    ULONGLONG Offset;

    /* Calculate offset and return PDE address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PDI_SHIFT) << MM_PTE_SHIFT);
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
    return ((((ULONGLONG)Address) >> MM_PDI_SHIFT) & 0x1FF);
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
    return (PVOID)(((LONGLONG)PdePointer << 34) >> 16);
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
MM::PageMap::GetPpeAddress(IN PVOID Address)
{
    ULONGLONG Offset;

    /* Calculate offset and return PPE address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PPI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPPE)(PageMapInfo.PpeBase + Offset);
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
    return ((((ULONGLONG)Address) >> MM_PPI_SHIFT) & 0x1FF);
}

/**
 * Gets the virtual address that is mapped by a given Page Directory Pointer Table Entry.
 *
 * @param PpePointer
 *        Specifies the address of the PPE.
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
    return (PVOID)(((LONGLONG)PpePointer << 43) >> 16);
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
    ULONGLONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PTI_SHIFT) << MM_PTE_SHIFT);
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
    return ((((ULONGLONG)Address) >> MM_PTI_SHIFT) & 0x1FF);
}

/**
 * Gets the size of a PTE.
 *
 * @return This routine returns the size of a PTE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPteSize(VOID)
{
    /* Return the size of MMPTE */
    return sizeof(MMPTE);
}

/**
 * Gets the virtual address that is mapped by a given Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the address of the PTE.
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
    return (PVOID)(((LONGLONG)PtePointer << 25) >> 16);
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
MM::PageMap::GetPxeAddress(IN PVOID Address)
{
    ULONGLONG Offset;

    /* Calculate offset and return PXE address */
    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << PageMapInfo.VaBits) - 1)) >> MM_PXI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPXE)(PageMapInfo.PxeBase + Offset);
}

/**
 * Gets the index of the PXE (Extended Page Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PXE.
 *
 * @return This routine returns the index of the PXE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPxeIndex(IN PVOID Address)
{
    /* Return PXE index */
    return ((((ULONGLONG)Address) >> MM_PXI_SHIFT) & 0x1FF);
}

/**
 * Gets the virtual address that is mapped by a given Extended Page Entry.
 *
 * @param PxePointer
 *        Specifies the address of the PXE.
 *
 * @return This routine returns the virtual address mapped by the PXE.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
MM::PageMap::GetPxeVirtualAddress(IN PMMPXE PxePointer)
{
    /* Return PXE virtual address */
    return (PVOID)(((LONGLONG)PxePointer << 52) >> 16);
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
MM::PageMap::PteValid(IN PMMPTE PtePointer)
{
    /* Check if PTE is valid */
    return (BOOLEAN)PtePointer->Hardware.Valid;
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
MM::PageMap::SetNextEntry(IN PMMPTE Pte,
                          IN ULONG_PTR Value)
{
    /* Set next entry in PTE list */
    Pte->List.NextEntry = Value;
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
MM::PageMap::SetOneEntry(IN PMMPTE Pte,
                         IN BOOLEAN Value)
{
    /* Set one entry status */
    Pte->List.OneEntry = Value;
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
MM::PageMap::SetPte(IN PMMPTE PtePointer,
                    IN PFN_NUMBER PageFrameNumber,
                    IN ULONG_PTR AttributesMask)
{
    /* Set PTE */
    PtePointer->Hardware.PageFrameNumber = PageFrameNumber;
    PtePointer->Hardware.Valid = 1;
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
MM::PageMap::SetPteCaching(IN PMMPTE PtePointer,
                           IN BOOLEAN CacheDisable,
                           IN BOOLEAN WriteThrough)
{
    /* Set caching attributes */
    PtePointer->Hardware.CacheDisable = CacheDisable;
    PtePointer->Hardware.WriteThrough = WriteThrough;
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

    /* Set PML4 empty PTE list mask */
    PageMapInfo.EmptyPteList = 0xFFFFFFFFUI64;

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

    /* Set PML5 empty PTE list mask */
    PageMapInfo.EmptyPteList = 0xFFFFFFFFUI64;

    /* Set PML5 base addresses */
    PageMapInfo.PteBase = MM_PTE_LA57_BASE;
    PageMapInfo.PdeBase = MM_PDE_LA57_BASE;
    PageMapInfo.PpeBase = MM_PPE_LA57_BASE;
    PageMapInfo.PxeBase = MM_PXE_LA57_BASE;
    PageMapInfo.P5eBase = MM_P5E_LA57_BASE;

    /* PML5 use 57-bit virtual addresses */
    PageMapInfo.VaBits = 57;
}
