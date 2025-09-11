/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/dispatch.hh
 * DESCRIPTION:     Dispatching support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_DISPATCH_HH
#define __XTOSKRNL_RTL_DISPATCH_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Dispatcher
    {
        public:
            STATIC XTAPI VOID GetStackLimits(OUT PULONG_PTR StackBase,
                                             OUT PULONG_PTR StackLimit);
    };
}

#endif /* __XTOSKRNL_RTL_DISPATCH_HH */
