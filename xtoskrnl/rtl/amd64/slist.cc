/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/amd64/slist.cc
 * DESCRIPTION:     Singly linked list manipulation routines for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
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

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Extract pointer from Region */
        return (PSINGLE_LIST_ENTRY)(ListHead->Region & ~0xFLL);
    }
    else
    {
        /* Decompress and return the pointer */
        return (PSINGLE_LIST_ENTRY)(((ULONG64)ListHead & 0xFFFFF8000000000FULL) |
                                    ((ULONG64)ListHead->Header8.NextEntry << 4));
    }
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
    ListHead->Region = 0;
    ListHead->Header16.HeaderType = 1;
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
    OriginalEntry = RTL::SinglyList::GetFirstEntry(ListHead);

    /* Insert entry at the head of the list */
    Entry->Next = OriginalEntry;

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Update pointer and increment depth and sequence */
        ListHead->Region = ((ULONG64)Entry & ~0xFLL) | (ListHead->Region & 0xFLL);
        ListHead->Header16.Depth++;
        ListHead->Header16.Sequence++;
    }
    else
    {
        /* Update pointer and increment depth and sequence */
        ListHead->Header8.NextEntry = ((ULONG64)Entry >> 4) & 0x7FFFFFFFFFULL;
        ListHead->Header8.Depth++;
        ListHead->Header8.Sequence++;
    }

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
        OriginalEntry = NULLPTR;

        /* Check if the header is initialized as a modern 16-byte header */
        if(ListHead->Header16.HeaderType)
        {
            /* Insert entry at the head */
            ListHead->Region = ((ULONG64)Entry & ~0xFLL) | (ListHead->Region & 0xFLL);
        }
        else
        {
            /* Insert entry at the head */
            ListHead->Header8.NextEntry = ((ULONG64)Entry >> 4) & 0x7FFFFFFFFFULL;
        }
    }
    else
    {
        /* Traverse the list to find the last entry */
        CurrentEntry = RTL::SinglyList::GetFirstEntry(ListHead);
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

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Increment list depth and sequence */
        ListHead->Header16.Depth++;
        ListHead->Header16.Sequence++;
    }
    else
    {
        /* Increment list depth and sequence */
        ListHead->Header8.Depth++;
        ListHead->Header8.Sequence++;
    }

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
    /* Check if the list is uninitialized */
    if(ListHead == NULLPTR)
    {
        /* Empty list, return TRUE */
        return TRUE;
    }

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Check if the list is empty */
        return ((ListHead->Region & ~0xFLL) == 0);
    }
    else
    {
        /* Check if the list is empty */
        return (ListHead->Header8.NextEntry == 0);
    }
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

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Return the list depth */
        return (USHORT)ListHead->Header16.Depth;
    }
    else
    {
        /* Return the list depth */
        return (USHORT)ListHead->Header8.Depth;
    }
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
    PSINGLE_LIST_ENTRY FirstEntry, PreviousEntry;

    /* Check if the list is empty */
    if(ListEmpty(ListHead))
    {
        /* List is empty, nothing to remove, return */
        return;
    }

    /* Retrieve the first entry dynamically */
    FirstEntry = RTL::SinglyList::GetFirstEntry(ListHead);

    /* Check if the entry is the first one */
    if(FirstEntry == Entry)
    {
        /* Check if the header is initialized as a modern 16-byte header */
        if(ListHead->Header16.HeaderType)
        {
            /* Remove the first entry and decrement depth */
            ListHead->Region = ((ULONG64)Entry->Next & ~0xFLL) | (ListHead->Region & 0xFLL);
            ListHead->Header16.Depth--;
        }
        else
        {
            /* Remove the first entry and decrement depth */
            ListHead->Header8.NextEntry = ((ULONG64)Entry->Next >> 4) & 0x7FFFFFFFFFULL;
            ListHead->Header8.Depth--;
        }

        /* Nothing else to do, return */
        return;
    }

    /* Find the previous entry */
    PreviousEntry = FirstEntry;
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

    /* Remove the entry */
    PreviousEntry->Next = Entry->Next;

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Decrement depth */
        ListHead->Header16.Depth--;
    }
    else
    {
        /* Decrement depth */
        ListHead->Header8.Depth--;
    }
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
    PSINGLE_LIST_ENTRY LastEntry, OriginalEntry, SpliceFirstEntry;

    /* Store the original first entry */
    OriginalEntry = RTL::SinglyList::GetFirstEntry(ListHead);

    /* Check if the list to splice is empty */
    if(ListEmpty(SpliceList))
    {
        /* Nothing to splice, return original first entry */
        return OriginalEntry;
    }

    /* Find the last entry of the list to splice */
    SpliceFirstEntry = RTL::SinglyList::GetFirstEntry(SpliceList);
    LastEntry = SpliceFirstEntry;
    while(LastEntry->Next != NULLPTR)
    {
        /* Move to the next entry */
        LastEntry = LastEntry->Next;
    }

    /* Splice the list at the head of destination */
    LastEntry->Next = OriginalEntry;

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Update destination list header pointers and values */
        ListHead->Region = ((ULONG64)SpliceFirstEntry & ~0xFLL) | (ListHead->Region & 0xFLL);
        ListHead->Header16.Depth += RTL::SinglyList::QueryListDepth(SpliceList);
        ListHead->Header16.Sequence++;

        /* Reinitialize the source list and preserve format flags */
        SpliceList->Region &= 0xFLL;
        SpliceList->Header16.Depth = 0;
    }
    else
    {
        /* Update destination list header pointers and values */
        ListHead->Header8.NextEntry = ((ULONG64)SpliceFirstEntry >> 4) & 0x7FFFFFFFFFULL;
        ListHead->Header8.Depth += RTL::SinglyList::QueryListDepth(SpliceList);
        ListHead->Header8.Sequence++;

        /* Reinitialize the source list and preserve format flags */
        SpliceList->Header8.NextEntry = 0;
        SpliceList->Header8.Depth = 0;
    }

    /* Return the original first entry */
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
    PSINGLE_LIST_ENTRY LastEntry, OriginalEntry, SpliceFirstEntry;

    /* Get the first entry of the list to splice */
    SpliceFirstEntry = RTL::SinglyList::GetFirstEntry(SpliceList);

    /* Check if the destination list is empty */
    if(ListEmpty(ListHead))
    {
        /* Destination is empty, original last entry is NULLPTR */
        OriginalEntry = NULLPTR;

        /* Check if the header is initialized as a modern 16-byte header */
        if(ListHead->Header16.HeaderType)
        {
            /* Move the splice list to the destination head */
            ListHead->Region = ((ULONG64)SpliceFirstEntry & ~0xFLL) | (ListHead->Region & 0xFLL);
        }
        else
        {
            /* Move the splice list to the destination head */
            ListHead->Header8.NextEntry = ((ULONG64)SpliceFirstEntry >> 4) & 0x7FFFFFFFFFULL;
        }
    }
    else
    {
        /* Find the last entry of the destination list */
        LastEntry = RTL::SinglyList::GetFirstEntry(ListHead);
        while(LastEntry->Next != NULLPTR)
        {
            /* Move to the next entry */
            LastEntry = LastEntry->Next;
        }

        /* Store the original last entry */
        OriginalEntry = LastEntry;

        /* Splice the list at the tail of destination */
        LastEntry->Next = SpliceFirstEntry;
    }

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Update depth and sequence of the destination list */
        ListHead->Header16.Depth += RTL::SinglyList::QueryListDepth(SpliceList);
        ListHead->Header16.Sequence++;

        /* Reinitialize the source list to empty while preserving format flags */
        SpliceList->Region &= 0xFLL;
        SpliceList->Header16.Depth = 0;
    }
    else
    {
        /* Update depth and sequence of the destination list */
        ListHead->Header8.Depth += RTL::SinglyList::QueryListDepth(SpliceList);
        ListHead->Header8.Sequence++;

        /* Reinitialize the source list to empty while preserving format flags */
        SpliceList->Header8.NextEntry = 0;
        SpliceList->Header8.Depth = 0;
    }

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

    /* Get the first entry dynamically */
    Entry = RTL::SinglyList::GetFirstEntry(ListHead);

    /* Check if the header is initialized as a modern 16-byte header */
    if(ListHead->Header16.HeaderType)
    {
        /* Remove entry from the list and decrement depth */
        ListHead->Region = ((ULONG64)Entry->Next & ~0xFLL) | (ListHead->Region & 0xFLL);
        ListHead->Header16.Depth--;
    }
    else
    {
        /* Remove entry from the list and decrement depth */
        ListHead->Header8.NextEntry = ((ULONG64)Entry->Next >> 4) & 0x7FFFFFFFFFULL;
        ListHead->Header8.Depth--;
    }

    /* Return the first entry */
    return Entry;
}
