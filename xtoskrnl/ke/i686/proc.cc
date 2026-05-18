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
