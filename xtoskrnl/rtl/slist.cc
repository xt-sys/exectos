/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/slist.cc
 * DESCRIPTION:     Singly linked list manipulation routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Retrieves the first entry from a singly linked list without removing it from the list.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that serves as the list header.
 *
 * @return This routine returns a pointer to the first entry in the list, or NULLPTR if the list is empty.
 *
 * @since XT 1.0
 */
XTCDECL
PSINGLE_LIST_ENTRY
RTL::SinglyList::GetFirstEntry(IN PSINGLE_LIST_HEADER ListHead)
{
    /* Check if the list is empty */
    if(ListEmpty(ListHead))
    {
        /* Empty list, return NULLPTR */
        return NULLPTR;
    }

    /* Return first entry in the list */
    return ListHead->Next.Next;
}

/**
 * Initializes a structure representing the head of a singly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that serves as the list header.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::SinglyList::InitializeListHead(IN PSINGLE_LIST_HEADER ListHead)
{
    /* Initialize the singly linked list head */
    ListHead->Alignment = 0;
}

/**
 * Inserts an entry at the head of a singly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the list.
 *
 * @param Entry
 *        Supplies a pointer to the entry that will be inserted in the list.
 *
 * @return This routine returns a pointer to the original first entry in the list.
 *
 * @since XT 1.0
 */
XTCDECL
PSINGLE_LIST_ENTRY
RTL::SinglyList::InsertHeadList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                IN PSINGLE_LIST_ENTRY Entry)
{
    PSINGLE_LIST_ENTRY OriginalEntry;

    /* Store the original first entry */
    OriginalEntry = ListHead->Next.Next;

    /* Insert entry at the head of the list and increment depth and sequence */
    Entry->Next = ListHead->Next.Next;
    ListHead->Next.Next = Entry;
    ListHead->Depth++;
    ListHead->Sequence++;

    /* Return original first entry */
    return OriginalEntry;
}

/**
 * Inserts an entry at the tail of a singly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the list.
 *
 * @param Entry
 *        Supplies a pointer to the entry that will be inserted in the list.
 *
 * @return This routine returns a pointer to the original last entry in the list.
 *
 * @since XT 1.0
 */
XTCDECL
PSINGLE_LIST_ENTRY
RTL::SinglyList::InsertTailList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                IN PSINGLE_LIST_ENTRY Entry)
{
    PSINGLE_LIST_ENTRY CurrentEntry, OriginalEntry;

    /* Set Next pointer of the new entry to NULLPTR */
    Entry->Next = NULLPTR;

    /* Check if the list is empty */
    if(ListEmpty(ListHead))
    {
        /* Store the original last entry */
        OriginalEntry = ListHead->Next.Next;

        /* Insert entry at the head */
        ListHead->Next.Next = Entry;
    }
    else
    {
        /* Traverse the list to find the last entry */
        CurrentEntry = ListHead->Next.Next;
        while(CurrentEntry->Next != NULLPTR)
        {
            /* Move to the next entry */
            CurrentEntry = CurrentEntry->Next;
        }

        /* Store the original last entry */
        OriginalEntry = CurrentEntry;

        /* Insert entry at the tail */
        CurrentEntry->Next = Entry;
    }

    /* Increment list depth and sequence */
    ListHead->Depth++;
    ListHead->Sequence++;

    /* Return original last entry */
    return OriginalEntry;
}

/**
 * Indicates whether a singly linked list structure is empty, or not initialized at all.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that represents the head of the list.
 *
 * @return This routine returns TRUE if there are currently no entries in the list or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
RTL::SinglyList::ListEmpty(IN PSINGLE_LIST_HEADER ListHead)
{
    /* Check if the list is empty */
    return (ListHead == NULLPTR || ListHead->Next.Next == NULLPTR);
}

/**
 * Queries the current depth (number of entries) of a singly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that represents the head of the singly linked list.
 *
 * @return This routine returns the number of entries currently in the list, or zero if the list head is NULLPTR.
 *
 * @since XT 1.0
 */
XTAPI
USHORT
RTL::SinglyList::QueryListDepth(IN PSINGLE_LIST_HEADER ListHead)
{
    /* Check if the list head is initialized and valid */
    if(ListHead == NULLPTR)
    {
        /* Return zero */
        return 0;
    }

    /* Return the list depth */
    return ListHead->Depth;
}

