/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/affinity.hh
 * DESCRIPTION:     XT kernel processor affinity management support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_AFFINITY_HH
#define __XTOSKRNL_KE_AFFINITY_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Affinity
    {
        public:
            STATIC XTFASTCALL VOID AtomicSetProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                                              IN ULONG CpuNumber);
            STATIC XTFASTCALL ULONG CalculateAffinityMapSize(IN ULONG CpuCount);
            STATIC XTFASTCALL BOOLEAN CheckProcessorAffinity(IN PKAFFINITY_MAP AffinityMap,
                                                             IN ULONG CpuNumber);
            STATIC XTFASTCALL VOID ClearAffinityMap(IN OUT PKAFFINITY_MAP AffinityMap);
            STATIC XTFASTCALL VOID ClearProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                                          IN ULONG CpuNumber);

            STATIC XTAPI VOID CopyAffinity(OUT PKAFFINITY_MAP Destination,
                                           IN PKAFFINITY_MAP Source);
            STATIC XTAPI ULONG FindNextLeftSetProcessor(IN ULONG ThreadSeed,
                                                        IN PKAFFINITY_MAP AffinityMap);
            STATIC XTAPI ULONG FindNextRightSetProcessor(IN ULONG ThreadSeed,
                                                         IN PKAFFINITY_MAP AffinityMap);
            STATIC XTFASTCALL VOID SetProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                                        IN ULONG CpuNumber);
    };
}

#endif /* __XTOSKRNL_KE_AFFINITY_HH */
