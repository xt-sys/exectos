/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/obmgr.hh
 * DESCRIPTION:     Object Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_OBMGR_HH
#define __XTOSKRNL_OB_OBMGR_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class Manager
    {
        private:
            STATIC GENERAL_LOOKASIDE CreateInfoList;
            STATIC GENERAL_LOOKASIDE NameBufferList;

        public:
            STATIC XTAPI XTSTATUS InitializeObjectManager(VOID);
            STATIC XTAPI VOID InitializeSystemLookasideList(VOID);
    };
}

#endif /* __XTOSKRNL_OB_OBMGR_HH */
