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
            STATIC XTAPI VOID ClearPte(PHARDWARE_PTE PtePointer);
            STATIC XTAPI VOID FlushTlb(VOID);
            STATIC XTAPI PMMPDE GetPdeAddress(PVOID Address);
            STATIC XTAPI PMMPPE GetPpeAddress(PVOID Address);
            STATIC XTAPI PMMPTE GetPteAddress(PVOID Address);
            STATIC XTAPI VOID InitializePageMapSupport(VOID);
            STATIC XTAPI BOOLEAN PteValid(PHARDWARE_PTE PtePointer);
            STATIC XTAPI VOID SetPte(PHARDWARE_PTE PtePointer,
                                     PFN_NUMBER PageFrameNumber,
                                     BOOLEAN Writable);
            STATIC XTAPI VOID SetPteCaching(PHARDWARE_PTE PtePointer,
                                            BOOLEAN CacheDisable,
                                            BOOLEAN WriteThrough);
            STATIC XTFASTCALL VOID ZeroPages(IN PVOID Address,
                                             IN ULONG Size);

        private:
            STATIC XTAPI BOOLEAN GetExtendedPhysicalAddressingStatus(VOID);
            STATIC XTAPI PPAGEMAP GetPageMapBasicRoutines(VOID);
            STATIC XTAPI PPAGEMAP GetPageMapXpaRoutines(VOID);
    };
}

#endif /* __XTOSKRNL_MM_PAGING_HH */
