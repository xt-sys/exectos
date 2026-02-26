/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/alloc.hh
 * DESCRIPTION:     Memory manager pool allocation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_ALLOC_HH
#define __XTOSKRNL_MM_ALLOC_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Allocator
    {
        private:
            STATIC LIST_ENTRY NonPagedPoolFreeList[MM_MAX_FREE_PAGE_LIST_HEADS];

        public:
            STATIC XTAPI XTSTATUS AllocatePages(IN MMPOOL_TYPE PoolType,
                                                IN SIZE_T Bytes,
                                                OUT PVOID *Memory);
            STATIC XTAPI VOID InitializeNonPagedPool(VOID);
            STATIC XTAPI VOID InitializePagedPool(VOID);

        private:
            STATIC XTAPI XTSTATUS AllocateNonPagedPoolPages(IN PFN_COUNT Pages,
                                                            OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS AllocatePagedPoolPages(IN PFN_COUNT Pages,
                                                         OUT PVOID *Memory);
            STATIC XTAPI VOID MapNonPagedPool(VOID);
    };
}

#endif /* __XTOSKRNL_MM_ALLOC_HH */
