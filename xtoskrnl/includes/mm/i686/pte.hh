/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/i686/pte.hh
 * DESCRIPTION:     Page table entry (PTE) for i686 support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_I686_PTE_HH
#define __XTOSKRNL_MM_I686_PTE_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Pte
    {
        public:
            STATIC XTAPI ULONG GetPtesPerPage(VOID);
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

#endif /* __XTOSKRNL_MM_I686_PTE_HH */
