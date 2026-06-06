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
    };
}

#endif /* __XTOSKRNL_KD_DEBUG_HH */
