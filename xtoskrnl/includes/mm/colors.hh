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
            STATIC MMPFNLIST ModifiedPages[MM_PAGING_COLORS];
            STATIC ULONG PagingColors;
            STATIC ULONG PagingColorsMask;

        public:
            STATIC XTAPI VOID ComputePageColoring(VOID);
            STATIC XTAPI PMMCOLOR_TABLES GetFreePages(IN MMPAGELISTS PageList,
                                                      IN ULONG Color);
            STATIC XTAPI PMMPFNLIST GetModifiedPages(IN ULONG Color);
            STATIC XTAPI ULONG GetNextColor(VOID);
            STATIC XTAPI ULONG GetPagingColors(VOID);
            STATIC XTAPI ULONG GetPagingColorsMask(VOID);
            STATIC XTAPI VOID InitializeColorTables(VOID);
    };
}

#endif /* __XTOSKRNL_MM_PFN_HH */
