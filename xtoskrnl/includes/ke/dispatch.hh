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
            STATIC XTFASTCALL BOOLEAN SwitchContext(IN PKTHREAD CurrentThread,
                                                    IN KRUNLEVEL RunLevel);
            STATIC XTAPI VOID UpdateRunTime(IN PKTRAP_FRAME TrapFrame,
                                            IN KRUNLEVEL RunLevel);

        private:
            STATIC XTFASTCALL BOOLEAN SwitchThreadContext(IN PKTHREAD CurrentThread,
                                                          IN BOOLEAN ApcBypass);
            STATIC XTFASTCALL BOOLEAN SwitchThreadStack(IN PKTHREAD CurrentThread,
                                                        IN KRUNLEVEL RunLevel);
    };
}

#endif /* __XTOSKRNL_KE_DISPATCH_HH */
