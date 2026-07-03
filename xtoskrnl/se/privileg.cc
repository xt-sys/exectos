/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/se/privileg.cc
 * DESCRIPTION:     Privileges Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
* Checks if the current thread's access token holds a specific privilege.
*
* @param PrivilegeValue
*        Supplies the locally unique identifier (LUID) of the privilege to check.
*
* @param ProcessorMode
*        Supplies the processor mode from which the request originated.
*
* @return This routine returns TRUE if the privilege is held, or FALSE otherwise.
*
* @since XT 1.0
*/
XTAPI
BOOLEAN
SE::Privileges::CheckSinglePrivilege(LUID PrivilegeValue,
                                     KPROCESSOR_MODE ProcessorMode)
{
    UNIMPLEMENTED;

    /* Return TRUE */
    return TRUE;
}
