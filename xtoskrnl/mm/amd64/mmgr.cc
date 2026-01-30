/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/mmgr.cc
 * DESCRIPTION:     Memory Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Calculates the maximum possible size of the non-paged pool.
 *
 * @param PoolSize
 *        A pointer to a variable that will receive the number of pages available for the non-paged pool.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::ComputeMaximumNonPagedPoolSize(OUT PPFN_NUMBER PoolSize)
{
    ULONG_PTR MaximumNonPagedPoolSize;

    /* Start with the 1MiB and add 400KiB for each MiB above 16MiB */
    MaximumNonPagedPoolSize = 1048576 + (((MM::Pfn::GetNumberOfPhysicalPages() - 4096) / 256) * 409600);

    /* Check if non-paged pool does not exceed 128GiB */
    if(MaximumNonPagedPoolSize > 137438953472)
    {
        /* Limit non-paged pool size to 128GiB */
        MaximumNonPagedPoolSize = 137438953472;
    }

    /* Return non-paged pool size */
    *PoolSize = SIZE_TO_PAGES(MaximumNonPagedPoolSize);
}

/**
 * Calculates the size of the non-paged pool.
 *
 * @param PoolSize
 *        A pointer to a variable that will receive the number of pages available for the non-paged pool.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::ComputeNonPagedPoolSize(OUT PPFN_NUMBER PoolSize)
{
    ULONG_PTR NonPagedPoolSize;

    /* Start with 1MiB and add 32KiB for each MiB above 16MiB */
    NonPagedPoolSize = 1048576 + (((MM::Pfn::GetNumberOfPhysicalPages() - 4096) / 256) * 32768);

    /* Check if non-paged pool does not exceed 128GiB */
    if(NonPagedPoolSize > 137438953472)
    {
        /* Limit non-paged pool size to 128GiB */
        NonPagedPoolSize = 137438953472;
    }

    /* Return non-paged pool size in pages, aligned up to page size boundary */
    *PoolSize = SIZE_TO_PAGES(ROUND_UP(NonPagedPoolSize, MM_PAGE_SIZE));
}

/**
 * Calculates the size of the paged pool.
 *
 * @param PoolSize
 *        A pointer to a variable that will receive the number of pages available for the paged pool.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::ComputePagedPoolSize(OUT PPFN_NUMBER PoolSize)
{
    ULONGLONG PagedPoolSize, PteCount;
    ULONG PtesPerPage;

    /* Start with 4x maximum non-paged pool size and at least 48MiB */
    ComputeMaximumNonPagedPoolSize(&PagedPoolSize);
    PagedPoolSize *= 4 * MM_PAGE_SIZE;

    /* Ensure that paged pool size is at least 48MiB */
    if(PagedPoolSize < 50331648)
    {
        /* Increase paged pool size to at least 48MiB */
        PagedPoolSize = 50331648;
    }

    /* Check if paged pool does not overlap non-paged pool */
    if(PagedPoolSize > (ULONGLONG)MemoryLayout.NonPagedSystemPoolStart - (ULONGLONG)MemoryLayout.PagedPoolStart)
    {
        /* Limit paged pool size to maximum possible */
        PagedPoolSize = (ULONGLONG)MemoryLayout.NonPagedSystemPoolStart - (ULONGLONG)MemoryLayout.PagedPoolStart;
    }

    /* Check if paged pool does not exceed 128GiB */
    if(PagedPoolSize > 137438953472)
    {
        /* Limit paged pool size to 128GiB */
        PagedPoolSize = 137438953472;
    }

    /* Get the number of PTEs per page and calculate size of paged pool */
    PtesPerPage = MM::Pte::GetPtesPerPage();
    PteCount = ((SIZE_TO_PAGES(PagedPoolSize) + (PtesPerPage - 1)) / PtesPerPage);

    /* Return paged pool size */
    *PoolSize = PteCount * PtesPerPage;
}

/**
 * Calculates the size of the session space.
 *
 * @param SpaceSize
 *        A pointer to a variable that will receive the number of pages available by the session space.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::ComputeSessionSpaceSize(OUT PPFN_NUMBER SpaceSize)
{
    PFN_NUMBER SessionSpaceSize;

    /* Session Pool, Session View, Session Image, Session Working Set and System View takes 1120MiB */
    SessionSpaceSize = 1174405120;

    /* Return number of pages used by the session space */
    *SpaceSize = SessionSpaceSize / MM_PAGE_SIZE;
}

