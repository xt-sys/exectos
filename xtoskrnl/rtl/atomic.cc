/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/atomic.cc
 * DESCRIPTION:     Atomic operations support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Performs an atomic addition on the 8-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the addition is to be performed.
 *
 * @param Value
 *        Supplies the value to be added.
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
CHAR
RTL::Atomic::Add8(IN PCHAR Address,
                  IN CHAR Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs an atomic addition on the 16-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the addition is to be performed.
 *
 * @param Value
 *        Supplies the value to be added.
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
SHORT
RTL::Atomic::Add16(IN PSHORT Address,
                   IN SHORT Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs an atomic addition on the 32-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the addition is to be performed.
 *
 * @param Value
 *        Supplies the value to be added.
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG
RTL::Atomic::Add32(IN PLONG Address,
                   IN LONG Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs an atomic addition on the 64-bit value.
 *
 * @param Address
 *        Supplies the address of the value on which the addition is to be performed.
 *
 * @param Value
 *        Supplies the value to be added.
 *
 * @return This routine returns the initial value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG_PTR
RTL::Atomic::Add64(IN PLONG_PTR Address,
                   IN LONG_PTR Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs an atomic addition on the pointer value.
 *
 * @param Address
 *        Supplies the address of the pointer on which the addition is to be performed.
 *
 * @param Value
 *        Supplies the value (in bytes) to be added to the pointer.
 *
 * @return This routine returns the initial pointer value at the given address.
 *
 * @since XT 1.0
 */
XTFASTCALL
PVOID
RTL::Atomic::AddPointer(IN PVOID *Address,
                        IN PVOID Value)
{
    return (PVOID)__sync_fetch_and_add(Address, Value);
}

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
RTL::Atomic::And8(IN PCHAR Address,
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
RTL::Atomic::And16(IN PSHORT Address,
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
RTL::Atomic::And32(IN PLONG Address,
                   IN LONG Mask)
{
    return __sync_fetch_and_and(Address, Mask);
}

/**
 * Performs an atomic bitwise AND operation on the 32-bit or 64-bit value depending or architecture.
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
LONG_PTR
RTL::Atomic::And64(IN PLONG_PTR Address,
                   IN LONG_PTR Mask)
{
    return __sync_fetch_and_and(Address, Mask);
}

/**
 * Performs an atomic test of the specified bit of the specified long value and resets it to 0.
 *
 * @param Base
 *        Supplies a pointer to the variable.
 *
 * @param Offset
 *        Specifies the bit position to be tested.
 *
 * @return This routine returns the original value of the specified bit.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
RTL::Atomic::BitTestAndReset(IN PLONG Base,
                             IN LONG Offset)
{
    return (__atomic_fetch_and(Base, ~(1l << Offset), __ATOMIC_SEQ_CST) >> Offset) & 1;
}

/**
 * Performs an atomic test of the specified bit of the specified 64-bit long value and resets it to 0.
 *
 * @param Base
 *        Supplies a pointer to the variable.
 *
 * @param Offset
 *        Specifies the bit position to be tested.
 *
 * @return This routine returns the original value of the specified bit.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
RTL::Atomic::BitTestAndReset64(IN PLONGLONG Base,
                               IN LONGLONG Offset)
{
    return (__atomic_fetch_and(Base, ~(1ll << Offset), __ATOMIC_SEQ_CST) >> Offset) & 1;
}

/**
 * Performs an atomic test of the specified bit of the specified long value and sets it to 1.
 *
 * @param Base
 *        Specifies a pointer to the variable.
 *
 * @param Offset
 *        Specifies the bit position to be tested.
 *
 * @return This routine returns a value of the specified bit.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
RTL::Atomic::BitTestAndSet(IN PLONG Base,
                           IN LONG Offset)
{
    return (__atomic_fetch_or(Base, 1l << Offset, __ATOMIC_SEQ_CST) >> Offset) & 1;
}

/**
 * Performs an atomic test of the specified bit of the specified 64-bit long value and sets it to 1.
 *
 * @param Base
 *        Specifies a pointer to the variable.
 *
 * @param Offset
 *        Specifies the bit position to be tested.
 *
 * @return This routine returns a value of the specified bit.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
RTL::Atomic::BitTestAndSet64(IN PLONGLONG Base,
                             IN LONGLONG Offset)
{
    return (__atomic_fetch_or(Base, 1ll << Offset, __ATOMIC_SEQ_CST) >> Offset) & 1;
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
RTL::Atomic::CompareExchange8(IN PCHAR Address,
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
RTL::Atomic::CompareExchange16(IN PSHORT Address,
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
RTL::Atomic::CompareExchange32(IN PLONG Address,
                               IN LONG Comperand,
                               IN LONG Exchange)
{
    return __sync_val_compare_and_swap(Address, Comperand, Exchange);
}

/**
 * Performs atomically compare exchange operation on the 32-bit or 64-bit value depending or architecture.
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
LONG_PTR
RTL::Atomic::CompareExchange64(IN PLONG_PTR Address,
                               IN LONG_PTR Comperand,
                               IN LONG_PTR Exchange)
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
RTL::Atomic::CompareExchangePointer(IN PVOID *Address,
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
RTL::Atomic::Decrement8(IN PCHAR Address)
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
RTL::Atomic::Decrement16(IN PSHORT Address)
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
RTL::Atomic::Decrement32(IN PLONG Address)
{
    return __sync_sub_and_fetch(Address, 1);
}

/**
 * Performs atomically decrement of the 32-bit or 64-bit value depending or architecture.
 *
 * @param Address
 *        Supplies the address of the value to decrement.
 *
 * @return This routine returns the decremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG_PTR
RTL::Atomic::Decrement64(IN PLONG_PTR Address)
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
RTL::Atomic::Exchange8(IN PCHAR Address,
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
RTL::Atomic::Exchange16(IN PSHORT Address,
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
RTL::Atomic::Exchange32(IN PLONG Address,
                        IN LONG Exchange)
{
    return __sync_lock_test_and_set(Address, Exchange);
}

/**
 * Performs atomically operation on the 32-bit or 64-bit value depending or architecture.
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
LONG_PTR
RTL::Atomic::Exchange64(IN PLONG_PTR Address,
                        IN LONG_PTR Exchange)
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
RTL::Atomic::ExchangeAdd8(IN PCHAR Address,
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
RTL::Atomic::ExchangeAdd16(IN PSHORT Address,
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
RTL::Atomic::ExchangeAdd32(IN PLONG Address,
                           IN LONG Value)
{
    return __sync_fetch_and_add(Address, Value);
}

/**
 * Performs atomically addition of the 32-bit or 64-bit value depending or architecture.
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
LONG_PTR
RTL::Atomic::ExchangeAdd64(IN PLONG_PTR Address,
                           IN LONG_PTR Value)
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
RTL::Atomic::ExchangePointer(IN PVOID *Address,
                             IN PVOID Exchange)
{
    __sync_synchronize();
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
RTL::Atomic::Increment8(IN PCHAR Address)
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
RTL::Atomic::Increment16(IN PSHORT Address)
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
RTL::Atomic::Increment32(IN PLONG Address)
{
    return __sync_add_and_fetch(Address, 1);
}

/**
 * Performs atomically increment of the 32-bit or 64-bit value depending or architecture.
 *
 * @param Address
 *        Supplies the address of the value to increment.
 *
 * @return This routine returns the incremented value.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG_PTR
RTL::Atomic::Increment64(IN PLONG_PTR Address)
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
RTL::Atomic::Or8(IN PCHAR Address,
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
RTL::Atomic::Or16(IN PSHORT Address,
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
RTL::Atomic::Or32(IN PLONG Address,
                  IN LONG Mask)
{
    return __sync_fetch_and_or(Address, Mask);
}

/**
 * Performs an atomic bitwise OR operation on the 32-bit or 64-bit value depending or architecture.
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
LONG_PTR
RTL::Atomic::Or64(IN PLONG_PTR Address,
                  IN LONG_PTR Mask)
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
RTL::Atomic::Xor8(IN PCHAR Address,
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
RTL::Atomic::Xor16(IN PSHORT Address,
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
RTL::Atomic::Xor32(IN PLONG Address,
                   IN LONG Mask)
{
    return __sync_fetch_and_xor(Address, Mask);
}

/**
 * Performs an atomic bitwise XOR operation on the 32-bit or 64-bit value depending or architecture.
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
LONG_PTR
RTL::Atomic::Xor64(IN PLONG_PTR Address,
                   IN LONG_PTR Mask)
{
    return __sync_fetch_and_xor(Address, Mask);
}
