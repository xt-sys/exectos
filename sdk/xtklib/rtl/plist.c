/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtklib/rtl/plist.c
 * DESCRIPTION:     Linked list manipulation routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtkmapi.h>


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
XTINLINE
VOID
RtlInitializeListHead(IN PLIST_ENTRY ListHead)
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
XTINLINE
VOID
RtlInitializeListHead32(IN PLIST_ENTRY32 ListHead)
{
    ListHead->Blink = PtrToUlong(ListHead);
    ListHead->Flink = PtrToUlong(ListHead);
}

/**
 * This routine inserts an entry at the head of a double linked list.
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
XTINLINE
VOID
RtlInsertHeadList(IN OUT PLIST_ENTRY ListHead,
                  IN OUT PLIST_ENTRY Entry)
{
    Entry->Flink = ListHead->Flink;
    Entry->Blink = ListHead;
    ListHead->Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

/**
 * This routine inserts an entry at the tail of a double linked list.
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
XTINLINE
VOID
RtlInsertTailList(IN OUT PLIST_ENTRY ListHead,
                  IN OUT PLIST_ENTRY Entry)
{
    Entry->Flink = ListHead;
    Entry->Blink = ListHead->Blink;
    ListHead->Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

/**
 * Indicates whether a double linked list structure is empty.
 *
 * @param ListHead
 *        Pointer to a structure that represents the head of the list.
 *
 * @return TRUE if there are currently no entries in the list or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTINLINE
BOOLEAN
RtlListEmpty(PLIST_ENTRY ListHead)
{
    return (ListHead->Flink == ListHead);
}
