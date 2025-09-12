/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/proc.c
 * DESCRIPTION:     I686 processor-related functionality for the kernel
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Gets the processor block for the currently executing processor.
 *
 * @return This routine returns the current processor block read from the FS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_BLOCK
KE::Processor::GetCurrentProcessorBlock(VOID)
{
    /* Get processor block from FS register */
    return (PKPROCESSOR_BLOCK)AR::CpuFunc::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Self));
}

/**
 * Gets the processor control block for the currently executing processor.
 *
 * @return This routine returns the current processor control block read from the FS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_CONTROL_BLOCK
KE::Processor::GetCurrentProcessorControlBlock(VOID)
{
    return (PKPROCESSOR_CONTROL_BLOCK)AR::CpuFunc::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CurrentPrcb));
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
    return (ULONG)AR::CpuFunc::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CpuNumber));
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
    return (PKTHREAD)AR::CpuFunc::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb.CurrentThread));
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

    /* Save DR registers */
    CpuState->SpecialRegisters.KernelDr0 = AR::CpuFunc::ReadDebugRegister(0);
    CpuState->SpecialRegisters.KernelDr1 = AR::CpuFunc::ReadDebugRegister(1);
    CpuState->SpecialRegisters.KernelDr2 = AR::CpuFunc::ReadDebugRegister(2);
    CpuState->SpecialRegisters.KernelDr3 = AR::CpuFunc::ReadDebugRegister(3);
    CpuState->SpecialRegisters.KernelDr6 = AR::CpuFunc::ReadDebugRegister(6);
    CpuState->SpecialRegisters.KernelDr7 = AR::CpuFunc::ReadDebugRegister(7);

    /* Save GDT, IDT, LDT and TaskRegister */
    AR::CpuFunc::StoreGlobalDescriptorTable(&CpuState->SpecialRegisters.Gdtr.Limit);
    AR::CpuFunc::StoreInterruptDescriptorTable(&CpuState->SpecialRegisters.Idtr.Limit);
    AR::CpuFunc::StoreLocalDescriptorTable(&CpuState->SpecialRegisters.Ldtr);
    AR::CpuFunc::StoreTaskRegister(&CpuState->SpecialRegisters.Tr);
}




/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTAPI
PKPROCESSOR_BLOCK
KeGetCurrentProcessorBlock(VOID)
{
    return KE::Processor::GetCurrentProcessorBlock();
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTAPI
PKPROCESSOR_CONTROL_BLOCK
KeGetCurrentProcessorControlBlock(VOID)
{
    return KE::Processor::GetCurrentProcessorControlBlock();
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTAPI
ULONG
KeGetCurrentProcessorNumber(VOID)
{
    return KE::Processor::GetCurrentProcessorNumber();
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTAPI
PKTHREAD
KeGetCurrentThread(VOID)
{
    return KE::Processor::GetCurrentThread();
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTAPI
VOID
KepSaveProcessorState(OUT PKPROCESSOR_STATE CpuState)
{
    KE::Processor::SaveProcessorState(CpuState);
}