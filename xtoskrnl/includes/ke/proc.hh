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
            STATIC XTAPI PKPROCESSOR_BLOCK GetProcessorBlock(IN ULONG CpuNumber);
            STATIC XTAPI XTSTATUS InitializeProcessorStructures(IN ULONG CpuCount);
            STATIC XTAPI VOID RegisterHardwareId(IN ULONG HardwareId);
            STATIC XTAPI VOID SaveProcessorState(OUT PKPROCESSOR_STATE CpuState);
    };
}

#endif /* __XTOSKRNL_KE_PROC_HH */
