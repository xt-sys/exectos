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
    return (PKPROCESSOR_BLOCK)AR::CpuFunctions::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Self));
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
    return (PKPROCESSOR_CONTROL_BLOCK)AR::CpuFunctions::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CurrentPrcb));
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
    return (ULONG)AR::CpuFunctions::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, CpuNumber));
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
    return (PKTHREAD)AR::CpuFunctions::ReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb.CurrentThread));
}

/**
 * Retrieves the number of installed and enabled CPUs in the system.
 *
 * @return This routine returns the number of installed CPUs in the system.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
KE::Processor::GetInstalledCpus(VOID)
{
    /* Return number of installed CPUs */
    return InstalledCpus;
}

/**
 * Gets the processor block for the specified processor number.
 *
 * @param CpuNumber
 *        Supplies the zero-indexed processor number.
 *
 * @return This routine returns a pointer to the processor block, or NULLPTR if invalid.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_BLOCK
KE::Processor::GetProcessorBlock(IN ULONG CpuNumber)
{
    /* Check if the requested CPU number is within dynamic bounds */
    if(CpuNumber >= InstalledCpus || ProcessorBlocks == NULLPTR || ProcessorBlocks[CpuNumber] == NULLPTR)
    {
        /* Invalid CPU number, return NULLPTR */
        return NULLPTR;
    }

    /* Return requested processor block */
    return ProcessorBlocks[CpuNumber];
}

/**
 * Initializes the global processor structures by allocating an array of processor block pointers.
 *
 * @return This routine returns a status code indicating the success or failure of the allocation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::Processor::InitializeProcessorBlocks()
{
    PACPI_SYSTEM_INFO SystemInfo;
    XTSTATUS Status;

    /* Save number of CPUs installed */
    HL::Acpi::GetSystemInformation(&SystemInfo);
    InstalledCpus = SystemInfo->CpuCount;

    /* Allocate an array of pointers */
    Status = MM::Allocator::AllocatePool(NonPagedPool,
                                         InstalledCpus * sizeof(PKPROCESSOR_BLOCK),
                                         (PVOID*)&ProcessorBlocks);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to allocate memory, return error */
        return Status;
    }

    /* Zero the array initially */
    RTL::Memory::ZeroMemory(ProcessorBlocks, InstalledCpus * sizeof(PKPROCESSOR_BLOCK));

    /* Register the processor block for the BSP processor */
    KE::Processor::RegisterProcessorBlock(0, KE::Processor::GetCurrentProcessorBlock());

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Registers the hardware APIC ID for the currently executing processor.
 *
 * @param ApicId
 *        Supplies the hardware APIC ID to register in the processor block.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::RegisterHardwareId(IN ULONG HardwareId)
{
    PKPROCESSOR_BLOCK CurrentBlock;

    /* Retrieve the processor block for the executing core */
    CurrentBlock = GetCurrentProcessorBlock();
    if(CurrentBlock != NULLPTR)
    {
        /* Register the hardware identifier for IPI targeting */
        CurrentBlock->HardwareId = HardwareId;
    }
}

/**
 * Registers or deregisters a processor block in the global CPU table.
 *
 * @param CpuNumber
 *        Specifies the logical processor number.
 *
 * @param ProcessorBlock
 *        Supplies a pointer to the processor block. 
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::RegisterProcessorBlock(ULONG CpuNumber,
                                      PKPROCESSOR_BLOCK ProcessorBlock)
{
    /* Check if the requested CPU number is within dynamic bounds */
    if(ProcessorBlocks != NULLPTR && CpuNumber < InstalledCpus)
    {
        /* Register processor block */
        ProcessorBlocks[CpuNumber] = ProcessorBlock;
    }
}

