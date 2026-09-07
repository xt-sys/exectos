/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/schedule.hh
 * DESCRIPTION:     XT Kernel Thread Scheduler
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_SCHEDULE_HH
#define __XTOSKRNL_KE_SCHEDULE_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Scheduler
    {
        public:
            STATIC XTFASTCALL VOID InsertDeferredReadyList(IN PKTHREAD Thread);
            STATIC XTAPI VOID ProcessQuantumEnd(VOID);
            STATIC XTAPI VOID ProcessReadyThread(IN PKTHREAD Thread);
            STATIC XTFASTCALL VOID QueueReadyThread(IN PKTHREAD Thread,
                                                    IN PKPROCESSOR_CONTROL_BLOCK Prcb);
            STATIC XTAPI VOID ReadyThread(IN PKTHREAD Thread);
    };
}

#endif /* __XTOSKRNL_KE_SCHEDULE_HH */
