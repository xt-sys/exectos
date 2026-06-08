/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/panic.cc
 * DESCRIPTION:     System shutdown and kernel panic mechanism
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Freezes the execution of the current processor during a fatal system crash or an interactive debugging session.
 *
 * @param TrapFrame
 *        Supplies an optional pointer to the trap frame captured at the moment the IPI_FREEZE was delivered.
 *
 * @param ExceptionFrame
 *        Supplies an optional pointer to the exception frame containing the non-volatile processor state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Crash::FreezeCurrentExecution(IN PKTRAP_FRAME TrapFrame,
                                  IN PKEXCEPTION_FRAME ExceptionFrame)
{
    KCONTINUE_STATUS ContinueStatus;
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    BOOLEAN Interrupts;

    /* Check whether interrupts are enabled */
    Interrupts = AR::CpuFunctions::InterruptsEnabled();

    /* Disable interrupts */
    AR::CpuFunctions::ClearInterruptFlag();

    /* Start a guarded code block */
    {
        /* Raise runlevel to HIGH level */
        KE::RaiseRunLevel RunLevel(HIGH_LEVEL);

        /* Get the processor control block */
        Prcb = KE::Processor::GetCurrentProcessorControlBlock();

        /* Verify that this core was genuinely requested to freeze */
        if(Prcb->IpiFrozen != IPI_FROZEN_STATE_FREEZE)
        {
            /* Spurious call or already thawed, return */
            return;
        }

        /* Acknowledge the freeze request to the initiator */
        Prcb->IpiFrozen = IPI_FROZEN_STATE_FROZEN;

        /* Check if there is a valid trap frame to save the context */
        if(TrapFrame != NULLPTR)
        {
            /* Capture the processor state */
            KE::Processor::SaveProcessorState(TrapFrame, ExceptionFrame);
        }

        /* Enter the stateless polling loop */
        while(Prcb->IpiFrozen != IPI_FROZEN_STATE_THAW)
        {
            /* Check for debugger processor switch */
            if(Prcb->IpiFrozen & IPI_FROZEN_STATE_ACTIVE)
            {
                /* Let the CPU enter the debugger as the active processor */
                ContinueStatus = KD::Debugger::SwitchProcessor();

                /* Set the state back to passively frozen */
                Prcb->IpiFrozen = IPI_FROZEN_STATE_FROZEN;

                /* Check if the status is ContinueSuccess */
                if(ContinueStatus == ContinueSuccess)
                {
                    /* Release the freeze owner to globally resume system execution */
                    FreezeOwner->IpiFrozen = IPI_FROZEN_STATE_THAW;
                }
            }

            /* Mitigate spin-lock starvation and enforce memory coherence */
            AR::CpuFunctions::YieldProcessor();
            AR::CpuFunctions::MemoryBarrier();
        }

        /* Thaw signal received, check if there is a need to restore context */
        if(TrapFrame != NULLPTR)
        {
            /* Restore the processor state */
            KE::Processor::RestoreProcessorState(TrapFrame, ExceptionFrame);
        }

        /* Acknowledge the thaw and mark the processor as fully operational */
        Prcb->IpiFrozen = IPI_FROZEN_STATE_RUNNING;

        /* Flush the Translation Lookaside Buffer (TLB) */
        AR::CpuFunctions::FlushTlb();
    }

    /* Check whether interrupts need to be re-enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunctions::SetInterruptFlag();
    }
}

/**
 * Asserts control over the entire system by freezing all other executing processors.
 *
 * @param TrapFrame
 *        Supplies a pointer to the trap frame of the initiating processor.
 *
 * @param ExceptionFrame
 *        Supplies a pointer to the exception frame of the initiating processor.
 *
 * @return This routine returns the interrupt state prior to the freeze execution.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
KE::Crash::FreezeExecution(IN PKTRAP_FRAME TrapFrame,
                           IN PKEXCEPTION_FRAME ExceptionFrame)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    KRUNLEVEL OldRunLevel;
    ULONG CpuCount, Index;
    BOOLEAN Interrupts;

    /* Check whether interrupts are enabled */
    Interrupts = AR::CpuFunctions::InterruptsEnabled();

    /* Disable interrupts */
    AR::CpuFunctions::ClearInterruptFlag();

    /* Raise runlevel to HIGH level */
    OldRunLevel = KE::RunLevel::RaiseRunLevel(HIGH_LEVEL);

    /* Get processor control block for the executing core */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Check if CPU already owns the freeze */
    if(FreezeOwner == Prcb && Prcb->IpiFrozen == IPI_FROZEN_STATE_OWNER)
    {
        /* Freeze already owned, return interrupt state */
        return Interrupts;
    }

    /* Attempt to establish ownership of the freeze */
    while(RTL::Atomic::CompareExchangePointer((PVOID*)&FreezeOwner, NULLPTR, Prcb))
    {
        /* Freeze owned by another processor, spin until the lock is released */
        while(FreezeOwner != NULLPTR)
        {
            /* Check whether interrupts need to be re-enabled */
            if(Interrupts)
            {
                /* Re-enable interrupts */
                AR::CpuFunctions::SetInterruptFlag();
            }

            /* Process pending IPIs */
            KE::Ipi::HandleIpiService(TrapFrame, ExceptionFrame);

            /* Disable interrupts */
            AR::CpuFunctions::ClearInterruptFlag();
        }
    }

    /* Re-fetch the processor control block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Set the freeze state */
    Prcb->IpiFrozen = IPI_FROZEN_STATE_OWNER | IPI_FROZEN_STATE_ACTIVE;

    /* Get the number of installed CPUs */
    CpuCount = KE::Processor::GetInstalledCpus();

    /* Iterate over all installed CPUs */
    for(Index = 0; Index < CpuCount; Index++)
    {
        /* Get the processor block for next CPU */
        ProcessorBlock = KE::Processor::GetProcessorBlock(Index);

        /* Check if the processor block exists and the processor is running */
        if(ProcessorBlock != NULLPTR && ProcessorBlock->Started)
        {
            /* Do not freeze ourselves */
            if(ProcessorBlock->Prcb.CpuNumber != Prcb->CpuNumber)
            {
                /* Request the target processor to freeze execution */
                ProcessorBlock->Prcb.IpiFrozen = IPI_FROZEN_STATE_FREEZE;
            }
        }
    }

    /* Broadcast the freeze request to all other processors */
    KE::Ipi::SendBroadcastIpi(IPI_FREEZE, FALSE);

    /* Iterate over all installed CPUs to verify their frozen state */
    for(Index = 0; Index < CpuCount; Index++)
    {
        /* Get the processor block for next CPU */
        ProcessorBlock = KE::Processor::GetProcessorBlock(Index);

        /* Check if the processor block exists and the processor is running */
        if(ProcessorBlock != NULLPTR && ProcessorBlock->Started)
        {
            /* Do not wait for ourselves */
            if(ProcessorBlock->Prcb.CpuNumber != Prcb->CpuNumber)
            {
                /* Wait for the target processor to acknowledge the freeze */
                while(ProcessorBlock->Prcb.IpiFrozen != IPI_FROZEN_STATE_FROZEN)
                {
                    /* Mitigate spin-lock starvation */
                    AR::CpuFunctions::YieldProcessor();
                    AR::CpuFunctions::MemoryBarrier();
                }
            }
        }
    }

    /* Save original runlevel and return the original interrupt state */
    RunLevel = OldRunLevel;
    return Interrupts;
}

