/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/dispatch.cc
 * DESCRIPTION:     Kernel Thread Dispatcher
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Entry point for thread context switching.
 *
 * @param CurrentThread
 *        Pointer to the KTHREAD structure of the current thread being suspended.
 *
 * @param RunLevel
 *        Supplies the running level at which the wait was initiated.
 *
 * @return This routine returns TRUE if a kernel APC is pending and can be delivered, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
KE::Dispatcher::SwitchContext(IN PKTHREAD CurrentThread,
                              IN KRUNLEVEL RunLevel)
{
    BOOLEAN PendingApc;

    /* Save non-volatile and XMM registers to the exception frame, align the stack and invoke the switch routine */
    __asm__ volatile("subq %[ExFrameSize], %%rsp\n"
                     "movq %%rbp, %c[ExRbp](%%rsp)\n"
                     "movq %%rbx, %c[ExRbx](%%rsp)\n"
                     "movq %%rdi, %c[ExRdi](%%rsp)\n"
                     "movq %%rsi, %c[ExRsi](%%rsp)\n"
                     "movq %%r12, %c[ExR12](%%rsp)\n"
                     "movq %%r13, %c[ExR13](%%rsp)\n"
                     "movq %%r14, %c[ExR14](%%rsp)\n"
                     "movq %%r15, %c[ExR15](%%rsp)\n"
                     "movdqa %%xmm6, %c[ExXmm6](%%rsp)\n"
                     "movdqa %%xmm7, %c[ExXmm7](%%rsp)\n"
                     "movdqa %%xmm8, %c[ExXmm8](%%rsp)\n"
                     "movdqa %%xmm9, %c[ExXmm9](%%rsp)\n"
                     "movdqa %%xmm10, %c[ExXmm10](%%rsp)\n"
                     "movdqa %%xmm11, %c[ExXmm11](%%rsp)\n"
                     "movdqa %%xmm12, %c[ExXmm12](%%rsp)\n"
                     "movdqa %%xmm13, %c[ExXmm13](%%rsp)\n"
                     "movdqa %%xmm14, %c[ExXmm14](%%rsp)\n"
                     "movdqa %%xmm15, %c[ExXmm15](%%rsp)\n"
                     "callq %P[SwitchRoutine]\n"
                     "movq %c[ExRbp](%%rsp), %%rbp\n"
                     "movq %c[ExRbx](%%rsp), %%rbx\n"
                     "movq %c[ExRdi](%%rsp), %%rdi\n"
                     "movq %c[ExRsi](%%rsp), %%rsi\n"
                     "movq %c[ExR12](%%rsp), %%r12\n"
                     "movq %c[ExR13](%%rsp), %%r13\n"
                     "movq %c[ExR14](%%rsp), %%r14\n"
                     "movq %c[ExR15](%%rsp), %%r15\n"
                     "movdqa %c[ExXmm6](%%rsp), %%xmm6\n"
                     "movdqa %c[ExXmm7](%%rsp), %%xmm7\n"
                     "movdqa %c[ExXmm8](%%rsp), %%xmm8\n"
                     "movdqa %c[ExXmm9](%%rsp), %%xmm9\n"
                     "movdqa %c[ExXmm10](%%rsp), %%xmm10\n"
                     "movdqa %c[ExXmm11](%%rsp), %%xmm11\n"
                     "movdqa %c[ExXmm12](%%rsp), %%xmm12\n"
                     "movdqa %c[ExXmm13](%%rsp), %%xmm13\n"
                     "movdqa %c[ExXmm14](%%rsp), %%xmm14\n"
                     "movdqa %c[ExXmm15](%%rsp), %%xmm15\n"
                     "addq %[ExFrameSize], %%rsp\n"
                     : "=a" (PendingApc)
                     : "c" (CurrentThread),
                       "d" (RunLevel),
                       [ExFrameSize] "i" (sizeof(KEXCEPTION_FRAME) - 8),
                       [ExR12] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, R12)),
                       [ExR13] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, R13)),
                       [ExR14] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, R14)),
                       [ExR15] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, R15)),
                       [ExRbp] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Rbp)),
                       [ExRbx] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Rbx)),
                       [ExRdi] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Rdi)),
                       [ExRsi] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Rsi)),
                       [ExXmm6] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm6)),
                       [ExXmm7] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm7)),
                       [ExXmm8] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm8)),
                       [ExXmm9] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm9)),
                       [ExXmm10] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm10)),
                       [ExXmm11] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm11)),
                       [ExXmm12] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm12)),
                       [ExXmm13] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm13)),
                       [ExXmm14] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm14)),
                       [ExXmm15] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Xmm15)),
                       [SwitchRoutine] "i" (SwitchThreadStack)
                     : "cc", "memory", "r8", "r9", "r10", "r11");

    /* Return the APC status */
    return PendingApc;
}

