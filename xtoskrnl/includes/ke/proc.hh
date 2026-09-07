/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/proc.hh
 * DESCRIPTION:     Processor-related functionality for the kernel
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_PROC_HH
#define __XTOSKRNL_KE_PROC_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class Processor
    {
        private:
            STATIC ULONG InstalledCpus;
            STATIC PKPROCESSOR_BLOCK *ProcessorBlocks;

        public:
            STATIC XTAPI PKPROCESSOR_BLOCK GetCurrentProcessorBlock(VOID);
            STATIC XTAPI PKPROCESSOR_CONTROL_BLOCK GetCurrentProcessorControlBlock(VOID);
            STATIC XTAPI ULONG GetCurrentProcessorNumber(VOID);
            STATIC XTAPI PKTHREAD GetCurrentThread(VOID);
            STATIC XTAPI ULONG GetInstalledCpus(VOID);
            STATIC XTAPI PKPROCESSOR_BLOCK GetProcessorBlock(IN ULONG CpuNumber);
            STATIC XTAPI XTSTATUS InitializeProcessorBlocks();
            STATIC XTAPI VOID RegisterHardwareId(IN ULONG HardwareId);
            STATIC XTAPI VOID RegisterProcessorBlock(ULONG CpuNumber,
                                                     PKPROCESSOR_BLOCK ProcessorBlock);
            STATIC XTAPI VOID RestoreProcessorContext(IN OUT PKTRAP_FRAME TrapFrame,
                                                      IN OUT PKEXCEPTION_FRAME ExceptionFrame,
                                                      IN PCONTEXT ContextFrame,
                                                      IN ULONG ContextFlags);
            STATIC XTAPI VOID RestoreProcessorControlState(IN PKPROCESSOR_STATE CpuState);
            STATIC XTAPI VOID RestoreProcessorState(OUT PKTRAP_FRAME TrapFrame,
                                                    OUT PKEXCEPTION_FRAME ExceptionFrame);
            STATIC XTAPI VOID SaveProcessorContext(IN PKTRAP_FRAME TrapFrame,
                                                   IN PKEXCEPTION_FRAME ExceptionFrame,
                                                   IN OUT PCONTEXT ContextRecord);
            STATIC XTAPI VOID SaveProcessorControlState(OUT PKPROCESSOR_STATE CpuState);
            STATIC XTAPI VOID SaveProcessorState(IN PKTRAP_FRAME TrapFrame,
                                                 IN PKEXCEPTION_FRAME ExceptionFrame);
    };
}

#endif /* __XTOSKRNL_KE_PROC_HH */
