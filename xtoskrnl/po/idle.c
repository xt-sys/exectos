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
    RtlZeroMemory(&Prcb->PowerState, sizeof(Prcb->PowerState));

    Prcb->PowerState.Idle0TimeLimit = 0xFFFFFFFF;
    Prcb->PowerState.CurrentThrottle = 100;
    Prcb->PowerState.IdleFunction = PopIdle0Function;

    KeInitializeTimerEx(&Prcb->PowerState.PerfTimer, SynchronizationTimer);
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
