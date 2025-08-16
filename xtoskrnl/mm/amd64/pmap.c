/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/pmap.c
 * DESCRIPTION:     Low-level support for AMD64 page map manipulation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Checks if eXtended Physical Addressing (XPA) is enabled.
 *
 * @return This routine returns TRUE if LA57 is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MmpGetExtendedPhysicalAddressingStatus(VOID)
{
    /* Check if LA57 is enabled */
    return ((ArReadControlRegister(4) & CR4_LA57) != 0) ? TRUE : FALSE;
}
