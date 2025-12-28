/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/colors.hh
 * DESCRIPTION:     Memory manager page coloring subsystem
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_COLORS_HH
#define __XTOSKRNL_MM_COLORS_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Colors
    {
        private:
            STATIC PMMCOLOR_TABLES FreePages[FreePageList + 1];
            STATIC ULONG PagingColors;
            STATIC ULONG PagingColorsMask;

        public:
            STATIC XTAPI VOID ComputePageColoring(VOID);
            STATIC XTAPI PMMCOLOR_TABLES GetFreePages(MMPAGELISTS PageList,
                                                      ULONG Color);
            STATIC XTAPI ULONG GetNextColor();
            STATIC XTAPI ULONG GetPagingColors();
            STATIC XTAPI ULONG GetPagingColorsMask();
            STATIC XTAPI VOID InitializeColorTables(VOID);
    };
}

#endif /* __XTOSKRNL_MM_PFN_HH */
