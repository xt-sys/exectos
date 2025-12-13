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
 * Retrieves the total number of physical pages managed by the system.
 *
 * @return Returns the total count of physical memory pages.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::Pfn::GetNumberOfPhysicalPages(VOID)
{
    /* Return the number of physical pages */
    return NumberOfPhysicalPages;
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
