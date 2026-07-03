/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/se/privileg.hh
 * DESCRIPTION:     Privileges Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_SE_PRIVILEG_HH
#define __XTOSKRNL_SE_PRIVILEG_HH

#include <xtos.hh>


/* Kernel Security */
namespace SE
{
    class Privileges
    {

        public:
            STATIC XTAPI BOOLEAN CheckSinglePrivilege(LUID PrivilegeValue,
                                                      KPROCESSOR_MODE ProcessorMode);
    };
}

#endif /* __XTOSKRNL_SE_PRIVILEG_HH */
