/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/amd64/pte.hh
 * DESCRIPTION:     Page Table Entry (PTE) for AMD64 support
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
            STATIC PFN_COUNT TotalSystemFreePtes[MaximumPtePoolTypes];
            STATIC MMPTE ValidPte;

        public:
            STATIC XTAPI BOOLEAN AddressValid(IN PVOID VirtualAddress);
            STATIC XTAPI PFN_COUNT GetPtesPerPage(VOID);
            STATIC XTAPI PMMPTE GetSystemPteBaseAddress(VOID);
            STATIC XTAPI PMMPTE GetValidPte(VOID);
            STATIC XTAPI VOID InitializePageTable(VOID);
            STATIC XTAPI VOID InitializeSystemPte(VOID);
            STATIC XTAPI VOID InitializeSystemPteSpace(VOID);
            STATIC XTAPI VOID MapP5E(IN PVOID StartAddress,
                                     IN PVOID EndAddress,
                                     IN PMMP5E TemplateP5e);
            STATIC XTAPI VOID MapPDE(IN PVOID StartAddress,
                                     IN PVOID EndAddress,
                                     IN PMMPDE TemplatePde);
            STATIC XTAPI VOID MapPPE(IN PVOID StartAddress,
                                     IN PVOID EndAddress,
                                     IN PMMPPE TemplatePpe);
            STATIC XTAPI VOID MapPTE(IN PVOID StartAddress,
                                     IN PVOID EndAddress,
                                     IN PMMPTE TemplatePte);
            STATIC XTAPI VOID MapPXE(IN PVOID StartAddress,
                                     IN PVOID EndAddress,
                                     IN PMMPXE TemplatePxe);
            STATIC XTAPI VOID ReleaseSystemPtes(IN PMMPTE StartingPte,
                                                IN PFN_COUNT NumberOfPtes,
                                                IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType);
            STATIC XTAPI PMMPTE ReserveSystemPtes(IN PFN_COUNT NumberOfPtes,
                                                  IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType);

        private:
            STATIC XTAPI BOOLEAN FindFreeCluster(IN PFN_COUNT NumberOfPtes,
                                                 IN MMSYSTEM_PTE_POOL_TYPE SystemPtePoolType,
                                                 OUT PMMPTE *FoundCluster,
                                                 OUT PMMPTE *PreviousClusterNode);
            STATIC XTAPI ULONG GetClusterSize(IN PMMPTE Pte);
            STATIC XTAPI VOID InitializeSystemPtePool(IN PMMPTE StartingPte,
                                                      IN PFN_COUNT NumberOfPtes,
                                                      IN MMSYSTEM_PTE_POOL_TYPE PoolType);
    };
}

#endif /* __XTOSKRNL_MM_AMD64_PTE_HH */
