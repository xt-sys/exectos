/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ps/process.hh
 * DESCRIPTION:     Process Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_PS_PROCESS_HH
#define __XTOSKRNL_PS_PROCESS_HH

#include <xtos.hh>


/* Process and thread management */
namespace PS
{
    class Process
    {
        private:
            STATIC PEPROCESS SystemProcess;

        public:
            STATIC XTAPI XTSTATUS CreateIdleProcess(IN PKPROCESSOR_CONTROL_BLOCK Prcb);
            STATIC XTAPI VOID DeleteProcess(IN PVOID ProcessObject);
            STATIC XTFASTCALL PEPROCESS GetCurrentProcess(VOID);
            STATIC XTFASTCALL PEPROCESS GetSystemProcess(VOID);
    };
}

#endif /* __XTOSKRNL_PS_PROCESS_HH */
