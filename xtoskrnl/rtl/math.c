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
    LONGLONG DividendSign, DivisorSign, Quotient, UDividend, UDivisor;

    /* Remove the sign bit from dividend and divisor if present */
    DividendSign = Dividend >> ((sizeof(LONGLONG) * BITS_PER_BYTE) - 1);
    DivisorSign = Divisor >> ((sizeof(LONGLONG) * BITS_PER_BYTE) - 1);
    UDividend = (Dividend ^ DividendSign) - DividendSign;
    UDivisor = (Divisor ^ DivisorSign) - DivisorSign;

    /* Calculate the quotient */
    DividendSign ^= DivisorSign;
    Quotient = (RtlDivideUnsigned64(UDividend, UDivisor, NULL) ^ DividendSign) - DividendSign;

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
    ULARGE_INTEGER DividendParts, DivisorParts, QuotientParts, RemainderParts;
    LONGLONG Difference;
    ULONGLONG Shift;
    ULONG Carry;

    /* Make sure divisor is not 0 */
    if(Divisor == 0)
    {
        /* Cannot divide by 0 */
        return 0;
    }

    /* Assign dividend and divisor to large integer representations */
    DividendParts.QuadPart = Dividend;
    DivisorParts.QuadPart = Divisor;

    /* Check if dividend is 32-bit value */
    if(DividendParts.u.HighPart == 0)
    {
        /* Check if devisor is 32-bit value */
        if(DivisorParts.u.HighPart == 0)
        {
            /* 32-bit divide operation, check if remainder provided */
            if(Remainder != NULL)
            {
                /* Calculate remainder */
                *Remainder = DividendParts.u.LowPart % DivisorParts.u.LowPart;
            }

            /* Return the quotient */
            return DividendParts.u.LowPart / DivisorParts.u.LowPart;
        }

        /* 32-bit value divided by a 64-bit value, check if remainder provided */
        if(Remainder != NULL)
        {
            /* Calculate remainder */
            *Remainder = DividendParts.u.LowPart;
        }

        /* Return zero as quotient */
        return 0;
    }

    /* Dividend is a 64-bit value, check if divisor has a low part */
    if(DivisorParts.u.LowPart != 0)
    {
        /* Divisor has a non-zero low part, check if divisor has a high part */
        if(DivisorParts.u.HighPart != 0)
        {
            /* Divisor is 64-bit value, calculate the shift count */
            Shift = RtlCountLeadingZeroes32(DivisorParts.u.HighPart) - RtlCountLeadingZeroes32(DividendParts.u.HighPart);

            /* Check if shift count exceeds 32-bits */
            if(Shift > ((sizeof(ULONG) * BITS_PER_BYTE) - 1))
            {
                /* Check if remainder provided */
                if(Remainder != NULL)
                {
                    /* Calculate remainder */
                    *Remainder = DividendParts.QuadPart;
                }

                return 0;
            }

            /* Increase shift and clear quotient low part */
            Shift++;
            QuotientParts.u.LowPart = 0;

            /* Check if shift is 32-bits */
            if(Shift == (sizeof(ULONG) * BITS_PER_BYTE)) {
                /* Get the quotient high part and remainder */
                QuotientParts.u.HighPart = DividendParts.u.LowPart;
                RemainderParts.u.LowPart = DividendParts.u.HighPart;
                RemainderParts.u.HighPart = 0;
            } else {
                /* Get the quotient high part and remainder */
                QuotientParts.u.HighPart = DividendParts.u.LowPart << ((sizeof(ULONG) * BITS_PER_BYTE) - Shift);
                RemainderParts.u.LowPart = (DividendParts.u.HighPart << ((sizeof(ULONG) * BITS_PER_BYTE) - Shift)) |
                                           (DividendParts.u.LowPart >> Shift);
                RemainderParts.u.HighPart = DividendParts.u.HighPart >> Shift;
            }
        }
        else
        {
            /* Divisor is 32-bit value, calculate the shift count */
            Shift = (sizeof(ULONG) * BITS_PER_BYTE) + 1 +
                    RtlCountLeadingZeroes32(DivisorParts.u.LowPart) -
                    RtlCountLeadingZeroes32(DividendParts.u.HighPart);

            /* Check if shift is 32-bit */
            if(Shift == (sizeof(ULONG) * BITS_PER_BYTE))
            {
                /* Get the quotient and remainder */
                QuotientParts.u.LowPart = 0;
                QuotientParts.u.HighPart = DividendParts.u.LowPart;
                RemainderParts.u.LowPart = DividendParts.u.HighPart;
                RemainderParts.u.HighPart = 0;
            }
            else if(Shift < (sizeof(ULONG) * BITS_PER_BYTE))
            {
                /* Shift is smaller, get the quotient and remainder */
                QuotientParts.u.LowPart = 0;
                QuotientParts.u.HighPart = DividendParts.u.LowPart << ((sizeof(ULONG) * BITS_PER_BYTE) - Shift);
                RemainderParts.u.LowPart = (DividendParts.u.HighPart <<
                                           ((sizeof(ULONG) * BITS_PER_BYTE) - Shift)) |
                                           (DividendParts.u.LowPart >> Shift);
                RemainderParts.u.HighPart = DividendParts.u.HighPart >> Shift;
            }
            else
            {
                /* Shift is larger, get the quotient and remainder */
                QuotientParts.u.LowPart = DividendParts.u.LowPart << ((sizeof(ULONGLONG) * BITS_PER_BYTE) - Shift);
                QuotientParts.u.HighPart = (DividendParts.u.HighPart << ((sizeof(ULONGLONG) * BITS_PER_BYTE) - Shift)) |
                                           (DividendParts.u.LowPart >> (Shift - (sizeof(ULONG) * BITS_PER_BYTE)));
                RemainderParts.u.LowPart = DividendParts.u.HighPart >> (Shift - (sizeof(ULONG) * BITS_PER_BYTE));
                RemainderParts.u.HighPart = 0;
            }
        }
    }
    else
    {
        /* Divisor is 64-bit value, check if dividend has low part set */
        if(DividendParts.u.LowPart == 0)
        {
            /* Check if remainder provided */
            if(Remainder != NULL)
            {
                /* Calculate the remainder */
                RemainderParts.u.HighPart = DividendParts.u.HighPart % DivisorParts.u.HighPart;
                RemainderParts.u.LowPart = 0;
                *Remainder = RemainderParts.QuadPart;
            }

            /* Return the quotient */
            return DividendParts.u.HighPart / DivisorParts.u.HighPart;
        }

        /* Calculate the shift count */
        Shift = RtlCountLeadingZeroes32(DivisorParts.u.HighPart) - RtlCountLeadingZeroes32(DividendParts.u.HighPart);

        /* Check if shift exceeds 32-bits */
        if(Shift > ((sizeof(ULONG) * BITS_PER_BYTE) - 2))
        {
            /* Check if remainder provided */
            if(Remainder != NULL)
            {
                /* Calculate the remainder */
                *Remainder = DividendParts.QuadPart;
            }

            /* Return 0 */
            return 0;
        }

        /* Increase shift and clear quotient low part */
        Shift++;
        QuotientParts.u.LowPart = 0;

        /* Get the quotient high part and remainder */
        QuotientParts.u.HighPart = DividendParts.u.LowPart << ((sizeof(ULONG) * BITS_PER_BYTE) - Shift);
        RemainderParts.u.HighPart = DividendParts.u.HighPart >> Shift;
        RemainderParts.u.LowPart = (DividendParts.u.HighPart <<
                                   ((sizeof(ULONG) * BITS_PER_BYTE) - Shift)) |
                                   (DividendParts.u.LowPart >> Shift);
    }

    /* Perform the division until shift is zero */
    Carry = 0;
    while(Shift > 0)
    {
        /* Shift the remainder and the quotient */
        RemainderParts.u.HighPart = (RemainderParts.u.HighPart << 1) |
                                    (RemainderParts.u.LowPart >>
                                    ((sizeof(ULONG) * BITS_PER_BYTE) - 1));
        RemainderParts.u.LowPart = (RemainderParts.u.LowPart << 1) |
                                   (QuotientParts.u.HighPart >>
                                   ((sizeof(ULONG) * BITS_PER_BYTE) - 1));
        QuotientParts.u.HighPart = (QuotientParts.u.HighPart << 1) |
                                   (QuotientParts.u.LowPart >>
                                   ((sizeof(ULONG) * BITS_PER_BYTE) - 1));
        QuotientParts.u.LowPart = (QuotientParts.u.LowPart << 1) | Carry;

        /* Set the carry and subtract the divisor */
        Difference = (LONGLONG)(DivisorParts.QuadPart - RemainderParts.QuadPart - 1) >>
                     ((sizeof(ULONGLONG) * BITS_PER_BYTE) - 1);
        Carry = Difference & 0x1;
        RemainderParts.QuadPart -= DivisorParts.QuadPart & Difference;
        Shift -= 1;
    }

    /* Add the carry to the quotient */
    QuotientParts.QuadPart = (QuotientParts.QuadPart << 1) | Carry;

    /* Check if remainder provided */
    if(Remainder != NULL)
    {
        /* Calculate the remainder */
        *Remainder = RemainderParts.QuadPart;
    }

    /* Return the quotient */
    return QuotientParts.QuadPart;
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
    LONGLONG DividendSign, UDividend;
    LARGE_INTEGER LargeInt;

    /* Remove the sign bit from dividend if present */
    DividendSign = Dividend.QuadPart >> ((sizeof(LONGLONG) * BITS_PER_BYTE) - 1);
    UDividend = (Dividend.QuadPart ^ DividendSign) - DividendSign;

    /* Calculate the quotient */
    LargeInt.QuadPart = (RtlDivideUnsigned64(UDividend, Divisor, NULL) ^ DividendSign) - DividendSign;

    /* Make sure a pointer to remainder provided */
    if(Remainder)
    {
        /* Calculate remainder */
        *Remainder = Dividend.QuadPart - (LargeInt.QuadPart * Divisor);
    }

    /* Return the quotient */
    return LargeInt;
}

