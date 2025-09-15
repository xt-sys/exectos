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
            XTAPI VOID ClearPte(PHARDWARE_PTE PtePointer);
            XTAPI PMMPDE GetPdeAddress(PVOID Address);
            XTAPI PMMPPE GetPpeAddress(PVOID Address);
            XTAPI PMMPTE GetPteAddress(PVOID Address);
            virtual XTAPI VOID InitializePageMapInfo(VOID) = 0;
            virtual XTAPI BOOLEAN PteValid(PHARDWARE_PTE PtePointer) = 0;
            virtual XTAPI VOID SetPte(PHARDWARE_PTE PtePointer,
                                      PFN_NUMBER PageFrameNumber,
                                      BOOLEAN Writable) = 0;
            virtual XTAPI VOID SetPteCaching(PHARDWARE_PTE PtePointer,
                                             BOOLEAN CacheDisable,
                                             BOOLEAN WriteThrough) = 0;
    } PAGEMAP, *PPAGEMAP;

    class PageMapBasic final : public PageMap
    {
        public:
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(PHARDWARE_PTE PtePointer);
            XTAPI VOID SetPte(PHARDWARE_PTE PtePointer,
                              PFN_NUMBER PageFrameNumber,
                              BOOLEAN Writable);
            XTAPI VOID SetPteCaching(PHARDWARE_PTE PtePointer,
                                     BOOLEAN CacheDisable,
                                     BOOLEAN WriteThrough);
    };

    class PageMapXpa final : public PageMap
    {
        public:
            XTAPI VOID InitializePageMapInfo(VOID);
            XTAPI BOOLEAN PteValid(PHARDWARE_PTE PtePointer);
            XTAPI VOID SetPte(PHARDWARE_PTE PtePointer,
                              PFN_NUMBER PageFrameNumber,
                              BOOLEAN Writable);
            XTAPI VOID SetPteCaching(PHARDWARE_PTE PtePointer,
                                     BOOLEAN CacheDisable,
                                     BOOLEAN WriteThrough);
    };
}

#endif /* __XTOSKRNL_MM_PAGEMAP_HH */
