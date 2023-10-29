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
RtlAtomicAnd64(IN VOLATILE PLONG_PTR Address,
               IN LONG_PTR Mask)
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
RtlAtomicCompareExchange64(IN VOLATILE PLONG_PTR Address,
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
RtlAtomicDecrement64(IN VOLATILE PLONG_PTR Address)
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
RtlAtomicExchange64(IN VOLATILE PLONG_PTR Address,
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
RtlAtomicExchangeAdd64(IN VOLATILE PLONG_PTR Address,
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
RtlAtomicExchangePointer(IN VOLATILE PVOID *Address,
                         IN PVOID Exchange)
{
    __sync_synchronize();
    return (PVOID)__sync_lock_test_and_set(Address, Exchange);
}

/**
 * Removes all entries from single linked list.
 *
 * @param Header
 *        Supplies a pointer to the header of linked list.
 *
 * @return This routine returns a pointer to the original list, or NULL if the list was already empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RtlAtomicFlushSingleList(IN PSINGLE_LIST_HEADER Header)
{
    return (PSINGLE_LIST_ENTRY)RtlAtomicExchange64((PLONG_PTR)&Header->Alignment, (LONGLONG)NULL);
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
RtlAtomicIncrement64(IN VOLATILE PLONG_PTR Address)
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
RtlAtomicOr64(IN VOLATILE PLONG_PTR Address,
              IN LONG_PTR Mask)
{
    return __sync_fetch_and_or(Address, Mask);
}

/**
 * Removes and returns the first entry from single linked list.
 *
 * @param Header
 *        Supplies a pointer to the header of a single linked list.
 *
 * @return This routine returns a pointer to the removed element, or NULL if the list was empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RtlAtomicPopEntrySingleList(IN PSINGLE_LIST_HEADER Header)
{
    PSINGLE_LIST_ENTRY ListHead, FirstEntry, NextEntry;

    /* Save header and first entry */
    ListHead = (PVOID)Header;
    FirstEntry = ListHead->Next;
    do
    {
        /* Check if list is not empty */
        if(!FirstEntry)
        {
            /* Empty list */
            return NULL;
        }

        /* Update link */
        NextEntry = FirstEntry;

        /* Compare and exchange */
        FirstEntry = (PVOID)RtlAtomicCompareExchange64((PLONG_PTR)ListHead,
                                                       (LONG_PTR)FirstEntry->Next,
                                                       (LONG_PTR)FirstEntry);
    } while(FirstEntry != NextEntry);

    /* Return removed element */
    return FirstEntry;
}

/**
 * Inserts new entry at the beginning of single linked list.
 *
 * @param Header
 *        Supplies a pointer to the header of linked list.
 *
 * @param Entry
 *        Supplies a pointer to entry, that will be inserted into linked list.
 *
 * @return This routine returns a pointer to original heading, or NULL if the list was originally empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RtlAtomicPushEntrySingleList(IN PSINGLE_LIST_HEADER Header,
                             IN PSINGLE_LIST_ENTRY Entry)
{
    PSINGLE_LIST_ENTRY ListHead, ListEntry, FirstEntry, NextEntry;

    /* Save header and new entry */
    ListHead = (PVOID)Header;
    ListEntry = (PVOID)Entry;

    /* Save next link in new first element */
    FirstEntry = ListHead->Next;
    do
    {
        /* Update links */
        ListEntry->Next = FirstEntry;
        NextEntry = FirstEntry;

        /* Compare and exchange */
        FirstEntry = (PVOID)RtlAtomicCompareExchange64((PLONG_PTR)ListHead,
                                                       (LONG_PTR)ListEntry,
                                                       (LONG_PTR)FirstEntry);
    } while(FirstEntry != NextEntry);

    /* Return original first element */
    return FirstEntry;
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
RtlAtomicXor64(IN VOLATILE PLONG_PTR Address,
               IN LONG_PTR Mask)
{
    return __sync_fetch_and_xor(Address, Mask);
}