/**
 * Gets the base exponent of a given floating point value.
 *
 * @param Value
 *        Supplies the floating point value to get the base exponent of.
 *
 * @param PowerOfTen
 *        Supplies a pointer that receives the power of ten associated with the base exponent.
 *
 * @return This routine returns the base exponent value.
 *
 * @since XT 1.0
 */
XTAPI
LONG
RtlGetBaseExponent(IN DOUBLE Value,
                   OUT PDOUBLE PowerOfTen)
{
    LONG BaseExponent, CurrentExponent, Exponent;
    ULONG ExponentShift, ExponentMask;
    LARGE_DOUBLE Parts;
    DOUBLE Power;

    /* Calculate the exponent mask and shift */
    ExponentMask = DOUBLE_EXPONENT_MASK >> (sizeof(ULONG) * BITS_PER_BYTE);
    ExponentShift = DOUBLE_EXPONENT_SHIFT - (sizeof(ULONG) * BITS_PER_BYTE);

    /* Check if value is zero */
    if(Value == 0.0)
    {
        /* Return the power of ten and the exponent */
        *PowerOfTen = 1.0;
        return 0;
    }

    /* Get the parts of the value and calculate the exponent by multiplying by log10(2) */
    Parts.DoublePart = Value;
    BaseExponent = ((Parts.u.HighPart & ExponentMask) >> ExponentShift) - DOUBLE_EXPONENT_BIAS;
    Exponent = (LONG)((DOUBLE)BaseExponent * 0.30102999566) + 1;

    /* Set the initial values and calculate the exponent */
    CurrentExponent = 0;
    Power = 1.0;
    if(Exponent > 0)
    {
        /* Calculate the exponent */
        while(CurrentExponent + 10 <= Exponent)
        {
            Power *= 0.0000000001;
            CurrentExponent += 10;
        }

        while(CurrentExponent + 1 <= Exponent)
        {
            Power *= 0.1;
            CurrentExponent += 1;
        }
    }
    else
    {
        /* Calculate the exponent */
        while(CurrentExponent - 10 >= Exponent)
        {
            Power *= 10000000000;
            CurrentExponent -= 10;
        }

        while(CurrentExponent - 1 >= Exponent)
        {
            Power *= 10.0;
            CurrentExponent -= 1;
        }
    }

    /* Normalize input value */
    Value *= Power;

    /* Remove all leading zeros, if any */
    while((Value != 0.0) && ((LONG)Value == 0))
    {
        Value *= 10.0;
        Exponent -= 1;
        Power *= 10.0;
    }

    /* Return the power of ten and the exponent */
    *PowerOfTen = Power;
    return Exponent;
}

