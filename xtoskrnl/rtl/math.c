/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/math.c
 * DESCRIPTION:     Kernel math support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Converts the 32-bit signed value to a large integer.
 *
 * @param Value
 *        Supplies the value to convert.
 *
 * @return This routine returns the large integer representation of the given value.
 *
 * @since XT 1.0
 */
XTAPI
LARGE_INTEGER
RtlConvertToLargeInteger32(IN LONG Value)
{
    LARGE_INTEGER LargeInt;

    /* Convert the value to a large integer and return it */
    LargeInt.QuadPart = Value;
    return LargeInt;
}

/**
 * Converts the 32-bit unsigned value to a large integer.
 *
 * @param Value
 *        Supplies the value to convert.
 *
 * @return This routine returns the large integer representation of the given value.
 *
 * @since XT 1.0
 */
XTAPI
LARGE_INTEGER
RtlConvertToLargeIntegerUnsigned32(IN ULONG Value)
{
    LARGE_INTEGER LargeInt;

    /* Convert the value to a large integer and return it */
    LargeInt.QuadPart = Value;
    return LargeInt;
}

/**
 * Determines the number of leading zero bits in a 32-bit unsigned value.
 *
 * @param Value
 *        Supplies the value whose leading zero bits are to be counted.
 *
 * @return This routine returns the number of leading zero bits in the given value.
 *
 * @since XT 1.0
 */
XTAPI
INT
RtlCountLeadingZeroes32(IN ULONG Value)
{
    /* Return a number of leading zero bits */
    return __builtin_clzl(Value);
}

/**
 * Determines the number of leading zero bits in a 64-bit unsigned value.
 *
 * @param Value
 *        Supplies the value whose leading zero bits are to be counted.
 *
 * @return This routine returns the number of leading zero bits in the given value.
 *
 * @since XT 1.0
 */
XTAPI
INT
RtlCountLeadingZeroes64(IN ULONG Value)
{
    /* Return a number of leading zero bits */
    return __builtin_clzll(Value);
}

/**
 * Determines the number of trailing zero bits in a 32-bit unsigned value.
 *
 * @param Value
 *        Supplies the value whose trailing zero bits are to be counted.
 *
 * @return This routine returns the number of trailing zero bits in the given value.
 *
 * @since XT 1.0
 */
XTAPI
INT
RtlCountTrailingZeroes32(IN ULONG Value)
{
    /* Return a number of trailing zero bits */
    return __builtin_ctzl(Value);
}

/**
 * Determines the number of trailing zero bits in a 64-bit unsigned value.
 *
 * @param Value
 *        Supplies the value whose trailing zero bits are to be counted.
 *
 * @return This routine returns the number of trailing zero bits in the given value.
 *
 * @since XT 1.0
 */
XTAPI
INT
RtlCountTrailingZeroes64(IN ULONGLONG Value)
{
    /* Return a number of trailing zero bits */
    return __builtin_ctzll(Value);
}

/**
 * Performs a 32-bit divide operation on signed integer numbers.
 *
 * @param Dividend
 *        Supplies a number that is going to be divided.
 *
 * @param Divisor
 *        Supplies a number by which the dividend is divided.
 *
 * @param Remainder
 *        Supplies a pointer that receives the divide remainder.
 *
 * @return This routine returns the quotient.
 *
 * @since XT 1.0
 */
XTAPI
LONGLONG
RtlDivide32(IN LONG Dividend,
            IN LONG Divisor,
            OUT PLONG Remainder)
{
    LONG Quotient;

    /* Calculate the quotient */
    Quotient = Dividend / Divisor;

    /* Make sure a pointer to remainder provided */
    if(Remainder)
    {
        /* Calculate remainder */
        *Remainder = Dividend - (Quotient * Divisor);
    }

    /* Return the quotient */
    return Quotient;
}

