/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/llist.cc
 * DESCRIPTION:     Linked list manipulation routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * This routine initializes a structure representing the head of a double-linked list.
 *
 * @param ListHead
 *        Pointer to a structure that serves as the list header.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::LinkedList::InitializeListHead(IN PLIST_ENTRY ListHead)
{
    ListHead->Blink = ListHead;
    ListHead->Flink = ListHead;
}

/**
 * This routine initializes a structure representing the head of a 32bit double-linked list.
 *
 * @param ListHead
 *        Pointer to a structure that serves as the list header.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::LinkedList::InitializeListHead32(IN PLIST_ENTRY32 ListHead)
{
    ListHead->Blink = PtrToUlong(ListHead);
    ListHead->Flink = PtrToUlong(ListHead);
}

/**
 * This routine inserts an entry at the head of a doubly linked list.
 *
 * @param ListHead
 *        Pointer to the head of the list.
 *
 * @param Entry
 *        Pointer to the entry that will be inserted in the list.
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
    Entry->Flink = ListHead->Flink;
    Entry->Blink = ListHead;
    ListHead->Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

/**
 * This routine inserts an entry at the tail of a doubly linked list.
 *
 * @param ListHead
 *        Pointer to the head of the list.
 *
 * @param Entry
 *        Pointer to the entry that will be inserted in the list.
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
    Entry->Flink = ListHead;
    Entry->Blink = ListHead->Blink;
    ListHead->Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

/**
 * Indicates whether a doubly linked list structure is empty, or not initialized at all.
 *
 * @param ListHead
 *        Pointer to a structure that represents the head of the list.
 *
 * @return TRUE if there are currently no entries in the list or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
RTL::LinkedList::ListEmpty(IN PLIST_ENTRY ListHead)
{
    return (BOOLEAN)(((ListHead->Flink == NULL) && (ListHead->Blink == NULL)) || (ListHead->Flink == ListHead));
}

/**
 * This routine detects a loop in a doubly linked list.
 *
 * @param ListHead
 *        Pointer to a structure that represents the head of the list.
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
    if(ListHead == NULL)
    {
        /* No loop in non-existen list */
        return FALSE;
    }

    /* Make both references pointing to the start of the list */
    FastEntry = ListHead;
    SlowEntry = ListHead;

    /* Iterate through the linked list to find a loop */
    while(SlowEntry != NULL && FastEntry != NULL && FastEntry->Flink != NULL)
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
 * This routine removes an entry from a doubly linked list.
 *
 * @param Entry
 *        Pointer to the entry that will be removed from the list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
RTL::LinkedList::RemoveEntryList(IN PLIST_ENTRY Entry)
{
    Entry->Flink->Blink = Entry->Blink;
    Entry->Blink->Flink = Entry->Flink;
}
