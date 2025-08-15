/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/pages.c
 * DESCRIPTION:     Architecture dependent paging support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Fills a section of memory with zeroes like RtlZeroMemory(), but in more efficient way.
 *
 * @param Address
 *        Supplies an address of the page to be filled with zeroes.
 *
 * @param Size
 *        Number of bytes to be filled with zeros. This always should be a multiply of page size.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
MmZeroPages(IN PVOID Address,
            IN ULONG Size)
{
    __asm__ volatile("xor %%rax, %%rax\n"
                     "mov %0, %%rdi\n"
                     "mov %1, %%ecx\n"
                     "shr $3, %%ecx\n"
                     "rep stosq\n"
                     :
                     : "m" (Address),
                       "m" (Size)
                     : "rax",
                       "rdi",
                       "ecx",
                       "memory");
}
