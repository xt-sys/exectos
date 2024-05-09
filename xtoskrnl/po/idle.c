/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/idle.c
 * DESCRIPTION:     Processor idle functionality support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes Processor Control Block's (PRCB) power state structures.
 *
 * @param Prcb
 *        Supplies a pointer to the PRCB being initialized.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
PoInitializeProcessorControlBlock(IN OUT PKPROCESSOR_CONTROL_BLOCK Prcb)
{
    /* Zero memory */
    RtlZeroMemory(&Prcb->PowerState, sizeof(Prcb->PowerState));

    /* Initialize default power state */
    Prcb->PowerState.Idle0TimeLimit = 0xFFFFFFFF;
    Prcb->PowerState.CurrentThrottle = 100;
    Prcb->PowerState.CurrentThrottleIndex = 0;
    Prcb->PowerState.IdleFunction = PopIdle0Function;

    /* Initialize DPC and Timer */
    KeInitializeDpc(&Prcb->PowerState.PerfDpc, PopPerfIdleDpc, Prcb);
    KeSetTargetProcessorDpc(&Prcb->PowerState.PerfDpc, Prcb->CpuNumber);
    KeInitializeTimer(&Prcb->PowerState.PerfTimer, SynchronizationTimer);
}

/**
 * Processor idle loop routine.
 *
 * @param PowerState
 *        Supplies a pointer to the structere containing current processor power state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
PopIdle0Function(IN PPROCESSOR_POWER_STATE PowerState)
{
    UNIMPLEMENTED;
}

/**
 * Switches CPU between different performance levels.
 *
 * @param PowerState
 *        Supplies a pointer to the structure containing IDLE processor power state.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTAPI
VOID
PopPerfIdle(PPROCESSOR_POWER_STATE PowerState)
{
    UNIMPLEMENTED;
}

/**
 * Checks if CPU is running at the maximum (performance) frequency.
 *
 * @param Dpc
 *        Supplies a pointer to the DPC object.
 *
 * @param DeferredContext
 *        Supplies a pointer to memory area containing current CPU's PRCB.
 *
 * @param SystemArgument1
 *        Supplies a pointer to an unused system argument.
 *
 * @param SystemArgument2
 *        Supplies a pointer to an unused system argument.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTAPI
VOID
PopPerfIdleDpc(IN PKDPC Dpc,
               IN PVOID DeferredContext,
               IN PVOID SystemArgument1,
               IN PVOID SystemArgument2)
{
    UNIMPLEMENTED;
}
