/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/apc.hh
 * DESCRIPTION:     Kernel APC objects support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_APC_HH
#define __XTOSKRNL_KE_APC_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Apc
    {
        public:
            STATIC XTAPI VOID CheckApcDelivery(VOID);
            STATIC XTAPI VOID DeliverApc(IN KPROCESSOR_MODE ProcessorMode,
                                         IN PKEXCEPTION_FRAME ExceptionFrame,
                                         IN PKTRAP_FRAME TrapFrame);
            STATIC XTAPI VOID InitializeApc(IN PKAPC Apc,
                                            IN PKTHREAD Thread,
                                            IN KAPC_ENVIRONMENT Environment,
                                            IN PKKERNEL_ROUTINE KernelRoutine,
                                            IN PKRUNDOWN_ROUTINE RundownRoutine,
                                            IN PKNORMAL_ROUTINE NormalRoutine,
                                            IN KPROCESSOR_MODE ApcMode,
                                            IN PVOID Context);
    };
}

#endif /* __XTOSKRNL_KE_APC_HH */
