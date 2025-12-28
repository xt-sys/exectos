/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/pfn.cc
 * DESCRIPTION:     Physical Frame Number (PFN) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 *                  Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Allocates a block of physical pages for early kernel initialization.
 *
 * @param NumberOfPages
 *        The number of physical pages to allocate.
 *
 * @return This routine returns the base page frame number (PFN) of the allocated block.
 *
 * @since XT 1.0
 */
XTAPI
PFN_NUMBER
MM::Pfn::AllocateBootstrapPages(IN PFN_NUMBER NumberOfPages)
{
    PFN_NUMBER Pfn;

    /* Check if the largest free memory block has enough pages */
    if(NumberOfPages > FreeDescriptor->PageCount)
    {
        /* Not enough physical memory available, kernel panic */
        DebugPrint(L"Insufficient physical pages! Install additional memory\n");
        KE::Crash::Panic(0);
    }

    /* Allocate pages from the beginning of the free descriptor */
    Pfn = FreeDescriptor->BasePage;
    FreeDescriptor->BasePage += NumberOfPages;
    FreeDescriptor->PageCount -= NumberOfPages;

    /* Return the base page frame number of the allocated block */
    return Pfn;
}

/**
 * Calculates the total number of pages required for the PFN database and its associated color tables.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::ComputePfnDatabaseSize(VOID)
{
    /* Calculate the total number of pages required for the PFN database */
    PfnDatabaseSize = (HighestPhysicalPage + 1) * sizeof(MMPFN);
    PfnDatabaseSize = ROUND_UP(PfnDatabaseSize, MM_PAGE_SIZE);
    PfnDatabaseSize >>= MM_PAGE_SHIFT;
}

/**
 * Decrements the global count of available pages.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::DecrementAvailablePages(VOID)
{
    /* Decrement the global count of available pages */
    AvailablePages--;
}

/**
 * Retrieves the total number of physical pages managed by the system.
 *
 * @return Returns the total count of physical memory pages.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
MM::Pfn::GetNumberOfPhysicalPages(VOID)
{
    /* Return the number of physical pages */
    return NumberOfPhysicalPages;
}

/**
 * Gets the size of the PFN database and its associated structures, in pages.
 *
 * @return This routine returns the total number of pages required for the PFN database and its associated structures.
 *
 * @since XT 1.0
 */
XTAPI
PFN_NUMBER
MM::Pfn::GetPfnDatabaseSize(VOID)
{
    /* Return the pre-calculated size of the PFN database in pages */
    return PfnDatabaseSize;
}

/**
 * Retrieves a pointer to the PFN database entry for a given physical page.
 *
 * @param Pfn
 *        The Page Frame Number (PFN) to look up.
 *
 * @return This routine returns a pointer to the MMPFN structure for the given PFN, or NULLPTR if the PFN is invalid.
 *
 * @since XT 1.0
 */
XTAPI
PMMPFN
MM::Pfn::GetPfnEntry(IN PFN_NUMBER Pfn)
{
    PMMMEMORY_LAYOUT MemoryLayout;

    /* Validate that the PFN is within the range of managed physical memory */
    if(Pfn > HighestPhysicalPage)
    {
        /* The requested page number is outside the bounds, return NULLPTR */
        return NULLPTR;
    }

    /* Get the memory layout */
    MemoryLayout = MM::Manager::GetMemoryLayout();

    /* Calculate the address of the PFN entry by indexing into the PFN database array and return it */
    return &((PMMPFN)MemoryLayout->PfnDatabaseAddress)[Pfn];
}

/**
 * Increments the global count of available pages.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::IncrementAvailablePages(VOID)
{
    /* Increment the global count of available pages */
    AvailablePages++;
}

/**
 * Scans memory descriptors provided by the boot loader.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Pfn::ScanMemoryDescriptors(VOID)
{
    PLIST_ENTRY LoaderMemoryDescriptors, MemoryMappings;
    PLOADER_MEMORY_DESCRIPTOR MemoryDescriptor;
    PFN_NUMBER FreePages;

    /* Initially, set number of free pages to 0 */
    FreePages = 0;

    /* Get the list head of memory descriptors */
    LoaderMemoryDescriptors = KE::BootInformation::GetMemoryDescriptors();

    /* Iterate through the memory descriptors */
    MemoryMappings = LoaderMemoryDescriptors->Flink;
    while(MemoryMappings != LoaderMemoryDescriptors)
    {
        /* Get the memory descriptor */
        MemoryDescriptor = CONTAIN_RECORD(MemoryMappings, LOADER_MEMORY_DESCRIPTOR, ListEntry);

        /* Skip invisible or hardware cached memory regions */
        if(MM::Manager::VerifyMemoryTypeInvisible(MemoryDescriptor->MemoryType) ||
            (MemoryDescriptor->MemoryType == LoaderHardwareCachedMemory))
        {
            /* Move to the next descriptor and skip further processing */
            MemoryMappings = MemoryMappings->Flink;
            continue;
        }

        /* Count the number of physical pages, excluding bad memory */
        if(MemoryDescriptor->MemoryType != LoaderBad)
        {
            /* Add the pages from this descriptor to the total count */
            NumberOfPhysicalPages += MemoryDescriptor->PageCount;
        }

        /* Check if this physical page is the lowest one yet */
        if(MemoryDescriptor->BasePage < LowestPhysicalPage)
        {
            /* Update the lowest physical page number found so far */
            LowestPhysicalPage = MemoryDescriptor->BasePage;
        }

        /* Check if this physical page is the highest one yet */
        if((MemoryDescriptor->BasePage + MemoryDescriptor->PageCount) > HighestPhysicalPage)
        {
            /* Update the highest physical page number found so far */
            HighestPhysicalPage = (MemoryDescriptor->BasePage + MemoryDescriptor->PageCount) - 1;
        }

        /* Identify the largest block of free memory */
        if(MM::Manager::VerifyMemoryTypeFree(MemoryDescriptor->MemoryType))
        {
            /* Check if this free memory block is the largest one yet */
            if(MemoryDescriptor->PageCount >= FreePages)
            {
                /* Update the largest free block size and save the descriptor */
                FreePages = MemoryDescriptor->PageCount;
                FreeDescriptor = MemoryDescriptor;
            }
        }

        /* Get next memory descriptor */
        MemoryMappings = MemoryMappings->Flink;
    }

    /* Ensure a free memory descriptor was found */
    if(!FreeDescriptor)
    {
        /* No free memory available to bootstrap the system */
        KE::Crash::Panic(0);
    }

    /* Save a copy of the original free descriptor before it gets modified */
    RTL::Memory::CopyMemory(&OriginalFreeDescriptor, FreeDescriptor, sizeof(LOADER_MEMORY_DESCRIPTOR));
}
