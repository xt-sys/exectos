/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/i686/pagemap.hh
 * DESCRIPTION:     Low-level support for page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_I686_PAGEMAP_HH
#define __XTOSKRNL_MM_I686_PAGEMAP_HH

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
            VIRTUAL XTAPI VOID ClearPte(IN PMMPTE PtePointer) = 0;
            XTAPI ULONG GetEmptyPteList(VOID);
            VIRTUAL XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte) = 0;
            VIRTUAL XTAPI PMMPTE GetNextPte(IN PMMPTE Pte) = 0;
            VIRTUAL XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte) = 0;
            VIRTUAL XTAPI PFN_NUMBER GetPageFrameNumber(IN PMMPTE Pte) = 0;
            XTAPI PMMPDE GetPdeAddress(IN PVOID Address);
            XTAPI ULONG GetPdeOffset(IN PVOID Address);
            VIRTUAL XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer) = 0;
            XTAPI PMMPPE GetPpeAddress(IN PVOID Address);
            XTAPI ULONG GetPpeOffset(IN PVOID Address);
            XTAPI PVOID GetPpeVirtualAddress(IN PMMPPE PpePointer);
            VIRTUAL XTAPI ULONG_PTR GetPte(IN PMMPTE PtePointer) = 0;
            XTAPI PMMPTE GetPteAddress(IN PVOID Address);
            XTAPI ULONG GetPteOffset(IN PVOID Address);
            VIRTUAL XTAPI LONG GetPteDistance(PMMPTE EndPte,
                                              PMMPTE StartPte) = 0;
            VIRTUAL XTAPI ULONG GetPteSize(VOID) = 0;
            VIRTUAL XTAPI ULONG GetPteSoftwarePrototype(IN PMMPTE PtePointer) = 0;
            VIRTUAL XTAPI ULONG GetPteSoftwareTransition(IN PMMPTE PtePointer) = 0;
            VIRTUAL XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer) = 0;
            XTAPI BOOLEAN GetXpaStatus();
            VIRTUAL XTAPI VOID InitializePageMapInfo(VOID) = 0;
            VIRTUAL XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer) = 0;
            VIRTUAL XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                            IN ULONG_PTR Value) = 0;
            VIRTUAL XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                           IN BOOLEAN Value) = 0;
            VIRTUAL XTAPI VOID SetPte(IN PMMPTE PtePointer,
                                      IN PFN_NUMBER PageFrameNumber,
                                      IN ULONG_PTR AttributesMask) = 0;
            VIRTUAL XTAPI VOID SetPte(IN PMMPTE PtePointer,
                                      IN ULONG_PTR Attributes) = 0;
            VIRTUAL XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                             IN BOOLEAN CacheDisable,
                                             IN BOOLEAN WriteThrough) = 0;
            VIRTUAL XTAPI VOID WritePte(IN PMMPTE Pte,
                                        IN MMPTE Value) = 0;

    } PAGEMAP, *PPAGEMAP;

    class PageMapBasic final : public PageMap
    {
        public:
            XTAPI PMMPTE AdvancePte(IN PMMPTE Pte,
                                    IN ULONG Count);
            XTAPI VOID ClearPte(IN PMMPTE PtePointer);
            XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            XTAPI PFN_NUMBER GetPageFrameNumber(IN PMMPTE Pte);
            XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer);
            XTAPI ULONG_PTR GetPte(IN PMMPTE PtePointer);
            XTAPI LONG GetPteDistance(PMMPTE EndPte,
                                      PMMPTE StartPte);
            XTAPI ULONG GetPteSize(VOID);
            XTAPI ULONG GetPteSoftwarePrototype(IN PMMPTE PtePointer);
            XTAPI ULONG GetPteSoftwareTransition(IN PMMPTE PtePointer);
            XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer);
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer);
            XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                    IN ULONG_PTR Value);
            XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                   IN BOOLEAN Value);
            XTAPI VOID SetPte(IN PMMPTE PtePointer,
                              IN PFN_NUMBER PageFrameNumber,
                              IN ULONG_PTR AttributesMask);
            XTAPI VOID SetPte(IN PMMPTE PtePointer,
                              IN ULONG_PTR Attributes);
            XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                     IN BOOLEAN CacheDisable,
                                     IN BOOLEAN WriteThrough);
            XTAPI VOID WritePte(IN PMMPTE Pte,
                                IN MMPTE Value);
    };

    class PageMapXpa final : public PageMap
    {
        public:
            XTAPI PMMPTE AdvancePte(IN PMMPTE Pte,
                                    IN ULONG Count);
            XTAPI VOID ClearPte(IN PMMPTE PtePointer);
            XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            XTAPI PFN_NUMBER GetPageFrameNumber(IN PMMPTE Pte);
            XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer);
            XTAPI ULONG_PTR GetPte(IN PMMPTE PtePointer);
            XTAPI LONG GetPteDistance(PMMPTE EndPte,
                                      PMMPTE StartPte);
            XTAPI ULONG GetPteSize(VOID);
            XTAPI ULONG GetPteSoftwarePrototype(IN PMMPTE PtePointer);
            XTAPI ULONG GetPteSoftwareTransition(IN PMMPTE PtePointer);
            XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer);
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer);
            XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                    IN ULONG_PTR Value);
            XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                   IN BOOLEAN Value);
            XTAPI VOID SetPte(IN PMMPTE PtePointer,
                              IN PFN_NUMBER PageFrameNumber,
                              IN ULONG_PTR AttributesMask);
            XTAPI VOID SetPte(IN PMMPTE PtePointer,
                              IN ULONG_PTR Attributes);
            XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                     IN BOOLEAN CacheDisable,
                                     IN BOOLEAN WriteThrough);
            XTAPI VOID WritePte(IN PMMPTE Pte,
                                IN MMPTE Value);
    };
}

#endif /* __XTOSKRNL_MM_I686_PAGEMAP_HH */
