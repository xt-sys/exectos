/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/pte.cc
 * DESCRIPTION:     Page Table Entry (PTE) for AMD64 support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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
