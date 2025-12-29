/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/pfault.hh
 * DESCRIPTION:     Page fault support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_PFAULT_HH
#define __XTOSKRNL_MM_PFAULT_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class PageFault
    {
        public:
            STATIC XTFASTCALL XTSTATUS CheckPdeForPagedPool(IN PVOID VirtualAddress);
    };
}

#endif /* __XTOSKRNL_MM_PFAULT_HH */
