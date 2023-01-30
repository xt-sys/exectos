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
KepArchInitialize(VOID)
{
    /* Enable global paging support */
    ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_PGE);

    /* Enable write-protection */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_WP);

    /* Set alignment mask */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_AM);

    /* Re-enable IDE interrupts */
    HlIoPortOutByte(0x376, 0);
    HlIoPortOutByte(0x3F6, 0);

    /* Set system call extensions (SCE) flag in EFER MSR */
    ArWriteModelSpecificRegister(X86_MSR_EFER, ArReadModelSpecificRegister(X86_MSR_EFER) | X86_MSR_EFER_SCE);
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
    LdrPrint(L"Hello from new kernel stack!\n");
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