/**
 * Calculates the size of the system PTEs.
 *
 * @param PteSize
 *        A pointer to a variable that will receive the number of system PTEs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::ComputeSystemPteSize(OUT PPFN_NUMBER PteSize)
{
    PFN_NUMBER SystemPteSize;

    /* Check if system has less than 24MiB of physical memory */
    if(MM::Pfn::GetNumberOfPhysicalPages() < 6144)
    {
        /* Set minimal system PTE size */
        SystemPteSize = 7000;
    }
    else
    {
        /* Use standard system PTE size */
        SystemPteSize = 11000;

        /* Check if system has more than 32MiB of physical memory */
        if(MM::Pfn::GetNumberOfPhysicalPages() > 8192)
        {
            /* Double system PTE size */
            SystemPteSize *= 2;

            /* Check if system has more than 256MiB of physical memory */
            if(MM::Pfn::GetNumberOfPhysicalPages() > 65536)
            {
                /* Double system PTE size */
                SystemPteSize *= 2;
            }
        }
    }

    /* Return system PTE size */
    *PteSize = SystemPteSize;
}

/**
 * Dumps the kernel's memory layout.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::DumpMemoryLayout(VOID)
{
    /* Dump memory layout */
    DebugPrint(L"System with %zu MiB of installed memory:\n"
               L"User Space:               %.16P - %.16P\n"
               L"Non-Canonical:            %.16P - %.16P\n"
               L"Reserved System Pool:     %.16P - %.16P\n"
               L"PTE Space:                %.16P - %.16P\n"
               L"Hyper Space:              %.16P - %.16P\n"
               L"Shared System Page:       %.16P - %.16P\n"
               L"System Working Set:       %.16P - %.16P\n"
               L"Loader Mappings:          %.16P - %.16P\n"
               L"Non-Paged System Pool:    %.16P - %.16P\n"
               L"Paged Pool:               %.16P - %.16P\n"
               L"Session Space:            %.16P - %.16P\n"
               L"System Cache:             %.16P - %.16P\n"
               L"PFN Database:             %.16P - %.16P\n"
               L"Non-Paged Pool:           %.16P - %.16P\n"
               L"Non-Paged Expansion Pool: %.16P - %.16P\n"
               L"Hardware Pool:            %.16P - %.16P\n",
               GetInstalledMemorySize(),
               MemoryLayout.UserSpaceStart,
               MemoryLayout.UserSpaceEnd,
               MemoryLayout.NonCanonicalStart,
               MemoryLayout.NonCanonicalEnd,
               MemoryLayout.ReservedSystemPoolStart,
               MemoryLayout.ReservedSystemPoolEnd,
               MemoryLayout.PteSpaceStart,
               MemoryLayout.PteSpaceEnd,
               MemoryLayout.HyperSpaceStart,
               MemoryLayout.HyperSpaceEnd,
               MemoryLayout.SharedSystemPageStart,
               MemoryLayout.SharedSystemPageEnd,
               MemoryLayout.SystemWorkingSetStart,
               MemoryLayout.SystemWorkingSetEnd,
               MemoryLayout.LoaderMappingsStart,
               MemoryLayout.LoaderMappingsEnd,
               MemoryLayout.NonPagedSystemPoolStart,
               MemoryLayout.NonPagedSystemPoolEnd,
               MemoryLayout.PagedPoolStart,
               MemoryLayout.PagedPoolEnd,
               MemoryLayout.SessionSpaceStart,
               MemoryLayout.SessionSpaceEnd,
               MemoryLayout.SystemCacheStart,
               MemoryLayout.SystemCacheEnd,
               MemoryLayout.PfnDatabase,
               (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabase + (ULONG_PTR)MemoryLayout.PfnDatabaseSize * MM_PAGE_SIZE),
               MemoryLayout.NonPagedPoolStart,
               MemoryLayout.NonPagedPoolEnd,
               MemoryLayout.NonPagedExpansionPoolStart,
               MemoryLayout.NonPagedExpansionPoolEnd,
               MemoryLayout.HardwarePoolStart,
               MemoryLayout.HardwarePoolEnd);
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
    PFN_NUMBER MaximumNonPagedPoolSize;
    ULONG_PTR PfnDatabaseEnd;

    /* Check if 5-level paging (LA57) is enabled */
    if(MM::Paging::GetXpaStatus())
    {
        /* Set PML5 base address */
        MemoryLayout.SelfMapAddress = (PVOID)MM_P5E_LA57_BASE;

        /* Define memory layout for 5-level paging */
        MemoryLayout.UserSpaceStart = (PVOID)0x0000000000010000;
        MemoryLayout.UserSpaceEnd = (PVOID)0x00FFFFFFFFFEFFFF;
        MemoryLayout.NonCanonicalStart = (PVOID)0x0080000000000000;
        MemoryLayout.NonCanonicalEnd = (PVOID)0xFEFFFFFFFFFFFFFF;
        MemoryLayout.ReservedSystemPoolStart = (PVOID)0xFF00000000000000;
        MemoryLayout.ReservedSystemPoolEnd = (PVOID)0xFFECFFFFFFFFFFFF;
        MemoryLayout.PteSpaceStart = (PVOID)0xFFED000000000000;
        MemoryLayout.PteSpaceEnd = (PVOID)0xFFEDFFFFFFFFFFFF;
        MemoryLayout.HyperSpaceStart = (PVOID)0xFFFFF70000000000;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xFFFFF77FFFFFFFFF;
        MemoryLayout.SharedSystemPageStart = (PVOID)0xFFFFF78000000000;
        MemoryLayout.SharedSystemPageEnd = (PVOID)0xFFFFF78000000FFF;
        MemoryLayout.SystemWorkingSetStart = (PVOID)0xFFFFF78000001000;
        MemoryLayout.SystemWorkingSetEnd = (PVOID)0xFFFFF7FFFFFFFFFF;
        MemoryLayout.LoaderMappingsStart = (PVOID)0xFFFFF80000000000;
        MemoryLayout.LoaderMappingsEnd = (PVOID)0xFFFFF87FFFFFFFFF;
        MemoryLayout.NonPagedSystemPoolStart = (PVOID)0xFFFFF88000000000;
        MemoryLayout.NonPagedSystemPoolEnd = (PVOID)0xFFFFF89FFFFFFFFF;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFFF8A000000000;
        MemoryLayout.PagedPoolEnd = (PVOID)0xFFFFF8BFFFFFFFFF;
        MemoryLayout.SessionSpaceStart = (PVOID)0xFFFFF90000000000;
        MemoryLayout.SessionSpaceEnd = (PVOID)0xFFFFF98000000000;
        MemoryLayout.SystemCacheStart = (PVOID)0xFFFFF98000000000;
        MemoryLayout.SystemCacheEnd = (PVOID)0xFFFFFA7FFFFFFFFF;
        MemoryLayout.NonPagedPoolStart = (PVOID)0xFFFFFA8000000000;
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFFFFFFFFBFFFFF;
        MemoryLayout.HardwarePoolStart = (PVOID)0xFFFFFFFFFFC00000;
        MemoryLayout.HardwarePoolEnd = (PVOID)0xFFFFFFFFFFFFFFFF;
    }
    else
    {
        /* Set PML4 base address */
        MemoryLayout.SelfMapAddress = (PVOID)MM_PXE_BASE;

        /* Define memory layout for 4-level paging */
        MemoryLayout.UserSpaceStart = (PVOID)0x0000000000010000;
        MemoryLayout.UserSpaceEnd = (PVOID)0x000007FFFFFEFFFF;
        MemoryLayout.NonCanonicalStart = (PVOID)0x0000800000000000;
        MemoryLayout.NonCanonicalEnd = (PVOID)0xFFFF7FFFFFFFFFFF;
        MemoryLayout.ReservedSystemPoolStart = (PVOID)0xFFFF800000000000;
        MemoryLayout.ReservedSystemPoolEnd = (PVOID)0xFFFFF67FFFFFFFFF;
        MemoryLayout.PteSpaceStart = (PVOID)0xFFFFF68000000000;
        MemoryLayout.PteSpaceEnd = (PVOID)0xFFFFF6FFFFFFFFFF;
        MemoryLayout.HyperSpaceStart = (PVOID)0xFFFFF70000000000;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xFFFFF77FFFFFFFFF;
        MemoryLayout.SharedSystemPageStart = (PVOID)0xFFFFF78000000000;
        MemoryLayout.SharedSystemPageEnd = (PVOID)0xFFFFF78000000FFF;
        MemoryLayout.SystemWorkingSetStart = (PVOID)0xFFFFF78000001000;
        MemoryLayout.SystemWorkingSetEnd = (PVOID)0xFFFFF7FFFFFFFFFF;
        MemoryLayout.LoaderMappingsStart = (PVOID)0xFFFFF80000000000;
        MemoryLayout.LoaderMappingsEnd = (PVOID)0xFFFFF87FFFFFFFFF;
        MemoryLayout.NonPagedSystemPoolStart = (PVOID)0xFFFFF88000000000;
        MemoryLayout.NonPagedSystemPoolEnd = (PVOID)0xFFFFF89FFFFFFFFF;
        MemoryLayout.PagedPoolStart = (PVOID)0xFFFFF8A000000000;
        MemoryLayout.PagedPoolEnd = (PVOID)0xFFFFF8BFFFFFFFFF;
        MemoryLayout.SessionSpaceStart = (PVOID)0xFFFFF90000000000;
        MemoryLayout.SessionSpaceEnd = (PVOID)0xFFFFF98000000000;
        MemoryLayout.SystemCacheStart = (PVOID)0xFFFFF98000000000;
        MemoryLayout.SystemCacheEnd = (PVOID)0xFFFFFA7FFFFFFFFF;
        MemoryLayout.NonPagedPoolStart = (PVOID)0xFFFFFA8000000000;
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xFFFFFFFFFFBFFFFF;
        MemoryLayout.HardwarePoolStart = (PVOID)0xFFFFFFFFFFC00000;
        MemoryLayout.HardwarePoolEnd = (PVOID)0xFFFFFFFFFFFFFFFF;
    }

    /* Compute allocation size for the PFN database */
    MM::Pfn::ComputePfnDatabaseSize(&MemoryLayout.PfnDatabaseSize);

    /* Compute boot image size */
    ComputeBootImageSize(&MemoryLayout.LoaderMappingsSize);

    /* Compute session space size */
    ComputeSessionSpaceSize(&MemoryLayout.SessionSpaceSize);

    /* Update loader mappings space end address */
    MemoryLayout.LoaderMappingsEnd = (PVOID)((ULONGLONG)MemoryLayout.LoaderMappingsStart +
                                             MemoryLayout.LoaderMappingsSize * MM_PAGE_SIZE);

    /* Update session space start address */
    MemoryLayout.SessionSpaceStart = (PVOID)((ULONGLONG)MemoryLayout.SessionSpaceEnd -
                                             MemoryLayout.SessionSpaceSize * MM_PAGE_SIZE);

    /* Compute system PTE size */
    ComputeSystemPteSize(&NumberOfSystemPtes);

    /* Compute non-paged pool size */
    ComputeNonPagedPoolSize(&MemoryLayout.NonPagedPoolSize);
    ComputeMaximumNonPagedPoolSize(&MaximumNonPagedPoolSize);

    /* Compute paged pool size */
    ComputePagedPoolSize(&MemoryLayout.PagedPoolSize);

    /* Insert the PFN database at the beginning of the non-paged pool */
    MemoryLayout.PfnDatabase = (PMMPFN)MemoryLayout.NonPagedPoolStart;

    /* Compute the PFN database page-aligned end address */
    PfnDatabaseEnd = (ULONGLONG)MemoryLayout.PfnDatabase + (MemoryLayout.PfnDatabaseSize * MM_PAGE_SIZE);
    PfnDatabaseEnd = ROUND_UP(PfnDatabaseEnd, MM_PAGE_SIZE);

    /* Shrink the non-paged pool to fit the PFN database */
    MemoryLayout.NonPagedPoolStart = (PVOID)PfnDatabaseEnd;

    /* Assign the rest of the non-paged pool to the expansion pool */
    MemoryLayout.NonPagedExpansionPoolStart = (PVOID)((ULONGLONG)MemoryLayout.NonPagedPoolStart +
                                                      MemoryLayout.NonPagedPoolSize * MM_PAGE_SIZE);
    MemoryLayout.NonPagedPoolEnd = MemoryLayout.NonPagedExpansionPoolStart;
    MemoryLayout.NonPagedExpansionPoolEnd = (PVOID)((ULONGLONG)MemoryLayout.NonPagedPoolStart +
                                                    MaximumNonPagedPoolSize * MM_PAGE_SIZE);
    MemoryLayout.NonPagedExpansionPoolSize = ((ULONGLONG)MemoryLayout.NonPagedExpansionPoolEnd -
                                              (ULONGLONG)MemoryLayout.NonPagedExpansionPoolStart) / MM_PAGE_SIZE;

    /* Update paged pool end address */
    MemoryLayout.PagedPoolEnd = (PVOID)(((ULONGLONG)MemoryLayout.PagedPoolStart +
                                         MemoryLayout.PagedPoolSize * MM_PAGE_SIZE) - 1);

    /* Dump memory layout */
    DumpMemoryLayout();
}
