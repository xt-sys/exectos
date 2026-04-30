/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/shdata.hh
 * DESCRIPTION:     Kernel Shared Data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_SHDATA_HH
#define __XTOSKRNL_KE_SHDATA_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class SharedData
    {
        private:
            STATIC PKSHARED_DATA KernelSharedData;

        public:
            STATIC XTAPI LARGE_INTEGER GetInterruptTime(VOID);
            STATIC XTAPI PKSHARED_DATA GetKernelSharedData(VOID);
            STATIC XTAPI LARGE_INTEGER GetSystemTime(VOID);
            STATIC XTAPI VOID InitializeKernelSharedData(VOID);
            STATIC XTAPI VOID SetInterruptTime(IN LARGE_INTEGER Time);
            STATIC XTAPI VOID SetSystemTime(IN LARGE_INTEGER Time);
    };
}

#endif /* __XTOSKRNL_KE_SHDATA_HH */
