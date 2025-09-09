/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/crash.hh
 * DESCRIPTION:     System shutdown and kernel panic mechanism
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_CRASH_HH
#define __XTOSKRNL_KE_CRASH_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Crash
    {
        public:
            STATIC XTAPI VOID HaltSystem(VOID);
            STATIC XTAPI VOID Panic(IN ULONG Code);
            STATIC XTAPI VOID PanicEx(IN ULONG Code,
                                      IN ULONG_PTR Parameter1,
                                      IN ULONG_PTR Parameter2,
                                      IN ULONG_PTR Parameter3,
                                      IN ULONG_PTR Parameter4);
    };
}

#endif /* __XTOSKRNL_KE_CRASH_HH */
