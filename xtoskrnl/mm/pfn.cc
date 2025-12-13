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

    /* Get a list of memory descriptors provided by the boot loader */
    LoaderMemoryDescriptors = KE::BootInformation::GetMemoryDescriptors();

    /* Iterate through memory mappings provided by the boot loader */
    MemoryMappings = LoaderMemoryDescriptors->Flink;
    while(MemoryMappings != LoaderMemoryDescriptors)
    {
        /* Get memory descriptor */
        MemoryDescriptor = CONTAIN_RECORD(MemoryMappings, LOADER_MEMORY_DESCRIPTOR, ListEntry);

        /* Check if memory type is invisible or cached */
        if(VerifyMemoryTypeInvisible(MemoryDescriptor->MemoryType) ||
           (MemoryDescriptor->MemoryType == LoaderHardwareCachedMemory))
        {
            /* Skip this mapping */
            MemoryMappings = MemoryMappings->Flink;
            continue;
        }

        /* Make sure that memory type is not bad */
        if(MemoryDescriptor->MemoryType != LoaderBad)
        {
            /* Increment number of physical pages */
            NumberOfPhysicalPages += MemoryDescriptor->PageCount;
        }

        /* Find lowest physical page */
        if(MemoryDescriptor->BasePage < LowestPhysicalPage)
        {
            /* Update lowest physical page */
            LowestPhysicalPage = MemoryDescriptor->BasePage;
        }

        /* Find highest physical page */
        if(MemoryDescriptor->BasePage + MemoryDescriptor->PageCount > HighestPhysicalPage)
        {
            /* Update highest physical page */
            HighestPhysicalPage = (MemoryDescriptor->BasePage + MemoryDescriptor->PageCount) - 1;
        }

        /* Check if memory type should be considered as free */
        if(VerifyMemoryTypeFree(MemoryDescriptor->MemoryType))
        {
            /* Check if this descriptor contains more free pages */
            if(MemoryDescriptor->PageCount >= FreePages)
            {
                /* Update free descriptor */
                FreePages = MemoryDescriptor->PageCount;
                FreeDescriptor = MemoryDescriptor;
            }
        }

        /* Get next memory descriptor */
        MemoryMappings = MemoryMappings->Flink;
    }

    /* Store original free descriptor */
    RTL::Memory::CopyMemory(&OldFreeDescriptor, FreeDescriptor, sizeof(LOADER_MEMORY_DESCRIPTOR));
}
