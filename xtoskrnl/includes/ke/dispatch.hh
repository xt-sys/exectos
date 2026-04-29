/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/dispatch.hh
 * DESCRIPTION:     Kernel Thread Dispatcher
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_KE_DISPATCH_HH
#define __XTOSKRNL_KE_DISPATCH_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Dispatcher
    {
        public:
            STATIC XTFASTCALL VOID ExitDispatcher(IN KRUNLEVEL OldRunLevel);
    };
}

#endif /* __XTOSKRNL_KE_DISPATCH_HH */
