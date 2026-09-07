/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/dispobj.hh
 * DESCRIPTION:     Kernel Thread Dispatcher Objects
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_DISPOBJ_HH
#define __XTOSKRNL_KE_DISPOBJ_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class DispatcherObject
    {
        public:
            STATIC XTFASTCALL VOID SatisfyWaitingMutexObject(IN PKMUTEX MutexObject,
                                                             IN PKTHREAD Thread);
            STATIC XTFASTCALL VOID SatisfyWaitingNonMutexObject(IN PDISPATCHER_HEADER MutexObject);
            STATIC XTFASTCALL VOID SatisfyWaitingObject(IN PDISPATCHER_HEADER MutexObject,
                                                        IN PKTHREAD Thread);
    };
}

#endif /* __XTOSKRNL_KE_DISPOBJ_HH */
