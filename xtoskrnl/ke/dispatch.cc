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
