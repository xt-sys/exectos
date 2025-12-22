/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/i686/paging.hh
 * DESCRIPTION:     Low level page management support for i686
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_I686_PAGING_HH
#define __XTOSKRNL_MM_I686_PAGING_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Paging
    {
        private:
            STATIC PPAGEMAP PmlRoutines;

        public:
            STATIC XTAPI PMMPTE AdvancePte(IN PMMPTE Pte,
                                           IN LONG Count);
            STATIC XTAPI VOID ClearPte(IN PMMPTE PtePointer);
            STATIC XTAPI VOID FlushTlb(VOID);
            STATIC XTAPI ULONG_PTR GetEmptyPteList(VOID);
            STATIC XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            STATIC XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            STATIC XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            STATIC XTAPI PFN_NUMBER GetPageFrameNumber(IN PMMPTE Pte);
            STATIC XTAPI PMMPDE GetPdeAddress(IN PVOID Address);
            STATIC XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer);
            STATIC XTAPI PMMPPE GetPpeAddress(IN PVOID Address);
            STATIC XTAPI PVOID GetPpeVirtualAddress(IN PMMPPE PpePointer);
            STATIC XTAPI ULONG_PTR GetPte(IN PMMPTE PtePointer);
            STATIC XTAPI PMMPTE GetPteAddress(IN PVOID Address);
            STATIC XTAPI LONG GetPteDistance(PMMPTE EndPte,
                                             PMMPTE StartPte);
            STATIC XTAPI ULONG GetPteSize(VOID);
            STATIC XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer);
            STATIC XTAPI BOOLEAN GetXpaStatus(VOID);
            STATIC XTAPI VOID InitializePageMapSupport(VOID);
            STATIC XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer);
            STATIC XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                           IN ULONG_PTR Value);
            STATIC XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                          IN BOOLEAN Value);
            STATIC XTAPI VOID SetPte(IN PMMPTE PtePointer,
                                     IN PFN_NUMBER PageFrameNumber,
                                     IN ULONG_PTR AttributesMask);
            STATIC XTAPI VOID SetPte(IN PMMPTE PtePointer,
                                     IN ULONG_PTR Attributes);
            STATIC XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                            IN BOOLEAN CacheDisable,
                                            IN BOOLEAN WriteThrough);
            STATIC XTAPI VOID WritePte(IN PMMPTE Pte,
                                       IN MMPTE Value);
            STATIC XTFASTCALL VOID ZeroPages(IN PVOID Address,
                                             IN ULONG Size);

        private:
            STATIC XTAPI BOOLEAN GetExtendedPhysicalAddressingStatus(VOID);
            STATIC XTAPI PPAGEMAP GetPageMapBasicRoutines(VOID);
            STATIC XTAPI PPAGEMAP GetPageMapXpaRoutines(VOID);
    };
}

#endif /* __XTOSKRNL_MM_I686_PAGING_HH */
