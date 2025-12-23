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
            STATIC MMPTE ValidPte;

        public:
            STATIC XTAPI BOOLEAN AddressValid(IN PVOID VirtualAddress);
            STATIC XTAPI ULONG GetPtesPerPage(VOID);
            STATIC XTAPI PMMPTE GetSystemPteBaseAddress(VOID);
            STATIC XTAPI VOID InitializePageTable(VOID);
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
    };
}

#endif /* __XTOSKRNL_MM_AMD64_PTE_HH */
