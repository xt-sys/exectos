/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/amd64/pte.hh
 * DESCRIPTION:     Page table entry (PTE) for AMD64 support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_AMD64_PTE_HH
#define __XTOSKRNL_MM_AMD64_PTE_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Pte
    {
        private:
            STATIC MMPTE FirstSystemFreePte[MaximumPtePoolTypes];
            STATIC PMMPTE SystemPteBase;
            STATIC PMMPTE SystemPtesEnd[MaximumPtePoolTypes];
            STATIC PMMPTE SystemPtesStart[MaximumPtePoolTypes];
            STATIC ULONG TotalSystemFreePtes[MaximumPtePoolTypes];
            STATIC MMPTE ValidPte;

        public:
            STATIC XTAPI BOOLEAN AddressValid(IN PVOID VirtualAddress);
            STATIC XTAPI ULONG GetPtesPerPage(VOID);
            STATIC XTAPI PMMPTE GetSystemPteBaseAddress(VOID);
            STATIC XTAPI VOID InitializePageTable(VOID);
            STATIC XTAPI VOID InitializeSystemPteSpace(VOID);
            STATIC XTAPI VOID MapP5E(PVOID StartAddress,
                                     PVOID EndAddress,
                                     PMMP5E TemplateP5e);
            STATIC XTAPI VOID MapPDE(PVOID StartAddress,
                                     PVOID EndAddress,
                                     PMMPDE TemplatePde);
            STATIC XTAPI VOID MapPPE(PVOID StartAddress,
                                     PVOID EndAddress,
                                     PMMPPE TemplatePpe);
            STATIC XTAPI VOID MapPTE(PVOID StartAddress,
                                     PVOID EndAddress,
                                     PMMPTE TemplatePte);
            STATIC XTAPI VOID MapPXE(PVOID StartAddress,
                                     PVOID EndAddress,
                                     PMMPXE TemplatePxe);
            STATIC XTAPI PMMPTE ReserveSystemPtes(IN ULONG NumberOfPtes,
                                                  IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType,
                                                  IN ULONG Alignment);

        private:
            STATIC XTAPI BOOLEAN FindFreeCluster(IN ULONG NumberOfPtes,
                                                 IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType,
                                                 OUT PMMPTE *FoundCluster,
                                                 OUT PMMPTE *PreviousClusterNode);
            STATIC XTAPI ULONG GetClusterSize(IN PMMPTE Pte);
            STATIC XTAPI VOID InitializeSystemPtePool(IN PMMPTE StartingPte,
                                                      IN ULONG NumberOfPtes,
                                                      IN MMSYSTEM_PTE_POOL_TYPE PoolType);
    };
}

#endif /* __XTOSKRNL_MM_AMD64_PTE_HH */
