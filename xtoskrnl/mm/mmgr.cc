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

    if(MM::Paging::GetXpaStatus())
    {
        MemoryLayout.PfnDatabaseAddress = (PMMPFN)0xFFFEFA8000000000ULL;
        MemoryLayout.SelfMapAddress = (PVOID)0xFFEDF6FB7DBEDF68ULL;

        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabaseAddress + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFEFFFFFFBFFFFFULL;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFEF8A000000000ULL;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);
    }
    else
    {
        MemoryLayout.PfnDatabaseAddress = (PMMPFN)0xFFFFFA8000000000ULL;
        MemoryLayout.SelfMapAddress = (PVOID)0xFFFFF6FB7DBEDF68ULL;

        MemoryLayout.NonPagedPoolStart = (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabaseAddress + PfnDatabaseSize * MM_PAGE_SIZE);
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFFFFFFFFBFFFFFULL;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFFF8A000000000ULL;
        MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart + PagedPoolSize) - 1);
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
