/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/kd/debug.hh
 * DESCRIPTION:     Kernel Debugger
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KD_DEBUG_HH
#define __XTOSKRNL_KD_DEBUG_HH

#include <xtos.hh>


/* Kernel Debugger */
namespace KD
{
    class Debugger
    {
        public:
            STATIC PKD_PRINT_ROUTINE KdPrint;

        private:
            STATIC BOOLEAN Active;

        public:
            STATIC XTAPI BOOLEAN DebuggerActive(VOID);
            STATIC XTAPI VOID EnterDebugger(IN PKTRAP_FRAME TrapFrame);
            STATIC XTAPI KCONTINUE_STATUS SwitchCpu(VOID);

        private:
            STATIC XTAPI BOOLEAN ProcessCpuStateChange(IN PEXCEPTION_RECORD ExceptionRecord,
                                                       IN OUT PCONTEXT Context,
                                                       IN BOOLEAN SecondChanceException);
    };
}

#endif /* __XTOSKRNL_KD_DEBUG_HH */
