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
            STATIC PFN_NUMBER AvailablePages;
            STATIC PLOADER_MEMORY_DESCRIPTOR FreeDescriptor;
            STATIC MMPFNLIST FreePagesList;
            STATIC ULONG_PTR HighestPhysicalPage;
            STATIC ULONG_PTR LowestPhysicalPage;
            STATIC ULONGLONG NumberOfPhysicalPages;
            STATIC LOADER_MEMORY_DESCRIPTOR OriginalFreeDescriptor;
            STATIC PFN_NUMBER PfnDatabaseSize;

        public:
            STATIC XTAPI PFN_NUMBER AllocateBootstrapPages(IN PFN_NUMBER NumberOfPages);
            STATIC XTAPI VOID ComputePfnDatabaseSize(VOID);
            STATIC XTAPI ULONG_PTR GetHighestPhysicalPage(VOID);
            STATIC XTAPI ULONGLONG GetNumberOfPhysicalPages(VOID);
            STATIC XTAPI PFN_NUMBER GetPfnDatabaseSize(VOID);
            STATIC XTAPI PMMPFN GetPfnEntry(IN PFN_NUMBER Pfn);
            STATIC XTAPI VOID InitializePfnDatabase(VOID);
            STATIC XTAPI VOID ScanMemoryDescriptors(VOID);

        private:
            STATIC XTAPI VOID DecrementAvailablePages(VOID);
            STATIC XTAPI VOID IncrementAvailablePages(VOID);
            STATIC XTAPI VOID InitializePageTablePfns(VOID);
            STATIC XTAPI VOID LinkFreePage(IN PFN_NUMBER PageFrameIndex);
            STATIC XTAPI VOID LinkPfnForPageTable(PFN_NUMBER PageFrameIndex,
                                                  PMMPTE PointerPte);
            STATIC XTAPI VOID ProcessMemoryDescriptor(PFN_NUMBER BasePage,
                                                      PFN_NUMBER PageCount,
                                                      LOADER_MEMORY_TYPE MemoryType);
            STATIC XTAPI VOID ScanPageTable(IN PMMPTE PointerPte,
                                            ULONG Level);
    };
}

#endif /* __XTOSKRNL_MM_PFN_HH */
