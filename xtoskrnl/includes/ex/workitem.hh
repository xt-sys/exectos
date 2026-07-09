/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ex/workitem.hh
 * DESCRIPTION:     Asynchronous Work Queue Items and System Worker Thread Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_EX_WORKITEM_HH
#define __XTOSKRNL_EX_WORKITEM_HH

#include <xtos.hh>


/* Kernel Executive */
namespace EX
{
    class WorkItem
    {
        public:
            STATIC XTAPI VOID InitializeWorkItem(IN PWORK_QUEUE_ITEM Item,
                                                 IN PWORKER_THREAD_ROUTINE Routine,
                                                 IN PVOID Context);
            STATIC XTAPI VOID QueueWorkItem(IN PWORK_QUEUE_ITEM WorkItem,
                                            IN WORK_QUEUE_TYPE QueueType);
    };
}

#endif /* __XTOSKRNL_EX_WORKITEM_HH */
