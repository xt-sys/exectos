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
            STATIC LOADER_MEMORY_DESCRIPTOR OriginalFreeDescriptor;
            STATIC PFN_NUMBER PfnDatabaseSize;

        public:
            STATIC XTAPI PFN_NUMBER AllocateBootstrapPages(IN PFN_NUMBER NumberOfPages);
            STATIC XTAPI VOID ComputePfnDatabaseSize(VOID);
            STATIC XTAPI ULONG GetNumberOfPhysicalPages(VOID);
            STATIC XTAPI PFN_NUMBER GetPfnDatabaseSize(VOID);
            STATIC XTAPI VOID ScanMemoryDescriptors(VOID);
    };
}

#endif /* __XTOSKRNL_MM_PFN_HH */
