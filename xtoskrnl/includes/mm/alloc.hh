/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/alloc.hh
 * DESCRIPTION:     Memory Manager pool allocator
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_ALLOC_HH
#define __XTOSKRNL_MM_ALLOC_HH

#include <xtos.hh>
#include <mm/pool.hh>


/* Memory Manager */
namespace MM
{
    class Allocator final : private Pool
    {
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
            STATIC XTAPI XTSTATUS FreePages(IN PVOID VirtualAddress,
                                            OUT PPFN_NUMBER PagesFreed);
            STATIC XTAPI XTSTATUS FreePool(IN PVOID VirtualAddress);
            STATIC XTAPI XTSTATUS FreePool(IN PVOID VirtualAddress,
                                           IN ULONG Tag);

        private:
            STATIC XTAPI XTSTATUS AllocateNonPagedPoolPages(IN PFN_COUNT Pages,
                                                            OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS AllocatePagedPoolPages(IN PFN_COUNT Pages,
                                                         OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS FreeNonPagedPoolPages(IN PVOID VirtualAddress,
                                                        OUT PPFN_NUMBER PagesFreed);
            STATIC XTAPI XTSTATUS FreePagedPoolPages(IN PVOID VirtualAddress,
                                                     OUT PPFN_NUMBER PagesFreed);
    };
}

#endif /* __XTOSKRNL_MM_ALLOC_HH */
