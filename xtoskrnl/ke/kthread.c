/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/kthread.c
 * DESCRIPTION:     XT kernel thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes the thread.
 *
 * @param Thread
 *        Supplies a pointer to thread that will be initialized.
 *
 * @param Stack
 *        Supplies a pointer to the stack of the thread.
 *
 * @param SystemRoutine
 *        Supplies a pointer to the routine called during first scheduling.
 *
 * @param StartRoutine
 *        Supplies a pointer to the routine called during thread startup.
 *
 * @param StartContext
 *        Supplies a pointer to a context data that will be passed to start routine.
 *
 * @param Context
 *        Supplies a pointer to the context frame containing state of the user mode thread.
 *
 * @param EnvironmentBlock
 *        Supplies a pointer to the environment block of the thread.
 *
 * @param Process
 *        Supplies a pointer to the process that owns the thread.
 *
 * @return This routine returns a status code.
 *
 * @since NT 3.5
 */
XTAPI
XTSTATUS
KeInitializeThread(IN PKTHREAD Thread,
                   IN PVOID Stack,
                   IN PKSYSTEM_ROUTINE SystemRoutine,
                   IN PKSTART_ROUTINE StartRoutine,
                   IN PVOID StartContext,
                   IN PCONTEXT Context,
                   IN PVOID EnvironmentBlock,
                   IN PKPROCESS Process)
{
    UNIMPLEMENTED;
    return STATUS_NOT_IMPLEMENTED;
}
