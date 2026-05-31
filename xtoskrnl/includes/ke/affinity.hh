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
            STATIC XTAPI VOID CopyAffinity(OUT PKAFFINITY_MAP Destination,
                                           IN PKAFFINITY_MAP Source);
            STATIC XTAPI ULONG FindNextLeftSetProcessor(IN ULONG ThreadSeed,
                                                        IN PKAFFINITY_MAP AffinityMap);
            STATIC XTAPI ULONG FindNextRightSetProcessor(IN ULONG ThreadSeed,
                                                         IN PKAFFINITY_MAP AffinityMap);
    };
}

#endif /* __XTOSKRNL_KE_AFFINITY_HH */
