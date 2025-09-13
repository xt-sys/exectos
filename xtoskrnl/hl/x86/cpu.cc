/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/cpu.cc
 * DESCRIPTION:     HAL x86 (i686/AMD64) processor support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Initializes the processor.
 *
 * @param CpuNumber
 *        Supplies the number of the CPU, that is being initialized.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Cpu::InitializeProcessor(VOID)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    KAFFINITY Affinity;

    /* Get current processor block */
    ProcessorBlock = KE::Processor::GetCurrentProcessorBlock();

    /* Set initial stall factor, CPU number and mask interrupts */
    ProcessorBlock->StallScaleFactor = INITIAL_STALL_FACTOR;
    ProcessorBlock->Idr = 0xFFFFFFFF;

    /* Set processor affinity */
    Affinity = (KAFFINITY) 1 << ProcessorBlock->CpuNumber;

    /* Apply affinity to a set of processors */
    ActiveProcessors |= Affinity;

    /* Initialize APIC for this processor */
    Pic::InitializePic();

    /* Set the APIC running level */
    HL::RunLevel::SetRunLevel(KeGetCurrentProcessorBlock()->RunLevel);
}
