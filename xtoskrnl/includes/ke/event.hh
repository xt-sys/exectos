/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/event.hh
 * DESCRIPTION:     Kernel events support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_EVENT_HH
#define __XTOSKRNL_KE_EVENT_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Event
    {
        public:
            STATIC XTAPI VOID ClearEvent(IN PKEVENT Event);
            STATIC XTAPI VOID InitializeEvent(OUT PKEVENT Event,
                                              IN KEVENT_TYPE EventType,
                                              IN BOOLEAN InitialState);
            STATIC XTAPI LONG SetEvent(IN PKEVENT Event,
                                       IN KPRIORITY Increment,
                                       IN BOOLEAN Wait);
    };
}

#endif /* __XTOSKRNL_KE_EVENT_HH */
