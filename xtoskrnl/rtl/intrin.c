/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/intrin.c
 * DESCRIPTION:     Interlocked operations support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Performs an atomic compare exchange operation on the pointer value.
 *
 * @param Destination
 *        Supplues a pointer to the variable for the compare exchange operation.
 *
 * @param Comperand
 *        Supplies a pointer value used in compare operation.
 *
 * @param Exchange
 *        Supplies a pointer value used in exchange operation.
 *
 * @return This routine returns the initial pointer value of the Destination parameter.
 *
 * @since XT 1.0
 */
XTFASTCALL
PVOID
RtlInterlockedCompareExchangePointer(IN VOLATILE PVOID *Destination,
                                     IN PVOID Comperand,
                                     IN PVOID Exchange)
{
    return (PVOID)__sync_val_compare_and_swap(Destination, Comperand, Exchange);
}

/**
 * Performs an atomic decrement of the 32-bit or 64bit value, depending on architecture.
 *
 * @param Addend
 *        Specifies a pointer to the variable to be decremented.
 *
 * @return This routine returns the decremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG_PTR
RtlInterlockedDecrementLongPtr(IN VOLATILE PLONG_PTR Addend)
{
    return (LONG_PTR)__sync_sub_and_fetch(Addend, 1);
}
