/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/mmgr.cc
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
    ULONGLONG PhysicalPages;

    /* Get number of physical pages */
    PhysicalPages = MM::Pfn::GetNumberOfPhysicalPages();

    /* Start with 1MiB and reserve space for PFN database */
    MaximumNonPagedPoolSize = 1048576;

    /* Check if system has at least 512MiB of physical memory */
    if(PhysicalPages >= 126976)
    {
        /* Add 200KiB for each MiB above 4MiB */
        MaximumNonPagedPoolSize += ((PhysicalPages - 1024)/256) * 204800;

        /* Check if non-paged pool has at least 128MiB */
        if(MaximumNonPagedPoolSize < 134217728)
        {
            /* Expand non-paged pool size to 128MiB */
            MaximumNonPagedPoolSize = 134217728;
        }
    }
    else
    {
        /* Add 400KiB for each MiB above 4MiB */
        MaximumNonPagedPoolSize += ((PhysicalPages - 1024)/256) * 409600;
    }

    /* Check if non-paged pool does not exceed 256MiB */
    if(MaximumNonPagedPoolSize > 268435456)
    {
        /* Limit non-paged pool size to 256MiB */
        MaximumNonPagedPoolSize = 268435456;
    }

    /* Return maximum non-paged pool size in pages */
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
    ULONGLONG PhysicalPages;

    /* Get number of physical pages */
    PhysicalPages = MM::Pfn::GetNumberOfPhysicalPages();

    /* Verify if system has less than 256MiB of physical memory */
    if(PhysicalPages <= 65536)
    {
        /* Reduce initial non-paged pool size to 2MiB to save memory */
        NonPagedPoolSize = 2097152;
    }
    else
    {
        /* Start with 256KiB and add 32KiB for each MiB above 4MiB */
        NonPagedPoolSize = 262144 + (((PhysicalPages - 1024) / 256) * 32768);

        if(NonPagedPoolSize > 134217728)
        {
            /* Limit non-paged pool size to 128MiB */
            NonPagedPoolSize = 134217728;
        }
    }

    /* Return non-paged pool size in pages, aligned down to page size boundary */
    *PoolSize = SIZE_TO_PAGES(ROUND_DOWN(NonPagedPoolSize, MM_PAGE_SIZE));
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
    ULONG_PTR PagedPoolSize, PteCount;
    ULONG PtesPerPage;

    /* Start with maximum non-paged pool size */
    ComputeMaximumNonPagedPoolSize(&PagedPoolSize);
    PagedPoolSize *= MM_PAGE_SIZE;

    /* Check XPA status */
    if(MM::Paging::GetXpaStatus())
    {
        /* Four times the non-paged pool size on PAE-enabled systems */
        PagedPoolSize *= 4;
    }
    else
    {
        /* Double the non-paged pool size on PAE-disabled systems */
        PagedPoolSize *= 2;
    }

    /* Check if paged pool does not overlap non-paged pool */
    if(PagedPoolSize > (ULONG_PTR)MemoryLayout.NonPagedSystemPoolStart - (ULONG_PTR)MemoryLayout.PagedPoolStart)
    {
        /* Limit paged pool size to maximum possible */
        PagedPoolSize = (ULONG_PTR)MemoryLayout.NonPagedSystemPoolStart - (ULONG_PTR)MemoryLayout.PagedPoolStart;
    }

    /* Ensure that paged pool size is at least 32MiB */
    if(PagedPoolSize < 33554432)
    {
        /* Increase paged pool size to at least 32MiB */
        PagedPoolSize = 33554432;
    }

    /* Get the number of PTEs per page and calculate size of paged pool */
    PtesPerPage = MM::Pte::GetPtesPerPage();
    PteCount = ((SIZE_TO_PAGES(PagedPoolSize) + (PtesPerPage - 1)) / PtesPerPage);

    /* Return paged pool size */
    *PoolSize = (PFN_NUMBER)(PteCount * PtesPerPage);
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

    /* Session Pool, Session View, Session Image, Session Working Set and System View takes 108MiB */
    SessionSpaceSize = 113246208;

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

    /* Check if system has less than 19MiB of physical memory */
    if(MM::Pfn::GetNumberOfPhysicalPages() < 4864)
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
               L"User Space:               %.8P - %.8P\n"
               L"Loader Mappings:          %.8P - %.8P\n"
               L"PFN Database:             %.8P - %.8P\n"
               L"Non-Paged Pool:           %.8P - %.8P\n"
               L"Session Space:            %.8P - %.8P\n"
               L"PTE Space:                %.8P - %.8P\n"
               L"Hyper Space:              %.8P - %.8P\n"
               L"System Working Set:       %.8P - %.8P\n"
               L"System Cache:             %.8P - %.8P\n"
               L"Paged Pool:               %.8P - %.8P\n"
               L"Non-Paged System Pool:    %.8P - %.8P\n"
               L"Non-Paged Expansion Pool: %.8P - %.8P\n"
               L"Shared System Page:       %.8P - %.8P\n"
               L"Hardware Pool:            %.8P - %.8P\n",
               GetInstalledMemorySize(),
               MemoryLayout.UserSpaceStart,
               MemoryLayout.UserSpaceEnd,
               MemoryLayout.LoaderMappingsStart,
               MemoryLayout.LoaderMappingsEnd,
               MemoryLayout.PfnDatabase,
               (PVOID)((ULONG_PTR)MemoryLayout.PfnDatabase + (ULONG_PTR)MemoryLayout.PfnDatabaseSize * MM_PAGE_SIZE),
               MemoryLayout.NonPagedPoolStart,
               MemoryLayout.NonPagedPoolEnd,
               MemoryLayout.SessionSpaceStart,
               MemoryLayout.SessionSpaceEnd,
               MemoryLayout.PteSpaceStart,
               MemoryLayout.PteSpaceEnd,
               MemoryLayout.HyperSpaceStart,
               MemoryLayout.HyperSpaceEnd,
               MemoryLayout.SystemWorkingSetStart,
               MemoryLayout.SystemWorkingSetEnd,
               MemoryLayout.SystemCacheStart,
               MemoryLayout.SystemCacheEnd,
               MemoryLayout.PagedPoolStart,
               MemoryLayout.PagedPoolEnd,
               MemoryLayout.NonPagedSystemPoolStart,
               MemoryLayout.NonPagedSystemPoolEnd,
               MemoryLayout.NonPagedExpansionPoolStart,
               MemoryLayout.NonPagedExpansionPoolEnd,
               MemoryLayout.SharedSystemPageStart,
               MemoryLayout.SharedSystemPageEnd,
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

    /* Check if 3-level paging (PAE) is enabled */
    if(MM::Paging::GetXpaStatus())
    {
        /* Set PML3 base address */
        MemoryLayout.SelfMapAddress = (PVOID)MM_PTE_BASE;

        /* Define memory layout for 3-level paging */
        MemoryLayout.NonCanonicalStart = (PVOID)0x00000000;
        MemoryLayout.NonCanonicalEnd = (PVOID)0x00000000;
        MemoryLayout.ReservedSystemPoolStart = (PVOID)0x00000000;
        MemoryLayout.ReservedSystemPoolEnd = (PVOID)0x00000000;
        MemoryLayout.UserSpaceStart = (PVOID)0x00010000;
        MemoryLayout.UserSpaceEnd = (PVOID)0x7FFEFFFF;
        MemoryLayout.LoaderMappingsStart = (PVOID)0x80000000;
        MemoryLayout.LoaderMappingsEnd = (PVOID)0x90000000;
        MemoryLayout.NonPagedPoolStart = (PVOID)0x90000000;
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xB0000000;
        MemoryLayout.SessionSpaceStart = (PVOID)0xB0000000;
        MemoryLayout.SessionSpaceEnd = (PVOID)0xC0000000;
        MemoryLayout.PteSpaceStart = (PVOID)0xC0000000;
        MemoryLayout.PteSpaceEnd = (PVOID)0xC07FFFFF;
        MemoryLayout.HyperSpaceStart = (PVOID)0xC0800000;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xC0BFFFFF;
        MemoryLayout.SystemWorkingSetStart = (PVOID)0xC0C00000;
        MemoryLayout.SystemWorkingSetEnd = (PVOID)0xC0FFFFFF;
        MemoryLayout.SystemCacheStart = (PVOID)0xC1000000;
        MemoryLayout.SystemCacheEnd = (PVOID)0xE0FFFFFF;
        MemoryLayout.PagedPoolStart = (PVOID)0xE1000000;
        MemoryLayout.PagedPoolEnd = (PVOID)0xECC00000;
        MemoryLayout.NonPagedSystemPoolStart = (PVOID)0xECC00000;
        MemoryLayout.NonPagedSystemPoolEnd = (PVOID)0xF7BE0000;
        MemoryLayout.NonPagedExpansionPoolStart = (PVOID)0xF7BE0000;
        MemoryLayout.NonPagedExpansionPoolEnd = (PVOID)0xFFBFF000;
        MemoryLayout.SharedSystemPageStart = (PVOID)0xFFBFF000;
        MemoryLayout.SharedSystemPageEnd = (PVOID)0xFFC00000;
        MemoryLayout.HardwarePoolStart = (PVOID)0xFFC00000;
        MemoryLayout.HardwarePoolEnd = (PVOID)0xFFFFFFFF;
    }
    else
    {
        /* Set PML2 base address */
        MemoryLayout.SelfMapAddress = (PVOID)MM_PTE_BASE;

        /* Define memory layout for 2-level paging */
        MemoryLayout.NonCanonicalStart = (PVOID)0x00000000;
        MemoryLayout.NonCanonicalEnd = (PVOID)0x00000000;
        MemoryLayout.ReservedSystemPoolStart = (PVOID)0x00000000;
        MemoryLayout.ReservedSystemPoolEnd = (PVOID)0x00000000;
        MemoryLayout.UserSpaceStart = (PVOID)0x00010000;
        MemoryLayout.UserSpaceEnd = (PVOID)0x7FFEFFFF;
        MemoryLayout.LoaderMappingsStart = (PVOID)0x80000000;
        MemoryLayout.LoaderMappingsEnd = (PVOID)0x90000000;
        MemoryLayout.NonPagedPoolStart = (PVOID)0x90000000;
        MemoryLayout.NonPagedPoolEnd = (PVOID)0xB0000000;
        MemoryLayout.SessionSpaceStart = (PVOID)0xB0000000;
        MemoryLayout.SessionSpaceEnd = (PVOID)0xC0000000;
        MemoryLayout.PteSpaceStart = (PVOID)0xC0000000;
        MemoryLayout.PteSpaceEnd = (PVOID)0xC03FFFFF;
        MemoryLayout.HyperSpaceStart = (PVOID)0xC0400000;
        MemoryLayout.HyperSpaceEnd = (PVOID)0xC07FFFFF;
        MemoryLayout.SystemWorkingSetStart = (PVOID)0xC0C00000;
        MemoryLayout.SystemWorkingSetEnd = (PVOID)0xC0FFFFFF;
        MemoryLayout.SystemCacheStart = (PVOID)0xC1000000;
        MemoryLayout.SystemCacheEnd = (PVOID)0xE0FFFFFF;
        MemoryLayout.PagedPoolStart = (PVOID)0xE1000000;
        MemoryLayout.PagedPoolEnd = (PVOID)0xECC00000;
        MemoryLayout.NonPagedSystemPoolStart = (PVOID)0xECC00000;
        MemoryLayout.NonPagedSystemPoolEnd = (PVOID)0xF7BE0000;
        MemoryLayout.NonPagedExpansionPoolStart = (PVOID)0xF7BE0000;
        MemoryLayout.NonPagedExpansionPoolEnd = (PVOID)0xFFBFF000;
        MemoryLayout.SharedSystemPageStart = (PVOID)0xFFBFF000;
        MemoryLayout.SharedSystemPageEnd = (PVOID)0xFFC00000;
        MemoryLayout.HardwarePoolStart = (PVOID)0xFFC00000;
        MemoryLayout.HardwarePoolEnd = (PVOID)0xFFFFFFFF;
    }

    /* Compute allocation size for the PFN database */
    MM::Pfn::ComputePfnDatabaseSize(&MemoryLayout.PfnDatabaseSize);

    /* Compute boot image size */
    ComputeBootImageSize(&MemoryLayout.LoaderMappingsSize);

    /* Compute session space size */
    ComputeSessionSpaceSize(&MemoryLayout.SessionSpaceSize);

    /* Update loader mappings space end address */
    MemoryLayout.LoaderMappingsEnd = (PVOID)((ULONG_PTR)MemoryLayout.LoaderMappingsStart +
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

    /* Insert the PFN database right after the loader mappings */
    MemoryLayout.PfnDatabase = (PMMPFN)MemoryLayout.LoaderMappingsEnd;

    /* Compute the PFN database page-aligned end address */
    PfnDatabaseEnd = (ULONG_PTR)MemoryLayout.PfnDatabase + (MemoryLayout.PfnDatabaseSize * MM_PAGE_SIZE);
    PfnDatabaseEnd = ROUND_UP(PfnDatabaseEnd, MM_PAGE_SIZE);

    /* Check in non-paged pool fits before session space */
    if(MemoryLayout.NonPagedPoolSize * MM_PAGE_SIZE <= ((ULONG_PTR)MemoryLayout.SessionSpaceStart - PfnDatabaseEnd))
    {
        /* Set non-paged pool start and end addresses */
        MemoryLayout.NonPagedPoolStart = (PVOID)PfnDatabaseEnd;
        MemoryLayout.NonPagedPoolEnd = (PVOID)(PfnDatabaseEnd + MemoryLayout.NonPagedPoolSize * MM_PAGE_SIZE);

        /* Check if non-paged expansion pool overflows */
        if((ULONG_PTR)MemoryLayout.NonPagedExpansionPoolStart + MaximumNonPagedPoolSize *
           MM_PAGE_SIZE >= (ULONG_PTR)MemoryLayout.NonPagedExpansionPoolStart)
        {
            /* Check if non-paged expansion pool fits */
            if((ULONG_PTR)MemoryLayout.NonPagedExpansionPoolStart + MaximumNonPagedPoolSize *
               MM_PAGE_SIZE <= (ULONG_PTR)MemoryLayout.NonPagedExpansionPoolEnd)
            {
                /* Set non-paged expansion pool end address */
                MemoryLayout.NonPagedExpansionPoolEnd = (PVOID)((ULONG_PTR)MemoryLayout.NonPagedExpansionPoolStart +
                                                                MaximumNonPagedPoolSize * MM_PAGE_SIZE);
            }
        }

        /* Compute non-paged expansion pool size */
        MemoryLayout.NonPagedExpansionPoolSize = ((ULONG_PTR)MemoryLayout.NonPagedExpansionPoolEnd -
                                                  (ULONG_PTR)MemoryLayout.NonPagedExpansionPoolStart) / MM_PAGE_SIZE;
    }
    else
    {
        /* Unfortunally non-paged pool does not fit before session space. What can we do? */
    }

    /* Update paged pool end address */
    MemoryLayout.PagedPoolEnd = (PVOID)(((ULONG_PTR)MemoryLayout.PagedPoolStart +
                                         MemoryLayout.PagedPoolSize * MM_PAGE_SIZE) - 1);

    /* Dump memory layout */
    DumpMemoryLayout();
}