/**
 * Performs a 64-bit divide operation on signed integer numbers.
 *
 * @param Dividend
 *        Supplies a number that is going to be divided.
 *
 * @param Divisor
 *        Supplies a number by which the dividend is divided.
 *
 * @param Remainder
 *        Supplies a pointer that receives the divide remainder.
 *
 * @return This routine returns the quotient.
 *
 * @since XT 1.0
 */
XTAPI
LONGLONG
RtlDivide64(IN LONGLONG Dividend,
            IN LONGLONG Divisor,
            OUT PLONGLONG Remainder)
{
    LONGLONG Quotient;

    /* Calculate the quotient */
    Quotient = Dividend / Divisor;

    /* Make sure a pointer to remainder provided */
    if(Remainder)
    {
        /* Calculate remainder */
        *Remainder = Dividend - (Quotient * Divisor);
    }

    /* Return the quotient */
    return Quotient;
}

/**
 * Performs a 32-bit divide operation on unsigned integer numbers.
 *
 * @param Dividend
 *        Supplies an unsigned number that is going to be divided.
 *
 * @param Divisor
 *        Supplies an unsigned number by which the dividend is divided.
 *
 * @param Remainder
 *        Supplies a pointer that receives the unsigned divide remainder.
 *
 * @return This routine returns the quotient.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
RtlDivideUnsigned32(IN ULONG Dividend,
                    IN ULONG Divisor,
                    OUT PULONG Remainder)
{
    /* Make sure a pointer to remainder provided */
    if(Remainder)
    {
        /* Calculate remainder */
        *Remainder = Dividend % Divisor;
    }

    /* Return the quotient */
    return Dividend / Divisor;
}

/**
 * Performs a 64-bit divide operation on unsigned integer numbers.
 *
 * @param Dividend
 *        Supplies an unsigned number that is going to be divided.
 *
 * @param Divisor
 *        Supplies an unsigned number by which the dividend is divided.
 *
 * @param Remainder
 *        Supplies a pointer that receives the unsigned divide remainder.
 *
 * @return This routine returns the quotient.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
RtlDivideUnsigned64(IN ULONGLONG Dividend,
                    IN ULONGLONG Divisor,
                    OUT PULONGLONG Remainder)
{
    /* Make sure a pointer to remainder provided */
    if(Remainder)
    {
        /* Calculate remainder */
        *Remainder = Dividend % Divisor;
    }

    /* Return the quotient */
    return Dividend / Divisor;
}

/**
 * Divides a signed large integer by a 32-bit divisor.
 *
 * @param Dividend
 *        Supplies a large integer to be divided.
 *
 * @param Divisor
 *        Supplies a 32-bit divisor.
 *
 * @param Remainder
 *        Supplies a pointer that receives the divide remainder.
 *
 * @return This routine returns the quotient.
 *
 * @since XT 1.0
 */
XTAPI
LARGE_INTEGER
RtlDivideLargeInteger(IN LARGE_INTEGER Dividend,
                      IN ULONG Divisor,
                      OUT PULONG Remainder)
{
    LARGE_INTEGER LargeInt;

    /* Make sure a pointer to remainder provided */
    if(Remainder)
    {
        /* Calculate remainder */
        *Remainder = Dividend.QuadPart % Divisor;
    }

    /* Return the quotient */
    LargeInt.QuadPart = Dividend.QuadPart / Divisor;
    return LargeInt;
}

/**
 * Multiplies a signed large integer by a signed integer.
 *
 * @param Multiplicand
 *        Supplies a large integer to be multiplied.
 *
 * @param Multiplier
 *        Supplies an integer by which the large integer is multiplied.
 *
 * @return This routine returns the result of the multiplication.
 *
 * @since XT 1.0
 */
XTAPI
LARGE_INTEGER
RtlMultiplyLargeInteger(IN LARGE_INTEGER Multiplicand,
                        IN LONG Multiplier)
{
    LARGE_INTEGER LargeInt;

    /* Perform multiplication and return the result */
    LargeInt.QuadPart = (LONGLONG) Multiplicand.QuadPart * Multiplier;
    return LargeInt;
}