/**
 * Halts the system.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Crash::HaltSystem(VOID)
{
    /* Enter infinite loop */
    for(;;)
    {
        /* Halt system */
        AR::CpuFunctions::ClearInterruptFlag();
        AR::CpuFunctions::Halt();
    }
}

/**
 * Crashes the system upon detecting a fatal error in which either it is unable to recover or continue to run system.
 *
 * @param Code
 *        Specifies the reason for the kernel panic.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Crash::Panic(IN ULONG Code)
{
    /* Call panic function */
    Panic(Code, 0, 0, 0, 0);
}

/**
 * Crashes the system upon detecting a fatal error in which either it is unable to recover or continue to run system.
 *
 * @param Code
 *        Specifies the reason for the kernel panic.
 *
 * @param Parameter1
 *        Supplies additional information about the kernel panic.
 *
 * @param Parameter2
 *        Supplies additional information about the kernel panic.
 *
 * @param Parameter3
 *        Supplies additional information about the kernel panic.
 *
 * @param Parameter4
 *        Supplies additional information about the kernel panic.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Crash::Panic(IN ULONG Code,
                 IN ULONG_PTR Parameter1,
                 IN ULONG_PTR Parameter2,
                 IN ULONG_PTR Parameter3,
                 IN ULONG_PTR Parameter4)
{
    /* Set kernel panic state */
    KernelPanic = TRUE;

    /* Broadcast IPI_FREEZE to all other processors */
    FreezeExecution(NULLPTR, NULLPTR);

    /* Print error message to debug console */
    KD::DebugIo::KdPrint(L"Fatal System Error: 0x%08lx (0x%zx 0x%zx 0x%zx 0x%zx)\nKernel Panic!\n\n",
                         Code, Parameter1, Parameter2, Parameter3, Parameter4);

    /* Halt system */
    HaltSystem();
}

