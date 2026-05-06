/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/dispatch.cc
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

    /* Save registers to the exception frame and invoke the stack switch routine */
    __asm__ volatile("subl %[ExFrameSize], %%esp\n"
                     "movl %%esi, %c[ExEsi](%%esp)\n"
                     "movl %%edi, %c[ExEdi](%%esp)\n"
                     "movl %%ebx, %c[ExEbx](%%esp)\n"
                     "movl %%ebp, %c[ExEbp](%%esp)\n"
                     "call %P[SwitchRoutine]\n"
                     "movl %c[ExEbp](%%esp), %%ebp\n"
                     "movl %c[ExEbx](%%esp), %%ebx\n"
                     "movl %c[ExEdi](%%esp), %%edi\n"
                     "movl %c[ExEsi](%%esp), %%esi\n"
                     "addl %[ExFrameSize], %%esp\n"
                     : "=a" (PendingApc)
                     : "c" (CurrentThread),
                       "d" (RunLevel),
                       [ExFrameSize] "i" (sizeof(KEXCEPTION_FRAME) - 4),
                       [ExEbp] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Ebp)),
                       [ExEbx] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Ebx)),
                       [ExEdi] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Edi)),
                       [ExEsi] "i" (FIELD_OFFSET(KEXCEPTION_FRAME, Esi)),
                       [SwitchRoutine] "i" (SwitchThreadStack)
                     : "cc", "memory");

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

    /* Save old state, synchronize with CPUs, switch stack and call the switch routine */
    __asm__ volatile("subl %[FrameSize], %%esp\n"
                     "movl %%fs:%c[PrcbcCurrentThread], %%ebx\n"
                     "BusyLoop:\n"
                     "cmpb $0, %c[ThrdSwapBusy](%%ebx)\n"
                     "je ExitLoop\n"
                     "pause\n"
                     "jmp BusyLoop\n"
                     "ExitLoop:\n"
                     "movl %%fs:%c[ThrdInfoExceptions], %%eax\n"
                     "movl %%eax, %c[SwExceptionList](%%esp)\n"
                     "movb %%dl, %c[SwApcBypass](%%esp)\n"
                     "movl %%esp, %c[ThrdStack](%%ecx)\n"
                     "movl %c[ThrdStack](%%ebx), %%esp\n"
                     "movzbl %c[SwApcBypass](%%esp), %%edx\n"
                     "call %P[SwitchRoutine]\n"
                     "addl %[FrameSize], %%esp\n"
                     : "=a" (PendingApc)
                     : "c" (CurrentThread),
                       "d" (RunLevel),
                       [FrameSize] "i" (sizeof(KSWITCH_FRAME) - 4),
                       [PrcbcCurrentThread] "i" (FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb.CurrentThread)),
                       [SwApcBypass] "i" (FIELD_OFFSET(KSWITCH_FRAME, ApcBypassDisabled)),
                       [SwExceptionList] "i" (FIELD_OFFSET(KSWITCH_FRAME, ExceptionList)),
                       [SwitchRoutine] "i" (SwitchThreadContext),
                       [ThrdInfoExceptions] "i" (FIELD_OFFSET(THREAD_INFORMATION_BLOCK, ExceptionList)),
                       [ThrdStack] "i" (FIELD_OFFSET(KTHREAD, KernelStack)),
                       [ThrdSwapBusy] "i" (FIELD_OFFSET(KTHREAD, SwapBusy))
                     : "cc", "memory");

    /* Return the APC status */
    return PendingApc;
}
