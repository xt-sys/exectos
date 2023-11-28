/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/irqs.c
 * DESCRIPTION:     Kernel interrupts support for i686 architecture
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
    ProcessorBlock->IdtBase[(UCHAR) Vector].Offset = (USHORT)((ULONG)Handler & 0xFFFF);
    ProcessorBlock->IdtBase[(UCHAR) Vector].ExtendedOffset = (USHORT)((ULONG)Handler >> 16);
}
