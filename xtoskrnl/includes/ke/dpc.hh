/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/dpc.hh
 * DESCRIPTION:     Deferred Procedure Call (DPC) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_DPC_HH
#define __XTOSKRNL_KE_DPC_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Dpc
    {
        public:
            STATIC XTAPI VOID InitializeDpc(IN PKDPC Dpc,
                                            IN PKDEFERRED_ROUTINE DpcRoutine,
                                            IN PVOID DpcContext);
            STATIC XTAPI VOID InitializeThreadedDpc(IN PKDPC Dpc,
                                                    IN PKDEFERRED_ROUTINE DpcRoutine,
                                                    IN PVOID DpcContext);
            STATIC XTAPI VOID SetTargetProcessor(IN PKDPC Dpc,
                                                 IN CCHAR Number);
            STATIC XTAPI VOID SignalCallDone(IN PVOID SystemArgument);
            STATIC XTAPI BOOLEAN SignalCallSynchronize(IN PVOID SystemArgument);

        private:
            STATIC XTFASTCALL VOID RetireList(IN PKPROCESSOR_CONTROL_BLOCK Prcb);
    };
}

#endif /* __XTOSKRNL_KE_DPC_HH */
