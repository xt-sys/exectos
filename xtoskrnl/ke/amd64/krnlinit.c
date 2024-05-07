/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/krnlinit.c
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

    /* Initialize Idle process */
    RtlInitializeListHead(&KepProcessListHead);
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;
    KeInitializeProcess(CurrentProcess, 0, 0xFFFFFFFF, PageDirectory, FALSE);
    CurrentProcess->Quantum = MAXCHAR;

    /* Initialize Idle thread */
    KeInitializeThread(CurrentProcess, CurrentThread, NULL, NULL, NULL, NULL, NULL, ArKernelBootStack, TRUE);
    CurrentThread->NextProcessor = Prcb->Number;
    CurrentThread->Priority = THREAD_HIGH_PRIORITY;
    CurrentThread->State = Running;
    CurrentThread->Affinity = (ULONG_PTR)1 << Prcb->Number;
    CurrentThread->WaitRunLevel = DISPATCH_LEVEL;
    CurrentProcess->ActiveProcessors |= (ULONG_PTR)1 << Prcb->Number;
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
    HlInitializeProcessor(0);
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
    /* Initialize XTOS kernel */
    KepInitializeKernel();

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
    /* Discard old stack frame, switch stack and jump to KepStartKernel() */
    asm volatile("mov %0, %%rdx\n"
                 "xor %%rbp, %%rbp\n"
                 "mov %%rdx, %%rsp\n"
                 "sub %1, %%rsp\n"
                 "jmp KepStartKernel\n"
                 :
                 : "m" (Stack),
                   "i" (FLOATING_SAVE_AREA_SIZE | KEXCEPTION_FRAME_SIZE | KSWITCH_FRAME_SIZE | KRETURN_ADDRESS_SIZE),
                   "p" (KepStartKernel));
}
