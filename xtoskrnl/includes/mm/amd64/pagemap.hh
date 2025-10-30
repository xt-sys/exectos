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
            XTAPI PMMPTE AdvancePte(PMMPTE Pte,
                                    ULONG Count);
            XTAPI VOID ClearPte(PHARDWARE_PTE PtePointer);
            XTAPI ULONGLONG GetEmptyPteList(VOID);
            XTAPI ULONG_PTR GetNextEntry(PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(PMMPTE Pte);
            XTAPI PMMP5E GetP5eAddress(PVOID Address);
            XTAPI ULONG GetP5eIndex(PVOID Address);
            XTAPI PVOID GetP5eVirtualAddress(PMMP5E P5ePointer);
            XTAPI PMMPDE GetPdeAddress(PVOID Address);
            XTAPI ULONG GetPdeIndex(PVOID Address);
            XTAPI PVOID GetPdeVirtualAddress(PMMPDE PdePointer);
            XTAPI PMMPPE GetPpeAddress(PVOID Address);
            XTAPI ULONG GetPpeIndex(PVOID Address);
            XTAPI PVOID GetPpeVirtualAddress(PMMPPE PpePointer);
            XTAPI PMMPTE GetPteAddress(PVOID Address);
            XTAPI ULONG GetPteIndex(PVOID Address);
            XTAPI ULONG GetPteSize(VOID);
            XTAPI PVOID GetPteVirtualAddress(PMMPTE PtePointer);
            XTAPI PMMPXE GetPxeAddress(PVOID Address);
            XTAPI ULONG GetPxeIndex(PVOID Address);
            XTAPI PVOID GetPxeVirtualAddress(PMMPXE PxePointer);
            VIRTUAL XTAPI VOID InitializePageMapInfo(VOID) = 0;
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
    } PAGEMAP, *PPAGEMAP;

    class PageMapBasic final : public PageMap
    {
        public:
            XTAPI VOID InitializePageMapInfo(VOID);
    };

    class PageMapXpa final : public PageMap
    {
        public:
            XTAPI VOID InitializePageMapInfo(VOID);
    };
}

#endif /* __XTOSKRNL_MM_PAGEMAP_HH */
