/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/cpu.hh
 * DESCRIPTION:     HAL processor support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_CPU_HH
#define __XTOSKRNL_HL_CPU_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Cpu
    {
        private:
            STATIC KAFFINITY ActiveProcessors;
        public:
            STATIC XTAPI VOID InitializeProcessor(VOID);
    };
}

#endif /* __XTOSKRNL_HL_CPU_HH */
