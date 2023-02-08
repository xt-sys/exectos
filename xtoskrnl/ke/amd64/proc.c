/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/amd64/proc.c
 * DESCRIPTION:     AMD64 processor-related functionality for the kernel
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Gets the processor block for the currently executing processor.
 *
 * @return This routine returns the current processor block read from the GS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_BLOCK
KeGetCurrentProcessorBlock(VOID)
{
    /* Get processor block from GS register */
    return (PKPROCESSOR_BLOCK)ArReadGSQuadWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Self));
}

/**
 * Gets the processor control block for the currently executing processor.
 *
 * @return This routine returns the current processor control block read from the GS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_CONTROL_BLOCK
KeGetCurrentProcessorControlBlock(VOID)
{
    return (PKPROCESSOR_CONTROL_BLOCK)ArReadGSQuadWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb));
}
