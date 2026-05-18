/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/cpu.cc
 * DESCRIPTION:     HAL x86 (i686/AMD64) processor support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Initializes the processor.
 *
 * @param CpuNumber
 *        Supplies the number of the CPU, that is being initialized.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Cpu::InitializeProcessor(VOID)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    KAFFINITY Affinity;

    /* Get current processor block */
    ProcessorBlock = KE::Processor::GetCurrentProcessorBlock();

    /* Set initial stall factor, CPU number and mask interrupts */
    ProcessorBlock->StallScaleFactor = INITIAL_STALL_FACTOR;
    ProcessorBlock->Idr = 0xFFFFFFFF;

    /* Set processor affinity */
    Affinity = (KAFFINITY) 1 << ProcessorBlock->CpuNumber;

    /* Apply affinity to a set of processors */
    ActiveProcessors |= Affinity;

    /* Initialize APIC for this processor */
    HL::Pic::InitializePic();

    /* Set the APIC running level */
    HL::RunLevel::SetRunLevel(KE::Processor::GetCurrentProcessorBlock()->RunLevel);
}

/**
 * Wakes up and initializes all Application Processors (APs) and transitions them into the active kernel.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Cpu::StartAllProcessors(VOID)
{
    ULONG CpuNumber, Index, MaxCpus, SipiVector, Timeout, TrampolinePages;
    PVOID CpuStructures, TrampolineAddress, TrampolineCode;
    ULONG_PTR AllocationSize, TrampolineCodeSize;
    PPROCESSOR_START_BLOCK StartBlock;
    PKPROCESSOR_BLOCK ProcessorBlock;
    PACPI_SYSTEM_INFO SysInfo;
    WCHAR ParameterValue[16];
    XTSTATUS Status;

    /* Determine the maximum number of CPUs to start */
    HL::Acpi::GetSystemInformation(&SysInfo);
    MaxCpus = SysInfo->CpuCount;

    /* Check if user forced a specific CPU limit */
    if(KE::BootInformation::GetKernelParameterValue(L"MAXCPUS", ParameterValue, 16) == STATUS_SUCCESS)
    {
        /* Convert string value to number */
        Status = RTL::WideString::WideStringToNumber(ParameterValue, 0, &MaxCpus);
        if(Status == STATUS_SUCCESS)
        {
            /* Ensure safe boundaries */
            if(MaxCpus == 0)
            {
                /* Fallback to 1 CPU (BSP) */
                MaxCpus = 1;
            }
        }
        else
        {
            /* Failed to parse value, fallback to ACPI value */
            MaxCpus = SysInfo->CpuCount;
        }
    }

    /* Check if single core CPU or set a CPU limit */
    if(MaxCpus == 1 || SysInfo->CpuCount == 1)
    {
        /* Single core CPU, return success */
        return STATUS_SUCCESS;
    }

    /* Get trampoline information */
    AR::ProcessorSupport::GetTrampolineInformation(TrampolineApStartup, &TrampolineCode, &TrampolineCodeSize);

    /* Verify trampoline information */
    if(TrampolineCode == NULLPTR || TrampolineCodeSize == 0)
    {
        /* Failed to get trampoline information, return error */
        return STATUS_UNSUCCESSFUL;
    }

    /* Compute trampoline memory allocation size (trampoline + processor start block + temporary stack) */
    AllocationSize = TrampolineCodeSize + sizeof(PROCESSOR_START_BLOCK) + 512;
    TrampolinePages = (ULONG)(ROUND_UP(AllocationSize, MM_PAGE_SIZE) / MM_PAGE_SIZE);

    /* Allocate real mode memory for AP trampoline */
    Status = MM::HardwarePool::AllocateRealModeMemory(TrampolinePages, &TrampolineAddress);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to allocate memory, print error message and return error */
        DebugPrint(L"Failed to allocate %lu pages for AP Trampoline!\n", TrampolinePages);
        return Status;
    }

    /* Copy trampoline code to low memory */
    RTL::Memory::CopyMemory(TrampolineAddress, TrampolineCode, TrampolineCodeSize);

    /* Get start block address relative to trampoline address */
    StartBlock = (PPROCESSOR_START_BLOCK)((PUCHAR)TrampolineAddress + TrampolineCodeSize);

    /* Get SIPI vector */
    SipiVector = (ULONG)((ULONG_PTR)TrampolineAddress >> 12);

    /* Loop over all CPUs */
    CpuNumber = 0;
    for(Index = 0; Index < SysInfo->CpuCount; Index++)
    {
        /* Check if destination CPU is the BSP */
        if(SysInfo->CpuInfo[Index].ApicId == HL::Pic::GetCpuApicId())
        {
            /* Skip current CPU */
            continue;
        }

        /* Increment CPU number */
        CpuNumber++;

        /* Verify if the CPU limit has been reached */
        if((CpuNumber) >= MaxCpus)
        {
            /* Maximum allowed CPUs reached, break the loop */
            break;
        }

        /* Allocate memory for the processor structures (Stacks, GDT, and Processor Block) */
        Status = MM::KernelPool::AllocateProcessorStructures(&CpuStructures);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to allocate memory, unmap memory and return error */
            MM::HardwarePool::UnmapHardwareMemory(TrampolineAddress, TrampolinePages, TRUE);
            return Status;
        }

        /* Get ProcessorBlock and Stack address */
        AR::ProcessorSupport::InitializeProcessorStructures(CpuStructures, NULLPTR, NULLPTR, &ProcessorBlock,
                                                            &StartBlock->Stack, NULLPTR, NULLPTR);

        /* Set processor number directly in the processor block */
        ProcessorBlock->CpuNumber = CpuNumber;

        /* Save processor block in the array */
        KE::Processor::RegisterProcessorBlock(CpuNumber, ProcessorBlock);

        /* Initialize processor start block */
        StartBlock->Cr3 = AR::CpuFunc::ReadControlRegister(3);
        StartBlock->Cr4 = AR::CpuFunc::ReadControlRegister(4);
        StartBlock->EntryPoint = (PVOID)&KE::KernelInit::BootstrapApplicationProcessor;
        StartBlock->ProcessorStructures = CpuStructures;
        StartBlock->Started = FALSE;

        /* Memory barrier */
        AR::CpuFunc::MemoryBarrier();

        /* Send INIT IPI and wait for 10ms */
        HL::Pic::SendIpi(SysInfo->CpuInfo[Index].ApicId, 0, APIC_DM_INIT, APIC_DSH_Destination, APIC_TGM_EDGE);
        HL::Timer::StallExecution(10000);

        /* Send STARTUP IPI (SIPI) and wait for 200us */
        HL::Pic::SendIpi(SysInfo->CpuInfo[Index].ApicId, SipiVector, APIC_DM_STARTUP, APIC_DSH_Destination, APIC_TGM_EDGE);
        HL::Timer::StallExecution(200);

        /* Send STARTUP IPI (SIPI) again */
        HL::Pic::SendIpi(SysInfo->CpuInfo[Index].ApicId, SipiVector, APIC_DM_STARTUP, APIC_DSH_Destination, APIC_TGM_EDGE);

        /* Wait until the processor has started or timeout expires */
        Timeout = 0;
        while(!StartBlock->Started && Timeout < 100000)
        {
            /* Yield processor and wait for 10us */
            AR::CpuFunc::YieldProcessor();
            HL::Timer::StallExecution(10);
            Timeout++;
        }

        /* Check if the processor has not started */
        if(!StartBlock->Started)
        {
            /* Free processor structures and unregister processor block */
            MM::KernelPool::FreeProcessorStructures(CpuStructures);
            KE::Processor::RegisterProcessorBlock(CpuNumber, NULLPTR);

            /* Decrement the CPU counter back */
            CpuNumber--;
        }
    }

    /* Unmap trampoline memory and return success */
    MM::HardwarePool::UnmapHardwareMemory(TrampolineAddress, TrampolinePages, TRUE);
    return STATUS_SUCCESS;
}
