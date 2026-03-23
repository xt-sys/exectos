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
    ULONG_PTR Alignment;

    /* Get the kernel initialization block */
    InitializationBlock = KE::BootInformation::GetInitializationBlock();

    /* Calculate the alignment based on the PTE size */
    Alignment = ((MM_PAGE_SIZE / MM::Paging::GetPteSize()) * MM_PAGE_SIZE);

    /* Calculate the size of the boot image */
    ImageSize = InitializationBlock->BootImageSize * MM_PAGE_SIZE;
    ImageSize = (ImageSize + Alignment - 1) & ~(Alignment - 1);

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
MM::Manager::GetNumberOfSystemPtes(VOID)
{
    return NumberOfSystemPtes;
}

/**
 * Initializes and returns the system physical memory descriptor block.
 *
 * @return This routine returns a pointer to the structure representing the system usable physical memory block.
 *
 * @since XT 1.0
 */
XTAPI
PPHYSICAL_MEMORY_DESCRIPTOR
MM::Manager::GetPhysicalMemoryBlock(VOID)
{
    PPHYSICAL_MEMORY_DESCRIPTOR PrimaryBuffer, SecondaryBuffer;
    PKERNEL_INITIALIZATION_BLOCK InitializationBlock;
    PLOADER_MEMORY_DESCRIPTOR MemoryDescriptor;
    PFN_NUMBER PageFrameNumer, NumberOfPages;
    ULONG DescriptorCount, RunCount;
    PLIST_ENTRY ListEntry;
    XTSTATUS Status;

    /* Check if the physical memory block has already been initialized */
    if(!PhysicalMemoryBlock)
    {
        /* Reset local tracking variables */
        DescriptorCount = 0;
        NumberOfPages = 0;
        PageFrameNumer = -1;
        RunCount = 0;

        /* Retrieve the kernel initialization block */
        InitializationBlock = KE::BootInformation::GetInitializationBlock();

        /* Iterate through the loader memory descriptor list to determine its size */
        ListEntry = InitializationBlock->MemoryDescriptorListHead.Flink;
        while(ListEntry != &InitializationBlock->MemoryDescriptorListHead)
        {
            /* Count this descriptor */
            DescriptorCount++;

            /* Go to the next descriptor */
            ListEntry = ListEntry->Flink;
        }

        /* Ensure the memory descriptor list is not empty */
        if(DescriptorCount == 0)
        {
            /* Fail gracefully if no memory descriptors were found, by returning NULLPTR */
            return NULLPTR;
        }

        /* Allocate a primary buffer sized for the maximum possible number of runs */
        Status = MM::Allocator::AllocatePool(NonPagedPool,
                                             sizeof(PHYSICAL_MEMORY_DESCRIPTOR) +
                                             sizeof(PHYSICAL_MEMORY_RUN) *
                                             (DescriptorCount - 1),
                                             (PVOID*)&PrimaryBuffer,
                                             SIGNATURE32('M', 'M', 'g', 'r'));
        if(Status != STATUS_SUCCESS || !PrimaryBuffer)
        {
            /* Primary pool allocation failed, return NULLPTR */
            return NULLPTR;
        }

        /* Traverse the memory descriptor list a second time to build the map */
        ListEntry = InitializationBlock->MemoryDescriptorListHead.Flink;
        while(ListEntry != &InitializationBlock->MemoryDescriptorListHead)
        {
            /* Resolve the memory descriptor record from the current list entry */
            MemoryDescriptor = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_DESCRIPTOR, ListEntry);

            /* Filter out bad, reserved, or invisible memory types */
            if((MemoryDescriptor->MemoryType < LoaderMaximum) &&
               (MemoryDescriptor->MemoryType != LoaderBad) &&
               !VerifyMemoryTypeInvisible(MemoryDescriptor->MemoryType))
            {
                /* Accumulate the total number of usable physical pages */
                NumberOfPages += MemoryDescriptor->PageCount;

                /* Check if the current descriptor is contiguous with the previous run */
                if(RunCount > 0 && MemoryDescriptor->BasePage == PageFrameNumer)
                {
                    /* Coalesce the contiguous descriptor into the existing physical run */
                    PrimaryBuffer->Run[RunCount - 1].PageCount += MemoryDescriptor->PageCount;
                    PageFrameNumer += MemoryDescriptor->PageCount;
                }
                else
                {
                    /* Start a new physical run with the new descriptor's boundaries */
                    PrimaryBuffer->Run[RunCount].BasePage = MemoryDescriptor->BasePage;
                    PrimaryBuffer->Run[RunCount].PageCount = MemoryDescriptor->PageCount;

                    /* Update the expected next page frame number for future contiguity checks */
                    PageFrameNumer = PrimaryBuffer->Run[RunCount].BasePage + PrimaryBuffer->Run[RunCount].PageCount;

                    /* Increment the total number of distinct physical memory runs */
                    RunCount++;
                }
            }

            /* Go to the next descriptor */
            ListEntry = ListEntry->Flink;
        }

        /* Check if the buffer can be shrunk due to coalesced memory runs */
        if(DescriptorCount > RunCount)
        {
            /* Allocate a secondary, more tightly sized buffer to reduce memory footprint */
            Status = MM::Allocator::AllocatePool(NonPagedPool,
                                                 sizeof(PHYSICAL_MEMORY_DESCRIPTOR) +
                                                 sizeof(PHYSICAL_MEMORY_RUN) *
                                                 (RunCount - 1),
                                                 (PVOID*)&SecondaryBuffer,
                                                 SIGNATURE32('M', 'M', 'g', 'r'));
            if(Status == STATUS_SUCCESS && SecondaryBuffer)
            {
                /* Copy the coalesced runs from the oversized primary buffer */
                RtlCopyMemory(SecondaryBuffer->Run, PrimaryBuffer->Run, sizeof(PHYSICAL_MEMORY_RUN) * RunCount);

                /* Free the primary buffer */
                MM::Allocator::FreePool(PrimaryBuffer, SIGNATURE32('M', 'M', 'g', 'r'));

                /* Update the primary buffer pointer */
                PrimaryBuffer = SecondaryBuffer;
            }
        }

        /* Populate the final metadata and save the physical memory block globally */
        PrimaryBuffer->NumberOfRuns = RunCount;
        PrimaryBuffer->NumberOfPages = NumberOfPages;
        PhysicalMemoryBlock = PrimaryBuffer;
    }

    /* Return a pointer to the physical memory block */
    return PhysicalMemoryBlock;
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
        KE::Crash::Panic(0x7D, MM::Pfn::GetNumberOfPhysicalPages(), MM_MINIMUM_PHYSICAL_PAGES, 0x0, 0x2);
    }

    /* Compute page colors to reduce CPU cache conflicts */
    MM::Colors::ComputePageColoring();

    /* Initialize and dump memory layout */
    InitializeMemoryLayout();
    DumpMemoryLayout();

    /* Initialize PTE template */
    MM::Pte::InitializeSystemPte();

    /* Initialize page table */
    MM::Pte::InitializePageTable();

    /* Initialize system PTE space */
    MM::Pte::InitializeSystemPteSpace();

    /* Initialize memory pool security */
    MM::Pool::InitializePoolSecurity();

    /* Initialize non-paged pool */
    MM::Pool::InitializeNonPagedPool();

    /* Initialize PFN database */
    MM::Pfn::InitializePfnDatabase();

    /* Initialize allocations tracking tables */
    MM::Allocator::InitializeAllocationsTracking();
    MM::Allocator::InitializeBigAllocationsTracking();

    /* Initialize PFN bitmap */
    MM::Pfn::InitializePfnBitmap();

    /* Initialize paged pool */
    MM::Pool::InitializePagedPool();

    /* Flush TLB */
    AR::CpuFunc::FlushTlb();
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
