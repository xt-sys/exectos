/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/mmgr.hh
 * DESCRIPTION:     Memory Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_MMGR_HH
#define __XTOSKRNL_MM_MMGR_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Manager
    {
        public:
            STATIC XTAPI VOID InitializeMemoryManager(VOID);
            STATIC XTAPI BOOLEAN VerifyMemoryTypeFree(LOADER_MEMORY_TYPE MemoryType);
            STATIC XTAPI BOOLEAN VerifyMemoryTypeInvisible(LOADER_MEMORY_TYPE MemoryType);
       };
}

#endif /* __XTOSKRNL_MM_MMGR_HH */
