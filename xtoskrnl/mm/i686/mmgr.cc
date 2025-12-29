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
    ULONG_PTR PagedPoolSize, PteCount;
    PFN_NUMBER PfnDatabaseSize, PhysicalPages;
    ULONG PtesPerPage;

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

    /* Calculate size of paged pool */
    PtesPerPage = MM::Pte::GetPtesPerPage();
    PteCount = (SIZE_TO_PAGES(33554432) + (PtesPerPage - 1)) / PtesPerPage;
    PagedPoolSize = PteCount * PtesPerPage * MM_PAGE_SIZE;

    /* Retrieve the PFN database size */
    PfnDatabaseSize = MM::Pfn::GetPfnDatabaseSize();

    /* Check if 3-level paging (PAE) is enabled */
    if(MM::Paging::GetXpaStatus())
    {
        /* Configure memory layout for 3-level paging, using 36bit address space and providing a 64 GB address space */
        MemoryLayout.PfnDatabaseAddress = (PMMPFN)0xB0000000;
        MemoryLayout.SelfMapAddress = (PVOID)MM_PTE_BASE;

        /* Define the non-paged and paged pool regions */
        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabaseAddress + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xEEFFFFFF;
        MemoryLayout.PagedPoolStart = (PVOID)0xE2000000;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);

        /* Define hyperspace, system PTE space, and the user space limit */
        MemoryLayout.HyperSpaceStart = (PVOID)0xC0800000;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xC0BFFFFF;
        MemoryLayout.SystemSpaceStart = (PVOID)0xC0C00000;
        MemoryLayout.SystemSpaceEnd = (PVOID)((ULONG_PTR)MemoryLayout.SystemSpaceStart + (NumberOfSystemPtes + 1) * MM_PAGE_SIZE);
        MemoryLayout.UserSpaceEnd = (PVOID)0x7FFEFFFF;
    }
    else
    {
        /* Configure memory layout for 2-level paging, using 32bit address space and providing a 4 GB address space */
        MemoryLayout.PfnDatabaseAddress = (PMMPFN)0xB0000000;
        MemoryLayout.SelfMapAddress = (PVOID)MM_PTE_BASE;

        /* Define the non-paged and paged pool regions */
        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabaseAddress + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFBE0000;
        MemoryLayout.PagedPoolStart = (PVOID)0xE1000000;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);

        /* Define hyperspace, system PTE space, and the user space limit */
        MemoryLayout.HyperSpaceStart = (PVOID)0xC0400000;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xC07FFFFF;
        MemoryLayout.SystemSpaceStart = (PVOID)0xC0800000;
        MemoryLayout.SystemSpaceEnd = (PVOID)((ULONG_PTR)MemoryLayout.SystemSpaceStart + (NumberOfSystemPtes + 1) * MM_PAGE_SIZE);
        MemoryLayout.UserSpaceEnd = (PVOID)0x7FFEFFFF;
    }
}
