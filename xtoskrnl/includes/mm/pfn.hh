/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/pfn.hh
 * DESCRIPTION:     Physical Frame Number (PFN) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_PFN_HH
#define __XTOSKRNL_MM_PFN_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Pfn
    {
        private:
            STATIC PLOADER_MEMORY_DESCRIPTOR FreeDescriptor;
            STATIC ULONG_PTR HighestPhysicalPage;
            STATIC ULONG_PTR LowestPhysicalPage;
            STATIC ULONG NumberOfPhysicalPages;
            STATIC LOADER_MEMORY_DESCRIPTOR OldFreeDescriptor;

        public:
            STATIC XTAPI VOID ScanMemoryDescriptors(VOID);
    };
}

#endif /* __XTOSKRNL_MM_PFN_HH */
