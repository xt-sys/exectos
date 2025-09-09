/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/apc.cc
 * DESCRIPTION:     Kernel APC objects support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

/**
 * Initializes an APC object.
 *
 * @param Apc
 *        Supplies a pointer to the APC object.
 *
 * @param Thread
 *        Supplies a pointer to the thread object.
 *
 * @param Environment
 *        Specifies an environment in which the APC will run.
 *
 * @param KernelRoutine
 *        Supplies a pointer to routine called at APC_LEVEL.
 *
 * @param RundownRoutine
 *        Supplies a pointer to routine called on thread exit.
 *
 * @param NormalRoutine
 *        Supplies a pointer to routine called at IRQL 0.
 *
 * @param ApcMode
 *        Specifies processor mode, in which NormalRoutine gets called.
 *
 * @param Context
 *        Supplies a pointer to memory area containing data passed to NormalRoutine.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
Apc::InitializeApc(IN PKAPC Apc,
                   IN PKTHREAD Thread,
                   IN KAPC_ENVIRONMENT Environment,
                   IN PKKERNEL_ROUTINE KernelRoutine,
                   IN PKRUNDOWN_ROUTINE RundownRoutine,
                   IN PKNORMAL_ROUTINE NormalRoutine,
                   IN KPROCESSOR_MODE ApcMode,
                   IN PVOID Context)
{
    /* Set APC type and thread */
    Apc->Type = ApcObject;
    Apc->Thread = Thread;

    /* Set routines */
    Apc->KernelRoutine = KernelRoutine;
    Apc->RundownRoutine = RundownRoutine;
    Apc->NormalRoutine = NormalRoutine;

    /* Check target environment */
    if(Environment == CurrentApcEnvironment)
    {
        /* Use current APC environment taken from thread */
        Apc->ApcStateIndex = Thread->ApcStateIndex;
    }
    else
    {
        /* Use new APC environment */
        Apc->ApcStateIndex = Environment;
    }

    /* Check if normal routine specified */
    if(NormalRoutine)
    {
        /* Set context and mode for notmal APC */
        Apc->ApcMode = ApcMode;
        Apc->NormalContext = Context;
    }
    else
    {
        /* Set context and mode for special APC */
        Apc->ApcMode = KernelMode;
        Apc->NormalContext = NULL;
    }

    /* Mark APC as not inserted yet */
    Apc->Inserted = FALSE;
}

} /* namespace */
