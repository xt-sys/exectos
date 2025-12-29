/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/pfault.cc
 * DESCRIPTION:     Page fault support for AMD64 architecture
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Evaluates the PDE for for paged pool and per-session mappings.
 *
 * @param VirtualAddress
 *        Specifies the virtual address to verify.
 *
 * @return This routine returns ACCESS_VIOLATION regardless PML4 or PML5 is used.
 */
XTFASTCALL
XTSTATUS
MM::PageFault::CheckPdeForPagedPool(IN PVOID VirtualAddress)
{
    /* Return access violation */
    return STATUS_ACCESS_VIOLATION;
}
