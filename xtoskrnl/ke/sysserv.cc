/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/sysserv.cc
 * DESCRIPTION:     System Services Descriptor Table
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves a pointer to the System Services Descriptor Table.
 *
 * @return This routine returns a pointer to the system services descriptor table.
 *
 * @since XT 1.0
 */
XTAPI
PKSERVICE_DESCRIPTOR_TABLE
KE::SystemServices::GetSystemServicesDescriptorTable(VOID)
{
    /* Return NULLPTR as the system services table is not yet implemented */
    return NULLPTR;
}
