/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/init.hh
 * DESCRIPTION:     Memory Manager initialization
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_INIT_HH
#define __XTOSKRNL_MM_INIT_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Init
    {
        public:
            STATIC XTAPI VOID InitializeMemoryManager(VOID);
            STATIC XTAPI VOID InitializePageMapSupport(VOID);
            STATIC XTAPI VOID ScanMemoryDescriptors(VOID);

        private:
            STATIC XTAPI VOID InitializeArchitecture(VOID);
            STATIC XTAPI BOOLEAN VerifyMemoryTypeFree(LOADER_MEMORY_TYPE MemoryType);
            STATIC XTAPI BOOLEAN VerifyMemoryTypeInvisible(LOADER_MEMORY_TYPE MemoryType);
       };
}

#endif /* __XTOSKRNL_MM_INIT_HH */
