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
    PMMPTE EndSpacePte, PointerPte;
    PMMMEMORY_LAYOUT MemoryLayout;
    CPUID_REGISTERS CpuRegisters;
    MMPTE TemplatePte;

    /* Retrieve current paging mode and memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Get CPU features */
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;
    AR::CpuFunc::CpuId(&CpuRegisters);

    /* Check if Paging Global Extensions (PGE) is supported */
    if(CpuRegisters.Edx & CPUID_FEATURES_EDX_PGE)
    {
        /* Enable the Global Paging (PGE) feature */
        AR::CpuFunc::WriteControlRegister(4, AR::CpuFunc::ReadControlRegister(4) | CR4_PGE);
    }

    /* Get the PD user-space range for both legacy and PAE paging */
    PointerPte = (PMMPTE)MM::Paging::GetPdeAddress(0);
    EndSpacePte = (PMMPTE)MM::Paging::GetPdeAddress(MemoryLayout->UserSpaceEnd);

    /* Clear all top-level entries mapping the user address space */
    while(PointerPte <= EndSpacePte)
    {
        MM::Paging::ClearPte(PointerPte);
        PointerPte = MM::Paging::GetNextPte(PointerPte);
    }

    /* Flush the TLB to invalidate all non-global entries */
    AR::CpuFunc::FlushTlb();

    /* Create a template PTE for mapping kernel pages */
    MM::Paging::ClearPte(&TemplatePte);
    MM::Paging::SetPte(&TemplatePte, 0, MM_PTE_READWRITE | MM_PTE_CACHE_ENABLE);

    /* Map the kernel's PD entries */
    MM::Pte::MapPDE(MemoryLayout->SystemSpaceStart, (PVOID)MM_HIGHEST_SYSTEM_ADDRESS, &TemplatePte);
}

/**
 * Initializes the system's PTE.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::InitializeSystemPte(VOID)
{
    /* Initialize the PTE template */
    ValidPte = {{MM_PTE_VALID|MM_PTE_EXECUTE_READWRITE|MM_PTE_DIRTY|MM_PTE_ACCESSED}};
}

/**
 * Maps a range of virtual addresses at the PPE (Page Directory Pointer Entry) level.
 *
 * @param StartAddress
 *        The beginning of the virtual address range to map.
 *
 * @param EndAddress
 *        The end of the virtual address range to map.
 *
 * @param TemplatePpe
 *        A template PPE to use for creating new entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pte::MapPPE(PVOID StartAddress,
                PVOID EndAddress,
                PMMPPE TemplatePpe)
{
    /* Just a stub on i686 platform */
    return;
}
