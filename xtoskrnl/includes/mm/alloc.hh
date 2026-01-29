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
        public:
            STATIC XTAPI VOID InitializeNonPagedPool(VOID);
    };
}

#endif /* __XTOSKRNL_MM_ALLOC_HH */
