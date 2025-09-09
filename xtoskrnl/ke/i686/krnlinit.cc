/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/krnlinit.cc
 * DESCRIPTION:     CPU architecture specific kernel initialization
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

/**
 * This routine initializes XT kernel.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KernelInit::InitializeKernel(VOID)
{
    XTSTATUS Status;

    /* Initialize hardware layer subsystem */
    Status = HlInitializeSystem();
    if(Status != STATUS_SUCCESS)
    {
        /* Hardware layer initialization failed, kernel panic */
        DebugPrint(L"Failed to initialize hardware layer subsystem!\n");
        Crash::Panic(0);
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
KernelInit::InitializeMachine(VOID)
{
    /* Re-enable IDE interrupts */
    HlIoPortOutByte(0x376, 0);
    HlIoPortOutByte(0x3F6, 0);

    /* Initialize frame buffer */
    HlInitializeFrameBuffer();

    /* Initialize processor */
    HlInitializeProcessor();

    /* Initialize page map support */
    MmInitializePageMapSupport();
}

/**
 * This routine starts up the XT kernel. It is called after switching boot stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KernelInit::StartKernel(VOID)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    ULONG_PTR PageDirectory[2];
    PKPROCESS CurrentProcess;
    PKTHREAD CurrentThread;

    /* Get processor control block and current thread */
    Prcb = Processor::GetCurrentProcessorControlBlock();
    CurrentThread = Processor::GetCurrentThread();

    /* Get current process */
    CurrentProcess = CurrentThread->ApcState.Process;

    /* Initialize CPU power state structures */
    PoInitializeProcessorControlBlock(Prcb);

    /* Save processor state */
    Processor::SaveProcessorState(&Prcb->ProcessorState);

    /* Lower to APC runlevel */
    RunLevel::LowerRunLevel(APC_LEVEL);

    /* Initialize XTOS kernel */
    InitializeKernel();

    /* Initialize Idle process */
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;
    KProcess::InitializeProcess(CurrentProcess, 0, MAXULONG_PTR, PageDirectory, FALSE);
    CurrentProcess->Quantum = MAXCHAR;

    /* Initialize Idle thread */
    KThread::InitializeThread(CurrentProcess, CurrentThread, nullptr, nullptr, nullptr, nullptr, nullptr, ArGetBootStack(), TRUE);
    CurrentThread->NextProcessor = Prcb->CpuNumber;
    CurrentThread->Priority = THREAD_HIGH_PRIORITY;
    CurrentThread->State = Running;
    CurrentThread->Affinity = (ULONG_PTR)1 << Prcb->CpuNumber;
    CurrentThread->WaitRunLevel = DISPATCH_LEVEL;
    CurrentProcess->ActiveProcessors |= (ULONG_PTR)1 << Prcb->CpuNumber;

    /* Enter infinite loop */
    DebugPrint(L"KernelInit::StartKernel() finished. Entering infinite loop.\n");
    Crash::HaltSystem();
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
KernelInit::SwitchBootStack(VOID)
{
    ULONG_PTR Stack;
    PVOID StartKernel;

    /* Calculate the stack pointer at the top of the buffer, ensuring it is properly aligned as required by the ABI */
    Stack = ((ULONG_PTR)ArGetBootStack() + KERNEL_STACK_SIZE) & ~(STACK_ALIGNMENT - 1);

    /* Get address of KernelInit::StartKernel() */
    StartKernel = (PVOID)KernelInit::StartKernel;

    /* Discard old stack frame, switch stack, make space for NPX and jump to KernelInit::StartKernel() */
    __asm__ volatile("mov %0, %%edx\n"
                     "xor %%ebp, %%ebp\n"
                     "mov %%edx, %%esp\n"
                     "sub %1, %%esp\n"
                     "push %2\n"
                     "jmp *%3\n"
                     :
                     : "m" (Stack),
                       "i" (KTRAP_FRAME_ALIGN | KTRAP_FRAME_SIZE | NPX_FRAME_SIZE | KRETURN_ADDRESS_SIZE),
                       "i" (CR0_EM | CR0_MP | CR0_TS),
                       "r" (StartKernel)
                     : "edx", "ebp", "esp", "memory");
}

} /* namespace */
