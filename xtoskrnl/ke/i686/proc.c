/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/i686/proc.c
 * DESCRIPTION:     I686 processor-related functionality for the kernel
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Gets the processor block for the currently executing processor.
 *
 * @return This routine returns the current processor block read from the FS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_BLOCK
KeGetCurrentProcessorBlock(VOID)
{
    /* Get processor block from FS register */
    return (PKPROCESSOR_BLOCK)ArReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Self));
}

/**
 * Gets the processor control block for the currently executing processor.
 *
 * @return This routine returns the current processor control block read from the FS register.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESSOR_CONTROL_BLOCK
KeGetCurrentProcessorControlBlock(VOID)
{
    return (PKPROCESSOR_CONTROL_BLOCK)ArReadFSDualWord(FIELD_OFFSET(KPROCESSOR_BLOCK, Prcb));
}
