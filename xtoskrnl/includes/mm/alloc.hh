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
            STATIC PFN_NUMBER NonPagedPoolFrameEnd;
            STATIC PFN_NUMBER NonPagedPoolFrameStart;
            STATIC LIST_ENTRY NonPagedPoolFreeList[MM_MAX_FREE_PAGE_LIST_HEADS];

        public:
            STATIC XTAPI XTSTATUS AllocatePages(IN MMPOOL_TYPE PoolType,
                                                IN SIZE_T Bytes,
                                                OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS AllocatePool(IN MMPOOL_TYPE PoolType,
                                               IN SIZE_T Bytes,
                                               OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS AllocatePool(IN MMPOOL_TYPE PoolType,
                                               IN SIZE_T Bytes,
                                               OUT PVOID *Memory,
                                               IN ULONG Tag);
            STATIC XTAPI XTSTATUS FreePages(IN PVOID VirtualAddress);
            STATIC XTAPI XTSTATUS FreePool(IN PVOID VirtualAddress);
            STATIC XTAPI XTSTATUS FreePool(IN PVOID VirtualAddress,
                                           IN ULONG Tag);
            STATIC XTAPI VOID InitializeNonPagedPool(VOID);
            STATIC XTAPI VOID InitializePagedPool(VOID);

        private:
            STATIC XTAPI XTSTATUS AllocateNonPagedPoolPages(IN PFN_COUNT Pages,
                                                            OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS AllocatePagedPoolPages(IN PFN_COUNT Pages,
                                                         OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS FreeNonPagedPoolPages(IN PVOID VirtualAddress);
            STATIC XTAPI XTSTATUS FreePagedPoolPages(IN PVOID VirtualAddress);
            STATIC XTAPI VOID MapNonPagedPool(VOID);
            STATIC XTINLINE VOID VerifyRunLevel(IN MMPOOL_TYPE PoolType,
                                                IN SIZE_T Bytes,
                                                IN PVOID Entry);
    };
}

#endif /* __XTOSKRNL_MM_ALLOC_HH */