/**
 * Restores the processor's architectural state from the provided context frame back into the hardware trap
 * and exception frames.
 *
 * @param TrapFrame
 *        Supplies a pointer to the trap frame that will receive the volatile processor state and control registers.
 *
 * @param ExceptionFrame
 *        Supplies a pointer to the exception frame that will receive the non-volatile integer and FP registers.
 *
 * @param ContextFrame
 *        Supplies a pointer to the context frame containing the state to be restored.
 *
 * @param ContextFlags
 *        Supplies a bitmask indicating which specific register groups should be restored from the ContextFrame.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::RestoreProcessorContext(IN OUT PKTRAP_FRAME TrapFrame,
                                       IN OUT PKEXCEPTION_FRAME ExceptionFrame,
                                       IN PCONTEXT ContextFrame,
                                       IN ULONG ContextFlags)
{
    UNIMPLEMENTED;
}

/**
 * Restores the processor's low-level hardware control state.
 *
 * @param CpuState
 *        Supplies a pointer to the processor state block containing the previously saved control data.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::RestoreProcessorControlState(IN PKPROCESSOR_STATE CpuState)
{
    /* Restore CR registers */
    AR::CpuFunctions::WriteControlRegister(0, CpuState->SpecialRegisters.Cr0);
    AR::CpuFunctions::WriteControlRegister(2, CpuState->SpecialRegisters.Cr2);
    AR::CpuFunctions::WriteControlRegister(3, CpuState->SpecialRegisters.Cr3);
    AR::CpuFunctions::WriteControlRegister(4, CpuState->SpecialRegisters.Cr4);

    /* Restore DR registers */
    AR::CpuFunctions::WriteDebugRegister(0, CpuState->SpecialRegisters.KernelDr0);
    AR::CpuFunctions::WriteDebugRegister(1, CpuState->SpecialRegisters.KernelDr1);
    AR::CpuFunctions::WriteDebugRegister(2, CpuState->SpecialRegisters.KernelDr2);
    AR::CpuFunctions::WriteDebugRegister(3, CpuState->SpecialRegisters.KernelDr3);
    AR::CpuFunctions::WriteDebugRegister(6, CpuState->SpecialRegisters.KernelDr6);
    AR::CpuFunctions::WriteDebugRegister(7, CpuState->SpecialRegisters.KernelDr7);

    /* Restore GDT, IDT and LDT */
    AR::CpuFunctions::LoadGlobalDescriptorTable(&CpuState->SpecialRegisters.Gdtr.Limit);
    AR::CpuFunctions::LoadInterruptDescriptorTable(&CpuState->SpecialRegisters.Idtr.Limit);
    AR::CpuFunctions::LoadLocalDescriptorTable(CpuState->SpecialRegisters.Ldtr);

    /* Force the TSS descriptor into a non-busy state and restore TaskRegister */
    *(VOLATILE PUCHAR)((ULONG_PTR)CpuState->SpecialRegisters.Gdtr.Base + CpuState->SpecialRegisters.Tr + 5) &= ~0x02;
    AR::CpuFunctions::LoadTaskRegister(CpuState->SpecialRegisters.Tr);
}

/**
 * Restores the executing processor's state.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame that will be populated with the restored volatile state.
 *
 * @param ExceptionFrame
 *        Supplies a pointer to the exception frame that will be populated with the restored non-volatile state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::RestoreProcessorState(OUT PKTRAP_FRAME TrapFrame,
                                     OUT PKEXCEPTION_FRAME ExceptionFrame)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;

    /* Retrieve current processor control block */
    Prcb = GetCurrentProcessorControlBlock();

    /* Restore processor context */
    RestoreProcessorContext(TrapFrame, ExceptionFrame, &Prcb->ProcessorState.ContextFrame,
                            CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS);

    /* Restore processor control registers */
    RestoreProcessorControlState(&Prcb->ProcessorState);
}

