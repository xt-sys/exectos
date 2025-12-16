/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/mmgr.cc
 * DESCRIPTION:     Memory Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Initializes the kernel's virtual memory layout.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::InitializeMemoryLayout(VOID)
{
    ULONG PhysicalPages;

    /* Not finished yet */
    UNIMPLEMENTED;

    /* Define the number of system PTEs */
    NumberOfSystemPtes = MM_DEFAULT_NUMBER_SYSTEM_PTES;

    /* Retrieve the number of physical pages */
    PhysicalPages = MM::Pfn::GetNumberOfPhysicalPages();

    /* Verify the number of physical pages */
    if(PhysicalPages < 8192)
    {
        /* Less than 32MiB of physical memory (8192 pages), use the minimum number of system PTEs */
        NumberOfSystemPtes = MM_MINIMUM_NUMBER_SYSTEM_PTES;
    }
    else if(PhysicalPages > 32768)
    {
        /* More than 128MiB of physical memory (32768 pages), use the maximum number of system PTEs */
        NumberOfSystemPtes = MM_MAXIMUM_NUMBER_SYSTEM_PTES;
    }

    /* Check if 3-level paging (PAE) is enabled */
    if(MM::Paging::GetXpaStatus())
    {
        /* Configure memory layout for 3-level paging, using 36bit address space and providing a 64 GB address space */
    }
    else
    {
        /* Configure memory layout for 2-level paging, using 32bit address space and providing a 4 GB address space */
    }
}
