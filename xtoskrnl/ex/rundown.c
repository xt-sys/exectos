/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/rundown.c
 * DESCRIPTION:     Rundown protection mechanism
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes the rundown protection descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be initialized.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
ExInitializeRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    /* Reset descriptor counter */
    Descriptor->Count = 0;
}
