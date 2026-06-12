/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/sysserv.hh
 * DESCRIPTION:     System Services Descriptor Table
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_SYSSERV_HH
#define __XTOSKRNL_KE_SYSSERV_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class SystemServices
    {
        public:
            STATIC XTAPI PKSERVICE_DESCRIPTOR_TABLE GetSystemServicesDescriptorTable(VOID);
    };
}

#endif /* __XTOSKRNL_KE_SYSSERV_HH */
