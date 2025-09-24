/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/bitmap.hh
 * DESCRIPTION:     Bit maps support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_BITMAP_HH
#define __XTOSKRNL_RTL_BITMAP_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class BitMap
    {
        public:
            STATIC XTAPI VOID ClearAllBits(IN PRTL_BITMAP BitMap);
            STATIC XTAPI VOID ClearBit(IN PRTL_BITMAP BitMap,
                                       IN ULONG_PTR Bit);
            STATIC XTAPI VOID ClearBits(IN PRTL_BITMAP BitMap,
                                        IN ULONG_PTR StartingIndex,
                                        IN ULONG_PTR Length);
            STATIC XTAPI ULONG ClearSetBits(IN PRTL_BITMAP BitMap,
                                            IN ULONG_PTR Length,
                                            IN ULONG_PTR Index);
            STATIC XTAPI VOID DumpBitMap(IN PRTL_BITMAP BitMap);
            STATIC XTAPI ULONG_PTR FindClearBits(IN PRTL_BITMAP BitMap,
                                                 IN ULONG_PTR Length,
                                                 IN ULONG_PTR Index);
            STATIC XTAPI ULONG_PTR FindSetBits(IN PRTL_BITMAP BitMap,
                                               IN ULONG_PTR Length,
                                               IN ULONG_PTR Index);
            STATIC XTAPI VOID InitializeBitMap(IN PRTL_BITMAP BitMap,
                                               IN PULONG_PTR Buffer,
                                               IN ULONG Size);
            STATIC XTAPI VOID SetAllBits(IN PRTL_BITMAP BitMap);
            STATIC XTAPI VOID SetBit(IN PRTL_BITMAP BitMap,
                                     IN ULONG_PTR Bit);
            STATIC XTAPI VOID SetBits(IN PRTL_BITMAP BitMap,
                                      IN ULONG_PTR StartingIndex,
                                      IN ULONG_PTR Length);
            STATIC XTAPI ULONG SetClearBits(IN PRTL_BITMAP BitMap,
                                            IN ULONG_PTR Length,
                                            IN ULONG_PTR Index);
            STATIC XTAPI BOOLEAN TestBit(IN PRTL_BITMAP BitMap,
                                         IN ULONG_PTR Bit);

        private:
            STATIC XTAPI ULONG_PTR CountBits(IN PRTL_BITMAP BitMap,
                                             IN ULONG_PTR Length,
                                             IN ULONG_PTR StartingIndex,
                                             IN BOOLEAN SetBits);
            STATIC XTAPI ULONG_PTR FindBits(IN PRTL_BITMAP BitMap,
                                            IN ULONG_PTR Length,
                                            IN ULONG_PTR StartingIndex,
                                            IN BOOLEAN SetBits);
    };
}

#endif /* __XTOSKRNL_RTL_BITMAP_HH */
