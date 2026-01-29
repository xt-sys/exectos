/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/alloc.cc
 * DESCRIPTION:     Memory manager pool allocation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


XTAPI
VOID
MM::Allocator::InitializeNonPagedPool(VOID)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    UNIMPLEMENTED;

    /* Retrieve memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Map PPE and PDE for whole non-paged pool */
    MM::Pte::MapPPE(MemoryLayout->NonPagedPoolStart, MemoryLayout->NonPagedExpansionPoolEnd, MM::Pte::GetValidPte());
    MM::Pte::MapPDE(MemoryLayout->NonPagedPoolStart, MemoryLayout->NonPagedExpansionPoolEnd, MM::Pte::GetValidPte());

    /* Map PTE only for the base of the non-paged pool */
    MM::Pte::MapPTE(MemoryLayout->NonPagedPoolStart, (PCHAR)MemoryLayout->NonPagedPoolEnd - 1, MM::Pte::GetValidPte());
}
