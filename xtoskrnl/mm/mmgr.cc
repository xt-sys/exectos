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
 * Computes the size of the boot image.
 *
 * @param BootImageSize
 *        Supplies a pointer to a variable that will receive the size of the boot image in pages.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Manager::ComputeBootImageSize(OUT PPFN_NUMBER BootImageSize)
{
    PKERNEL_INITIALIZATION_BLOCK InitializationBlock;
    PFN_NUMBER ImageSize;
    ULONG PteSize;

    /* Get the kernel initialization block */
    InitializationBlock = KE::BootInformation::GetInitializationBlock();

    /* Get the size of a PTE */
    PteSize = MM::Paging::GetPteSize();

    /* Calculate the size of the boot image */
    ImageSize = InitializationBlock->BootImageSize * MM_PAGE_SIZE;
    ImageSize = (ImageSize + ((MM_PAGE_SIZE / PteSize) * MM_PAGE_SIZE) - 1) &
                ~(((MM_PAGE_SIZE / PteSize) * MM_PAGE_SIZE) - 1);

    /* Return number of pages used by the boot image */
    *BootImageSize = ImageSize / MM_PAGE_SIZE;
}

/**
 * Retrieves the amount of total available memory in the system.
 *
 * @return This routine returns the amount of available memory in the system in megabytes.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
MM::Manager::GetInstalledMemorySize(VOID)
{
    /* Return the amount of installed memory */
    return (MM::Pfn::GetNumberOfPhysicalPages() * MM_PAGE_SIZE) / 1048576;
}

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
 * Retrieves the number of system PTEs.
 *
 * @return This routine returns the number of system PTEs.
 *
 * @since XT 1.0
 */
XTAPI
PFN_NUMBER
MM::Manager::GetNumberOfSystemPtes()
{
    return NumberOfSystemPtes;
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

    /* Compute page colors to reduce CPU cache conflicts */
    MM::Colors::ComputePageColoring();

    /* Compute allocation size for the PFN database */
    MM::Pfn::ComputePfnDatabaseSize();

    /* Initialize memory layout */
    InitializeMemoryLayout();

    /* Initialize PTE template */
    MM::Pte::InitializeSystemPte();

    /* Initialize page table */
    MM::Pte::InitializePageTable();

    /* Initialize system PTE space */
    MM::Pte::InitializeSystemPteSpace();

    /* Initialize PFN database */
    MM::Pfn::InitializePfnDatabase();
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
