/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/proc.cc
 * DESCRIPTION:     AMD64 processor-related functionality for the kernel
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Gets the processor block for the currently executing processor.
 *
 * @return This routine returns the current processor block read from the GS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_BLOCK
KE::Processor::GetCurrentProcessorBlock(VOID)
{
    /* Get processor block from GS register */
    return (PKPROCESSOR_BLOCK)AR::CpuFunc::ReadGSQuadWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Self));
}

/**
 * Gets the processor control block for the currently executing processor.
 *
 * @return This routine returns the current processor control block read from the GS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_CONTROL_BLOCK
KE::Processor::GetCurrentProcessorControlBlock(VOID)
{
    return (PKPROCESSOR_CONTROL_BLOCK)AR::CpuFunc::ReadGSQuadWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CurrentPrcb));
}

/**
 * Gets the number of the currently executing processor.
 *
 * @return This routine returns the zero-indexed processor number.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
KE::Processor::GetCurrentProcessorNumber(VOID)
{
    return (ULONG)AR::CpuFunc::ReadGSQuadWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CpuNumber));
}

/**
 * Gets the current thread running on the currently executing processor.
 *
 * @return This routine returns the address of the current thread object.
 *
 * @since NT 3.5
 */
XTAPI
PKTHREAD
KE::Processor::GetCurrentThread(VOID)
{
    return (PKTHREAD)AR::CpuFunc::ReadGSQuadWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb.CurrentThread));
}

/**
 * Saves the current processor state.
 *
 * @param State
 *        Supplies a pointer to the processor state structure.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::SaveProcessorState(OUT PKPROCESSOR_STATE CpuState)
{
    /* Save CR registers */
    CpuState->SpecialRegisters.Cr0 = AR::CpuFunc::ReadControlRegister(0);
    CpuState->SpecialRegisters.Cr2 = AR::CpuFunc::ReadControlRegister(2);
    CpuState->SpecialRegisters.Cr3 = AR::CpuFunc::ReadControlRegister(3);
    CpuState->SpecialRegisters.Cr4 = AR::CpuFunc::ReadControlRegister(4);
    CpuState->SpecialRegisters.Cr8 = AR::CpuFunc::ReadControlRegister(8);

    /* Save DR registers */
    CpuState->SpecialRegisters.KernelDr0 = AR::CpuFunc::ReadDebugRegister(0);
    CpuState->SpecialRegisters.KernelDr1 = AR::CpuFunc::ReadDebugRegister(1);
    CpuState->SpecialRegisters.KernelDr2 = AR::CpuFunc::ReadDebugRegister(2);
    CpuState->SpecialRegisters.KernelDr3 = AR::CpuFunc::ReadDebugRegister(3);
    CpuState->SpecialRegisters.KernelDr6 = AR::CpuFunc::ReadDebugRegister(6);
    CpuState->SpecialRegisters.KernelDr7 = AR::CpuFunc::ReadDebugRegister(7);

    /* Save MSR registers */
    CpuState->SpecialRegisters.MsrGsBase = AR::CpuFunc::ReadModelSpecificRegister(X86_MSR_GSBASE);
    CpuState->SpecialRegisters.MsrGsSwap = AR::CpuFunc::ReadModelSpecificRegister(X86_MSR_KERNEL_GSBASE);
    CpuState->SpecialRegisters.MsrCStar = AR::CpuFunc::ReadModelSpecificRegister(X86_MSR_CSTAR);
    CpuState->SpecialRegisters.MsrLStar = AR::CpuFunc::ReadModelSpecificRegister(X86_MSR_LSTAR);
    CpuState->SpecialRegisters.MsrStar = AR::CpuFunc::ReadModelSpecificRegister(X86_MSR_STAR);
    CpuState->SpecialRegisters.MsrSyscallMask = AR::CpuFunc::ReadModelSpecificRegister(X86_MSR_FMASK);

    /* Save XMM control/status register */
    CpuState->SpecialRegisters.MxCsr = AR::CpuFunc::ReadMxCsrRegister();

    /* Save GDT, IDT, LDT and TaskRegister */
    AR::CpuFunc::StoreGlobalDescriptorTable(&CpuState->SpecialRegisters.Gdtr.Limit);
    AR::CpuFunc::StoreInterruptDescriptorTable(&CpuState->SpecialRegisters.Idtr.Limit);
    AR::CpuFunc::StoreLocalDescriptorTable(&CpuState->SpecialRegisters.Ldtr);
    AR::CpuFunc::StoreTaskRegister(&CpuState->SpecialRegisters.Tr);
}
