/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/llist.cc
 * DESCRIPTION:     Doubly linked list manipulation routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Retrieves the first entry from a doubly linked list without removing it from the list.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that serves as the list header.
 *
 * @return This routine returns a pointer to the first entry in the list, or NULLPTR if the list is empty.
 *
 * @since XT 1.0
 */
XTCDECL
PLIST_ENTRY
RTL::LinkedList::GetFirstEntry(IN PLIST_ENTRY ListHead)
{
    /* Check if the list is empty */
    if(ListEmpty(ListHead))
    {
        /* Empty list, return NULLPTR */
        return NULLPTR;
    }

    /* Return first entry in the list */
    return ListHead->Flink;
}

/**
 * Initializes a structure representing the head of a doubly linked list.
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
RTL::LinkedList::InitializeListHead(IN PLIST_ENTRY ListHead)
{
    /* Initialize list head */
    ListHead->Blink = ListHead;
    ListHead->Flink = ListHead;
}

/**
 * Initializes a structure representing the head of a 32bit doubly linked list.
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
RTL::LinkedList::InitializeListHead32(IN PLIST_ENTRY32 ListHead)
{
    /* Initialize list head */
    ListHead->Blink = PtrToUlong(ListHead);
    ListHead->Flink = PtrToUlong(ListHead);
}

/**
 * Inserts an entry at the head of a doubly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the list.
 *
 * @param Entry
 *        Supplies a pointer to the entry that will be inserted in the list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::LinkedList::InsertHeadList(IN OUT PLIST_ENTRY ListHead,
                                IN PLIST_ENTRY Entry)
{
    /* Insert entry at the head of the list */
    Entry->Flink = ListHead->Flink;
    Entry->Blink = ListHead;
    ListHead->Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

/**
 * Inserts an entry at the tail of a doubly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the list.
 *
 * @param Entry
 *        Supplies a pointer to the entry that will be inserted in the list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::LinkedList::InsertTailList(IN OUT PLIST_ENTRY ListHead,
                                IN PLIST_ENTRY Entry)
{
    /* Insert entry at the tail of the list */
    Entry->Flink = ListHead;
    Entry->Blink = ListHead->Blink;
    ListHead->Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

/**
 * Indicates whether a doubly linked list structure is empty, or not initialized at all.
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
RTL::LinkedList::ListEmpty(IN PLIST_ENTRY ListHead)
{
    /* Check if the list is empty */
    return (((ListHead->Flink == NULLPTR) && (ListHead->Blink == NULLPTR)) || (ListHead->Flink == ListHead));
}

/**
 * Detects a loop in a doubly linked list.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that represents the head of the list.
 *
 * @return TRUE if linked list contains a loop or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
RTL::LinkedList::ListLoop(IN PLIST_ENTRY ListHead)
{
    PLIST_ENTRY SlowEntry, FastEntry;

    /* Check if list exists */
    if(ListHead == NULLPTR)
    {
        /* No loop in non-existen list */
        return FALSE;
    }

    /* Make both references pointing to the start of the list */
    FastEntry = ListHead;
    SlowEntry = ListHead;

    /* Iterate through the linked list to find a loop */
    while(SlowEntry != NULLPTR && FastEntry != NULLPTR && FastEntry->Flink != NULLPTR)
    {
        /* Move slow and fast pointers by one and two positions accordingly */
        SlowEntry = SlowEntry->Flink;
        FastEntry = FastEntry->Flink->Flink;

        /* Compare both pointers */
        if(SlowEntry == FastEntry)
        {
            /* Loop found */
            return TRUE;
        }
    }

    /* No loop found */
    return FALSE;
}

/**
 * Removes an entry from a doubly linked list.
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
RTL::LinkedList::RemoveEntryList(IN PLIST_ENTRY Entry)
{
    /* Remove entry from the list */
    Entry->Flink->Blink = Entry->Blink;
    Entry->Blink->Flink = Entry->Flink;
}

/**
 * Splices a doubly linked list at the head of another list. The source list is reinitialized to empty.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that represents the head of the list.
 *
 * @param SpliceList
 *        Supplies a pointer to a structure that represents the head of the list that will be spliced.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::LinkedList::SpliceHeadList(IN OUT PLIST_ENTRY ListHead,
                                IN OUT PLIST_ENTRY SpliceList)
{
    PLIST_ENTRY FirstEntry, LastEntry;

    /* Check if the list to splice is empty */
    if(SpliceList->Flink == SpliceList)
    {
        /* Nothing to splice, return */
        return;
    }

    /* Get first and last entries of the list to splice */
    FirstEntry = SpliceList->Flink;
    LastEntry = SpliceList->Blink;

    /* Splice the list at the head of destination */
    FirstEntry->Blink = ListHead;
    LastEntry->Flink = ListHead->Flink;
    ListHead->Flink->Blink = LastEntry;
    ListHead->Flink = FirstEntry;

    /* Reinitialize the source list to empty */
    SpliceList->Blink = SpliceList;
    SpliceList->Flink = SpliceList;
}

/**
 * Splices a doubly linked list at the tail of another list. The source list is reinitialized to empty.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the destination list.
 *
 * @param SpliceList
 *        Supplies a pointer to the head of the list that will be spliced.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::LinkedList::SpliceTailList(IN OUT PLIST_ENTRY ListHead,
                                IN OUT PLIST_ENTRY SpliceList)
{
    PLIST_ENTRY FirstEntry, LastEntry;

    /* Check if the list to splice is empty */
    if(SpliceList->Flink == SpliceList)
    {
        /* Nothing to splice, return */
        return;
    }

    /* Get first and last entries of the list to splice */
    FirstEntry = SpliceList->Flink;
    LastEntry = SpliceList->Blink;

    /* Splice the list at the tail of destination */
    FirstEntry->Blink = ListHead->Blink;
    LastEntry->Flink = ListHead;
    ListHead->Blink->Flink = FirstEntry;
    ListHead->Blink = LastEntry;

    /* Reinitialize the source list to empty */
    SpliceList->Blink = SpliceList;
    SpliceList->Flink = SpliceList;
}
