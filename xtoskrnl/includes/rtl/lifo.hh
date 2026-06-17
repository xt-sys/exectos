/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/lifo.hh
 * DESCRIPTION:     Last In First Out (LIFO) queue implementation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_LIFO_HH
#define __XTOSKRNL_RTL_LIFO_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class LifoQueue
    {
        public:
            STATIC XTFASTCALL BOOLEAN EmptyList(IN PSINGLE_LIST_ENTRY ListHead);
            STATIC XTFASTCALL PSINGLE_LIST_ENTRY PeekEntryList(IN PSINGLE_LIST_ENTRY ListHead);
            STATIC XTFASTCALL PSINGLE_LIST_ENTRY PopEntryList(IN OUT PSINGLE_LIST_ENTRY ListHead);
            STATIC XTFASTCALL VOID PushEntryList(IN OUT PSINGLE_LIST_ENTRY ListHead,
                                                 IN PSINGLE_LIST_ENTRY Entry);
            STATIC XTFASTCALL BOOLEAN SearchEntryList(IN PSINGLE_LIST_ENTRY ListHead,
                                                      IN PSINGLE_LIST_ENTRY Entry);
    };
}

#endif /* __XTOSKRNL_RTL_LIFO_HH */