/**
 * Removes an entry from a singly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the list.
 *
 * @param Entry
 *        Supplies a pointer to the entry that will be removed from the list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::SinglyList::RemoveEntryList(IN PSINGLE_LIST_HEADER ListHead,
                                 IN PSINGLE_LIST_ENTRY Entry)
{
    PSINGLE_LIST_ENTRY PreviousEntry;

    /* Check if the list is empty */
    if(ListEmpty(ListHead))
    {
        /* List is empty, nothing to remove, return */
        return;
    }

    /* Check if the entry is the first one */
    if(ListHead->Next.Next == Entry)
    {
        /* Remove the first entry and decrement depth */
        ListHead->Next.Next = Entry->Next;
        ListHead->Depth--;

        /* Nothing else to do, return */
        return;
    }

    /* Find the previous entry */
    PreviousEntry = ListHead->Next.Next;
    while(PreviousEntry->Next != Entry)
    {
        /* Move to the next entry */
        PreviousEntry = PreviousEntry->Next;

        /* Check if we reached the end of the list */
        if(PreviousEntry == NULLPTR)
        {
            /* Entry not found, return */
            return;
        }
    }

    /* Remove the entry and decrement depth */
    PreviousEntry->Next = Entry->Next;
    ListHead->Depth--;
}

/**
 * Splices a singly linked list at the head of another list. The source list is reinitialized to empty.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that represents the head of the list.
 *
 * @param SpliceList
 *        Supplies a pointer to a structure that represents the head of the list that will be spliced.
 *
 * @return This routine returns a pointer to the original first entry in the list.
 *
 * @since XT 1.0
 */
XTCDECL
PSINGLE_LIST_ENTRY
RTL::SinglyList::SpliceHeadList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                IN OUT PSINGLE_LIST_HEADER SpliceList)
{
    PSINGLE_LIST_ENTRY LastEntry, OriginalEntry;

    /* Store the original last entry */
    OriginalEntry = ListHead->Next.Next;

    /* Check if the list to splice is empty */
    if(ListEmpty(SpliceList))
    {
        /* Nothing to splice, return original first entry */
        return OriginalEntry;
    }

    /* Find the last entry of the list to splice */
    LastEntry = SpliceList->Next.Next;
    while(LastEntry->Next != NULLPTR)
    {
        /* Move to the next entry */
        LastEntry = LastEntry->Next;
    }

    /* Splice the list at the head of destination */
    LastEntry->Next = ListHead->Next.Next;
    ListHead->Next.Next = SpliceList->Next.Next;

    /* Update depth and sequence of the destination list */
    ListHead->Depth += SpliceList->Depth;
    ListHead->Sequence++;

    /* Reinitialize the source list to empty */
    SpliceList->Next.Next = NULLPTR;
    SpliceList->Depth = 0;

    /* Return the original last entry */
    return OriginalEntry;
}

/**
 * Splices a singly linked list at the tail of another list. The source list is reinitialized to empty.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the destination list.
 *
 * @param SpliceList
 *        Supplies a pointer to the head of the list that will be spliced.
 *
 * @return This routine returns a pointer to the original last entry in the list.
 *
 * @since XT 1.0
 */
XTCDECL
PSINGLE_LIST_ENTRY
RTL::SinglyList::SpliceTailList(IN OUT PSINGLE_LIST_HEADER ListHead,
                                IN OUT PSINGLE_LIST_HEADER SpliceList)
{
    PSINGLE_LIST_ENTRY LastEntry, OriginalEntry;

    /* Check if the destination list is empty */
    if(ListEmpty(ListHead))
    {
        /* Destination is empty, original last entry is NULLPTR */
        OriginalEntry = NULLPTR;

        /* Move the splice list to the destination head */
        ListHead->Next.Next = SpliceList->Next.Next;
    }
    else
    {
        /* Find the last entry of the destination list */
        LastEntry = ListHead->Next.Next;
        while(LastEntry->Next != NULLPTR)
        {
            /* Move to the next entry */
            LastEntry = LastEntry->Next;
        }

        /* Store the original last entry */
        OriginalEntry = LastEntry;

        /* Splice the list at the tail of destination */
        LastEntry->Next = SpliceList->Next.Next;
    }

    /* Update depth and sequence of the destination list */
    ListHead->Depth += SpliceList->Depth;
    ListHead->Sequence++;

    /* Reinitialize the source list to empty */
    SpliceList->Next.Next = NULLPTR;
    SpliceList->Depth = 0;

    /* Return the original last entry */
    return OriginalEntry;
}

/**
 * Retrieves the first entry from a singly linked list and removes it from the list.
 *
 * @param ListHead
 *        Pointer to a structure that serves as the list header.
 *
 * @return This routine returns a pointer to the first entry in the list, or NULLPTR if the list is empty.
 *
 * @since XT 1.0
 */
XTCDECL
PSINGLE_LIST_ENTRY
RTL::SinglyList::TakeFirstEntry(IN PSINGLE_LIST_HEADER ListHead)
{
    PSINGLE_LIST_ENTRY Entry;

    /* Check if the list is empty */
    if(ListEmpty(ListHead))
    {
        /* List is empty, return NULLPTR */
        return NULLPTR;
    }

    /* Get the first entry */
    Entry = ListHead->Next.Next;

    /* Remove entry from the list */
    ListHead->Next.Next = Entry->Next;
    ListHead->Depth--;

    /* Return the first entry */
    return Entry;
}
