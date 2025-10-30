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
            VIRTUAL XTAPI PMMPTE AdvancePte(PMMPTE Pte,
                                            ULONG Count) = 0;
            XTAPI VOID ClearPte(PHARDWARE_PTE PtePointer);
            XTAPI ULONG GetEmptyPteList(VOID);
            VIRTUAL XTAPI ULONG_PTR GetNextEntry(PMMPTE Pte) = 0;
            VIRTUAL XTAPI PMMPTE GetNextPte(PMMPTE Pte) = 0;
            VIRTUAL XTAPI BOOLEAN GetOneEntry(PMMPTE Pte) = 0;
            XTAPI PMMPDE GetPdeAddress(PVOID Address);
            XTAPI ULONG GetPdeIndex(PVOID Address);
            XTAPI PVOID GetPdeVirtualAddress(PMMPDE PdePointer);
            XTAPI PMMPPE GetPpeAddress(PVOID Address);
            XTAPI ULONG GetPpeIndex(PVOID Address);
            XTAPI PVOID GetPpeVirtualAddress(PMMPPE PpePointer);
            XTAPI PMMPTE GetPteAddress(PVOID Address);
            XTAPI ULONG GetPteIndex(PVOID Address);
            VIRTUAL XTAPI ULONG GetPteSize(VOID) = 0;
            XTAPI PVOID GetPteVirtualAddress(PMMPTE PtePointer);
            VIRTUAL XTAPI VOID InitializePageMapInfo(VOID) = 0;
            VIRTUAL XTAPI BOOLEAN PteValid(PHARDWARE_PTE PtePointer) = 0;
            VIRTUAL XTAPI VOID SetNextEntry(PMMPTE Pte,
                                            ULONG_PTR Value) = 0;
            VIRTUAL XTAPI VOID SetOneEntry(PMMPTE Pte,
                                           BOOLEAN Value) = 0;
            VIRTUAL XTAPI VOID SetPte(PHARDWARE_PTE PtePointer,
                                      PFN_NUMBER PageFrameNumber,
                                      BOOLEAN Writable) = 0;
            VIRTUAL XTAPI VOID SetPteCaching(PHARDWARE_PTE PtePointer,
                                             BOOLEAN CacheDisable,
                                             BOOLEAN WriteThrough) = 0;
    } PAGEMAP, *PPAGEMAP;

    class PageMapBasic final : public PageMap
    {
        public:
            XTAPI PMMPTE AdvancePte(PMMPTE Pte,
                                    ULONG Count);
            XTAPI ULONG_PTR GetNextEntry(PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(PMMPTE Pte);
            XTAPI ULONG GetPteSize(VOID);
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(PHARDWARE_PTE PtePointer);
            XTAPI VOID SetNextEntry(PMMPTE Pte,
                                    ULONG_PTR Value);
            XTAPI VOID SetOneEntry(PMMPTE Pte,
                                   BOOLEAN Value);
            XTAPI VOID SetPte(PHARDWARE_PTE PtePointer,
                              PFN_NUMBER PageFrameNumber,
                              BOOLEAN Writable);
            XTAPI VOID SetPteCaching(PHARDWARE_PTE PtePointer,
                                     BOOLEAN CacheDisable,
                                     BOOLEAN WriteThrough);
    };

    class PageMapXpa final : public PageMap
    {
        public:
            XTAPI PMMPTE AdvancePte(PMMPTE Pte,
                                    ULONG Count);
            XTAPI ULONG_PTR GetNextEntry(PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(PMMPTE Pte);
            XTAPI ULONG GetPteSize(VOID);
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(PHARDWARE_PTE PtePointer);
            XTAPI VOID SetNextEntry(PMMPTE Pte,
                                    ULONG_PTR Value);
            XTAPI VOID SetOneEntry(PMMPTE Pte,
                                   BOOLEAN Value);
            XTAPI VOID SetPte(PHARDWARE_PTE PtePointer,
                              PFN_NUMBER PageFrameNumber,
                              BOOLEAN Writable);
            XTAPI VOID SetPteCaching(PHARDWARE_PTE PtePointer,
                                     BOOLEAN CacheDisable,
                                     BOOLEAN WriteThrough);
    };
}

#endif /* __XTOSKRNL_MM_PAGEMAP_HH */
