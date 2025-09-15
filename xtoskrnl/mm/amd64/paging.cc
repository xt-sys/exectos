/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/paging.cc
 * DESCRIPTION:     Architecture dependent paging support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Checks if eXtended Physical Addressing (XPA) is enabled.
 *
 * @return This routine returns TRUE if LA57 is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Paging::GetExtendedPhysicalAddressingStatus(VOID)
{
    /* Check if LA57 is enabled */
    return ((AR::CpuFunc::ReadControlRegister(4) & CR4_LA57) != 0) ? TRUE : FALSE;
}

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
MM::Paging::ZeroPages(IN PVOID Address,
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
