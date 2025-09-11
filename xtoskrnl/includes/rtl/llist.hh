/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/llist.hh
 * DESCRIPTION:     Linked list manipulation routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_LLIST_HH
#define __XTOSKRNL_RTL_LLIST_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class LinkedList
    {
        public:
            STATIC XTCDECL VOID InitializeListHead(IN PLIST_ENTRY ListHead);
            STATIC XTCDECL VOID InitializeListHead32(IN PLIST_ENTRY32 ListHead);
            STATIC XTCDECL VOID InsertHeadList(IN OUT PLIST_ENTRY ListHead,
                                               IN PLIST_ENTRY Entry);
            STATIC XTCDECL VOID InsertTailList(IN OUT PLIST_ENTRY ListHead,
                                               IN PLIST_ENTRY Entry);
            STATIC XTCDECL BOOLEAN ListEmpty(IN PLIST_ENTRY ListHead);
            STATIC XTCDECL BOOLEAN ListLoop(IN PLIST_ENTRY ListHead);
            STATIC XTCDECL VOID RemoveEntryList(IN PLIST_ENTRY Entry);
    };
}

#endif /* __XTOSKRNL_RTL_LLIST_HH */
