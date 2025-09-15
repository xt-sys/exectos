/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/paging.cc
 * DESCRIPTION:     Low level page management support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
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
MM::Paging::ClearPte(PHARDWARE_PTE PtePointer)
{
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
MM::Paging::GetPdeAddress(PVOID Address)
{
    return PmlRoutines->GetPdeAddress(Address);
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
MM::Paging::GetPpeAddress(PVOID Address)
{
    return PmlRoutines->GetPpeAddress(Address);
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
MM::Paging::GetPteAddress(PVOID Address)
{
    return PmlRoutines->GetPteAddress(Address);
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
MM::Paging::PteValid(PHARDWARE_PTE PtePointer)
{
    return PmlRoutines->PteValid(PtePointer);
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
MM::Paging::SetPte(PHARDWARE_PTE PtePointer,
                   PFN_NUMBER PageFrameNumber,
                   BOOLEAN Writable)
{
    PmlRoutines->SetPte(PtePointer, PageFrameNumber, Writable);
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
MM::Paging::SetPteCaching(PHARDWARE_PTE PtePointer,
                          BOOLEAN CacheDisable,
                          BOOLEAN WriteThrough)
{
    PmlRoutines->SetPteCaching(PtePointer, CacheDisable, WriteThrough);
}
