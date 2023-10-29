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
