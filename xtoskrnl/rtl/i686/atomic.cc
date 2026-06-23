/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/i686/atomic.cc
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
    SINGLE_LIST_HEADER ExpectedHeader, NewHeader;
    PSINGLE_LIST_ENTRY FirstEntry;
    ULONG64 ResultValue;

    /* Load the initial state of the header */
    ExpectedHeader.Alignment = *(VOLATILE PULONGLONG)&ListHead->Alignment;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Extract the entry address */
        FirstEntry = ExpectedHeader.Next.Next;

        /* Check if list is empty */
        if(FirstEntry == NULLPTR)
        {
            /* Return NULLPTR */
            return NULLPTR;
        }

        /* Prepare the new header state */
        NewHeader.Next.Next = NULLPTR;
        NewHeader.Depth = 0;
        NewHeader.Sequence = ExpectedHeader.Sequence;

        /* Perform the compare and swap operation */
        ResultValue = CompareExchange64((VOLATILE PLONG_PTR)&ListHead->Alignment,
                                        (LONG_PTR)ExpectedHeader.Alignment,
                                        (LONG_PTR)NewHeader.Alignment);

        /* Evaluate the CAS result */
        if(ResultValue == ExpectedHeader.Alignment)
        {
            /* Swap succeeded, return the original first entry */
            return FirstEntry;
        }

        /* Collision detected, retry */
        ExpectedHeader.Alignment = ResultValue;
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
    SINGLE_LIST_HEADER ExpectedHeader, NewHeader;
    PSINGLE_LIST_ENTRY FirstEntry;
    ULONGLONG ResultValue;

    /* Load the initial state of the header */
    ExpectedHeader.Alignment = *(VOLATILE PULONGLONG)&ListHead->Alignment;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Extract the entry address */
        FirstEntry = ExpectedHeader.Next.Next;

        /* Check if list is empty */
        if(FirstEntry == NULLPTR)
        {
            /* Return NULLPTR */
            return NULLPTR;
        }

        /* Prepare the new header state */
        NewHeader.Next.Next = FirstEntry->Next;
        NewHeader.Depth = ExpectedHeader.Depth - 1;
        NewHeader.Sequence = ExpectedHeader.Sequence;

        /* Perform the compare and swap operation */
        ResultValue = CompareExchange64((VOLATILE PLONG_PTR)&ListHead->Alignment,
                                        (LONG_PTR)ExpectedHeader.Alignment,
                                        (LONG_PTR)NewHeader.Alignment);

        /* Evaluate the CAS result */
        if(ResultValue == ExpectedHeader.Alignment)
        {
            /* Swap succeeded, return the original first entry */
            return FirstEntry;
        }

        /* Collision detected, retry */
        ExpectedHeader.Alignment = ResultValue;
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
    SINGLE_LIST_HEADER ExpectedHeader, NewHeader;
    PSINGLE_LIST_ENTRY FirstEntry;
    ULONGLONG ResultValue;

    /* Load the initial state of the header */
    ExpectedHeader.Alignment = *(VOLATILE PULONGLONG)&ListHead->Alignment;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Extract the entry address */
        FirstEntry = ExpectedHeader.Next.Next;

        /* Set next entry to the previous first */
        Entry->Next = FirstEntry;

        /* Prepare the new header state */
        NewHeader.Next.Next = Entry;
        NewHeader.Depth = ExpectedHeader.Depth + 1;
        NewHeader.Sequence = ExpectedHeader.Sequence + 1;

        /* Perform the compare and swap operation */
        ResultValue = CompareExchange64((VOLATILE PLONG_PTR)&ListHead->Alignment,
                                        (LONG_PTR)ExpectedHeader.Alignment,
                                        (LONG_PTR)NewHeader.Alignment);

        /* Evaluate the CAS result */
        if(ResultValue == ExpectedHeader.Alignment)
        {
            /* Swap succeeded, return the original first entry */
            return FirstEntry;
        }

        /* Collision detected, retry */
        ExpectedHeader.Alignment = ResultValue;
    }
}
