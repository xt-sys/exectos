/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/i686/irq.cc
 * DESCRIPTION:     Interrupts support for i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Sets new interrupt handler for the existing IDT entry.
 *
 * @param HalVector
 *        Supplies the HAL vector number.
 *
 * @param Handler
 *        Supplies the new interrupt handler.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Irq::SetInterruptHandler(IN ULONG Vector,
                             IN PVOID Handler)
{
    PKPROCESSOR_BLOCK ProcessorBlock;

    /* Get current processor block */
    ProcessorBlock = KE::Processor::GetCurrentProcessorBlock();

    /* Update interrupt handler */
    AR::ProcSup::SetIdtGate(ProcessorBlock->IdtBase,
                            Vector,
                            Handler,
                            KGDT_R0_CODE,
                            0,
                            KIDT_ACCESS_RING0,
                            I686_INTERRUPT_GATE);
}
