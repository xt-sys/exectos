/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ps/process.hh
 * DESCRIPTION:     Process Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_PS_PSMGR_HH
#define __XTOSKRNL_PS_PSMGR_HH

#include <xtos.hh>


/* Process and thread management */
namespace PS
{
    class ProcessManager
    {
        private:
            STATIC POBJECT_TYPE ProcessType;
            STATIC POBJECT_TYPE ThreadType;

        public:
            STATIC XTFASTCALL POBJECT_TYPE GetProcessType(VOID);
            STATIC XTFASTCALL POBJECT_TYPE GetThreadType(VOID);
            STATIC XTAPI VOID InitializeProcessManager(VOID);
    };
}

#endif /* __XTOSKRNL_PS_PSMGR_HH */
