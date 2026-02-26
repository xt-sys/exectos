/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/alloc.cc
 * DESCRIPTION:     Memory manager pool allocation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Maps the PTE for the base of the non-paged pool.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::MapNonPagedPool(VOID)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Map PDE and PTE for the base of the non-paged pool */
    MM::Pte::MapPDE(MemoryLayout->NonPagedPoolStart, (PCHAR)MemoryLayout->NonPagedPoolEnd - 1, MM::Pte::GetValidPte());
    MM::Pte::MapPTE(MemoryLayout->NonPagedPoolStart, (PCHAR)MemoryLayout->NonPagedPoolEnd - 1, MM::Pte::GetValidPte());
}
