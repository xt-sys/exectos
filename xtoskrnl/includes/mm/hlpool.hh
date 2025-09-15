/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/hlpool.hh
 * DESCRIPTION:     Hardware layer pool memory management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_HLPOOL_HH
#define __XTOSKRNL_MM_HLPOOL_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class HardwarePool
    {
        public:
            STATIC XTAPI XTSTATUS AllocateHardwareMemory(IN PFN_NUMBER PageCount,
                                                         IN BOOLEAN Aligned,
                                                         OUT PPHYSICAL_ADDRESS Buffer);
            STATIC XTAPI XTSTATUS MapHardwareMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                                    IN PFN_NUMBER PageCount,
                                                    IN BOOLEAN FlushTlb,
                                                    OUT PVOID *VirtualAddress);
            STATIC XTAPI VOID MarkHardwareMemoryWriteThrough(IN PVOID VirtualAddress,
                                                             IN PFN_NUMBER PageCount);
            STATIC XTAPI VOID RemapHardwareMemory(IN PVOID VirtualAddress,
                                                  IN PHYSICAL_ADDRESS PhysicalAddress,
                                                  IN BOOLEAN FlushTlb);
            STATIC XTAPI XTSTATUS UnmapHardwareMemory(IN PVOID VirtualAddress,
                                                      IN PFN_NUMBER PageCount,
                                                      IN BOOLEAN FlushTlb);
    };
}

#endif /* __XTOSKRNL_MM_HLPOOL_HH */
