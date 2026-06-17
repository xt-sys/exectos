/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/lifo.cc
 * DESCRIPTION:     Last In First Out (LIFO) queue implementation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Evaluates whether the LIFO queue is completely empty.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the LIFO queue.
 *
 * @return This routine returns TRUE if the queue is empty, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
RTL::LifoQueue::EmptyList(IN PSINGLE_LIST_ENTRY ListHead)
{
    /* Return the queue status */
    return (ListHead->Next == NULLPTR);
}

/**
 * Retrieves the first entry from the LIFO queue without removing it.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the LIFO queue.
 *
 * @return This routine returns a pointer to the first entry, or NULLPTR if empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RTL::LifoQueue::PeekEntryList(IN PSINGLE_LIST_ENTRY ListHead)
{
    /* Return the entry at the top of the queue */
    return ListHead->Next;
}

/**
 * Removes and returns the entry at the top of the LIFO queue.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the LIFO queue.
 *
 * @return This routine returns a pointer to the removed entry, or NULLPTR if empty.
 *
 * @since XT 1.0
 */
XTFASTCALL
PSINGLE_LIST_ENTRY
RTL::LifoQueue::PopEntryList(IN OUT PSINGLE_LIST_ENTRY ListHead)
{
    PSINGLE_LIST_ENTRY Entry;

    /* Capture the entry currently at the top of the queue */
    Entry = ListHead->Next;

    /* Check if the queue is not empty */
    if(Entry)
    {
        /* Unlink the first entry */
        ListHead->Next = Entry->Next;
    }

    /* Return the captured entry */
    return Entry;
}

/**
 * Pushes an entry onto the top of the LIFO queue.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the LIFO queue.
 *
 * @param Entry
 *        Supplies a pointer to the entry to be pushed onto the queue.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
RTL::LifoQueue::PushEntryList(IN OUT PSINGLE_LIST_ENTRY ListHead,
                              IN PSINGLE_LIST_ENTRY Entry)
{
    /* Link the new entry and update queue head */
    Entry->Next = ListHead->Next;
    ListHead->Next = Entry;
}

/**
 * Searches the LIFO queue to determine if a specific entry is present.
 *
 * @param ListHead
 *        Supplies a pointer to the head of the LIFO queue.
 *
 * @param Entry
 *        Supplies a pointer to the entry being searched for.
 *
 * @return This routine returns TRUE if the entry is found, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
RTL::LifoQueue::SearchEntryList(IN PSINGLE_LIST_ENTRY ListHead,
                                IN PSINGLE_LIST_ENTRY Entry)
{
    PSINGLE_LIST_ENTRY CurrentEntry;

    /* Start traversing from the first entry in the queue */
    CurrentEntry = ListHead->Next;

    /* Iterate through the linked list until the end is reached */
    while(CurrentEntry != NULLPTR)
    {
        /* Check if the current node matches the target entry */
        if(CurrentEntry == Entry)
        {
            /* Entry found within the queue, return TRUE */
            return TRUE;
        }

        /* Move to the next entry in the chain */
        CurrentEntry = CurrentEntry->Next;
    }

    /* Target entry not found, return FALSE */
    return FALSE;
}
