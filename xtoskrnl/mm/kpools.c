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
