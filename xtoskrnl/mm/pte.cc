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
 * Maps a range of virtual addresses at the PPE (Page Directory Pointer Entry) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplatePpe
 *        A template PPE to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapPPE(PVOID StartAddress,
                PVOID EndAddress,
                PMMPPE TemplatePpe)
{
    PMMPPE EndSpace, PointerPpe;

    /* Get PPE addresses */
    PointerPpe = MM::Paging::GetPpeAddress(StartAddress);
    EndSpace = MM::Paging::GetPpeAddress(EndAddress);

    /* Iterate over all PPEs */
    while(PointerPpe <= EndSpace)
    {
        /* Check if PPE is already mapped */
        if(!MM::Paging::PteValid(PointerPpe))
        {
            /* Map PPE */
            MM::Paging::SetPte(TemplatePpe, MM::Pfn::AllocateBootstrapPages(1), 0);
            *PointerPpe = *TemplatePpe;

            /* Clear the page table */
            RtlZeroMemory(MM::Paging::GetPteVirtualAddress(PointerPpe), MM_PAGE_SIZE);
        }

        /* Get next table entry */
        PointerPpe = MM::Paging::GetNextPte(PointerPpe);
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
