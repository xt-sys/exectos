/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ex/rundown.hh
 * DESCRIPTION:     Rundown protection mechanism
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_EX_RUNDOWN_HH
#define __XTOSKRNL_EX_RUNDOWN_HH

#include <xtos.hh>


/* Architecture-specific Library */
namespace EX
{
    class Rundown
    {
        public:
            STATIC XTFASTCALL BOOLEAN AcquireProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);
            STATIC XTFASTCALL VOID CompleteProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);
            STATIC XTFASTCALL VOID InitializeProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);
            STATIC XTFASTCALL VOID ReInitializeProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);
            STATIC XTFASTCALL VOID ReleaseProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);
            STATIC XTFASTCALL VOID WaitForProtectionRelease(IN PEX_RUNDOWN_REFERENCE Descriptor);
    };
}

#endif /* __XTOSKRNL_EX_RUNDOWN_HH */
