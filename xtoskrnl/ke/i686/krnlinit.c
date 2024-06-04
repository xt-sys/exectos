/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/krnlinit.c
 * DESCRIPTION:     CPU architecture specific kernel initialization
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * This routine initializes XT kernel.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KepInitializeKernel(VOID)
{
    XTSTATUS Status;

    /* Initialize hardware layer subsystem */
    Status = HlInitializeSystem();
    if(Status != STATUS_SUCCESS)
    {
        /* Hardware layer initialization failed, kernel panic */
        DebugPrint(L"Failed to initialize hardware layer subsystem!\n");
        KePanic(0);
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
KepInitializeMachine(VOID)
{
    /* Re-enable IDE interrupts */
    HlIoPortOutByte(0x376, 0);
    HlIoPortOutByte(0x3F6, 0);

    /* Initialize frame buffer */
    HlInitializeFrameBuffer();

    /* Initialize processor */
    HlInitializeProcessor();
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
KepStartKernel(VOID)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    ULONG_PTR PageDirectory[2];
    PKPROCESS CurrentProcess;
    PKTHREAD CurrentThread;

    /* Get processor control block and current thread */
    Prcb = KeGetCurrentProcessorControlBlock();
    CurrentThread = KeGetCurrentThread();

    /* Get current process */
    CurrentProcess = CurrentThread->ApcState.Process;

    /* Initialize CPU power state structures */
    PoInitializeProcessorControlBlock(Prcb);

    /* Save processor state */
    KepSaveProcessorState(&Prcb->ProcessorState);

    /* Lower to APC runlevel */
    KeLowerRunLevel(APC_LEVEL);

    /* Initialize XTOS kernel */
    KepInitializeKernel();

    /* Initialize Idle process */
    RtlInitializeListHead(&KepProcessListHead);
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;
    KeInitializeProcess(CurrentProcess, 0, 0xFFFFFFFF, PageDirectory, FALSE);
    CurrentProcess->Quantum = MAXCHAR;

    /* Initialize Idle thread */
    KeInitializeThread(CurrentProcess, CurrentThread, NULL, NULL, NULL, NULL, NULL, ArKernelBootStack, TRUE);
    CurrentThread->NextProcessor = Prcb->CpuNumber;
    CurrentThread->Priority = THREAD_HIGH_PRIORITY;
    CurrentThread->State = Running;
    CurrentThread->Affinity = (ULONG_PTR)1 << Prcb->CpuNumber;
    CurrentThread->WaitRunLevel = DISPATCH_LEVEL;
    CurrentProcess->ActiveProcessors |= (ULONG_PTR)1 << Prcb->CpuNumber;

    /* Enter infinite loop */
    DebugPrint(L"KepStartKernel() finished. Entering infinite loop.\n");
    for(;;);
}

/**
 * Switches to a new kernel boot stack.
 *
 * @return This routine does not return any value
 *
 * @since XT 1.0
 */
XTAPI
VOID
KepSwitchBootStack(IN ULONG_PTR Stack)
{
    /* Discard old stack frame, switch stack, make space for NPX and jump to KepStartKernel() */
    asm volatile("mov %0, %%edx\n"
                 "xor %%ebp, %%ebp\n"
                 "mov %%edx, %%esp\n"
                 "sub %1, %%esp\n"
                 "push %2\n"
                 "jmp _KepStartKernel@0\n"
                 :
                 : "m" (Stack),
                   "i" (KTRAP_FRAME_ALIGN | KTRAP_FRAME_SIZE | NPX_FRAME_SIZE | KRETURN_ADDRESS_SIZE),
                   "i" (CR0_EM | CR0_MP | CR0_TS),
                   "p" (KepStartKernel));
}
