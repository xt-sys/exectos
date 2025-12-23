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
MM::Colors::GetFreePages(MMPAGELISTS PageList,
                         ULONG Color)
{
    /* Return a pointer to the requested color table entry */
    return &FreePages[PageList][Color];
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
MM::Colors::GetNextColor()
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
MM::Colors::GetPagingColors()
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
MM::Colors::GetPagingColorsMask()
{
    /* Return the mask used for page coloring calculations */
    return PagingColorsMask;
}
