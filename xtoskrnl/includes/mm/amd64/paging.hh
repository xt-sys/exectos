/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/amd64/paging.hh
 * DESCRIPTION:     Low level page management support for AMD64
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_AMD64_PAGING_HH
#define __XTOSKRNL_MM_AMD64_PAGING_HH

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
            STATIC XTAPI BOOLEAN GetExtendedPhysicalAddressingStatus(VOID);
            STATIC XTAPI ULONG_PTR GetNextEntry(IN PMMPTE Pte);
            STATIC XTAPI PMMPTE GetNextPte(IN PMMPTE Pte);
            STATIC XTAPI BOOLEAN GetOneEntry(IN PMMPTE Pte);
            STATIC XTAPI PMMPDE GetP5eAddress(IN PVOID Address);
            STATIC XTAPI PVOID GetP5eVirtualAddress(IN PMMP5E P5ePointer);
            STATIC XTAPI USHORT GetPageMapLevel();
            STATIC XTAPI PFN_NUMBER GetPageFrameNumber(IN PMMPTE Pte);
            STATIC XTAPI PMMPDE GetPdeAddress(IN PVOID Address);
            STATIC XTAPI PVOID GetPdeVirtualAddress(IN PMMPDE PdePointer);
            STATIC XTAPI PMMPPE GetPpeAddress(IN PVOID Address);
            STATIC XTAPI PVOID GetPpeVirtualAddress(IN PMMPPE PpePointer);
            STATIC XTAPI PMMPTE GetPteAddress(IN PVOID Address);
            STATIC XTAPI ULONG GetPteSize(VOID);
            STATIC XTAPI PVOID GetPteVirtualAddress(IN PMMPTE PtePointer);
            STATIC XTAPI PMMPXE GetPxeAddress(IN PVOID Address);
            STATIC XTAPI PVOID GetPxeVirtualAddress(IN PMMPXE PxePointer);
            STATIC XTAPI VOID InitializePageMapSupport(VOID);
            STATIC XTAPI BOOLEAN PteValid(IN PMMPTE PtePointer);
            STATIC XTAPI VOID SetNextEntry(IN PMMPTE Pte,
                                           IN ULONG_PTR Value);
            STATIC XTAPI VOID SetOneEntry(IN PMMPTE Pte,
                                          IN BOOLEAN Value);
            STATIC XTAPI VOID SetPte(IN PMMPTE PtePointer,
                                     IN PFN_NUMBER PageFrameNumber,
                                     IN ULONG_PTR AttributesMask);
            STATIC XTAPI VOID SetPteCaching(IN PMMPTE PtePointer,
                                            IN BOOLEAN CacheDisable,
                                            IN BOOLEAN WriteThrough);
            STATIC XTFASTCALL VOID ZeroPages(IN PVOID Address,
                                             IN ULONG Size);

        private:
            STATIC XTAPI PPAGEMAP GetPageMapBasicRoutines(VOID);
            STATIC XTAPI PPAGEMAP GetPageMapXpaRoutines(VOID);
    };
}

#endif /* __XTOSKRNL_MM_AMD64_PAGING_HH */
