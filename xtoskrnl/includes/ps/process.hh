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
            STATIC PEPROCESS IdleProcess;
            STATIC PEPROCESS SystemProcess;
            STATIC HANDLE SystemProcessHandle;

        public:
            STATIC XTAPI XTSTATUS CreateIdleProcess(IN PKPROCESSOR_CONTROL_BLOCK Prcb);
            STATIC XTAPI XTSTATUS CreateInitialSystemProcess(VOID);
            STATIC XTAPI XTSTATUS CreateSystemProcess(OUT PHANDLE ProcessHandle,
                                                      IN ACCESS_MASK AccessMask,
                                                      IN POBJECT_ATTRIBUTES Attributes);
            STATIC XTAPI VOID DeleteProcess(IN PVOID ProcessObject);
            STATIC XTFASTCALL PEPROCESS GetCurrentProcess(VOID);
            STATIC XTFASTCALL PEPROCESS GetSystemProcess(VOID);
            STATIC XTAPI VOID InitializeIdleProcess(VOID);

        private:
            STATIC XTAPI XTSTATUS CreateKernelProcess(OUT PHANDLE ProcessHandle,
                                                      IN ACCESS_MASK AccessMask,
                                                      IN POBJECT_ATTRIBUTES Attributes,
                                                      IN HANDLE ParentProcess,
                                                      IN ULONG Flags,
                                                      IN HANDLE SectionHandle,
                                                      IN HANDLE DebugPort,
                                                      IN HANDLE ExceptionPort,
                                                      IN BOOLEAN JobMember);
    };
}

#endif /* __XTOSKRNL_PS_PROCESS_HH */
