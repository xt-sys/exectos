/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/kthread.c
 * DESCRIPTION:     AMD64 thread manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes CPU architecture dependent context of a thread.
 *
 * @param Thread
 *        Supplies a pointer to the thread being initialized.
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
 * @param ContextRecord
 *        Supplies a pointer to a context record which stores the initial state of the user mode thread.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KepInitializeThreadContext(IN PKTHREAD Thread,
                           IN PKSYSTEM_ROUTINE SystemRoutine,
                           IN PKSTART_ROUTINE StartRoutine,
                           IN PVOID StartContext,
                           IN PCONTEXT ContextRecord)
{
    UNIMPLEMENTED;
}
