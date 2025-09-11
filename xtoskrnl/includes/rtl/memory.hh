/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/memory.hh
 * DESCRIPTION:     Memory related routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_MEMORY_HH
#define __XTOSKRNL_RTL_MEMORY_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Memory
    {
        public:
            STATIC XTAPI SIZE_T CompareMemory(IN PCVOID LeftBuffer,
                                              IN PCVOID RightBuffer,
                                              IN SIZE_T Length);
            STATIC XTAPI VOID CopyMemory(OUT PVOID Destination,
                                         IN PCVOID Source,
                                         IN SIZE_T Length);
            STATIC XTAPI VOID MoveMemory(OUT PVOID Destination,
                                         IN PCVOID Source,
                                         IN SIZE_T Length);
            STATIC XTAPI BOOLEAN SameMemory(IN PCVOID LeftBuffer,
                                            IN PCVOID RightBuffer,
                                            IN SIZE_T Length);
            STATIC XTAPI VOID SetMemory(OUT PVOID Destination,
                                        IN UCHAR Byte,
                                        IN SIZE_T Length);
            STATIC XTAPI VOID ZeroMemory(OUT PVOID Destination,
                                         IN SIZE_T Length);
    };
}

#endif /* __XTOSKRNL_RTL_MEMORY_HH */