/**
 * Constructs a standardized processor context from the hardware trap and exception frames.
 *
 * @param TrapFrame
 *        Supplies a pointer to the trap frame containing volatile processor state, control registers and debug
 *        registers saved at the time of the interrupt or exception.
 *
 * @param ExceptionFrame
 *        Supplies a pointer to the exception frame containing non-volatile integer and floating-point registers.
 *
 * @param ContextFrame
 *        Supplies a pointer to the context frame that will receive the processor state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::SaveProcessorContext(IN PKTRAP_FRAME TrapFrame,
                                    IN PKEXCEPTION_FRAME ExceptionFrame,
                                    IN OUT PCONTEXT ContextFrame)
{
    /* Elevate RunLevel to APC_LEVEL to prevent thread preemption */
    KE::RaiseRunLevel RunLevel(APC_LEVEL);

    /* Check if control registers are requested */
    if(ContextFrame->ContextFlags & CONTEXT_CONTROL)
    {
        /* Copy instruction pointer, stack pointer, and CPU status flags */
        ContextFrame->Flags = TrapFrame->Flags;
        ContextFrame->Ebp = TrapFrame->Ebp;
        ContextFrame->Eip = TrapFrame->Eip;
        ContextFrame->Esp = TrapFrame->Esp;
        ContextFrame->SegCs = TrapFrame->SegCs;
        ContextFrame->SegSs = TrapFrame->SegSs;
    }

    /* Check if general-purpose integer registers are requested */
    if(ContextFrame->ContextFlags & CONTEXT_INTEGER)
    {
        /* Copy integer registers directly from the trap frame */
        ContextFrame->Eax = TrapFrame->Eax;
        ContextFrame->Ebx = TrapFrame->Ebx;
        ContextFrame->Ecx = TrapFrame->Ecx;
        ContextFrame->Edx = TrapFrame->Edx;
        ContextFrame->Edi = TrapFrame->Edi;
        ContextFrame->Esi = TrapFrame->Esi;
    }

    /* Check if segment registers are requested */
    if(ContextFrame->ContextFlags & CONTEXT_SEGMENTS)
    {
        /* Populate segment selectors with standard Ring 3 values */
        TrapFrame->SegDs = KGDT_R3_DATA | RPL_MASK;
        TrapFrame->SegEs = KGDT_R3_DATA | RPL_MASK;
        TrapFrame->SegFs = KGDT_R0_PB;
        TrapFrame->SegGs = 0;
    }

    /* Check if floating-point registers are requested and if running in user mode */
    if((ContextFrame->ContextFlags & CONTEXT_FLOATING_POINT) && ((TrapFrame->SegCs & MODE_MASK) == UserMode))
    {
        /* No user-mode support */
        UNIMPLEMENTED;
    }

    /* Check if debug registers are requested */
    if(ContextFrame->ContextFlags & CONTEXT_DEBUG_REGISTERS)
    {
        /* Copy debug registers */
        ContextFrame->Dr0 = TrapFrame->Dr0;
        ContextFrame->Dr1 = TrapFrame->Dr1;
        ContextFrame->Dr2 = TrapFrame->Dr2;
        ContextFrame->Dr3 = TrapFrame->Dr3;
        ContextFrame->Dr6 = TrapFrame->Dr6;
        ContextFrame->Dr7 = TrapFrame->Dr7;
    }

    /* Check if extended registers are requested and if running in user mode */
    if((ContextFrame->ContextFlags & CONTEXT_EXTENDED_REGISTERS) && ((TrapFrame->SegCs & MODE_MASK) == UserMode))
    {
        /* No user-mode support */
        UNIMPLEMENTED;
    }
}

/**
 * Saves the current processor's low-level hardware control state.
 *
 * @param State
 *        Supplies a pointer to the processor state block that will receive processor's control data.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::SaveProcessorControlState(OUT PKPROCESSOR_STATE CpuState)
{
    /* Save CR registers */
    CpuState->SpecialRegisters.Cr0 = AR::CpuFunctions::ReadControlRegister(0);
    CpuState->SpecialRegisters.Cr2 = AR::CpuFunctions::ReadControlRegister(2);
    CpuState->SpecialRegisters.Cr3 = AR::CpuFunctions::ReadControlRegister(3);
    CpuState->SpecialRegisters.Cr4 = AR::CpuFunctions::ReadControlRegister(4);

    /* Save DR registers */
    CpuState->SpecialRegisters.KernelDr0 = AR::CpuFunctions::ReadDebugRegister(0);
    CpuState->SpecialRegisters.KernelDr1 = AR::CpuFunctions::ReadDebugRegister(1);
    CpuState->SpecialRegisters.KernelDr2 = AR::CpuFunctions::ReadDebugRegister(2);
    CpuState->SpecialRegisters.KernelDr3 = AR::CpuFunctions::ReadDebugRegister(3);
    CpuState->SpecialRegisters.KernelDr6 = AR::CpuFunctions::ReadDebugRegister(6);
    CpuState->SpecialRegisters.KernelDr7 = AR::CpuFunctions::ReadDebugRegister(7);

    /* Save GDT, IDT, LDT and TaskRegister */
    AR::CpuFunctions::StoreGlobalDescriptorTable(&CpuState->SpecialRegisters.Gdtr.Limit);
    AR::CpuFunctions::StoreInterruptDescriptorTable(&CpuState->SpecialRegisters.Idtr.Limit);
    AR::CpuFunctions::StoreLocalDescriptorTable(&CpuState->SpecialRegisters.Ldtr);
    AR::CpuFunctions::StoreTaskRegister(&CpuState->SpecialRegisters.Tr);
}

/**
 * Captures the executing processor's state.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame containing the processor's volatile state.
 *
 * @param ExceptionFrame
 *        Supplies a pointer to the exception frame containing the processor's non-volatile state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Processor::SaveProcessorState(IN PKTRAP_FRAME TrapFrame,
                                  IN PKEXCEPTION_FRAME ExceptionFrame)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;

    /* Retrieve current processor control block */
    Prcb = GetCurrentProcessorControlBlock();

    /* Set context flags to save whole full context and debug registers */
    Prcb->ProcessorState.ContextFrame.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

    /* Save processor context */
    SaveProcessorContext(TrapFrame, ExceptionFrame, &Prcb->ProcessorState.ContextFrame);

    /* Save processor control registers */
    SaveProcessorControlState(&Prcb->ProcessorState);
}
