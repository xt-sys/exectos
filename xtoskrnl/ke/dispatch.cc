/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/dispatch.cc
 * DESCRIPTION:     Kernel Thread Dispatcher
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Exits the dispatcher, switches context to a new thread and lowers runlevel to its original state.
 *
 * @param OldRunLevel
 *        Supplies the original runlevel state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::Dispatcher::ExitDispatcher(IN KRUNLEVEL OldRunLevel)
{
    UNIMPLEMENTED;

    /* Lower runlevel */
    RunLevel::LowerRunLevel(OldRunLevel);
}

/**
 * Handles the dispatch interrupt by retiring pending DPCs, asking the scheduler for the next runnable thread
 * and performing the context switch.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame representing the interrupted context.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KE::Dispatcher::HandleDispatchInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    /* End the interrupt */
    HL::Pic::SendEoi();
}

/**
 * Updates the runtime quantum of the currently executing thread and handles preemption.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame representing the interrupted execution context.
 *
 * @param RunLevel
 *        Supplies the system run level at which the interrupt was taken.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Dispatcher::UpdateRunTime(IN PKTRAP_FRAME TrapFrame,
                              IN KRUNLEVEL RunLevel)
{
}
