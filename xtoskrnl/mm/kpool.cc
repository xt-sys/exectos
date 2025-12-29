/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/kpool.cc
 * DESCRIPTION:     Kernel pool memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Allocates a new kernel stack.
 *
 * @param Stack
 *        Supplies a pointer to the memory area that will contain a new kernel stack.
 *
 * @param LargeStack
 *        Determines whether the stack is large or small.
 *
 * @param SystemNode
 *        Specifies a preferred node used for new stack on multi-processor systems.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::KernelPool::AllocateKernelStack(OUT PVOID *Stack,
                                    IN ULONG StackSize)
{
    PFN_COUNT StackPtes, StackPages;
    PMMPTE PointerPte, StackPte;
    MMPTE TempPte, InvalidPte;
    PFN_NUMBER PageFrameIndex;
    ULONG Index;

    /* Initialize the output stack pointer to NULLPTR */
    *Stack = NULLPTR;

    /* Convert the requested stack size into a page count */
    StackPtes = SIZE_TO_PAGES(StackSize);
    StackPages = StackPtes;

    /* Reserve PTEs for the stack pages, plus a guard page */
    StackPte = MM::Pte::ReserveSystemPtes(StackPtes + 1, SystemPteSpace, 0);
    if(!StackPte)
    {
        /* Failed to reserve PTEs for the new kernel stack */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Set up a template for an invalid PTE */
    MM::Paging::SetPte(&InvalidPte, 0, 0x18);

    /* Set up a template for a valid, writable stack PTE */
    MM::Paging::ClearPte(&TempPte);
    MM::Paging::SetPte(&TempPte, 0, MM_PTE_READWRITE | MM_PTE_CACHE_ENABLE);

    /* Acquire the PFN database lock and raise runlevel to DISPATCH_LEVEL */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard SpinLock(SystemSpaceLock);

    /* Start iterating from the base of the reserved PTE block */
    PointerPte = StackPte;

    /* Loop through each page of the stack that needs to be allocated */
    for(Index = 0; Index < StackPages; Index++)
    {
        /* Advance to the next PTE */
        PointerPte = MM::Paging::GetNextPte(PointerPte);

        /* Allocate a physical page and temporarily mark the PTE as invalid */
        PageFrameIndex = MM::Pfn::AllocatePhysicalPage(MM::Colors::GetNextColor());
        *PointerPte = InvalidPte;

        /* Associate the physical page with its corresponding PTE in the PFN database */
        MM::Pfn::LinkPfnToPte(PageFrameIndex, PointerPte, 1);

        /* Make the PTE valid, mapping the virtual address to the physical page */
        MM::Paging::SetPte(&TempPte, PageFrameIndex, 0);
        *PointerPte = TempPte;
    }

    /* Zero the newly allocated stack memory, skipping the guard page */
    RTL::Memory::ZeroMemory(MM::Paging::GetPteVirtualAddress(MM::Paging::GetNextPte(StackPte)), MM_PAGE_SIZE * StackPtes);

    /* Return a pointer to the top of the new stack */
    *Stack = MM::Paging::GetPteVirtualAddress(MM::Paging::AdvancePte(StackPte, StackPtes + 1));
    return STATUS_SUCCESS;
}

/**
 * Allocates a buffer for structures needed by a processor and assigns it to a corresponding CPU.
 *
 * @param CpuNumber
 *        Specifies the zero-indexed CPU number as an owner of the allocated structures.
 *
 * @param StructuresData
 *        Supplies a pointer to the memory area that will contain the allocated buffer.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::KernelPool::AllocateProcessorStructures(IN ULONG CpuNumber,
                                            OUT PVOID *StructuresData)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    PVOID ProcessorStructures;
    UINT_PTR Address;

    /* Not implemented yet, this is just a hack */
    UNIMPLEMENTED;

    /* Assign memory for processor structures from preallocated buffer */
    ProcessorStructures = &ProcessorStructuresData[CpuNumber - 1];

    /* Make sure all structures are zeroed */
    RTL::Memory::ZeroMemory(ProcessorStructures, KPROCESSOR_STRUCTURES_SIZE);

    /* Align address to page size boundary and find a space for processor block */
    Address = ROUND_UP((UINT_PTR)ProcessorStructures, MM_PAGE_SIZE);
    ProcessorBlock = (PKPROCESSOR_BLOCK)((PUCHAR)Address + (2 * KERNEL_STACK_SIZE) + (GDT_ENTRIES * sizeof(KGDTENTRY)));

    /* Store processor number in the processor block */
    ProcessorBlock->CpuNumber = CpuNumber;

    /* Return pointer to the processor structures */
    *StructuresData = ProcessorStructures;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Destroys a kernel stack and frees page table entry.
 *
 * @param Stack
 *        Supplies a pointer to the memory area containing a kernel stack.
 *
 * @param LargeStack
 *        Determines whether the stack is large or small.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::KernelPool::FreeKernelStack(IN PVOID Stack,
                                IN ULONG StackSize)
{
    UNIMPLEMENTED;
}

/**
 * Destroys an unused set of processor structures.
 *
 * @param StructuresData
 *        Supplies a pointer to the memory area containing the allocated buffer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::KernelPool::FreeProcessorStructures(IN PVOID StructuresData)
{
    UNIMPLEMENTED;
}
