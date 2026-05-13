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
 * Maps a specific virtual address to a specific physical page frame (i686 specific).
 *
 * @param VirtualAddress
 *        The virtual address to map.
 *
 * @param PageFrameNumber
 *        The physical frame number to back the virtual address.
 *
 * @param Attributes
 *        Specifies the attributes (protections, caching) to apply to the PTE.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Paging::MapVirtualAddress(IN PVOID VirtualAddress,
                              IN PFN_NUMBER PageFrameNumber,
                              IN ULONGLONG Attributes)
{
    MMPTE TemplatePte;
    PMMPTE PointerPte;

    /* Initialize the template PTE */
    MM::Paging::ClearPte(&TemplatePte);
    MM::Paging::SetPte(&TemplatePte, 0, Attributes | MM_PTE_CACHE_ENABLE);

    /* Check if XPA is enabled */
    if(MM::Paging::GetXpaStatus())
    {
        /* Map Page Directory Pointer Table */
        MM::Pte::MapPPE(VirtualAddress, VirtualAddress, (PMMPPE)&TemplatePte);
    }

    /* Map Page Directory Entry */
    MM::Pte::MapPDE(VirtualAddress, VirtualAddress, (PMMPDE)&TemplatePte);

    /* Get PTE address */
    PointerPte = MM::Paging::GetPteAddress(VirtualAddress);

    /* Initialize the template PTE */
    MM::Paging::ClearPte(&TemplatePte);
    MM::Paging::SetPte(&TemplatePte, PageFrameNumber, Attributes);

    /* Write the PTE */
    MM::Paging::WritePte(PointerPte, TemplatePte);

    /* Flush the TLB to reflect the changes */
    MM::Paging::FlushTlb();

    /* Return success */
    return STATUS_SUCCESS;
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
