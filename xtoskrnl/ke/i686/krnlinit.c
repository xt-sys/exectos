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
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    ULONG_PTR PageDirectory[2];
    PKTHREAD CurrentThread;

    /* Get processor control block and current thread */
    Prcb = KeGetCurrentProcessorControlBlock();
    CurrentThread = KeGetCurrentThread();

    /* Initialize CPU power state structures */
    PoInitializeProcessorControlBlock(Prcb);

    /* Initialize Idle process */
    RtlInitializeListHead(&KepProcessListHead);
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;
    KeInitializeProcess(CurrentThread->ApcState.Process, 0, 0xFFFFFFFF, PageDirectory, FALSE);
    CurrentThread->ApcState.Process->Quantum = MAXCHAR;

    /* Initialize Idle thread */
    KeInitializeThread(CurrentThread->ApcState.Process, CurrentThread, NULL, NULL, NULL, NULL, NULL, Prcb->DpcStack);
    CurrentThread->NextProcessor = Prcb->Number;
    CurrentThread->Priority = THREAD_HIGH_PRIORITY;
    CurrentThread->State = Running;
    CurrentThread->Affinity = (ULONG_PTR)1 << Prcb->Number;
    CurrentThread->WaitIrql = DISPATCH_LEVEL;
    CurrentThread->ApcState.Process->ActiveProcessors |= (ULONG_PTR)1 << Prcb->Number;
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
    /* Print debug message */
    DebugPrint(L"Starting ExectOS ...\n");

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
