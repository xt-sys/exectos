/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/info.hh
 * DESCRIPTION:     Generic kernel information support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_INFO_HH
#define __XTOSKRNL_KE_INFO_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Info
    {
        public:
            STATIC XTAPI SYSTEM_FIRMWARE_TYPE GetFirmwareType(VOID);
            STATIC XTAPI XTSTATUS GetKernelParameter(IN PCWSTR ParameterName,
                                                      OUT PCWSTR *Parameter);
    };
}

#endif /* __XTOSKRNL_KE_INFO_HH */
