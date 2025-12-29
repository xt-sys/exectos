/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/hlpool.hh
 * DESCRIPTION:     Kernel pool memory management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_KPOOL_HH
#define __XTOSKRNL_MM_KPOOL_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class KernelPool
    {
        private:
            STATIC UCHAR ProcessorStructuresData[MAXIMUM_PROCESSORS][KPROCESSOR_STRUCTURES_SIZE];

        public:
            STATIC XTAPI XTSTATUS AllocateKernelStack(OUT PVOID *Stack,
                                                      IN ULONG StackSize);
            STATIC XTAPI XTSTATUS AllocateProcessorStructures(IN ULONG CpuNumber,
                                                              OUT PVOID *StructuresData);
            STATIC XTAPI VOID FreeKernelStack(IN PVOID Stack,
                                              IN ULONG StackSize);
            STATIC XTAPI VOID FreeProcessorStructures(IN PVOID StructuresData);
    };
}

#endif /* __XTOSKRNL_MM_KPOOL_HH */
