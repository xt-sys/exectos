/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/kpools.c
 * DESCRIPTION:     Kernel pool memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
MmAllocateKernelStack(IN PVOID *Stack,
                      IN BOOLEAN LargeStack,
                      IN UCHAR SystemNode)
{
    UNIMPLEMENTED;
    return STATUS_NOT_IMPLEMENTED;
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
MmAllocateProcessorStructures(IN ULONG CpuNumber,
                              OUT PVOID *StructuresData)
{
    PKPROCESSOR_BLOCK ProcessorBlock;
    PVOID ProcessorStructures;
    UINT_PTR Address;

    /* Not implemented yet, this is just a hack */
    UNIMPLEMENTED;

    /* Assign memory for processor structures from preallocated buffer */
    ProcessorStructures = &MmProcessorStructuresData[CpuNumber - 1];

    /* Make sure all structures are zeroed */
    RtlZeroMemory(ProcessorStructures, KPROCESSOR_STRUCTURES_SIZE);

    /* Align address to page size boundary and find a space for processor block */
    Address = ROUND_UP((UINT_PTR)ProcessorStructures, MM_PAGE_SIZE);
    ProcessorBlock = (PKPROCESSOR_BLOCK)((PUCHAR)Address + (2 * KERNEL_STACK_SIZE) + sizeof(ArInitialGdt));

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
MmFreeKernelStack(IN PVOID Stack,
                  IN BOOLEAN LargeStack)
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
MmFreeProcessorStructures(IN PVOID StructuresData)
{
    UNIMPLEMENTED;
}
