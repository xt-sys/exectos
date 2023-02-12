/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/atomic.c
 * DESCRIPTION:     Atomic operations support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Performs an atomic bitwise AND operation on the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise AND operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise AND operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicAnd8(IN VOLATILE PCHAR Address,
              IN CHAR Mask)
{
    return __sync_fetch_and_and(Address, Mask);
}

/**
 * Performs an atomic bitwise AND operation on the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise AND operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise AND operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicAnd16(IN VOLATILE PSHORT Address,
               IN SHORT Mask)
{
    return __sync_fetch_and_and(Address, Mask);
}

/**
 * Performs an atomic bitwise AND operation on the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise AND operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise AND operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicAnd32(IN VOLATILE PLONG Address,
               IN LONG Mask)
{
    return __sync_fetch_and_and(Address, Mask);
}

/**
 * Performs an atomic bitwise AND operation on the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise AND operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise AND operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicAnd64(IN VOLATILE PLONGLONG Address,
               IN LONGLONG Mask)
{
    return __sync_fetch_and_and(Address, Mask);
}

/**
 * Performs atomically compare exchange operation on the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value to compare and potentially exchange.
 *
 * @param Comperand
 *        Supplies the value to compare against.
 *
 * @param Exchange
 *        Supplies the value to write if the comparison returns equality.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicCompareExchange8(IN VOLATILE PCHAR Address,
                          IN CHAR Comperand,
                          IN CHAR Exchange)
{
    return __sync_val_compare_and_swap(Address, Comperand, Exchange);
}

/**
 * Performs atomically compare exchange operation on the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value to compare and potentially exchange.
 *
 * @param Comperand
 *        Supplies the value to compare against.
 *
 * @param Exchange
 *        Supplies the value to write if the comparison returns equality.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicCompareExchange16(IN VOLATILE PSHORT Address,
                           IN SHORT Comperand,
                           IN SHORT Exchange)
{
    return __sync_val_compare_and_swap(Address, Comperand, Exchange);
}

/**
 * Performs atomically compare exchange operation on the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value to compare and potentially exchange.
 *
 * @param Comperand
 *        Supplies the value to compare against.
 *
 * @param Exchange
 *        Supplies the value to write if the comparison returns equality.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicCompareExchange32(IN VOLATILE PLONG Address,
                           IN LONG Comperand,
                           IN LONG Exchange)
{
    return __sync_val_compare_and_swap(Address, Comperand, Exchange);
}

/**
 * Performs atomically compare exchange operation on the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value to compare and potentially exchange.
 *
 * @param Comperand
 *        Supplies the value to compare against.
 *
 * @param Exchange
 *        Supplies the value to write if the comparison returns equality.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicCompareExchange64(IN VOLATILE PLONGLONG Address,
                           IN LONGLONG Comperand,
                           IN LONGLONG Exchange)
{
    return __sync_val_compare_and_swap(Address, Comperand, Exchange);
}

/**
 * Performs atomically compare exchange operation.
 *
 * @param Address
 *        Supplies the address of the value to compare and potentially exchange.
 *
 * @param Comperand
 *        Supplies a pointer to the value to compare against.
 *
 * @param Exchange
 *        Supplies a pointer to the value to write if the comparison returns equality.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
PVOID
RtlAtomicCompareExchangePointer(IN VOLATILE PVOID *Address,
                                IN PVOID Comperand,
                                IN PVOID Exchange)
{
    return (PVOID)__sync_val_compare_and_swap(Address, Comperand, Exchange);
}

/**
 * Performs atomically decrement of the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value to decrement.
 *
 * @return This routine returns the decremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicDecrement8(IN VOLATILE PCHAR Address)
{
    return __sync_sub_and_fetch(Address, 1);
}

/**
 * Performs atomically decrement of the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value to decrement.
 *
 * @return This routine returns the decremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicDecrement16(IN VOLATILE PSHORT Address)
{
    return __sync_sub_and_fetch(Address, 1);
}

/**
 * Performs atomically decrement of the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value to decrement.
 *
 * @return This routine returns the decremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicDecrement32(IN VOLATILE PLONG Address)
{
    return __sync_sub_and_fetch(Address, 1);
}

/**
 * Performs atomically decrement of the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value to decrement.
 *
 * @return This routine returns the decremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicDecrement64(IN VOLATILE PLONGLONG Address)
{
    return __sync_sub_and_fetch(Address, 1);
}

/**
 * Performs atomically operation on the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value to exchange with.
 *
 * @param Exchange
 *        Supplies the value to write to the address.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicExchange8(IN VOLATILE PCHAR Address,
                   IN CHAR Exchange)
{
    return __sync_lock_test_and_set(Address, Exchange);
}

/**
 * Performs atomically operation on the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value to exchange with.
 *
 * @param Exchange
 *        Supplies the value to write to the address.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicExchange16(IN VOLATILE PSHORT Address,
                    IN SHORT Exchange)
{
    return __sync_lock_test_and_set(Address, Exchange);
}

/**
 * Performs atomically operation on the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value to exchange with.
 *
 * @param Exchange
 *        Supplies the value to write to the address.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicExchange32(IN VOLATILE PLONG Address,
                    IN LONG Exchange)
{
    return __sync_lock_test_and_set(Address, Exchange);
}

/**
 * Performs atomically operation on the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value to exchange with.
 *
 * @param Exchange
 *        Supplies the value to write to the address.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicExchange64(IN VOLATILE PLONGLONG Address,
                    IN LONGLONG Exchange)
{
    return __sync_lock_test_and_set(Address, Exchange);
}

/**
 * Performs atomically addition of the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the original value.
 *
 * @param Value
 *        Supplies a value, to be add to variable found at specified address.
 *
 * @return Returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicExchangeAdd8(IN VOLATILE PCHAR Address,
                      IN CHAR Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs atomically addition of the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the original value.
 *
 * @param Value
 *        Supplies a value, to be add to variable found at specified address.
 *
 * @return Returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicExchangeAdd16(IN VOLATILE PSHORT Address,
                       IN SHORT Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs atomically addition of the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the original value.
 *
 * @param Value
 *        Supplies a value, to be add to variable found at specified address.
 *
 * @return Returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicExchangeAdd32(IN VOLATILE PLONG Address,
                       IN LONG Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs atomically addition of the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the original value.
 *
 * @param Value
 *        Supplies a value, to be add to variable found at specified address.
 *
 * @return Returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicExchangeAdd64(IN VOLATILE PLONGLONG Address,
                       IN LONGLONG Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs atomically exchange operation.
 *
 * @param Address
 *        Supplies the address of the value to exchange with.
 *
 * @param Exchange
 *        Supplies a pointer to the value to write to the address.
 *
 * @return This routine returns the original value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
PVOID
RtlAtomicExchangePointer(IN VOLATILE PVOID *Address,
                         IN PVOID Exchange)
{
    return (PVOID)__sync_lock_test_and_set(Address, Exchange);
}

/**
 * Performs atomically increment of the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value to increment.
 *
 * @return This routine returns the incremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicIncrement8(IN VOLATILE PCHAR Address)
{
    return __sync_add_and_fetch(Address, 1);
}

/**
 * Performs atomically increment of the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value to increment.
 *
 * @return This routine returns the incremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicIncrement16(IN VOLATILE PSHORT Address)
{
    return __sync_add_and_fetch(Address, 1);
}

/**
 * Performs atomically increment of the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value to increment.
 *
 * @return This routine returns the incremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicIncrement32(IN VOLATILE PLONG Address)
{
    return __sync_add_and_fetch(Address, 1);
}

/**
 * Performs atomically increment of the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value to increment.
 *
 * @return This routine returns the incremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicIncrement64(IN VOLATILE PLONGLONG Address)
{
    return __sync_add_and_fetch(Address, 1);
}

/**
 * Performs an atomic bitwise OR operation on the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise OR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise OR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicOr8(IN VOLATILE PCHAR Address,
             IN CHAR Mask)
{
    return __sync_fetch_and_or(Address, Mask);
}

/**
 * Performs an atomic bitwise OR operation on the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise OR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise OR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicOr16(IN VOLATILE PSHORT Address,
              IN SHORT Mask)
{
    return __sync_fetch_and_or(Address, Mask);
}

/**
 * Performs an atomic bitwise OR operation on the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise OR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise OR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicOr32(IN VOLATILE PLONG Address,
              IN LONG Mask)
{
    return __sync_fetch_and_or(Address, Mask);
}

/**
 * Performs an atomic bitwise OR operation on the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise OR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise OR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicOr64(IN VOLATILE PLONGLONG Address,
              IN LONGLONG Mask)
{
    return __sync_fetch_and_or(Address, Mask);
}

/**
 * Performs an atomic bitwise XOR operation on the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise XOR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise XOR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RtlAtomicXor8(IN VOLATILE PCHAR Address,
              IN CHAR Mask)
{
    return __sync_fetch_and_xor(Address, Mask);
}

/**
 * Performs an atomic bitwise XOR operation on the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise XOR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise XOR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RtlAtomicXor16(IN VOLATILE PSHORT Address,
               IN SHORT Mask)
{
    return __sync_fetch_and_xor(Address, Mask);
}

/**
 * Performs an atomic bitwise XOR operation on the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise XOR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise XOR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RtlAtomicXor32(IN VOLATILE PLONG Address,
               IN LONG Mask)
{
    return __sync_fetch_and_xor(Address, Mask);
}

/**
 * Performs an atomic bitwise XOR operation on the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the bitwise XOR operation is to be performed.
 *
 * @param Mask
 *        Supplies the mask with which the bitwise XOR operation is to be performed
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONGLONG
RtlAtomicXor64(IN VOLATILE PLONGLONG Address,
               IN LONGLONG Mask)
{
    return __sync_fetch_and_xor(Address, Mask);
}
