/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/amd64/atomic.cc
 * DESCRIPTION:     Atomic operations support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Removes all entries from single linked list.
 *
 * @param Header
 *        Supplies a pointer to the header of linked list.
 *
 * @return This routine returns a pointer to the original list, or NULLPTR if the list was already empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RTL::Atomic::FlushSingleList(IN PSINGLE_LIST_HEADER ListHead)
{
    ULONGLONG ExpectedValue, NewValue, ResultValue;
    PSINGLE_LIST_ENTRY FirstEntry;

    /* Load the initial state of the header */
    ExpectedValue = *(VOLATILE PULONGLONG)&ListHead->Alignment;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Extract the entry address */
        FirstEntry = (PSINGLE_LIST_ENTRY)((LONGLONG)ExpectedValue >> 21 );

        /* Check if list is empty */
        if(FirstEntry == NULLPTR)
        {
            /* Return NULLPTR */
            return NULLPTR;
        }

        /* Zero out both the compressed pointer and the Depth */
        NewValue = ExpectedValue & 0x01FF0000ULL;

        /* Perform the compare and swap operation */
        ResultValue = CompareExchange64((VOLATILE PLONG_PTR)&ListHead->Alignment,
                                        (LONG_PTR)ExpectedValue,
                                        (LONG_PTR)NewValue);

        /* Evaluate the CAS result */
        if(ResultValue == ExpectedValue)
        {
            /* Swap succeeded, return the original first entry */
            return FirstEntry;
        }

        /* Collision detected, retry */
        ExpectedValue = ResultValue;
    }
}

/**
 * Removes and returns the first entry from single linked list.
 *
 * @param Header
 *        Supplies a pointer to the header of a single linked list.
 *
 * @return This routine returns a pointer to the removed element, or NULLPTR if the list was empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RTL::Atomic::PopEntrySingleList(IN PSINGLE_LIST_HEADER ListHead)
{
    ULONGLONG AdjustedValue, ExpectedValue, NewValue, ResultValue, Successor;
    PSINGLE_LIST_ENTRY FirstEntry;

    /* Load the initial state of the header */
    ExpectedValue = *(VOLATILE PULONGLONG)&ListHead->Alignment;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Extract the entry address */
        FirstEntry = (PSINGLE_LIST_ENTRY)((LONGLONG)ExpectedValue >> 21 );

        /* Check if list is empty */
        if(FirstEntry == NULLPTR)
        {
            /* Return NULLPTR */
            return NULLPTR;
        }

        /* Shift the successor address */
        Successor = (ULONGLONG)FirstEntry->Next << 21;

        /* Adjust depth and merge sequence */
        AdjustedValue = (ExpectedValue & ~0xFFFFULL) | ((ExpectedValue - 1) & 0xFFFFULL);

        /* Mask to the exact 25 bits */
        AdjustedValue &= 0x01FFFFFFULL;

        /* Merge the packed address with the updated depth and sequence */
        NewValue = Successor | AdjustedValue;

        /* Perform the compare and swap operation */
        ResultValue = CompareExchange64((VOLATILE PLONG_PTR)&ListHead->Alignment,
                                        (LONG_PTR)ExpectedValue,
                                        (LONG_PTR)NewValue);

        /* Evaluate the CAS result */
        if(ResultValue == ExpectedValue)
        {
            /* Swap succeeded, return the original first entry */
            return FirstEntry;
        }

        /* Collision detected, retry */
        ExpectedValue = ResultValue;
    }
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
 * @return This routine returns a pointer to original heading, or NULLPTR if the list was originally empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RTL::Atomic::PushEntrySingleList(IN PSINGLE_LIST_HEADER ListHead,
                                 IN PSINGLE_LIST_ENTRY Entry)
{
    ULONGLONG AdjustedValue, ExpectedValue, NewValue, PackedEntry, ResultValue;
    PSINGLE_LIST_ENTRY FirstEntry;

    /* Shift the 64-bit pointer into the top 43 bits */
    PackedEntry = (ULONGLONG)Entry << 21;

    /* Load the initial state of the header */
    ExpectedValue = *(VOLATILE PULONGLONG)&ListHead->Alignment;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Extract the entry address */
        FirstEntry = (PSINGLE_LIST_ENTRY)((LONGLONG)ExpectedValue >> 21 );

        /* Set next entry to the previous first */
        Entry->Next = FirstEntry;

        /* Adjust depth and merge sequence */
        AdjustedValue = (ExpectedValue & ~0xFFFFULL) | ((ExpectedValue + 1) & 0xFFFFULL);

        /* Increment the sequence field */
        AdjustedValue += 0x10000ULL;

        /* Mask to the exact 25 bits */
        AdjustedValue &= 0x01FFFFFFULL;

        /* Merge the packed address with the updated depth and sequence */
        NewValue = PackedEntry | AdjustedValue;

        /* Perform the compare and swap operation */
        ResultValue = CompareExchange64((VOLATILE PLONG_PTR)&ListHead->Alignment,
                                        (LONG_PTR)ExpectedValue,
                                        (LONG_PTR)NewValue);

        /* Evaluate the CAS result */
        if(ResultValue == ExpectedValue)
        {
            /* Swap succeeded, return the original first entry */
            return FirstEntry;
        }

        /* Collision detected, retry */
        ExpectedValue = ResultValue;
    }
}
