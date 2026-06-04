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
        public:
            STATIC XTAPI VOID CreateIdleProcess(IN PKPROCESSOR_CONTROL_BLOCK Prcb);
    };
}

#endif /* __XTOSKRNL_PS_PROCESS_HH */
