/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/pagemap.hh
 * DESCRIPTION:     Low-level support for page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_PAGEMAP_HH
#define __XTOSKRNL_MM_PAGEMAP_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    typedef class PageMap
    {
        protected:
            MMPAGEMAP_INFO PageMapInfo;

        public:
            VIRTUAL XTAPI PMMPTE AdvancePte(IN PMMPTE Pte,
                                            IN ULONG Count) = 0;
            XTAPI VOID ClearPte(IN PMMPTE PtePointer);
            XTAPI ULONG GetEmptyPteList(VOID);
            VIRTUAL XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte) = 0;
            VIRTUAL XTAPI PMMPTE GetNextPte(IN PMMPTE Pte) = 0;
            VIRTUAL XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte) = 0;
            XTAPI PMMPDE GetPdeAddress(IN PVOID Address);
            XTAPI ULONG GetPdeIndex(IN PVOID Address);
            XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer);
            XTAPI PMMPPE GetPpeAddress(IN PVOID Address);
            XTAPI ULONG GetPpeIndex(IN PVOID Address);
            XTAPI PVOID GetPpeVirtualAddress(IN PMMPPE PpePointer);
            XTAPI PMMPTE GetPteAddress(IN PVOID Address);
            XTAPI ULONG GetPteIndex(IN PVOID Address);
            VIRTUAL XTAPI ULONG GetPteSize(VOID) = 0;
            XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer);
            VIRTUAL XTAPI VOID InitializePageMapInfo(VOID) = 0;
            VIRTUAL XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer) = 0;
            VIRTUAL XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                            IN ULONG_PTR Value) = 0;
            VIRTUAL XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                           IN BOOLEAN Value) = 0;
            VIRTUAL XTAPI VOID SetPte(IN PMMPTE PtePointer,
                                      IN PFN_NUMBER PageFrameNumber,
                                      IN ULONG_PTR AttributesMask) = 0;
            VIRTUAL XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                             IN BOOLEAN CacheDisable,
                                             IN BOOLEAN WriteThrough) = 0;
    } PAGEMAP, *PPAGEMAP;

    class PageMapBasic final : public PageMap
    {
        public:
            XTAPI PMMPTE AdvancePte(IN PMMPTE Pte,
                                    IN ULONG Count);
            XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            XTAPI ULONG GetPteSize(VOID);
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer);
            XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                    IN ULONG_PTR Value);
            XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                   IN BOOLEAN Value);
            XTAPI VOID SetPte(IN PMMPTE PtePointer,
                              IN PFN_NUMBER PageFrameNumber,
                              IN ULONG_PTR AttributesMask);
            XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                     IN BOOLEAN CacheDisable,
                                     IN BOOLEAN WriteThrough);
    };

    class PageMapXpa final : public PageMap
    {
        public:
            XTAPI PMMPTE AdvancePte(IN PMMPTE Pte,
                                    IN ULONG Count);
            XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            XTAPI ULONG GetPteSize(VOID);
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer);
            XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                    IN ULONG_PTR Value);
            XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                   IN BOOLEAN Value);
            XTAPI VOID SetPte(IN PMMPTE PtePointer,
                              IN PFN_NUMBER PageFrameNumber,
                              IN ULONG_PTR AttributesMask);
            XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                     IN BOOLEAN CacheDisable,
                                     IN BOOLEAN WriteThrough);
    };
}

#endif /* __XTOSKRNL_MM_PAGEMAP_HH */
