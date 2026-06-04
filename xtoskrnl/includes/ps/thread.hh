/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ps/thread.hh
 * DESCRIPTION:     Thread Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_PS_THREAD_HH
#define __XTOSKRNL_PS_THREAD_HH

#include <xtos.hh>


/* Process and thread management */
namespace PS
{
    class Thread
    {
        public:
            STATIC XTAPI XTSTATUS CreateIdleThread(IN PKPROCESSOR_CONTROL_BLOCK Prcb,
                                                   IN PVOID Stack);
    };
}

#endif /* __XTOSKRNL_PS_THREAD_HH */
