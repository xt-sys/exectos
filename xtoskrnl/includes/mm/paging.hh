/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/paging.hh
 * DESCRIPTION:     Low level page management support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_PAGING_HH
#define __XTOSKRNL_MM_PAGING_HH

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
                                           IN ULONG Count);
            STATIC XTAPI VOID ClearPte(IN PHARDWARE_PTE PtePointer);
            STATIC XTAPI VOID FlushTlb(VOID);
            STATIC XTAPI ULONG_PTR GetEmptyPteList(VOID);
            STATIC XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            STATIC XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            STATIC XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            STATIC XTAPI PMMPDE GetPdeAddress(IN PVOID Address);
            STATIC XTAPI PMMPPE GetPpeAddress(IN PVOID Address);
            STATIC XTAPI PMMPTE GetPteAddress(IN PVOID Address);
            STATIC XTAPI ULONG GetPteSize(VOID);
            STATIC XTAPI VOID InitializePageMapSupport(VOID);
            STATIC XTAPI BOOLEAN PteValid(IN PHARDWARE_PTE PtePointer);
            STATIC XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                           IN ULONG_PTR Value);
            STATIC XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                          IN BOOLEAN Value);
            STATIC XTAPI VOID SetPte(IN PHARDWARE_PTE PtePointer,
                                     IN PFN_NUMBER PageFrameNumber,
                                     IN BOOLEAN Writable);
            STATIC XTAPI VOID SetPteCaching(IN PHARDWARE_PTE PtePointer,
                                            IN BOOLEAN CacheDisable,
                                            IN BOOLEAN WriteThrough);
            STATIC XTFASTCALL VOID ZeroPages(IN PVOID Address,
                                             IN ULONG Size);

        private:
            STATIC XTAPI BOOLEAN GetExtendedPhysicalAddressingStatus(VOID);
            STATIC XTAPI PPAGEMAP GetPageMapBasicRoutines(VOID);
            STATIC XTAPI PPAGEMAP GetPageMapXpaRoutines(VOID);
    };
}

#endif /* __XTOSKRNL_MM_PAGING_HH */
