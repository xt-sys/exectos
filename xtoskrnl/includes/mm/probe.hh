/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/probe.hh
 * DESCRIPTION:     Memory Manager read/write probe support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_PROBE_HH
#define __XTOSKRNL_MM_PROBE_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Probe
    {
        public:
            STATIC XTAPI VOID ProbeForRead(IN CONST PVOID Address,
                                           IN SIZE_T Size,
                                           IN ULONG Alignment);
            STATIC XTAPI VOID ProbeForReadArray(IN CONST PVOID Array,
                                                IN ULONG ItemSize,
                                                IN ULONG ItemCount,
                                                IN ULONG Alignment);
            STATIC XTAPI VOID ProbeForReadStructure(IN CONST PVOID Address,
                                                    IN SIZE_T Size,
                                                    IN ULONG Alignment);
            STATIC XTAPI VOID ProbeForWrite(IN PVOID Address,
                                            IN SIZE_T Size,
                                            IN ULONG Alignment);
            STATIC XTAPI VOID ProbeForWriteArray(IN OUT PVOID Array,
                                                 IN ULONG ItemSize,
                                                 IN ULONG ItemCount,
                                                 IN ULONG Alignment);
            STATIC XTAPI VOID ProbeForWriteStructure(IN PVOID Address,
                                                     IN SIZE_T Size,
                                                     IN ULONG Alignment);
    };
}

#endif /* __XTOSKRNL_MM_PROBE_HH */
