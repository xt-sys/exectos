/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/runlevel.hh
 * DESCRIPTION:     Running Level management support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_RUNLEVEL_HH
#define __XTOSKRNL_KE_RUNLEVEL_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class RunLevel
    {
        public:
            STATIC XTFASTCALL KRUNLEVEL GetCurrentRunLevel(VOID);
            STATIC XTFASTCALL VOID LowerRunLevel(IN KRUNLEVEL RunLevel);
            STATIC XTFASTCALL KRUNLEVEL RaiseRunLevel(IN KRUNLEVEL RunLevel);
    };
}

#endif /* __XTOSKRNL_KE_RUNLEVEL_HH */
