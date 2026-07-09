/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/workitem.cc
 * DESCRIPTION:     Asynchronous Work Queue Items and System Worker Thread Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Initializes a work queue item.
 *
 * @param Item
 *        Supplies a pointer to the work queue item to be initialized.
 *
 * @param Routine
 *        Supplies the worker routine to be executed.
 *
 * @param Context
 *        Supplies the context parameter to pass to the worker routine.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::WorkItem::InitializeWorkItem(IN PWORK_QUEUE_ITEM Item,
                                 IN PWORKER_THREAD_ROUTINE Routine,
                                 IN PVOID Context)
{
    /* Initialize the work item fields */
    Item->WorkerRoutine = Routine;
    Item->Parameter = Context;

    /* Initialize the list entry links */
    Item->List.Flink = NULLPTR;
}

/**
 * Queues a work item to be executed by a system worker thread.
 *
 * @param WorkItem
 *        Supplies a pointer to the initialized work queue item.
 *
 * @param QueueType
 *        Supplies the type of work queue to insert the item into.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::WorkItem::QueueWorkItem(IN PWORK_QUEUE_ITEM WorkItem,
                            IN WORK_QUEUE_TYPE QueueType)
{
    /* Not implemented */
    UNIMPLEMENTED;
}
