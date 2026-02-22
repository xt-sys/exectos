/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/slist.hh
 * DESCRIPTION:     Singly linked list manipulation routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_RTL_SLIST_HH
#define __XTOSKRNL_RTL_SLIST_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class SinglyList
    {
        public:
            STATIC XTCDECL PSINGLE_LIST_ENTRY GetFirstEntry(IN PSINGLE_LIST_HEADER ListHead);
            STATIC XTCDECL VOID InitializeListHead(IN PSINGLE_LIST_HEADER ListHead);
            STATIC XTCDECL VOID InsertHeadList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                               IN PSINGLE_LIST_ENTRY Entry);
            STATIC XTCDECL VOID InsertTailList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                               IN PSINGLE_LIST_ENTRY Entry);
            STATIC XTCDECL BOOLEAN ListEmpty(IN PSINGLE_LIST_HEADER ListHead);
            STATIC XTAPI USHORT QueryListDepth(IN PSINGLE_LIST_HEADER ListHead);
            STATIC XTCDECL VOID RemoveEntryList(IN PSINGLE_LIST_HEADER ListHead,
                                                IN PSINGLE_LIST_ENTRY Entry);
            STATIC XTCDECL VOID SpliceHeadList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                               IN OUT PSINGLE_LIST_HEADER SpliceList);
            STATIC XTCDECL VOID SpliceTailList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                               IN OUT PSINGLE_LIST_HEADER SpliceList);
    };
}

#endif /* __XTOSKRNL_RTL_SLIST_HH */
