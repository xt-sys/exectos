/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/pte.cc
 * DESCRIPTION:     Page Table Entry (PTE) for AMD64 support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Checks if the virtual address is valid and mapped in the page tables.
 *
 * @param VirtualAddress
 *        The virtual address to check.
 *
 * @return This routine returns TRUE if the address is valid, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Pte::AddressValid(IN PVOID VirtualAddress)
{
    /* Check XPA status */
    if(MM::Paging::GetXpaStatus())
    {
        /* Check if the P5E is valid */
        if(!MM::Paging::PteValid(MM::Paging::GetP5eAddress(VirtualAddress)))
        {
            /* Invalid P5E, return FALSE */
            return FALSE;
        }
    }

    /* Check if PXE, PPE, PDE and PTE are valid */
    if(!MM::Paging::PteValid(MM::Paging::GetPxeAddress(VirtualAddress)) ||
       !MM::Paging::PteValid(MM::Paging::GetPpeAddress(VirtualAddress)) ||
       !MM::Paging::PteValid(MM::Paging::GetPdeAddress(VirtualAddress)) ||
       !MM::Paging::PteValid(MM::Paging::GetPteAddress(VirtualAddress)))
    {
        /* Invalid PXE, PPE, PDE or PTE, return FALSE */
        return FALSE;
    }

    /* Address is valid, return TRUE */
    return TRUE;
}

/**
 * Retrieves the base virtual address of the system PTEs.
 *
 * @return This routine returns a pointer to the first PTE in the system PTE space.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::Pte::GetSystemPteBaseAddress(VOID)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Retrieve the system's memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Determine the base address for system PTEs based on the paging mode */
    if(MM::Paging::GetXpaStatus())
    {
        /* For 5-level paging, system PTEs start at the beginning of system space */
        return MM::Paging::GetPteAddress((PVOID)MemoryLayout->SystemSpaceStart);
    }
    else
    {
        /* For 4-level paging, system PTEs start at the legacy KSEG0_BASE */
        return MM::Paging::GetPteAddress((PVOID)KSEG0_BASE);
    }
}


/**
 * Performs the initial setup of the system's page table hierarchy.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::InitializePageTable(VOID)
{
    UNIMPLEMENTED;
}

/**
 * Maps a range of virtual addresses at the P5E (PML5) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplateP5e
 *        A template P5E to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapP5E(PVOID StartAddress,
                PVOID EndAddress,
                PMMP5E TemplateP5e)
{
    PMMP5E EndSpace, PointerP5e;

    /* Get P5E addresses */
    PointerP5e = MM::Paging::GetP5eAddress(StartAddress);
    EndSpace = MM::Paging::GetP5eAddress(EndAddress);

    /* Iterate over all P5Es */
    while(PointerP5e <= EndSpace)
    {
        /* Check if P5E is already mapped */
        if(!MM::Paging::PteValid(PointerP5e))
        {
            /* Map P5E */
            MM::Paging::SetPte(TemplateP5e, MM::Pfn::AllocateBootstrapPages(1), 0);
            *PointerP5e = *TemplateP5e;

            /* Clear the page table */
            RtlZeroMemory(MM::Paging::GetPteVirtualAddress(PointerP5e), MM_PAGE_SIZE);
        }

        /* Get next table entry */
        PointerP5e = MM::Paging::GetNextPte(PointerP5e);
    }
}

/**
 * Maps a range of virtual addresses at the PXE (PML4) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplatePxe
 *        A template PXE to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapPXE(PVOID StartAddress,
                PVOID EndAddress,
                PMMPXE TemplatePxe)
{
    PMMPXE EndSpace, PointerPxe;

    /* Get PXE addresses */
    PointerPxe = MM::Paging::GetPxeAddress(StartAddress);
    EndSpace = MM::Paging::GetPxeAddress(EndAddress);

    /* Iterate over all PTEs */
    while(PointerPxe <= EndSpace)
    {
        /* Check if PTE is already mapped */
        if(!MM::Paging::PteValid(PointerPxe))
        {
            /* Map PTE */
            MM::Paging::SetPte(TemplatePxe, MM::Pfn::AllocateBootstrapPages(1), 0);
            *PointerPxe = *TemplatePxe;

            /* Clear the page table */
            RtlZeroMemory(MM::Paging::GetPteVirtualAddress(PointerPxe), MM_PAGE_SIZE);
        }

        /* Get next table entry */
        PointerPxe = MM::Paging::GetNextPte(PointerPxe);
    }
}
