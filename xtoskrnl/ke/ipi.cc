/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/ipi.cc
 * DESCRIPTION:     Interprocessor interrupt support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Handles an IPI interrupt.
 *
 * @param TrapFrame
 *        Supplies a pointer to the processor's trap frame captured at the moment the interrupt was delivered.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KE::Ipi::HandleIpiInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    KRUNLEVEL RunLevel;

    /* Start the interrupt */
    HL::Irq::BeginSystemInterrupt(IPI_LEVEL, &RunLevel);

    /* Call the IPI service routine */
    HandleIpiService(TrapFrame, NULLPTR);

    /* End the interrupt */
    HL::Irq::EndInterrupt(TrapFrame, RunLevel);
}

/**
 * Services an incoming Inter-Processor Interrupt (IPI).
 *
 * @param TrapFrame
 *        Supplies a pointer to the trap frame representing the processor state at the time of the interrupt.
 *
 * @param ExceptionFrame
 *        Supplies a pointer to the exception frame.
 *
 * @return This routine returns TRUE if a standard IPI request (other than IPI_FREEZE) was processed,
 *         or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
KE::Ipi::HandleIpiService(IN PKTRAP_FRAME TrapFrame,
                          IN PKEXCEPTION_FRAME ExceptionFrame)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    LONG_PTR RequestSummary;

    /* Get processor control block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Atomically retrieve and clear the pending IPI request summary */
    RequestSummary = RTL::Atomic::Exchange64((PLONG_PTR)&Prcb->RequestSummary, 0);

    /* Check if an Asynchronous Procedure Call (APC) interrupt was requested */
    if((RequestSummary & IPI_APC) != 0)
    {
        /* Dispatch a software interrupt to process the APC queue */
        HL::Irq::SendSoftwareInterrupt(APC_LEVEL);
    }

    /* Check if a Deferred Procedure Call (DPC) interrupt was requested */
    if((RequestSummary & IPI_DPC) != 0)
    {
        /* Flag the DPC request in the PRCB and dispatch a software interrupt */
        Prcb->DpcInterruptRequested = TRUE;
        HL::Irq::SendSoftwareInterrupt(DISPATCH_LEVEL);
    }

    /* Check if the IPI packet is ready to be processed */
    if((RequestSummary & IPI_PACKET_READY) != 0)
    {
        UNIMPLEMENTED;
    }

    /* Check if a synchronous inter-processor event was requested */
    if((RequestSummary & IPI_SYNC_REQUEST) != 0)
    {
        UNIMPLEMENTED;
    }

    /* Check if a debugger freeze request was issued */
    if((RequestSummary & IPI_FREEZE) != 0)
    {
        /* Suspend the current processor execution */
        KE::Crash::FreezeCurrentExecution(TrapFrame, ExceptionFrame);
    }

    /* Return TRUE if any standard execution requests were serviced, excluding freeze events */
    return (RequestSummary & ~IPI_FREEZE) != 0;
}

