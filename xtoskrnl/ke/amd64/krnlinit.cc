/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/krnlinit.cc
 * DESCRIPTION:     CPU architecture specific kernel initialization
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Bootstraps an Application Processor (AP) into the active kernel. This routine is executed exclusively by secondary
 * processors after being awakened by the BSP. It is called directly from the startup trampoline.
 *
 * @param StartBlock
 *        Supplies a pointer to the processor start block containing initialization information provided by the kernel.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::KernelInit::BootstrapApplicationProcessor(IN PPROCESSOR_START_BLOCK StartBlock)
{
    PKPROCESSOR_BLOCK ProcessorBlock;

    /* Initialize application CPU */
    AR::ProcSup::InitializeProcessor(StartBlock->ProcessorStructures);

    /* Initialize processor */
    HL::Cpu::InitializeProcessor();

    /* Raise to HIGH runlevel */
    KE::RunLevel::RaiseRunLevel(HIGH_LEVEL);

    /* Mark processor as started */
    StartBlock->Started = TRUE;

    /* Get current processor block */
    ProcessorBlock = KE::Processor::GetCurrentProcessorBlock();

    /* Enter infinite loop */
    DebugPrint(L"KernelInit::BootstrapApplicationProcessor() finished for CPU #%lu. Entering infinite loop.\n",
               ProcessorBlock->CpuNumber);
    KE::Crash::HaltSystem();
}

/**
 * Bootstraps the XT kernel and global subsystems. This routine is executed exclusively by the Bootstrap Processor
 * and it is called immediately after switching to the kernel boot stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::KernelInit::BootstrapKernel(VOID)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    ULONG_PTR PageDirectory[2];
    PKPROCESS CurrentProcess;
    PKTHREAD CurrentThread;

    /* Get processor control block and current thread */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Get current process */
    CurrentProcess = CurrentThread->ApcState.Process;

    /* Initialize CPU power state structures */
    PO::Idle::InitializeProcessorIdleState(Prcb);

    /* Save processor state */
    KE::Processor::SaveProcessorState(&Prcb->ProcessorState);

    /* Initialize spin locks */
    KE::SpinLock::InitializeAllLocks();
    KE::SpinLock::InitializeLockQueues();

    /* Lower to APC runlevel */
    KE::RunLevel::LowerRunLevel(APC_LEVEL);

    /* Initialize XTOS kernel */
    InitializeKernel();

    /* Initialize Idle process */
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;
    KE::KProcess::InitializeProcess(CurrentProcess, 0, MAXULONG_PTR, PageDirectory, FALSE);
    CurrentProcess->Quantum = MAXCHAR;

    /* Initialize Idle thread */
    KE::KThread::InitializeThread(CurrentProcess, CurrentThread, NULLPTR, NULLPTR, NULLPTR,
                                  NULLPTR, NULLPTR, AR::ProcSup::GetBootStack(), TRUE);
    CurrentThread->NextProcessor = Prcb->CpuNumber;
    CurrentThread->Priority = THREAD_HIGH_PRIORITY;
    CurrentThread->State = Running;
    CurrentThread->Affinity = (ULONG_PTR)1 << Prcb->CpuNumber;
    CurrentThread->WaitRunLevel = DISPATCH_LEVEL;
    CurrentProcess->ActiveProcessors |= (ULONG_PTR)1 << Prcb->CpuNumber;

    /* Initialize Memory Manager */
    MM::Manager::InitializeMemoryManager();

    /* Enable shadow buffer for framebuffer */
    HL::FrameBuffer::EnableShadowBuffer();

    /* Start all application processors */
    KE::Processor::InitializeProcessorBlocks();
    HL::Cpu::StartAllProcessors();

    /* Enter infinite loop */
    DebugPrint(L"KernelInit::BootstrapKernel() finished. Entering infinite loop.\n");
    KE::Crash::HaltSystem();
}

/**
 * This routine initializes XT kernel.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::KernelInit::InitializeKernel(VOID)
{
    XTSTATUS Status;

    /* Initialize hardware layer subsystem */
    Status = HL::Init::InitializeSystem();
    if(Status != STATUS_SUCCESS)
    {
        /* Hardware layer initialization failed, kernel panic */
        DebugPrint(L"Failed to initialize hardware layer subsystem!\n");
        KE::Crash::Panic(0);
    }
}

/**
 * Performs architecture-specific initialization for the kernel executive.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::KernelInit::InitializeMachine(VOID)
{
    /* Re-enable IDE interrupts */
    HL::IoPort::WritePort8(0x376, 0);
    HL::IoPort::WritePort8(0x3F6, 0);

    /* Initialize frame buffer */
    HL::FrameBuffer::InitializeFrameBuffer();

    /* Initialize page map support */
    MM::Paging::InitializePageMapSupport();

    /* Initialize Kernel Shared Data (KSD) */
    KE::SharedData::InitializeKernelSharedData();

    /* Initialize processor */
    HL::Cpu::InitializeProcessor();
}

/**
 * Switches execution to a new boot stack and transfers control to the KernelInit::StartKernel() routine.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::KernelInit::SwitchBootStack(VOID)
{
    ULONG_PTR Stack;
    PVOID StartKernel;

    /* Calculate the stack pointer at the top of the buffer, ensuring it is properly aligned as required by the ABI */
    Stack = ((ULONG_PTR)AR::ProcSup::GetBootStack() & ~(STACK_ALIGNMENT - 1));

    /* Get address of KernelInit::StartKernel() */
    StartKernel = (PVOID)KE::KernelInit::BootstrapKernel;

    /* Discard old stack frame, switch stack and jump to KernelInit::StartKernel() */
    __asm__ volatile("movq %[Stack], %%rsp\n"
                     "subq %[TotalSize], %%rsp\n"
                     "xorq %%rbp, %%rbp\n"
                     "jmp *%[TargetRoutine]\n"
                     :
                     : [Stack] "r" (Stack),
                       [TargetRoutine] "r" (StartKernel),
                       [TotalSize] "i" (FLOATING_SAVE_AREA_SIZE + KEXCEPTION_FRAME_SIZE +
                                        KSWITCH_FRAME_SIZE + KRETURN_ADDRESS_SIZE)
                     : "memory", "rbp", "rsp");
}
