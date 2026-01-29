/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/mmgr.cc
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
    PFN_NUMBER PfnDatabaseSize;
    ULONG PtesPerPage;

    /* Get the number of PTEs per page and calculate size of paged pool (at least 32MiB) */
    PtesPerPage = MM::Pte::GetPtesPerPage();
    PteCount = ((SIZE_TO_PAGES(33554432) + (PtesPerPage - 1)) / PtesPerPage);
    PagedPoolSize = PteCount * PtesPerPage * MM_PAGE_SIZE;

    /* Retrieve the PFN database size */
    PfnDatabaseSize = MM::Pfn::GetPfnDatabaseSize();

    /* Define the number of system PTEs */
    NumberOfSystemPtes = MM_DEFAULT_NUMBER_SYSTEM_PTES;

    /* Check if 5-level paging (LA57) is enabled */
    if(MM::Paging::GetXpaStatus())
    {
        /* Configure memory layout for 5-level paging, using 57bit address space and providing a 128 PB address space */
        MemoryLayout.PfnDatabase = (PMMPFN)0xFFFEFA8000000000ULL;
        MemoryLayout.SelfMapAddress = (PVOID)MM_P5E_LA57_BASE;

        /* Define the non-paged and paged pool regions */
        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabase + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFEFFFFFFBFFFFFULL;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFEF8A000000000ULL;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);

        /* Define hyperspace, system PTE space, and the user space limit */
        MemoryLayout.HyperSpaceStart = (PVOID)0xFFFEF70000000000ULL;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xFFFEF77FFFFFFFFFULL;
        MemoryLayout.UserSpaceEnd = (PVOID)0x07FFFFFFFFFFFFFULL;
    }
    else
    {
        /* Configure memory layout for 4-level paging, using 48bit address space and providing a 128 TB address space */
        MemoryLayout.PfnDatabase = (PMMPFN)0xFFFFFA8000000000ULL;
        MemoryLayout.SelfMapAddress = (PVOID)MM_PXE_BASE;

        /* Define the non-paged and paged pool regions */
        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabase + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFFFFFFFFBFFFFFULL;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFFF8A000000000ULL;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);

        /* Define hyperspace, system PTE space, and the user space limit */
        MemoryLayout.HyperSpaceStart = (PVOID)0xFFFFF70000000000ULL;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xFFFFF77FFFFFFFFFULL;
        MemoryLayout.UserSpaceEnd = (PVOID)0x000007FFFFFEFFFFULL;
    }
}
