/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/amd64/pagemap.hh
 * DESCRIPTION:     Low-level support for page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_AMD64_PAGEMAP_HH
#define __XTOSKRNL_MM_AMD64_PAGEMAP_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    typedef class PageMap
    {
        protected:
            MMPAGEMAP_INFO PageMapInfo;

        public:
            XTAPI PMMPTE AdvancePte(IN PMMPTE Pte,
                                    IN LONG Count);
            XTAPI VOID ClearPte(IN PMMPTE PtePointer);
            XTAPI ULONGLONG GetEmptyPteList(VOID);
            XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            XTAPI PMMP5E GetP5eAddress(IN PVOID Address);
            XTAPI ULONG GetP5eIndex(IN PVOID Address);
            XTAPI PVOID GetP5eVirtualAddress(IN PMMP5E P5ePointer);
            XTAPI PFN_NUMBER GetPageFrameNumber(IN PMMPTE Pte);
            XTAPI PMMPDE GetPdeAddress(IN PVOID Address);
            XTAPI ULONG GetPdeIndex(IN PVOID Address);
            VIRTUAL XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer) = 0;
            XTAPI PMMPPE GetPpeAddress(IN PVOID Address);
            XTAPI ULONG GetPpeIndex(IN PVOID Address);
            VIRTUAL XTAPI PVOID GetPpeVirtualAddress(IN PMMPPE PpePointer) = 0;
            XTAPI PMMPTE GetPteAddress(IN PVOID Address);
            XTAPI LONG GetPteDistance(PMMPTE EndPte,
                                      PMMPTE StartPte);
            XTAPI ULONG GetPteIndex(IN PVOID Address);
            XTAPI ULONG GetPteSize(VOID);
            VIRTUAL XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer) = 0;
            XTAPI PMMPXE GetPxeAddress(IN PVOID Address);
            XTAPI ULONG GetPxeIndex(IN PVOID Address);
            VIRTUAL XTAPI PVOID GetPxeVirtualAddress(IN PMMPXE PxePointer) = 0;
            XTAPI BOOLEAN GetXpaStatus();
            VIRTUAL XTAPI VOID InitializePageMapInfo(VOID) = 0;
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
    } PAGEMAP, *PPAGEMAP;

    class PageMapBasic final : public PageMap
    {
        public:
            XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer);
            XTAPI PVOID GetPpeVirtualAddress(IN PMMPPE PpePointer);
            XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer);
            XTAPI PVOID GetPxeVirtualAddress(IN PMMPXE PxePointer);
            XTAPI VOID InitializePageMapInfo(VOID);
    };

    class PageMapXpa final : public PageMap
    {
        public:
            XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer);
            XTAPI PVOID GetPpeVirtualAddress(IN PMMPPE PpePointer);
            XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer);
            XTAPI PVOID GetPxeVirtualAddress(IN PMMPXE PxePointer);
            XTAPI VOID InitializePageMapInfo(VOID);
    };
}

#endif /* __XTOSKRNL_MM_AMD64_PAGEMAP_HH */
