/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/paging.cc
 * DESCRIPTION:     Low level page management support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
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
MM::Paging::AdvancePte(IN PMMPTE Pte,
                       IN LONG Count)
{
    /* Return advanced PTE pointer */
    return PmlRoutines->AdvancePte(Pte, Count);
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
MM::Paging::ClearPte(IN PMMPTE PtePointer)
{
    /* Clear PTE */
    PmlRoutines->ClearPte(PtePointer);
}

/**
 * Flushes current Translation Lookaside Buffer (TLB)
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Paging::FlushTlb(VOID)
{
    CPUID_REGISTERS CpuRegisters;
    BOOLEAN Interrupts;
    ULONG_PTR Cr4;

    /* Save interrupts state and disable them */
    Interrupts = AR::CpuFunc::InterruptsEnabled();
    AR::CpuFunc::ClearInterruptFlag();

    /* Get CPU features */
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;
    AR::CpuFunc::CpuId(&CpuRegisters);

    /* Check if Paging Global Extensions (PGE) is supported */
    if(CpuRegisters.Edx & CPUID_FEATURES_EDX_PGE)
    {
        /* Read CR4 */
        Cr4 = AR::CpuFunc::ReadControlRegister(4);

        /* Disable PGE */
        AR::CpuFunc::WriteControlRegister(4, Cr4 & ~CR4_PGE);

        /* Flush the TLB */
        AR::CpuFunc::FlushTlb();

        /* Restore CR4 */
        AR::CpuFunc::WriteControlRegister(4, Cr4);
    }
    else
    {
        /* Simply flush the TLB */
        AR::CpuFunc::FlushTlb();
    }

    /* Check if interrupts should be enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunc::SetInterruptFlag();
    }
}

/**
 * Gets the value representing an empty PTE list.
 *
 * @return This routine returns the value representing an empty PTE list.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::Paging::GetEmptyPteList(VOID)
{
    /* Return empty PTE list mask */
    return (ULONG_PTR)PmlRoutines->GetEmptyPteList();
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
MM::Paging::GetNextEntry(IN PMMPTE Pte)
{
    /* Return next entry in PTE list */
    return PmlRoutines->GetNextEntry(Pte);
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
MM::Paging::GetNextPte(IN PMMPTE Pte)
{
    /* Return advanced PTE pointer */
    return PmlRoutines->GetNextPte(Pte);
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
MM::Paging::GetOneEntry(IN PMMPTE Pte)
{
    /* Return one entry status */
    return PmlRoutines->GetOneEntry(Pte);
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
MM::Paging::GetPageFrameNumber(IN PMMPTE Pte)
{
    return PmlRoutines->GetPageFrameNumber(Pte);
}

/**
 * Gets the page map routines for basic paging mode (non-XPA).
 *
 * @return This routine returns the address of the object containing non-XPA page map routines.
 *
 * @since XT 1.0
 */
XTAPI
MM::PPAGEMAP
MM::Paging::GetPageMapBasicRoutines(VOID)
{
    static MM::PageMapBasic PageMapBasicRoutines;

    /* Return non-XPA page map routines */
    return &PageMapBasicRoutines;
}

/**
 * Gets the page map routines for eXtended Physical Addressing (XPA) mode.
 *
 * @return This routine returns the address of the object containing XPA page map routines.
 *
 * @since XT 1.0
 */
XTAPI
MM::PPAGEMAP
MM::Paging::GetPageMapXpaRoutines(VOID)
{
    static MM::PageMapXpa PageMapXpaRoutines;

    /* Return XPA page map routines */
    return &PageMapXpaRoutines;
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
MM::Paging::GetPdeAddress(IN PVOID Address)
{
    /* Return PDE address */
    return PmlRoutines->GetPdeAddress(Address);
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
MM::Paging::GetPdeVirtualAddress(IN PMMPDE PdePointer)
{
    /* Return PTE virtual address */
    return PmlRoutines->GetPdeVirtualAddress(PdePointer);
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
MM::Paging::GetPpeAddress(IN PVOID Address)
{
    /* Return PPE address */
    return PmlRoutines->GetPpeAddress(Address);
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
MM::Paging::GetPpeVirtualAddress(IN PMMPPE PpePointer)
{
    /* Return PTE virtual address */
    return PmlRoutines->GetPpeVirtualAddress(PpePointer);
}

 /**
 * Gets the entire contents of a Page Table Entry (PTE) as a single value.
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
MM::Paging::GetPte(IN PMMPTE PtePointer)
{
    /* Return PTE value */
    return PmlRoutines->GetPte(PtePointer);
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
MM::Paging::GetPteAddress(IN PVOID Address)
{
    /* Return PTE address */
    return PmlRoutines->GetPteAddress(Address);
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
MM::Paging::GetPteDistance(PMMPTE EndPte,
                           PMMPTE StartPte)
{
    /* Return distance between PTE pointers */
    return PmlRoutines->GetPteDistance(EndPte, StartPte);
}

/**
 * Gets the terminator value for a PTE list.
 *
 * @return This routine returns the terminator value for a PTE list.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::Paging::GetPteListTerminator(VOID)
{
    /* Return PTE list terminator value */
    return PmlRoutines->GetPteListTerminator();
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
MM::Paging::GetPteSize(VOID)
{
    /* Return the size of MMPTE */
    return PmlRoutines->GetPteSize();
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
MM::Paging::GetPteSoftwareProtection(IN PMMPTE PtePointer)
{
    /* Return PTE software protection value */
    return PmlRoutines->GetPteSoftwareProtection(PtePointer);
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
MM::Paging::GetPteSoftwarePrototype(IN PMMPTE PtePointer)
{
    /* Return PTE software prototype value */
    return PmlRoutines->GetPteSoftwarePrototype(PtePointer);
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
MM::Paging::GetPteSoftwareTransition(IN PMMPTE PtePointer)
{
    /* Return PTE software transition value */
    return PmlRoutines->GetPteSoftwareTransition(PtePointer);
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
MM::Paging::GetPteVirtualAddress(IN PMMPTE PtePointer)
{
    /* Return PTE virtual address */
    return PmlRoutines->GetPteVirtualAddress(PtePointer);
}

/**
 * Gets current status of eXtended Physical Addressing (XPA).
 *
 * @return This routine returns TRUE if PAE or LA57 (XPA) is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Paging::GetXpaStatus()
{
    return PmlRoutines->GetXpaStatus();
}

/**
 * Detects if eXtended Physical Addressing (XPA) is enabled and initializes page map support.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Paging::InitializePageMapSupport(VOID)
{
    /* Check if XPA is enabled */
    if(GetExtendedPhysicalAddressingStatus())
    {
        /* XPA enabled, use modern paging (PAE / LA57) */
        PmlRoutines = GetPageMapXpaRoutines();
    }
    else
    {
        /* XPA disabled, use basic paging (PML2 / PML4) */
        PmlRoutines = GetPageMapBasicRoutines();
    }

    /* Set page map information */
    PmlRoutines->InitializePageMapInfo();
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
MM::Paging::PteValid(IN PMMPTE PtePointer)
{
    /* Check if PTE is valid */
    return PmlRoutines->PteValid(PtePointer);
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
MM::Paging::SetNextEntry(IN PMMPTE Pte,
                         IN ULONG_PTR Value)
{
    /* Set next entry in PTE list */
    PmlRoutines->SetNextEntry(Pte, Value);
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
MM::Paging::SetOneEntry(IN PMMPTE Pte,
                        IN BOOLEAN Value)
{
    /* Set one entry status */
    PmlRoutines->SetOneEntry(Pte, Value);
}

/**
 * Sets a Page Table Entry (PTE) with the specified physical page and access flags.
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
MM::Paging::SetPte(IN PMMPTE PtePointer,
                   IN PFN_NUMBER PageFrameNumber,
                   IN ULONG_PTR AttributesMask)
{
    /* Set PTE */
    PmlRoutines->SetPte(PtePointer, PageFrameNumber, AttributesMask);
}

/**
 * Sets a Page Table Entry (PTE) with the specified attributes.
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
MM::Paging::SetPte(IN PMMPTE PtePointer,
                   IN ULONG_PTR Attributes)
{
    PmlRoutines->SetPte(PtePointer, Attributes);
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
MM::Paging::SetPteCaching(IN PMMPTE PtePointer,
                          IN BOOLEAN CacheDisable,
                          IN BOOLEAN WriteThrough)
{
    /* Set caching attributes */
    PmlRoutines->SetPteCaching(PtePointer, CacheDisable, WriteThrough);
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
MM::Paging::TransitionPte(IN PMMPTE PointerPte,
                          IN ULONG_PTR Protection)
{
    /* Transition PTE */
    PmlRoutines->TransitionPte(PointerPte, Protection);
}

/**
 * Writes a Page Table Entry (PTE) with the specified value.
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
MM::Paging::WritePte(IN PMMPTE Pte,
                     IN MMPTE Value)
{
    /* Assign PTE value */
    PmlRoutines->WritePte(Pte, Value);
}
