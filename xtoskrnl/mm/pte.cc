/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/pte.cc
 * DESCRIPTION:     Page table entry (PTE) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Calculates the number of Page Table Entries (PTEs) that fit within a single page.
 *
 * @return This routine returns the number of PTEs per page.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::Pte::GetPtesPerPage(VOID)
{
    /* Calculate and return the number of PTEs per page */
    return MM_PAGE_SIZE / MM::Paging::GetPteSize();
}

/**
 * Maps a range of virtual addresses at the PDE (Page Directory Entry) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplatePde
 *        A template PDE to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapPDE(PVOID StartAddress,
                PVOID EndAddress,
                PMMPDE TemplatePde)
{
    PMMPDE EndSpace, PointerPde;

    /* Get PDE addresses */
    PointerPde = MM::Paging::GetPdeAddress(StartAddress);
    EndSpace = MM::Paging::GetPdeAddress(EndAddress);

    /* Iterate over all PDEs */
    while(PointerPde <= EndSpace)
    {
        /* Check if PDE is already mapped */
        if(!MM::Paging::PteValid(PointerPde))
        {
            /* Map PDE */
            MM::Paging::SetPte(TemplatePde, MM::Pfn::AllocateBootstrapPages(1), 0);
            *PointerPde = *TemplatePde;

            /* Clear the page table */
            RtlZeroMemory(MM::Paging::GetPteVirtualAddress(PointerPde), MM_PAGE_SIZE);
        }

        /* Get next table entry */
        PointerPde = MM::Paging::GetNextPte(PointerPde);
    }
}

/**
 * Maps a range of virtual addresses at the PTE (Page Table Entry) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplatePte
 *        A template PTE to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapPTE(PVOID StartAddress,
                PVOID EndAddress,
                PMMPTE TemplatePte)
{
    PMMPTE EndSpace, PointerPte;

    /* Get PTE addresses */
    PointerPte = MM::Paging::GetPteAddress(StartAddress);
    EndSpace = MM::Paging::GetPteAddress(EndAddress);

    /* Iterate over all PTEs */
    while(PointerPte <= EndSpace)
    {
        /* Check if PTE is already mapped */
        if(!MM::Paging::PteValid(PointerPte))
        {
            /* Map PTE */
            MM::Paging::SetPte(TemplatePte, MM::Pfn::AllocateBootstrapPages(1), 0);
            *PointerPte = *TemplatePte;

            /* Clear the page table */
            RtlZeroMemory(MM::Paging::GetPteVirtualAddress(PointerPte), MM_PAGE_SIZE);
        }

        /* Get next table entry */
        PointerPte = MM::Paging::GetNextPte(PointerPte);
    }
}