/**
 * Determines whether the system has experienced a fatal error and entered a kernel panic state.
 *
 * @return This routine returns TRUE if the system has halted due to a kernel panic, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
KE::Crash::SystemCrashed(VOID)
{
    /* Return kernel panic state */
    return KernelPanic;
}

/**
 * Resumes execution of the entire system following a system freeze or an interactive debugging session.
 *
 * @param Interrupts
 *        Supplies the original interrupt state captured prior to the freeze.
 *        This determines if local interrupts should be re-enabled.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Crash::ThawExecution(IN BOOLEAN Interrupts)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    ULONG CpuCount, Index;

    /* Get the processor control block and available CPU count */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();
    CpuCount = KE::Processor::GetInstalledCpus();

    /* Mark the freeze owner processor as running */
    Prcb->IpiFrozen = IPI_FROZEN_STATE_RUNNING;

    /* Iterate over all CPUs to signal them to thaw */
    for(Index = 0; Index < CpuCount; Index++)
    {
        /* Get the processor block for the next CPU */
        ProcessorBlock = KE::Processor::GetProcessorBlock(Index);

        /* Check if the processor block exists and the processor is running */
        if(ProcessorBlock != NULLPTR && ProcessorBlock->Started)
        {
            /* Do not thaw ourselves */
            if(ProcessorBlock->Prcb.CpuNumber != Prcb->CpuNumber)
            {
                /* Request the target processor to thaw execution */
                ProcessorBlock->Prcb.IpiFrozen = IPI_FROZEN_STATE_THAW;
            }
        }
    }

    /* Iterate over all installed CPUs to verify they have resumed */
    for(Index = 0; Index < CpuCount; Index++)
    {
        /* Get the processor block for the next CPU */
        ProcessorBlock = KE::Processor::GetProcessorBlock(Index);

        /* Check if the processor block exists and the processor is running */
        if(ProcessorBlock != NULLPTR && ProcessorBlock->Started)
        {
            /* Do not wait for ourselves */
            if(ProcessorBlock->Prcb.CpuNumber != Prcb->CpuNumber)
            {
                /* Wait for the target processor to acknowledge it is running */
                while(ProcessorBlock->Prcb.IpiFrozen != IPI_FROZEN_STATE_RUNNING)
                {
                    /* Mitigate spin-lock starvation */
                    AR::CpuFunctions::YieldProcessor();
                    AR::CpuFunctions::MemoryBarrier();
                }
            }
        }
    }

    /* Release the global freeze owner lock */
    RTL::Atomic::ExchangePointer((PVOID *)&FreezeOwner, NULLPTR);

    /* Flush the Translation Lookaside Buffer (TLB) */
    AR::CpuFunctions::FlushTlb();

    /* Restore the original runlevel from before the freeze */
    KE::RunLevel::LowerRunLevel(RunLevel);

    /* Check whether interrupts need to be re-enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunctions::SetInterruptFlag();
    }
}
