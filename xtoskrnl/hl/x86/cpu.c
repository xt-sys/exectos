/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/cpu.c
 * DESCRIPTION:     HAL x86 (i686/AMD64) processor support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes the processor.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlInitializeProcessor(VOID)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    KAFFINITY Affinity;
    ULONG CpuNumber = 0;

    /* Get current processor block */
    ProcessorBlock = KeGetCurrentProcessorBlock();

    /* Set initial stall factor */
    ProcessorBlock->StallScaleFactor = INITIAL_STALL_FACTOR;

    /* Record processor block in the processors table */
    HlpProcessorsIdentity[CpuNumber].ProcessorBlock = ProcessorBlock;

    /* Set processor affinity */
    Affinity = (KAFFINITY) 1 << CpuNumber;

    /* Apply affinity to a set of processors */
    HlpActiveProcessors |= Affinity;

    /* Initialize APIC for this processor */
    HlpInitializeApic();

    /* Set the APIC running level */
    HlSetRunLevel(KeGetCurrentProcessorBlock()->RunLevel);
}
