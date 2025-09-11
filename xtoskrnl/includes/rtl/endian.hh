/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/endian.hh
 * DESCRIPTION:     Endian conversion routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_ENDIAN_HH
#define __XTOSKRNL_RTL_ENDIAN_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Endianness
    {
        public:
            STATIC XTFASTCALL USHORT SwapByte16(IN USHORT Source);
            STATIC XTFASTCALL ULONG SwapByte32(IN ULONG Source);
            STATIC XTFASTCALL ULONGLONG SwapByte64(IN ULONGLONG Source);
    };
}

#endif /* __XTOSKRNL_RTL_ENDIAN_HH */
