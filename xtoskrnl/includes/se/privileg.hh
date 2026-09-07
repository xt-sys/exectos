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
            STATIC XTAPI BOOLEAN CheckPrivilege(IN OUT PPRIVILEGE_SET RequiredPrivileges,
                                                IN PSECURITY_SUBJECT_CONTEXT SecurityContext,
                                                IN KPROCESSOR_MODE ProcessorMode);
            STATIC XTAPI BOOLEAN CheckSinglePrivilege(LUID PrivilegeValue,
                                                      KPROCESSOR_MODE ProcessorMode);

        private:
            STATIC XTAPI BOOLEAN CheckPrivilege(IN PTOKEN Token,
                                                IN OUT PLUID_AND_ATTRIBUTES Privileges,
                                                IN ULONG PrivilegeCount,
                                                IN ULONG PrivilegeSet,
                                                IN KPROCESSOR_MODE ProcessorMode);
    };
}

#endif /* __XTOSKRNL_SE_PRIVILEG_HH */
