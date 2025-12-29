/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pfault.cc
 * DESCRIPTION:     Page fault support for i686 architecture
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Evaluates the PDE for for paged pool and per-session mappings.
 *
 * @param VirtualAddress
 *        Specifies the virtual address to verify.
 *
 * @return This routine returns ACCESS_VIOLATION on PML3 or status code on PML2.
 */
XTFASTCALL
XTSTATUS
MM::PageFault::CheckPdeForPagedPool(IN PVOID VirtualAddress)
{
    /* Check if XPA is enabled */
    if(MM::Paging::GetXpaStatus())
    {
        /* Access violation for PML3 */
        return STATUS_ACCESS_VIOLATION;
    }

    /* Unimplemented path for PML2 */
    UNIMPLEMENTED;

    /* Temporarily, just return access violation */
    return STATUS_ACCESS_VIOLATION;
}
