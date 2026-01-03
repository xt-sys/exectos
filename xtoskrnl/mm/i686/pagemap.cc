/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pagemap.cc
 * DESCRIPTION:     Low-level support for i686 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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
 * Gets the Offset of the PDE (Page Directory Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PDE.
 *
 * @return This routine returns the Offset of the PDE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPdeOffset(IN PVOID Address)
{
    /* Return PDE Offset */
    return ((((ULONG_PTR)(Address)) >> PageMapInfo.PdiShift) & (PageMapInfo.Xpa ? 0x1FF : 0x3FF));
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
 * Gets the offset of the PPE (Page Directory Pointer Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PPE.
 *
 * @return This routine returns the offset of the PPE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPpeOffset(IN PVOID Address)
{
    /* Return zero */
    return 0;
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
    /* Return zero */
    return (PVOID)0;
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
 * Gets the Offset of the PTE (Page Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PTE.
 *
 * @return This routine returns the Offset of the PTE.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMap::GetPteOffset(IN PVOID Address)
{
    /* Return PTE Offset */
    return ((((ULONG_PTR)(Address)) >> MM_PTI_SHIFT) & (PageMapInfo.Xpa ? 0x1FF : 0x3FF));
}


/**
 * Gets the status of Extended Paging Address (XPA) mode.
 *
 * @return This routine returns TRUE if XPA is enabled, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::PageMap::GetXpaStatus()
{
    return PageMapInfo.Xpa;
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
MM::PageMapBasic::ClearPte(IN PMMPTE PtePointer)
{
    /* Clear PTE */
    PtePointer->Pml2.Long = 0;
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
    /* Return next entry in PTE list, translating the hardware limit (0xFFFFF) to the logical sentinel (MAXULONG) */
    return (Pte->Pml2.List.NextEntry == 0xFFFFF) ? MAXULONG : Pte->Pml2.List.NextEntry;
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
MM::PageMapBasic::GetPageFrameNumber(IN PMMPTE Pte)
{
    return Pte->Pml2.Hardware.PageFrameNumber;
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
MM::PageMapBasic::GetPdeVirtualAddress(IN PMMPDE PdePointer)
{
    /* Return PDE virtual address */
    return ((PVOID)((ULONG)(PdePointer) << 20));
}

 /**
 * Gets the entire contents of a PML2 Page Table Entry (PTE) as a single value.
 *
 * @param PtePointer
 *        Pointer to the Page Table Entry (PTE) to read.
 *
 * @return This routine returns the contents of the PTE as a single value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::PageMapBasic::GetPte(IN PMMPTE PtePointer)
{
    /* Return PTE value */
    return PtePointer->Pml2.Long;
}
/**
 * Calculates the distance between two PTE pointers.
 *
 * @param EndPte
 *        Pointer to the ending Page Table Entry.
 *
 * @param StartPte
 *        Pointer to the starting Page Table Entry.
 *
 * @return This routine returns a signed value representing the number of PTEs between EndPte and StartPte.
 *
 * @since XT 1.0
 */
XTAPI
LONG
MM::PageMapBasic::GetPteDistance(PMMPTE EndPte,
                                 PMMPTE StartPte)
{
    /* Return distance between PTE pointers */
    return ((ULONG_PTR)EndPte - (ULONG_PTR)StartPte) / sizeof(MMPML2_PTE);
}

/**
 * Gets the terminator value for a PTE list (PML2).
 *
 * @return This routine returns the terminator value for a PTE list.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::PageMapBasic::GetPteListTerminator(VOID)
{
    /* Return PTE list terminator value for PML2 */
    return 0xFFFFF;
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
 * Gets the software protection value of the corresponding Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the address of the PTE.
 *
 * @return This routine returns the PTE software protection value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapBasic::GetPteSoftwareProtection(IN PMMPTE PtePointer)
{
    /* Return PTE software protection value */
    return (ULONG)PtePointer->Pml2.Software.Protection;
}

/**
 * Gets the software prototype value of the corresponding Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the address of the PTE.
 *
 * @return This routine returns the PTE software prototype value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapBasic::GetPteSoftwarePrototype(IN PMMPTE PtePointer)
{
    /* Return PTE software prototype value */
    return (ULONG)PtePointer->Pml2.Software.Prototype;
}

/**
 * Gets the software transition value of the corresponding Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the address of the PTE.
 *
 * @return This routine returns the PTE software transition value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapBasic::GetPteSoftwareTransition(IN PMMPTE PtePointer)
{
    /* Return PTE software transition value */
    return (ULONG)PtePointer->Pml2.Software.Transition;
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
MM::PageMapBasic::GetPteVirtualAddress(IN PMMPTE PtePointer)
{
    /* Return PTE virtual address */
    return ((PVOID)((ULONG)(PtePointer) << 10));
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
    /* Set next entry in PTE list, translating the logical sentinel (MAXULONG) to the 20-bit hardware limit (0xFFFFF) */
    Pte->Pml2.List.NextEntry = (Value == MAXULONG) ? 0xFFFFF : Value;
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
    PtePointer->Pml2.Long |= AttributesMask;
}

/**
 * Sets a PML2 page table entry (PTE) with the specified attributes.
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to set.
 *
 * @param Attributes
 *        Specifies the attributes to apply to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::SetPte(IN PMMPTE PtePointer,
                         IN ULONG_PTR Attributes)
{
    PtePointer->Pml2.Long = Attributes;
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
 * Transitions a Page Table Entry (PTE) to invalid state
 *
 * @param PointerPte
 *        Pointer to the page table entry (PTE) to transition.
 *
 * @param Protection
 *        Specifies the protection attribute to apply to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::TransitionPte(IN PMMPTE PointerPte,
                                IN ULONG_PTR Protection)
{
    MMPTE TempPte;

    /* Set transition PTE */
    TempPte = *PointerPte;
    TempPte.Pml2.Software.Protection = Protection;
    TempPte.Pml2.Software.Prototype = 0;
    TempPte.Pml2.Software.Transition = 1;
    TempPte.Pml2.Software.Valid = 0;

    /* Write PTE value */
    *PointerPte = TempPte;
}

/**
 * Writes a PML2 page table entry (PTE) with the specified value.
 *
 * @param Pte
 *        Pointer to the page table entry (PTE) to write.
 *
 * @param Value
 *        The value to write to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapBasic::WritePte(IN PMMPTE Pte,
                           IN MMPTE Value)
{
    /* Write PTE value */
    Pte->Pml2.Long = Value.Pml2.Long;
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
MM::PageMapXpa::ClearPte(IN PMMPTE PtePointer)
{
    /* Clear PTE */
    PtePointer->Pml3.Long = 0;
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
MM::PageMapXpa::GetPageFrameNumber(IN PMMPTE Pte)
{
    return Pte->Pml3.Hardware.PageFrameNumber;
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
MM::PageMapXpa::GetPdeVirtualAddress(IN PMMPDE PdePointer)
{
    /* Return PDE virtual address */
    return ((PVOID)((ULONG)(PdePointer) << 18));
}

 /**
 * Gets the entire contents of a PML3 Page Table Entry (PTE) as a single value.
 *
 * @param PtePointer
 *        Pointer to the Page Table Entry (PTE) to read.
 *
 * @return This routine returns the contents of the PTE as a single value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::PageMapXpa::GetPte(IN PMMPTE PtePointer)
{
    /* Return PTE value */
    return PtePointer->Pml3.Long;
}

/**
 * Calculates the distance between two PTE pointers.
 *
 * @param EndPte
 *        Pointer to the ending Page Table Entry.
 *
 * @param StartPte
 *        Pointer to the starting Page Table Entry.
 *
 * @return This routine returns a signed value representing the number of PTEs between EndPte and StartPte.
 *
 * @since XT 1.0
 */
XTAPI
LONG
MM::PageMapXpa::GetPteDistance(PMMPTE EndPte,
                               PMMPTE StartPte)
{
    /* Return distance between PTE pointers */
    return ((ULONG_PTR)EndPte - (ULONG_PTR)StartPte) / sizeof(MMPML3_PTE);
}

/**
 * Gets the terminator value for a PTE list (PML3).
 *
 * @return This routine returns the terminator value for a PTE list.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::PageMapXpa::GetPteListTerminator(VOID)
{
    /* Return PTE list terminator value for PML3 */
    return 0xFFFFFFFF;
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
 * Gets the software protection value of the corresponding Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the address of the PTE.
 *
 * @return This routine returns the PTE software protection value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapXpa::GetPteSoftwareProtection(IN PMMPTE PtePointer)
{
    /* Return PTE software protection value */
    return (ULONG)PtePointer->Pml3.Software.Protection;
}

/**
 * Gets the software prototype value of the corresponding Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the address of the PTE.
 *
 * @return This routine returns the PTE software prototype value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapXpa::GetPteSoftwarePrototype(IN PMMPTE PtePointer)
{
    /* Return PTE software prototype value */
    return (ULONG)PtePointer->Pml3.Software.Prototype;
}

/**
 * Gets the software transition value of the corresponding Page Table Entry.
 *
 * @param PtePointer
 *        Specifies the address of the PTE.
 *
 * @return This routine returns the PTE software transition value.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::PageMapXpa::GetPteSoftwareTransition(IN PMMPTE PtePointer)
{
    /* Return PTE software transition value */
    return (ULONG)PtePointer->Pml3.Software.Transition;
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
MM::PageMapXpa::GetPteVirtualAddress(IN PMMPTE PtePointer)
{
    /* Return PTE virtual address */
    return ((PVOID)((ULONG)(PtePointer) << 9));
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
    PtePointer->Pml3.Long |= AttributesMask;
}

/**
 * Sets a PML3 page table entry (PTE) with the specified attributes.
 *
 * @param PtePointer
 *        Pointer to the page table entry (PTE) to set.
 *
 * @param Attributes
 *        Specifies the attributes to apply to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::SetPte(IN PMMPTE PtePointer,
                       IN ULONG_PTR Attributes)
{
    PtePointer->Pml3.Long = Attributes;
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

/**
 * Transitions a Page Table Entry (PTE) to invalid state
 *
 * @param PointerPte
 *        Pointer to the page table entry (PTE) to transition.
 *
 * @param Protection
 *        Specifies the protection attribute to apply to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::TransitionPte(IN PMMPTE PointerPte,
                              IN ULONG_PTR Protection)
{
    MMPTE TempPte;

    /* Set transition PTE */
    TempPte = *PointerPte;
    TempPte.Pml3.Software.Protection = Protection;
    TempPte.Pml3.Software.Prototype = 0;
    TempPte.Pml3.Software.Transition = 1;
    TempPte.Pml3.Software.Valid = 0;

    /* Write PTE value */
    *PointerPte = TempPte;
}

/**
 * Writes a PML3 page table entry (PTE) with the specified value.
 *
 * @param Pte
 *        Pointer to the page table entry (PTE) to write.
 *
 * @param Value
 *        The value to write to the PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::PageMapXpa::WritePte(IN PMMPTE Pte,
                         IN MMPTE Value)
{
    /* Write PTE value */
    Pte->Pml3.Long = Value.Pml3.Long;
}