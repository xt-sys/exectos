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
        private:
            STATIC LOADER_MEMORY_DESCRIPTOR HardwareAllocationDescriptors[MM_HARDWARE_ALLOCATION_DESCRIPTORS];
            STATIC PVOID HardwareHeapStart;
            STATIC PHYSICAL_ADDRESS LowMemoryPhysicalAddress;
            STATIC PVOID LowMemoryVirtualAddress;
            STATIC ULONG UsedHardwareAllocationDescriptors;

        public:
            STATIC XTAPI XTSTATUS AllocateHardwareMemory(IN PFN_NUMBER PageCount,
                                                         IN BOOLEAN Aligned,
                                                         IN ULONGLONG MaximumAddress,
                                                         OUT PPHYSICAL_ADDRESS Buffer);
            STATIC XTAPI XTSTATUS AllocateLowMemory(OUT PPHYSICAL_ADDRESS PhysicalAddress,
                                                    OUT PVOID *VirtualAddress);
            STATIC XTAPI ULONG CalculateRealModeAllocationPages(IN ULONG TrampolineCodeSize);
            STATIC XTAPI XTSTATUS FreeHardwareMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                                     IN PFN_NUMBER PageCount);
            STATIC XTAPI XTSTATUS FreeRealModeMemory(IN PVOID VirtualAddress,
                                                     IN PFN_NUMBER PageCount);
            STATIC XTAPI XTSTATUS MapHardwareMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                                    IN PFN_NUMBER PageCount,
                                                    IN BOOLEAN FlushTlb,
                                                    OUT PVOID *VirtualAddress);
            STATIC XTAPI XTSTATUS MapRealModeMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                                    IN ULONG Size);
            STATIC XTAPI VOID MarkHardwareMemoryWriteThrough(IN PVOID VirtualAddress,
                                                             IN PFN_NUMBER PageCount);
            STATIC XTAPI VOID RemapHardwareMemory(IN PVOID VirtualAddress,
                                                  IN PHYSICAL_ADDRESS PhysicalAddress,
                                                  IN BOOLEAN FlushTlb);
            STATIC XTAPI XTSTATUS UnmapHardwareMemory(IN PVOID VirtualAddress,
                                                      IN PFN_NUMBER PageCount,
                                                      IN BOOLEAN FlushTlb);
            STATIC XTAPI VOID UnmapRealModeMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                                  IN ULONG Size);
    };
}

#endif /* __XTOSKRNL_MM_HLPOOL_HH */
