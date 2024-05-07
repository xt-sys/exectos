/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/proc.c
 * DESCRIPTION:     I686 processor-related functionality for the kernel
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Gets the processor block for the currently executing processor.
 *
 * @return This routine returns the current processor block read from the FS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_BLOCK
KeGetCurrentProcessorBlock(VOID)
{
    /* Get processor block from FS register */
    return (PKPROCESSOR_BLOCK)ArReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Self));
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
KeGetCurrentProcessorControlBlock(VOID)
{
    return (PKPROCESSOR_CONTROL_BLOCK)ArReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CurrentPrcb));
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
KeGetCurrentProcessorNumber(VOID)
{
    return (ULONG)ArReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CpuNumber));
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
KeGetCurrentThread(VOID)
{
    return (PKTHREAD)ArReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb.CurrentThread));
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
KepSaveProcessorState(OUT PKPROCESSOR_STATE CpuState)
{
    /* Save CR registers */
    CpuState->SpecialRegisters.Cr0 = ArReadControlRegister(0);
    CpuState->SpecialRegisters.Cr2 = ArReadControlRegister(2);
    CpuState->SpecialRegisters.Cr3 = ArReadControlRegister(3);
    CpuState->SpecialRegisters.Cr4 = ArReadControlRegister(4);

    /* Save DR registers */
    CpuState->SpecialRegisters.KernelDr0 = ArReadDebugRegister(0);
    CpuState->SpecialRegisters.KernelDr1 = ArReadDebugRegister(1);
    CpuState->SpecialRegisters.KernelDr2 = ArReadDebugRegister(2);
    CpuState->SpecialRegisters.KernelDr3 = ArReadDebugRegister(3);
    CpuState->SpecialRegisters.KernelDr6 = ArReadDebugRegister(6);
    CpuState->SpecialRegisters.KernelDr7 = ArReadDebugRegister(7);

    /* Save GDT, IDT, LDT and TaskRegister */
    ArStoreGlobalDescriptorTable(&CpuState->SpecialRegisters.Gdtr.Limit);
    ArStoreInterruptDescriptorTable(&CpuState->SpecialRegisters.Idtr.Limit);
    ArStoreLocalDescriptorTable(&CpuState->SpecialRegisters.Ldtr);
    ArStoreTaskRegister(&CpuState->SpecialRegisters.Tr);
}
