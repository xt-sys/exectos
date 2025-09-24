/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/paging.cc
 * DESCRIPTION:     Architecture dependent paging support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Checks if eXtended Physical Addressing (XPA) is enabled.
 *
 * @return This routine returns TRUE if PAE is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Paging::GetExtendedPhysicalAddressingStatus(VOID)
{
    /* Check if PAE is enabled */
    return ((AR::CpuFunc::ReadControlRegister(4) & CR4_PAE) != 0) ? TRUE : FALSE;
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
    __asm__ volatile("xor %%eax, %%eax\n"
                     "rep stosb"
                     : "=D"(Address),
                       "=c"(Size)
                     : "0"(Address),
                       "1"(Size),
                       "a"(0)
                     : "memory");
}
