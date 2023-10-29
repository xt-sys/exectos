/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/krnlinit.c
 * DESCRIPTION:     CPU architecture specific kernel initialization
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
                   "i" (FLOATING_SAVE_AREA_SIZE | KEXCEPTION_FRAME_SIZE | KSWITCH_FRAME_SIZE),
                   "p" (KepStartKernel));
}
