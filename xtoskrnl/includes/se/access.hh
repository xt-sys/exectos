/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/se/access.hh
 * DESCRIPTION:     Security access state check routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_SE_ACCESS_HH
#define __XTOSKRNL_SE_ACCESS_HH

#include <xtos.hh>


/* Kernel Security */
namespace SE
{
    class Access
    {
        public:
            STATIC XTFASTCALL ACCESS_MASK ComputeDeniedAccesses(IN ACCESS_MASK GrantedAccess,
                                                                IN ACCESS_MASK DesiredAccess);
            STATIC XTFASTCALL ACCESS_MASK ComputeGrantedAccesses(IN ACCESS_MASK GrantedAccess,
                                                                 IN ACCESS_MASK DesiredAccess);
    };
}

#endif /* __XTOSKRNL_SE_ACCESS_HH */
