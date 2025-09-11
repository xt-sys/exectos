/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/gui.hh
 * DESCRIPTION:     Endian conversion routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_GUID_HH
#define __XTOSKRNL_RTL_GUID_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Guid
    {
        public:
            STATIC XTAPI BOOLEAN CompareGuids(IN PGUID Guid1,
                                              IN PGUID Guid2);
    };
}

#endif /* __XTOSKRNL_RTL_GUID_HH */
