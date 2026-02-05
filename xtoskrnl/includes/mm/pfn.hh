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
            STATIC MMPFNLIST BadPagesList;
            STATIC PLOADER_MEMORY_DESCRIPTOR FreeDescriptor;
            STATIC MMPFNLIST FreePagesList;
            STATIC ULONG_PTR HighestPhysicalPage;
            STATIC PVOID HighestUserAddress;
            STATIC ULONG_PTR LowestPhysicalPage;
            STATIC MMPFNLIST ModifiedPagesList;
            STATIC MMPFNLIST ModifiedReadOnlyPagesList;
            STATIC ULONGLONG NumberOfPhysicalPages;
            STATIC LOADER_MEMORY_DESCRIPTOR OriginalFreeDescriptor;
            STATIC PMMPFNLIST PageLocationList[];
            STATIC MMPFNLIST RomPagesList;
            STATIC MMPFNLIST StandbyPagesList;
            STATIC MMPFNLIST ZeroedPagesList;

        public:
            STATIC XTAPI PFN_NUMBER AllocateBootstrapPages(IN PFN_NUMBER NumberOfPages);
            STATIC XTAPI PFN_NUMBER AllocatePhysicalPage(IN ULONG Color);
            STATIC XTAPI VOID ComputePfnDatabaseSize(OUT PPFN_NUMBER DatabaseSize);
            STATIC XTAPI VOID DecrementReferenceCount(IN PMMPFN Pfn1,
                                                      IN PFN_NUMBER PageFrameIndex,
                                                      IN BOOLEAN BeginStandbyList = FALSE);
            STATIC XTAPI VOID DecrementShareCount(IN PMMPFN Pfn1,
                                                  IN PFN_NUMBER PageFrameIndex,
                                                  IN BOOLEAN BeginStandbyList = FALSE);
            STATIC XTAPI VOID FreePhysicalPage(IN PMMPTE PointerPte);
            STATIC XTAPI ULONG_PTR GetHighestPhysicalPage(VOID);
            STATIC XTAPI ULONGLONG GetNumberOfPhysicalPages(VOID);
            STATIC XTAPI PMMPFN GetPfnEntry(IN PFN_NUMBER Pfn);
            STATIC XTAPI VOID InitializePfnDatabase(VOID);
            STATIC VOID XTAPI LinkPfnToPte(IN PFN_NUMBER PageFrameIndex,
                                           IN PMMPTE PointerPte,
                                           IN BOOLEAN Modified);
            STATIC XTAPI VOID ScanMemoryDescriptors(VOID);

        private:
            STATIC XTAPI VOID DecrementAvailablePages(VOID);
            STATIC XTAPI VOID IncrementAvailablePages(VOID);
            STATIC XTAPI VOID InitializePageDirectory(IN PMMPDE StartingPde,
                                                      IN PMMPDE EndingPde);
            STATIC XTAPI VOID InitializePageTablePfns(VOID);
            STATIC XTAPI VOID LinkFreePage(IN PFN_NUMBER PageFrameIndex);
            STATIC XTAPI VOID LinkPage(IN PMMPFNLIST ListHead,
                                       IN PFN_NUMBER PageFrameIndex);
            STATIC XTAPI VOID LinkPfnForPageTable(IN PFN_NUMBER PageFrameIndex,
                                                  IN PMMPTE PointerPte);
            STATIC XTFASTCALL VOID LinkStandbyPage(IN PFN_NUMBER PageFrameIndex);
            STATIC XTAPI VOID ProcessMemoryDescriptor(IN PFN_NUMBER BasePage,
                                                      IN PFN_NUMBER PageCount,
                                                      IN LOADER_MEMORY_TYPE MemoryType);
            STATIC XTAPI VOID ScanPageTable(IN PMMPTE PointerPte,
                                            IN ULONG Level);
            STATIC XTAPI PFN_NUMBER UnlinkFreePage(IN PFN_NUMBER PageFrameIndex,
                                                   IN ULONG Color);
    };
}

#endif /* __XTOSKRNL_MM_PFN_HH */
