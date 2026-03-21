/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/pool.hh
 * DESCRIPTION:     Memory Manager pool manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_POOL_HH
#define __XTOSKRNL_MM_POOL_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Pool
    {
        protected:
            STATIC POOL_DESCRIPTOR NonPagedPoolDescriptor;
            STATIC PFN_NUMBER NonPagedPoolFrameEnd;
            STATIC PFN_NUMBER NonPagedPoolFrameStart;
            STATIC LIST_ENTRY NonPagedPoolFreeList[MM_MAX_FREE_PAGE_LIST_HEADS];
            STATIC ULONG PoolSecureCookie;
            STATIC PPOOL_DESCRIPTOR PoolVector[2];

        public:
            STATIC XTAPI MMPOOL_TYPE DeterminePoolType(IN PVOID VirtualAddress);
            STATIC XTAPI VOID InitializeNonPagedPool(VOID);
            STATIC XTAPI VOID InitializePagedPool(VOID);
            STATIC XTAPI VOID InitializePoolSecurity(VOID);

        protected:
            STATIC XTAPI PLIST_ENTRY DecodePoolLink(IN PLIST_ENTRY PoolLink);
            STATIC XTAPI PLIST_ENTRY EncodePoolLink(IN PLIST_ENTRY PoolLink);
            STATIC XTAPI PPOOL_HEADER GetPoolBlock(IN PPOOL_HEADER Header, IN SSIZE_T Index);
            STATIC XTAPI PPOOL_HEADER GetPoolEntry(IN PVOID Payload);
            STATIC XTAPI PLIST_ENTRY GetPoolFreeBlock(IN PPOOL_HEADER Header);
            STATIC XTAPI PPOOL_HEADER GetPoolNextBlock(IN PPOOL_HEADER Header);
            STATIC XTAPI PPOOL_HEADER GetPoolPreviousBlock(IN PPOOL_HEADER Header);
            STATIC XTAPI VOID InsertPoolHeadList(IN PLIST_ENTRY ListHead,
                                                 IN PLIST_ENTRY Entry);
            STATIC XTAPI VOID InsertPoolTailList(IN PLIST_ENTRY ListHead,
                                                 IN PLIST_ENTRY Entry);
            STATIC XTAPI BOOLEAN PoolListEmpty(IN PLIST_ENTRY ListHead);
            STATIC XTAPI VOID RemovePoolEntryList(IN PLIST_ENTRY Entry);
            STATIC XTAPI PLIST_ENTRY RemovePoolHeadList(IN PLIST_ENTRY ListHead);
            STATIC XTAPI PLIST_ENTRY RemovePoolTailList(IN PLIST_ENTRY ListHead);
            STATIC XTAPI VOID VerifyPoolBlocks(IN PVOID Block);
            STATIC XTAPI VOID VerifyPoolHeader(IN PPOOL_HEADER Entry);
            STATIC XTAPI VOID VerifyPoolLinks(IN PLIST_ENTRY ListHead);
            STATIC XTAPI VOID VerifyRunLevel(IN MMPOOL_TYPE PoolType,
                                             IN SIZE_T Bytes,
                                             IN PVOID Entry);

        private:
            STATIC XTAPI VOID InitializePoolDescriptor(IN PPOOL_DESCRIPTOR Descriptor,
                                                       IN MMPOOL_TYPE PoolType,
                                                       IN ULONG Index,
                                                       IN ULONG Threshold,
                                                       IN PVOID LockAddress);
            STATIC XTAPI VOID InitializePoolListHead(IN PLIST_ENTRY ListHead);
            STATIC XTAPI VOID MapNonPagedPool(VOID);
    };
}

#endif /* __XTOSKRNL_MM_POOL_HH */
