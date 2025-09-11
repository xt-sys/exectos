/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/math.hh
 * DESCRIPTION:     Kernel math support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_MATH_HH
#define __XTOSKRNL_RTL_MATH_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Math
    {
        public:
            STATIC XTAPI LARGE_INTEGER ConvertToLargeInteger32(IN LONG Value);
            STATIC XTAPI LARGE_INTEGER ConvertToLargeIntegerUnsigned32(IN ULONG Value);
            STATIC XTAPI INT CountLeadingZeroes32(IN ULONG Value);
            STATIC XTAPI INT CountLeadingZeroes64(IN ULONGLONG Value);
            STATIC XTAPI INT CountTrailingZeroes32(IN ULONG Value);
            STATIC XTAPI INT CountTrailingZeroes64(IN ULONGLONG Value);
            STATIC XTAPI LONGLONG Divide32(IN LONG Dividend,
                                           IN LONG Divisor,
                                           OUT PLONG Remainder);
            STATIC XTAPI LONGLONG Divide64(IN LONGLONG Dividend,
                                           IN LONGLONG Divisor,
                                           OUT PLONGLONG Remainder);
            STATIC XTAPI ULONGLONG DivideUnsigned32(IN ULONG Dividend,
                                                    IN ULONG Divisor,
                                                    OUT PULONG Remainder);
            STATIC XTAPI ULONGLONG DivideUnsigned64(IN ULONGLONG Dividend,
                                                    IN ULONGLONG Divisor,
                                                    OUT PULONGLONG Remainder);
            STATIC XTAPI LARGE_INTEGER DivideLargeInteger(IN LARGE_INTEGER Dividend,
                                                          IN ULONG Divisor,
                                                          OUT PULONG Remainder);
            STATIC XTAPI LONG GetBaseExponent(IN DOUBLE Value,
                                               OUT PDOUBLE PowerOfTen);
            STATIC XTAPI BOOLEAN InfiniteDouble(IN DOUBLE Value);
            STATIC XTAPI LARGE_INTEGER MultiplyLargeInteger(IN LARGE_INTEGER Multiplicand,
                                                            IN LONG Multiplier);
            STATIC XTAPI BOOLEAN NanDouble(IN DOUBLE Value);
    };
}

#endif /* __XTOSKRNL_RTL_MATH_HH */
