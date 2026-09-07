/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/ipi.hh
 * DESCRIPTION:     Interprocessor interrupt support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_IPI_HH
#define __XTOSKRNL_KE_IPI_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Ipi
    {
        public:
            STATIC XTCDECL VOID HandleIpiInterrupt(IN PKTRAP_FRAME TrapFrame);
            STATIC XTAPI BOOLEAN HandleIpiService(IN PKTRAP_FRAME TrapFrame,
                                                  IN PKEXCEPTION_FRAME ExceptionFrame);
            STATIC XTAPI VOID SendBroadcastIpi(IN ULONG Request,
                                               IN BOOLEAN Self);
            STATIC XTAPI VOID SendIpi(IN ULONG Request,
                                      IN PKAFFINITY_MAP TargetSet);
    };
}

#endif /* __XTOSKRNL_KE_IPI_HH */
