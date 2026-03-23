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
        private:
            STATIC PPOOL_TRACKING_TABLE AllocationsTrackingTable;
            STATIC KSPIN_LOCK AllocationsTrackingTableLock;
            STATIC SIZE_T AllocationsTrackingTableMask;
            STATIC SIZE_T AllocationsTrackingTableSize;
            STATIC ULONG BigAllocationsInUse;
            STATIC PPOOL_TRACKING_BIG_ALLOCATIONS BigAllocationsTrackingTable;
            STATIC SIZE_T BigAllocationsTrackingTableHash;
            STATIC KSPIN_LOCK BigAllocationsTrackingTableLock;
            STATIC SIZE_T BigAllocationsTrackingTableSize;
            STATIC PPOOL_TRACKING_TABLE TagTables[MM_POOL_TRACKING_TABLES];

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
            STATIC XTAPI VOID InitializeAllocationsTracking(VOID);
            STATIC XTAPI VOID InitializeBigAllocationsTracking(VOID);

        private:
            STATIC XTAPI XTSTATUS AllocateNonPagedPoolPages(IN PFN_COUNT Pages,
                                                            OUT PVOID *Memory);
            STATIC XTAPI XTSTATUS AllocatePagedPoolPages(IN PFN_COUNT Pages,
                                                         OUT PVOID *Memory);
            STATIC XTINLINE ULONG ComputeHash(IN PVOID VirtualAddress);
            STATIC XTINLINE ULONG ComputeHash(IN ULONG Tag,
                                              IN ULONG TableMask);
            STATIC XTAPI BOOLEAN ExpandBigAllocationsTable(VOID);
            STATIC XTAPI XTSTATUS FreeNonPagedPoolPages(IN PVOID VirtualAddress,
                                                        OUT PPFN_NUMBER PagesFreed);
            STATIC XTAPI XTSTATUS FreePagedPoolPages(IN PVOID VirtualAddress,
                                                     OUT PPFN_NUMBER PagesFreed);
            STATIC XTAPI VOID RegisterAllocationTag(IN ULONG Tag,
                                                    IN SIZE_T Bytes,
                                                    IN MMPOOL_TYPE PoolType);
            STATIC XTAPI BOOLEAN RegisterBigAllocationTag(IN PVOID VirtualAddress,
                                                          IN ULONG Tag,
                                                          IN ULONG Pages,
                                                          IN MMPOOL_TYPE PoolType);
            STATIC XTAPI VOID UnregisterAllocationTag(IN ULONG Tag,
                                                      IN SIZE_T Bytes,
                                                      IN MMPOOL_TYPE PoolType);
            STATIC XTAPI ULONG UnregisterBigAllocationTag(IN PVOID VirtualAddress,
                                                          OUT PULONG_PTR Pages,
                                                          IN MMPOOL_TYPE PoolType);
    };
}

#endif /* __XTOSKRNL_MM_ALLOC_HH */
