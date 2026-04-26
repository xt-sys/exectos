/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/i686/intrin.cc
 * DESCRIPTION:     Compiler intrinsic support routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>



/**
 * Divides a 64-bit signed integer by a 64-bit signed integer.
 *
 * @param Dividend
 *        Supplies the 64-bit signed dividend.
 *
 * @param Divisor
 *        Supplies the 64-bit signed divisor.
 *
 * @return This routine returns the 64-bit signed quotient.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
LONGLONG
_alldiv(IN LONGLONG Dividend,
        IN LONGLONG Divisor)
{
    /* Call the internal signed division routine and return the quotient */
    return RTL::Math::Divide64(Dividend, Divisor, NULLPTR);
}

/**
 * Divides a 64-bit signed integer by a 64-bit signed integer and retrieves the remainder.
 *
 * @param Dividend
 *        Supplies the 64-bit signed dividend.
 *
 * @param Divisor
 *        Supplies the 64-bit signed divisor.
 *
 * @param Remainder
 *        Supplies a pointer to a variable that receives the 64-bit signed remainder.
 *
 * @return This routine returns the 64-bit signed quotient.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
LONGLONG
_alldvrm(IN LONGLONG Dividend,
         IN LONGLONG Divisor,
         OUT PLONGLONG Remainder)
{
    /* Call the internal signed division routine to compute both quotient and remainder */
    return RTL::Math::Divide64(Dividend, Divisor, Remainder);
}
/**
 * Calculates the remainder of a 64-bit signed integer division.
 *
 * @param Dividend
 *        Supplies the 64-bit signed dividend.
 *
 * @param Divisor
 *        Supplies the 64-bit signed divisor.
 *
 * @return This routine returns the 64-bit signed remainder.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
LONGLONG
_allrem(IN LONGLONG Dividend,
        IN LONGLONG Divisor)
{
    LONGLONG Remainder;

    /* Call the internal signed division routine and return the computed remainder */
    RTL::Math::Divide64(Dividend, Divisor, &Remainder);
    return Remainder;
}

/**
 * Divides a 64-bit unsigned integer by a 64-bit unsigned integer.
 *
 * @param Dividend
 *        Supplies the 64-bit unsigned dividend.
 *
 * @param Divisor
 *        Supplies the 64-bit unsigned divisor.
 *
 * @return This routine returns the 64-bit unsigned quotient.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
ULONGLONG
_aulldiv(IN ULONGLONG Dividend,
         IN ULONGLONG Divisor)
{
    /* Call the internal unsigned division routine and return the quotient */
    return RTL::Math::DivideUnsigned64(Dividend, Divisor, NULLPTR);
}

/**
 * Divides a 64-bit unsigned integer by a 64-bit unsigned integer and retrieves the remainder.
 *
 * @param Dividend
 *        Supplies the 64-bit unsigned dividend.
 *
 * @param Divisor
 *        Supplies the 64-bit unsigned divisor.
 *
 * @param Remainder
 *        Supplies a pointer to a variable that receives the 64-bit unsigned remainder.
 *
 * @return This routine returns the 64-bit unsigned quotient.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
ULONGLONG
_aulldvrm(ULONGLONG Dividend,
          ULONGLONG Divisor,
          PULONGLONG Remainder)
{
    /* Call the internal unsigned division routine to compute both quotient and remainder */
    return RTL::Math::DivideUnsigned64(Dividend, Divisor, Remainder);
}

/**
 * Calculates the remainder of a 64-bit unsigned integer division.
 *
 * @param Dividend
 *        Supplies the 64-bit unsigned dividend.
 *
 * @param Divisor
 *        Supplies the 64-bit unsigned divisor.
 *
 * @return This routine returns the 64-bit unsigned remainder.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
ULONGLONG
_aullrem(IN ULONGLONG Dividend,
         IN ULONGLONG Divisor)
{
    ULONGLONG Remainder;

    /* Call the internal unsigned division routine and return the computed remainder */
    RTL::Math::DivideUnsigned64(Dividend, Divisor, &Remainder);
    return Remainder;
}