/**
 * Determines whether a floating-point number is infinite.
 *
 * @param Value
 *        Supplies the floating-point value to test.
 *
 * @return This routine returns TRUE if the argument is infinite or a NaN, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
RtlInfiniteDouble(IN DOUBLE Value)
{
    /* DOUBLE argument in IEEE 754 standard format */
    union
    {
        PDOUBLE Double;
        struct
        {
            UINT MantissaLow:32;
            UINT MantissaHigh:20;
            UINT Exponent:11;
            UINT Sign:1;
        } *DoubleS;
    } Var;

    /* Convert input double value to IEEE 754 format */
    Var.Double = &Value;

    /* Return TRUE if it is infinite, or FALSE otherwise */
    return ((Var.DoubleS->Exponent & 0x7FF) == 0x7FF);
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

/**
 * Determines whether a floating-point number is a NaN ("Not a Number").
 *
 * @param Value
 *        Supplies the floating-point value to test.
 *
 * @return This routine returns TRUE if the argument is a NaN, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
RtlNanDouble(IN DOUBLE Value)
{
    /* DOUBLE argument in IEEE 754 standard format */
    union
    {
        PDOUBLE Double;
        struct
        {
            UINT MantissaLow:32;
            UINT MantissaHigh:20;
            UINT Exponent:11;
            UINT Sign:1;
        } *DoubleS;
    } Var;

    /* Convert input double value to IEEE 754 format */
    Var.Double = &Value;

    /* Return TRUE if it is NaN, or FALSE otherwise */
    return (Var.DoubleS->Exponent == 0x7FF && (Var.DoubleS->MantissaHigh != 0 || Var.DoubleS->MantissaLow != 0));
}
