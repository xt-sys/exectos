/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/colors.cc
 * DESCRIPTION:     Memory manager page coloring subsystem
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Computes & initializes the system's page coloring.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Colors::ComputePageColoring(VOID)
{
    UNIMPLEMENTED;

    /* Compute L2 paging colors and mask */
    PagingColors = MM_PAGING_COLORS;
    PagingColorsMask = PagingColors - 1;
}

/**
 * Retrieves a pointer to the color table for a specific page list and color.
 *
 * @param PageList
 *        The page list type (e.g., FreePageList, ZeroedPageList).
 *
 * @param Color
 *        Supplies the specific color index.
 *
 * @return This routine returns a pointer to the corresponding MMCOLOR_TABLES structure.
 *
 * @since XT 1.0
 */
XTAPI
PMMCOLOR_TABLES
MM::Colors::GetFreePages(IN MMPAGELISTS PageList,
                         IN ULONG Color)
{
    /* Return a pointer to the requested color table entry */
    return &FreePages[PageList][Color];
}

/**
 * Retrieves a pointer to the modified pages list for a specific color.
 *
 * @param Color
 *        Supplies the specific color index.
 *
 * @return This routine returns a pointer to the corresponding MMPFNLIST structure.
 *
 * @since XT 1.0
 */
XTAPI
PMMPFNLIST
MM::Colors::GetModifiedPages(IN ULONG Color)
{
    return &ModifiedPages[Color];
}

/**
 * Retrieves the next available color for page coloring.
 *
 * @return This routine returns the next color value, ensuring it stays within the valid color range.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::Colors::GetNextColor(VOID)
{
    /* Increment the color counter and wrap it around using the mask */
    return ((++PagingColors) & PagingColorsMask);
}

/**
 * Retrieves the total number of page colors configured in the system.
 *
 * @return This routine returns the number of page colors.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::Colors::GetPagingColors(VOID)
{
    /* Return the total number of page colors */
    return PagingColors;
}

/**
 * Retrieves the bitmask used for calculating a page's color.
 *
 * @return This routine returns the page color mask.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::Colors::GetPagingColorsMask(VOID)
{
    /* Return the mask used for page coloring calculations */
    return PagingColorsMask;
}

/**
 * Initializes the data structures for page coloring.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Colors::InitializeColorTables(VOID)
{
    PMMMEMORY_LAYOUT MemoryLayout;
    PMMPTE PointerPte, LastPte;
    ULONG Color;
    PMMPTE ValidPte;

    /* Get the memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Set the base address of the color tables to start right after the PFN database */
    FreePages[0] = (PMMCOLOR_TABLES)&((PMMPFN)MemoryLayout->PfnDatabase)[MM::Pfn::GetHighestPhysicalPage() + 1];

    /* Calculate the virtual address range for both color tables */
    PointerPte = MM::Paging::GetPteAddress(&FreePages[0][0]);
    LastPte = MM::Paging::GetPteAddress((PVOID)((ULONG_PTR)FreePages[0] +
                                        (2 * PagingColors * sizeof(MMCOLOR_TABLES)) - 1));

    /* Get a pointer to a PTE template */
    ValidPte = MM::Pte::GetValidPte();

    /* Ensure the entire virtual address range for the color tables is mapped */
    while(PointerPte <= LastPte)
    {
        /* Check if a page in the range is not mapped */
        if(!MM::Paging::PteValid(PointerPte))
        {
            /* Use the bootstrap allocator to get a physical page */
            MM::Paging::SetPte(ValidPte, MM::Pfn::AllocateBootstrapPages(1), 0);
            *PointerPte = *ValidPte;

            /* Zero out the newly mapped page */
            RTL::Memory::ZeroMemory(MM::Paging::GetPteVirtualAddress(PointerPte), MM_PAGE_SIZE);
        }

        /* Move to the next PTE in the range */
        PointerPte = MM::Paging::GetNextPte(PointerPte);
    }

    /* Set the pointer for the second list */
    FreePages[1] = &FreePages[0][PagingColors];

    /* Initialize all entries in both color tables */
    for(Color = 0; Color < PagingColors; Color++)
    {
        /* Initialize the FreePageList entry for the current color */
        FreePages[FreePageList][Color].Flink = MAXULONG_PTR;
        FreePages[FreePageList][Color].Blink = (PVOID)MAXULONG_PTR;
        FreePages[FreePageList][Color].Count = 0;

        /* Initialize the ZeroedPageList entry for the current color */
        FreePages[ZeroedPageList][Color].Flink = MAXULONG_PTR;
        FreePages[ZeroedPageList][Color].Blink = (PVOID)MAXULONG_PTR;
        FreePages[ZeroedPageList][Color].Count = 0;
    }
}
