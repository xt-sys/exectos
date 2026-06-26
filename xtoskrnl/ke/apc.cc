/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/apc.cc
 * DESCRIPTION:     Kernel APC objects support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Checks if a kernel APC can be delivered and initiates the delivery process.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Apc::CheckApcDelivery(VOID)
{
    PKTHREAD Thread;

    /* Check if system is running at PASSIVE level */
    if(KE::RunLevel::GetCurrentRunLevel() == PASSIVE_LEVEL)
    {
        /* Raise runlevel to APC level */
        KE::RaiseRunLevel RunLevel(APC_LEVEL);

        /* Deliver the APC */
        DeliverApc(KernelMode, NULLPTR, NULLPTR);
    }
    else
    {
        /* Mark the current thread as having a pending kernel APC */
        Thread = KE::Processor::GetCurrentThread();
        Thread->ApcState.KernelApcPending = TRUE;

        /* Request an APC software interrupt */
        HL::Irq::SendSoftwareInterrupt(APC_LEVEL);
    }
}

/**
 * Delivers pending Asynchronous Procedure Calls (APCs) to the current executing thread.
 *
 * @param ProcessorMode
 *        Supplies the processor execution mode at the time the APC delivery was requested.
 *
 * @param ExceptionFrame
 *        Supplies an optional pointer to the exception frame if the APC is being delivered following an exception.
 *
 * @param TrapFrame
 *        Supplies an optional pointer to the trap frame if the APC is being delivered following an interrupt.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::Apc::DeliverApc(IN KPROCESSOR_MODE ProcessorMode,
                    IN PKEXCEPTION_FRAME ExceptionFrame,
                    IN PKTRAP_FRAME TrapFrame)
{
    PKTHREAD Thread;

    UNIMPLEMENTED;

    /* Get the current thread */
    Thread = KE::Processor::GetCurrentThread();

    /* Clear the pending kernel APC flag */
    Thread->ApcState.KernelApcPending = FALSE;
}

/**
 * Handles the Asynchronous Procedure Call (APC) interrupt.
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
KE::Apc::HandleApcInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    /* Raise runlevel to APC level */
    KE::RaiseRunLevel RunLevel(APC_LEVEL);

    /* Deliver the APC */
    DeliverApc(TrapFrame->PreviousMode, NULLPTR, TrapFrame);
}

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
 * @since XT 1.0
 */
XTAPI
VOID
KE::Apc::InitializeApc(IN PKAPC Apc,
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
        Apc->NormalContext = NULLPTR;
    }

    /* Mark APC as not inserted yet */
    Apc->Inserted = FALSE;
}
