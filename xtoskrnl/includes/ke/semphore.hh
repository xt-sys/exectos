/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/semphore.hh
 * DESCRIPTION:     Semaphores support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_SEMPHORE_HH
#define __XTOSKRNL_KE_SEMPHORE_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Semaphore
    {
        public:
            STATIC XTAPI VOID InitializeSemaphore(IN PKSEMAPHORE Semaphore,
                                                  IN LONG Count,
                                                  IN LONG Limit);
            STATIC XTAPI LONG ReadState(IN PKSEMAPHORE Semaphore);
            STATIC XTAPI LONG ReleaseSemaphore(IN PKSEMAPHORE Semaphore,
                                               IN KPRIORITY Increment,
                                               IN LONG Adjustment,
                                               IN BOOLEAN Wait);
    };
}

#endif /* __XTOSKRNL_KE_SEMPHORE_HH */
