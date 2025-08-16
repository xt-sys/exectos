/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pmap.c
 * DESCRIPTION:     Low-level support for i686 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Checks if eXtended Physical Addressing (XPA) is enabled.
 *
 * @return This routine returns TRUE if PAE is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MmpGetExtendedPhysicalAddressingStatus(VOID)
{
    /* Check if PAE is enabled */
    return ((ArReadControlRegister(4) & CR4_PAE) != 0) ? TRUE : FALSE;
}