/**
 * Broadcasts an Inter-Processor Interrupt (IPI) to all active processors in the system.
 *
 * @param Request
 *        Supplies the logical request operation flags to be merged into the target processors' request summaries.
 *
 * @param Self
 *        Specifies whether the broadcast IPI should also be delivered to the originating processor.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Ipi::SendBroadcastIpi(IN ULONG Request,
                          IN BOOLEAN Self)
{
    PKPROCESSOR_BLOCK CurrentProcessorBlock, TargetProcessorBlock;
    ULONG CpuCount, Index;
    BOOLEAN Interrupts;

    /* Get the number of installed CPUs */
    CpuCount = KE::Processor::GetInstalledCpus();

    /* Check if there are any processors to broadcast to */
    if(CpuCount <= 1 && !Self)
    {
        /* There are no other processors, return */
        return;
    }

    /* Check whether interrupts are enabled */
    Interrupts = AR::CpuFunctions::InterruptsEnabled();

    /* Disable interrupts */
    AR::CpuFunctions::ClearInterruptFlag();

    /* Get the current processor block */
    CurrentProcessorBlock = KE::Processor::GetCurrentProcessorBlock();

    /* Iterate over all logical CPUs */
    for(Index = 0; Index < CpuCount; Index++)
    {
        /* Retrieve the target processor block by its Logical CPU Number */
        TargetProcessorBlock = KE::Processor::GetProcessorBlock(Index);

        /* Only target processors that exist and have successfully started */
        if(TargetProcessorBlock != NULLPTR && TargetProcessorBlock->Started)
        {
            /* Check if this processor originated the broadcast */
            if(TargetProcessorBlock->HardwareId == CurrentProcessorBlock->HardwareId)
            {
                /* Check if this is a self broadcast */
                if(Self)
                {
                    /* Update the logical request summary and dispatch a physical IPI to the current processor */
                    RTL::Atomic::Or64((PLONG_PTR)&TargetProcessorBlock->Prcb.RequestSummary, Request);
                    HL::Pic::SendSelfIpi(APIC_VECTOR_IPI);
                }
            }
            else
            {
                /* Update the logical request summary and dispatch a physical IPI to the target processor */
                RTL::Atomic::Or64((PLONG_PTR)&TargetProcessorBlock->Prcb.RequestSummary, Request);
                HL::Pic::SendIpi(TargetProcessorBlock->HardwareId, APIC_VECTOR_IPI, APIC_DM_FIXED,
                                 APIC_DSH_Destination, APIC_TGM_EDGE);
            }
        }
    }

    /* Check whether interrupts need to be re-enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunctions::SetInterruptFlag();
    }
}

/**
 * Sends an Inter-Processor Interrupt (IPI) to a specific set of target processors.
 *
 * @param Request
 *        Supplies the logical request operation flags to be merged into the target processors' * request summaries.
 *
 * @param TargetSet
 *        Supplies a pointer to the affinity map defining the processors to be interrupted.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Ipi::SendIpi(IN ULONG Request,
                 IN PKAFFINITY_MAP TargetSet)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    ULONG BlockIndex, CpuIndex;
    KAFFINITY AffinityBlock;
    BOOLEAN Interrupts;

    /* Check whether interrupts are enabled */
    Interrupts = AR::CpuFunctions::InterruptsEnabled();

    /* Disable interrupts */
    AR::CpuFunctions::ClearInterruptFlag();

    /* Iterate through all blocks within the target affinity map */
    for(BlockIndex = 0; BlockIndex < TargetSet->Size; BlockIndex++)
    {
        /* Retrieve the current affinity block */
        AffinityBlock = TargetSet->Bitmap[BlockIndex];

        /* Loop until all set bits in the current block have been processed */
        while(AffinityBlock != 0)
        {
            /* Find the index of the lowest set bit representing a target CPU */
            AR::CpuFunctions::ScanForwardBit(&CpuIndex, AffinityBlock);

            /* Retrieve the target processor block */
            ProcessorBlock = KE::Processor::GetProcessorBlock((BlockIndex * 64) + CpuIndex);

            /* Ensure the processor block exists */
            if(ProcessorBlock != NULLPTR)
            {
                /* Update the logical request summary and dispatch the physical IPI */
                RTL::Atomic::Or64((PLONG_PTR)&ProcessorBlock->Prcb.RequestSummary, Request);
                HL::Pic::SendIpi(ProcessorBlock->HardwareId, APIC_VECTOR_IPI, APIC_DM_FIXED,
                                 APIC_DSH_Destination, APIC_TGM_EDGE);
            }

            /* Clear the processed bit to move on to the next CPU */
            AffinityBlock &= ~((KAFFINITY)1 << CpuIndex);
        }
    }

    /* Check whether interrupts need to be re-enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunctions::SetInterruptFlag();
    }
}
