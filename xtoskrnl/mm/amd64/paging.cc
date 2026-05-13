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
 * Gets the address of the P5E (Page Map Level 5 Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding P5E.
 *
 * @return This routine returns the address of the P5E, or NULLPTR if LA57 is not enabled.
 *
 * @since XT 1.0
 */
XTAPI
PMMP5E
MM::Paging::GetP5eAddress(IN PVOID Address)
{
    /* Return PDE address */
    return PmlRoutines->GetP5eAddress(Address);
}

/**
 * Gets the virtual address that is mapped by a given Page Map Level 5 Entry.
 *
 * @param P5ePointer
 *        Specifies the address of the P5E.
 *
 * @return This routine returns the virtual address mapped by the P5E, or NULLPTR if LA57 is not enabled.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
MM::Paging::GetP5eVirtualAddress(IN PMMP5E P5ePointer)
{
    /* Return PTE virtual address */
    return PmlRoutines->GetP5eVirtualAddress(P5ePointer);
}

/**
 * Gets the address of the PXE (Extended Page Entry), that maps given address.
 *
 * @param Address
 *        Specifies the virtual address for which to retrieve the corresponding PXE.
 *
 * @return This routine returns the address of the PXE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPXE
MM::Paging::GetPxeAddress(IN PVOID Address)
{
    /* Return PXE address */
    return PmlRoutines->GetPxeAddress(Address);
}

/**
 * Gets the virtual address that is mapped by a given Extended Page Entry.
 *
 * @param PxePointer
 *        Specifies the address of the PXE.
 *
 * @return This routine returns the virtual address mapped by the PXE.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
MM::Paging::GetPxeVirtualAddress(IN PMMPXE PxePointer)
{
    /* Return PXE virtual address */
    return PmlRoutines->GetPxeVirtualAddress(PxePointer);
}

/**
 * Maps a specific virtual address to a specific physical page frame.
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
        /* Map Page 5-level Entry*/
        MM::Pte::MapP5E(VirtualAddress, VirtualAddress, (PMMP5E)&TemplatePte);
    }

    /* Map PXE, PPE and PDE for the corresponding virtual address */
    MM::Pte::MapPXE(VirtualAddress, VirtualAddress, (PMMPXE)&TemplatePte);
    MM::Pte::MapPPE(VirtualAddress, VirtualAddress, (PMMPPE)&TemplatePte);
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
