/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/arch/i686/memory.c
 * DESCRIPTION:     XT Boot Loader i686 specific memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Does the actual virtual memory mapping.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param VirtualAddress
 *        Supplies a virtual address of the mapping.
 *
 * @param PhysicalAddress
 *        Supplies a physical address of the mapping.
 *
 * @param NumberOfPages
 *        Supplies a number of the pages of the mapping.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlMapPage(IN PXTBL_PAGE_MAPPING PageMap,
          IN UINT_PTR VirtualAddress,
          IN UINT_PTR PhysicalAddress,
          IN UINT NumberOfPages)
{
    SIZE_T Pml1Entry, Pml2Entry, Pml3Entry;
    PHARDWARE_PTE Pml1, Pml2, Pml3;
    SIZE_T PageFrameNumber;
    EFI_STATUS Status;

    /* Set the Page Frame Number (PFN) */
    PageFrameNumber = PhysicalAddress >> EFI_PAGE_SHIFT;

    /* Do the recursive mapping */
    while(NumberOfPages > 0)
    {
        /* Calculate the indices in the various Page Tables from the virtual address */
        Pml3Entry = (VirtualAddress & ((ULONGLONG)0x1FF << 30)) >> 30;
        Pml2Entry = (VirtualAddress & ((ULONGLONG)0x1FF << 21)) >> 21;
        Pml1Entry = (VirtualAddress & ((ULONGLONG)0x1FF << 12)) >> 12;

        /* Check page map level */
        if(PageMap->PageMapLevel == 3)
        {
            /* Three level Page Map */
            Pml3 = ((PHARDWARE_PTE)(PageMap->PtePointer));

            /* Get PML2 */
            Status = BlpGetNextPageTable(PageMap, Pml3, Pml3Entry, &Pml2);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory mapping failure */
                return Status;
            }
        }
        else
        {
            /* Two level Page Map */
            Pml2 = ((PHARDWARE_PTE)(PageMap->PtePointer));
        }

        /* Get PML1 */
        Status = BlpGetNextPageTable(PageMap, Pml2, Pml2Entry, &Pml1);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory mapping failure */
            return Status;
        }

        /* Set paging entry settings */
        Pml1[Pml1Entry].PageFrameNumber = PageFrameNumber;
        Pml1[Pml1Entry].Valid = 1;
        Pml1[Pml1Entry].Write = 1;

        /* Take next virtual address and PFN */
        VirtualAddress += EFI_PAGE_SIZE;
        PageFrameNumber++;

        /* Decrease number of pages left */
        NumberOfPages--;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
