/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/dpc.c
 * DESCRIPTION:     Deferred Procedure Call (DPC) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes Deferred Procedure Call (DPC) object.
 *
 * @param Dpc
 *        Supplies a pointer to the DPC being initialized.
 *
 * @param DpcRoutine
 *        Supplies a pointer to the DPC routine being called on object removal.
 *
 * @param DpcContext
 *        Supplies a pointer to memory area containing context data for DPC routine.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
KeInitializeDpc(IN PKDPC Dpc,
                IN PKDEFERRED_ROUTINE DpcRoutine,
                IN PVOID DpcContext)
{
    /* Initialize DPC */
    Dpc->Type = DpcObject;
    Dpc->Number = 0;
    Dpc->Importance = MediumImportance;

    /* Initialize DPC routine and context data */
    Dpc->DeferredContext = DpcContext;
    Dpc->DeferredRoutine = DpcRoutine;
    Dpc->DpcData = NULL;
}

/**
 * Initializes Deferred Procedure Call (DPC) object.
 *
 * @param Dpc
 *        Supplies a pointer to the DPC being initialized.
 *
 * @param DpcRoutine
 *        Supplies a pointer to the DPC routine being called on object removal.
 *
 * @param DpcContext
 *        Supplies a pointer to memory area containing context data for DPC routine.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.2
 */
XTAPI
VOID
KeInitializeThreadedDpc(IN PKDPC Dpc,
                        IN PKDEFERRED_ROUTINE DpcRoutine,
                        IN PVOID DpcContext)
{
    /* Initialize threaded DPC */
    Dpc->Type = ThreadedDpcObject;
    Dpc->Number = 0;
    Dpc->Importance = MediumImportance;

    /* Initialize DPC routine and context data */
    Dpc->DeferredContext = DpcContext;
    Dpc->DeferredRoutine = DpcRoutine;
    Dpc->DpcData = NULL;
}

/**
 * Sets the target processor number for DPC.
 *
 * @param Dpc
 *        Supplies a pointer to the DPC object.
 *
 * @param Number
 *        Supplies the target processor number.
 *
 * @return This routine does not return any value.
 *
 * @since NT 4.0
 */
XTAPI
VOID
KeSetTargetProcessorDpc(IN PKDPC Dpc,
                        IN CCHAR Number)
{
    Dpc->Number = MAXIMUM_PROCESSORS + Number;
}

/**
 * Decrements the DPC call barier.
 *
 * @param SystemArgument
 *        Supplies an address of the DPC call barrier.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.2
 */
XTAPI
VOID
KeSignalCallDpcDone(IN PVOID SystemArgument)
{
    RtlAtomicDecrement32(SystemArgument);
}

/**
 * Decrements the DPC call reverse barier.
 *
 * @param SystemArgument
 *        Supplies an address of the DPC call barrier.
 *
 * @return This routine returns TRUE if just one processor is waiting on the barrier, FALSE if more.
 *
 * @since NT 5.2
 */
XTAPI
BOOLEAN
KeSignalCallDpcSynchronize(IN PVOID SystemArgument)
{
    UNIMPLEMENTED;

    /* SMP not yet implemented, return TRUE */
    return TRUE;
}

/**
 * Retires the expired DPC objects found in the DPC list.
 *
 * @param Prcb
 *        Supplies apointer to the Prcessor Control Block (PRCB).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KepRetireDpcList(IN PKPROCESSOR_CONTROL_BLOCK Prcb)
{
    UNIMPLEMENTED;
}
