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
            STATIC XTFASTCALL VOID CalculateAffinityMapSize(IN ULONG CpuCount,
                                                            OUT PULONG RequiredMapSize,
                                                            OUT PULONG RequiredBlockCount);
            STATIC XTFASTCALL BOOLEAN CheckProcessorAffinity(IN PKAFFINITY_MAP AffinityMap,
                                                             IN ULONG CpuNumber);
            STATIC XTFASTCALL VOID ClearAffinityMap(IN OUT PKAFFINITY_MAP AffinityMap);
            STATIC XTFASTCALL VOID ClearProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                                          IN ULONG CpuNumber);

            STATIC XTAPI XTSTATUS CopyAffinity(OUT PKAFFINITY_MAP Destination,
                                               IN PKAFFINITY_MAP Source);
            STATIC XTAPI XTSTATUS CreateAffinityMap(IN ULONG CpuCount,
                                                    OUT PKAFFINITY_MAP* AffinityMap);
            STATIC XTAPI VOID DestroyAffinityMap(IN PKAFFINITY_MAP AffinityMap);
            STATIC XTAPI ULONG FindNextLeftSetProcessor(IN ULONG ThreadSeed,
                                                        IN PKAFFINITY_MAP AffinityMap);
            STATIC XTAPI ULONG FindNextRightSetProcessor(IN ULONG ThreadSeed,
                                                         IN PKAFFINITY_MAP AffinityMap);
            STATIC XTAPI XTSTATUS InitializeAffinityMap(IN OUT PKAFFINITY_MAP AffinityMap,
                                                        IN ULONG BufferSize);
            STATIC XTFASTCALL VOID SetAllProcessorsAffinity(IN OUT PKAFFINITY_MAP AffinityMap);
            STATIC XTFASTCALL VOID SetProcessorAffinity(IN OUT PKAFFINITY_MAP AffinityMap,
                                                        IN ULONG CpuNumber);
    };
}

#endif /* __XTOSKRNL_KE_AFFINITY_HH */