/**
 * Switches context from current thread to the new thread.
 *
 * @param CurrentThread
 *        Pointer to the KTHREAD structure of the current thread being suspended.
 *
 * @param ApcBypass
 *        Indicates whether the APC delivery should be bypassed.
 *
 * @return This routine returns TRUE if a kernel APC is pending and can be delivered, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
KE::Dispatcher::SwitchThreadContext(IN PKTHREAD CurrentThread,
                                    IN BOOLEAN ApcBypass)
{
    UNIMPLEMENTED;

    return FALSE;
}

/**
 * Switches the thread stack and performs necessary operations to prepare for context switching.
 *
 * @param CurrentThread
 *        Pointer to the KTHREAD structure of the current thread being suspended.
 *
 * @param RunLevel
 *        Supplies the running level at which the wait was initiated.
 *
 * @return This routine returns TRUE if a kernel APC is pending and can be delivered, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
KE::Dispatcher::SwitchThreadStack(IN PKTHREAD CurrentThread,
                                  IN KRUNLEVEL RunLevel)
{
    BOOLEAN PendingApc;

    /* Preserve MXCSR, synchronize with CPUs, switch stack and call the switch routine */
    __asm__ volatile("pushq %%rbp\n"
                     "subq %[FrameSize], %%rsp\n"
                     "stmxcsr %c[SwMxCsr](%%rsp)\n"
                     "movq %%gs:%c[PrcbcCurrentThread], %%r8\n"
                     "BusyLoop:\n"
                     "cmpb $0, %c[ThrdSwapBusy](%%r8)\n"
                     "je ExitLoop\n"
                     "pause\n"
                     "jmp BusyLoop\n"
                     "ExitLoop:\n"
                     "movb %%dl, %c[SwApcBypass](%%rsp)\n"
                     "movq %%rsp, %c[ThrdStack](%%rcx)\n"
                     "movq %c[ThrdStack](%%r8), %%rsp\n"
                     "movzbl %c[SwApcBypass](%%rsp), %%edx\n"
                     "callq %P[SwitchRoutine]\n"
                     "ldmxcsr %c[SwMxCsr](%%rsp)\n"
                     "addq %[FrameSize], %%rsp\n"
                     "popq %%rbp\n"
                     : "=a" (PendingApc)
                     : "c" (CurrentThread),
                       "d" (RunLevel),
                       [FrameSize] "i" (FIELD_OFFSET(KSWITCH_FRAME, Rbp)),
                       [PrcbcCurrentThread] "i" (FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb.CurrentThread)),
                       [SwApcBypass] "i" (FIELD_OFFSET(KSWITCH_FRAME, ApcBypass)),
                       [SwMxCsr] "i" (FIELD_OFFSET(KSWITCH_FRAME, MxCsr)),
                       [SwitchRoutine] "i" (SwitchThreadContext),
                       [ThrdStack] "i" (FIELD_OFFSET(KTHREAD, KernelStack)),
                       [ThrdSwapBusy] "i" (FIELD_OFFSET(KTHREAD, SwapBusy))
                     : "cc", "memory", "r8", "r9", "r10", "r11");

    /* Return the APC status */
    return PendingApc;
}
