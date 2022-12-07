/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/i686/memory.c
 * DESCRIPTION:     EFI memory management for i686 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


VOID
BlCreateStack(IN PVOID *StackPtr,
              IN ULONG StackSize,
              IN PVOID Callback)
{
    EFI_PHYSICAL_ADDRESS Address;
    PVOID StackEnd;

    /* Allocate pages for new stack and calculate its end */
    BlEfiMemoryAllocatePages(StackSize, &Address);
    *StackPtr = (PVOID)(UINT_PTR)Address;
    StackEnd = (PUINT8)*StackPtr + (StackSize * EFI_PAGE_SIZE) - EFI_PAGE_SIZE;

    /* Create new stack and switch to it immediatelly by calling callback function */
    asm volatile("mov %1, %%eax\n"
                 "mov %%esp, %%ebx\n"
                 "mov %0, %%esp\n"
                 "push %%ebp\n"
                 "mov %%esp, %%ebp\n"
                 "push %%ebx\n"
                 "sub $32, %%esp\n"
                 "call *%%eax\n"
                 :
                 : "m" (StackEnd), "m" (Callback)
                 : "eax", "ebx");
}

/**
 * This routine does the actual virtual memory mapping.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a virtual address of the mapping.
 *
 * @param PhysicalAddress
 *        Supplies a physical address of the mapping.
 *
 * @param NumberOfPages
 *        Supplies a number of the pages of the mapping.
 *
 * @param PtePointer
 *        Supplies a pointer to an array of pointers to page table entries.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlMapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                   IN UINT_PTR VirtualAddress,
                   IN UINT_PTR PhysicalAddress,
                   IN UINT NumberOfPages,
                   IN OUT PVOID *PtePointer)
{
    return STATUS_EFI_SUCCESS;
}
