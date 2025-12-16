/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/pte.cc
 * DESCRIPTION:     Page Table Entry (PTE) for i686 support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Checks if the virtual address is valid and mapped in the page tables.
 *
 * @param VirtualAddress
 *        The virtual address to check.
 *
 * @return This routine returns TRUE if the address is valid, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Pte::AddressValid(IN PVOID VirtualAddress)
{
    /* Check XPA status */
    if(MM::Paging::GetXpaStatus())
    {
        /* Check if the P5E is valid */
        if(!MM::Paging::PteValid(MM::Paging::GetPpeAddress(VirtualAddress)))
        {
            /* Invalid PPE, return FALSE */
            return FALSE;
        }
    }

    /* Check if PDE and PTE are valid */
    if(!MM::Paging::PteValid(MM::Paging::GetPdeAddress(VirtualAddress)) ||
       !MM::Paging::PteValid(MM::Paging::GetPteAddress(VirtualAddress)))
    {
        /* Invalid PDE or PTE, return FALSE */
        return FALSE;
    }

    /* Address is valid, return TRUE */
    return TRUE;
}

/**
 * Retrieves the base virtual address of the system PTEs.
 *
 * @return This routine returns a pointer to the first PTE in the system PTE space.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MM::Pte::GetSystemPteBaseAddress(VOID)
{
    return MM::Paging::GetPteAddress(NULLPTR);
}

/**
 * Performs the initial setup of the system's page table hierarchy.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::InitializePageTable(VOID)
{
    UNIMPLEMENTED;
}
