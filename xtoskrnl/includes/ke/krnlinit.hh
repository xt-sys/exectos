/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/krnlinit.hh
 * DESCRIPTION:     XTOS Kernel initialization
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_KRNLINIT_HH
#define __XTOSKRNL_KE_KRNLINIT_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class KernelInit
    {
        public:
            STATIC XTAPI VOID InitializeMachine(VOID);
            STATIC XTAPI VOID SwitchBootStack(VOID);

        private:
            STATIC XTAPI VOID InitializeKernel(VOID);
            STATIC XTAPI VOID StartKernel(VOID);
    };
}

#endif /* __XTOSKRNL_KE_KRNLINIT_HH */
