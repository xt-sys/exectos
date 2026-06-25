/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ex/laslist.hh
 * DESCRIPTION:     Lookaside Lists support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_EX_LASLIST_HH
#define __XTOSKRNL_EX_LASLIST_HH

#include <xtos.hh>


/* Kernel Executive */
namespace EX
{
    class LookasideList
    {
        private:
            STATIC LIST_ENTRY NonPagedLookasideListHead;
            STATIC KSPIN_LOCK NonPagedLookasideListLock;
            STATIC GENERAL_LOOKASIDE NonPagedPoolLookasideLists[POOL_LOOKASIDE_LISTS];
            STATIC LIST_ENTRY PagedLookasideListHead;
            STATIC KSPIN_LOCK PagedLookasideListLock;
            STATIC GENERAL_LOOKASIDE PagedPoolLookasideLists[POOL_LOOKASIDE_LISTS];
            STATIC LIST_ENTRY PoolLookasideListHead;
            STATIC LIST_ENTRY SystemLookasideListHead;

        public:
            STATIC XTAPI PVOID AllocateFromLookasideList(IN PNONPAGED_LOOKASIDE_LIST LookasideList);
            STATIC XTAPI PVOID AllocateFromLookasideList(IN PPAGED_LOOKASIDE_LIST LookasideList);
            STATIC XTAPI VOID InitializeLookasideList(IN OUT PGENERAL_LOOKASIDE LookasideList,
                                                      IN MMPOOL_TYPE PoolType,
                                                      IN ULONG Size,
                                                      IN ULONG Tag,
                                                      IN USHORT MaxDepth,
                                                      IN PLIST_ENTRY ListHead);
            STATIC XTAPI VOID InitializePointers(VOID);
            STATIC XTAPI VOID InitializeSystemLookasideLists(VOID);
    };
}

#endif /* __XTOSKRNL_EX_LASLIST_HH */
