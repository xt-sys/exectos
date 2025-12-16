/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/mmgr.cc
 * DESCRIPTION:     Memory Manager
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves a pointer to the system's virtual memory layout structure.
 *
 * @return This routine returns a pointer to the memory layout structure.
 *
 * @since XT 1.0
 */
XTAPI
PMMMEMORY_LAYOUT
MM::Manager::GetMemoryLayout(VOID)
{
    /* Return a pointer to the global memory layout structure */
    return &MemoryLayout;
}

XTAPI
PFN_NUMBER
MM::Manager::GetNumberOfSystemPtes()
{
    return NumberOfSystemPtes;
}

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

    if(MM::Paging::GetXpaStatus())
    {
        /* Configure memory layout for 5-level paging, using 57bit address space and providing a 128 PB address space */
        MemoryLayout.PfnDatabaseAddress = (PMMPFN)0xFFFEFA8000000000ULL;
        MemoryLayout.SelfMapAddress = (PVOID)0xFFEDF6FB7DBEDF68ULL;

        /* Define the non-paged and paged pool regions */
        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabaseAddress + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFEFFFFFFBFFFFFULL;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFEF8A000000000ULL;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);

        /* Define hyperspace, system PTE space, and the user space limit */
        MemoryLayout.HyperSpaceStart = (PVOID)0xFFFEF70000000000ULL;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xFFFEF77FFFFFFFFFULL;
        MemoryLayout.SystemSpaceStart = (PVOID)0xFFFEF88000000000ULL;
        MemoryLayout.SystemSpaceEnd = (PVOID)((ULONG_PTR)MemoryLayout.SystemSpaceStart + (NumberOfSystemPtes + 1) * MM_PAGE_SIZE);
        MemoryLayout.UserSpaceEnd = (PVOID)0x07FFFFFFFFFFFFFULL;
    }
    else
    {
        /* Configure memory layout for 4-level paging, using 48bit address space and providing a 128 TB address space */
        MemoryLayout.PfnDatabaseAddress = (PMMPFN)0xFFFFFA8000000000ULL;
        MemoryLayout.SelfMapAddress = (PVOID)0xFFFFF6FB7DBEDF68ULL;

        /* Define the non-paged and paged pool regions */
        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabaseAddress + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFFFFFFFFBFFFFFULL;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFFF8A000000000ULL;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);

        /* Define hyperspace, system PTE space, and the user space limit */
        MemoryLayout.HyperSpaceStart = (PVOID)0xFFFFF70000000000ULL;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xFFFFF77FFFFFFFFFULL;
        MemoryLayout.SystemSpaceStart = (PVOID)0xFFFFF88000000000ULL;
        MemoryLayout.SystemSpaceEnd = (PVOID)((ULONG_PTR)MemoryLayout.SystemSpaceStart + (NumberOfSystemPtes + 1) * MM_PAGE_SIZE);
        MemoryLayout.UserSpaceEnd = (PVOID)0x000007FFFFFEFFFFULL;
    }
}

/**
 * Performs an early initialization of the XTOS Memory Manager.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::InitializeMemoryManager(VOID)
{
    /* Scan memory descriptors provided by the boot loader */
    MM::Pfn::ScanMemoryDescriptors();

    /* Check if there are enough physical pages */
    if(MM::Pfn::GetNumberOfPhysicalPages() < MM_MINIMUM_PHYSICAL_PAGES)
    {
        /* Insufficient physical pages, kernel panic */
        DebugPrint(L"Insufficient physical pages! Install additional memory\n");
        KE::Crash::Panic(0);
    }

    /* Compute allocation size for the PFN database */
    MM::Pfn::ComputePfnDatabaseSize();

    /* Initialize memory layout */
    InitializeMemoryLayout();
}

/**
 * Checks whether the specified memory type should be considered as free.
 *
 * @param MemoryType
 *        Specifies the memory type to verify.
 *
 * @return This routine returns TRUE if the specified memory type should be considered as free, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Manager::VerifyMemoryTypeFree(LOADER_MEMORY_TYPE MemoryType)
{
    return ((MemoryType == LoaderFirmwareTemporary) ||
            (MemoryType == LoaderFree) ||
            (MemoryType == LoaderLoadedProgram) ||
            (MemoryType == LoaderOsloaderStack));
}

/**
 * Checks whether the specified memory type should be considered as invisible for the memory manager.
 *
 * @param MemoryType
 *        Specifies the memory type to verify.
 *
 * @return This routine returns TRUE if the specified memory type should be considered as invisible, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Manager::VerifyMemoryTypeInvisible(LOADER_MEMORY_TYPE MemoryType)
{
    return ((MemoryType == LoaderBBTMemory) ||
            (MemoryType == LoaderFirmwarePermanent) ||
            (MemoryType == LoaderSpecialMemory));
}
