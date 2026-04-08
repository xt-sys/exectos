/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/amd64/irq.cc
 * DESCRIPTION:     Interrupts support for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Handles profiling interrupt.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common interrupt handler.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HL::Irq::HandleProfileInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    /* Send EOI*/
    HL::Pic::SendEoi();
}

/**
 * Handles unexpected or unmapped system interrupts.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common interrupt handler.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HL::Irq::HandleUnexpectedInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    UNIMPLEMENTED;

    /* Disable interrupts */
    AR::CpuFunc::ClearInterruptFlag();

    /* Print debug message and raise kernel panic */
    DebugPrint(L"ERROR: Caught unexpected interrupt (0x%.2llX)!\n", TrapFrame->Vector);
    KE::Crash::Panic(0x47, TrapFrame->Vector, 0, 0, 0);
}

/**
 * Returns the registered interrupt handler for the specified IDT vector.
 *
 * @param Vector
 *        Supplies the interrupt vector number.
 *
 * @return This routine returns the pointer to the IDT interrupt handler routine.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
HL::Irq::QueryInterruptHandler(IN ULONG Vector)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    PKIDTENTRY IdtEntry;

    /* Get current processor block and IDT entry */
    ProcessorBlock = KE::Processor::GetCurrentProcessorBlock();
    IdtEntry = &ProcessorBlock->IdtBase[Vector];

    /* Return address of the interrupt handler */
    return (PVOID)((ULONGLONG)IdtEntry->OffsetHigh << 32 |
                   (ULONGLONG)IdtEntry->OffsetMiddle << 16 |
                   (ULONGLONG)IdtEntry->OffsetLow);
}

/**
 * Returns the registered interrupt handler for the specified vector.
 *
 * @param Vector
 *        Supplies the interrupt vector number.
 *
 * @return This routine returns the pointer to the interrupt handler routine.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
HL::Irq::QuerySystemInterruptHandler(IN ULONG Vector)
{
    PKPROCESSOR_BLOCK ProcessorBlock;

    /* Get current processor block */
    ProcessorBlock = KE::Processor::GetCurrentProcessorBlock();

    return (PVOID)ProcessorBlock->InterruptDispatchTable[Vector];
}

/**
 * Registers new interrupt handler for the existing IDT entry.
 *
 * @param HalVector
 *        Supplies the interrupt vector number.
 *
 * @param Handler
 *        Supplies the pointer to the interrupt handler routine.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Irq::RegisterInterruptHandler(IN ULONG Vector,
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
                            AMD64_INTERRUPT_GATE);
}

/**
 * Registers the interrupt handler for the specified vector.
 *
 * @param HalVector
 *        Supplies the interrupt vector number.
 *
 * @param Handler
 *        Supplies the pointer to the interrupt handler routine.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Irq::RegisterSystemInterruptHandler(IN ULONG Vector,
                                        IN PINTERRUPT_HANDLER Handler)
{
    PKPROCESSOR_BLOCK ProcessorBlock;

    /* Get current processor block */
    ProcessorBlock = KE::Processor::GetCurrentProcessorBlock();

    /* Update interrupt handler in the processor's interrupt dispatch table */
    ProcessorBlock->InterruptDispatchTable[Vector] = Handler;
}
