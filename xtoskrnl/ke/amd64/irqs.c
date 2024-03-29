/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/irqs.c
 * DESCRIPTION:     Kernel interrupts support for amd64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
KeSetInterruptHandler(IN ULONG Vector,
                      IN PVOID Handler)
{
    PKPROCESSOR_BLOCK ProcessorBlock;

    /* Get current processor block */
    ProcessorBlock = KeGetCurrentProcessorBlock();

    /* Update interrupt handler */
    ProcessorBlock->IdtBase[(UCHAR) Vector].OffsetLow = ((ULONG_PTR)Handler & 0xFFFF);
    ProcessorBlock->IdtBase[(UCHAR) Vector].OffsetMiddle = (((ULONG_PTR)Handler >> 16) & 0xFFFF);
    ProcessorBlock->IdtBase[(UCHAR) Vector].OffsetHigh = (ULONG_PTR)Handler >> 32;
}
