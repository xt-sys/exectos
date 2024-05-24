/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/init.c
 * DESCRIPTION:     Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Performs an early initialization of the XTOS Memory Manager.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmInitializeMemoryManager(VOID)
{
    /* Scan memory descriptors provided by the boot loader */
    MmpScanMemoryDescriptors();

    /* Check if there are enough physical pages */
    if(MmNumberOfPhysicalPages < MM_MINIMUM_PHYSICAL_PAGES)
    {
        /* Insufficient physical pages, kernel panic */
        DebugPrint(L"Insufficient physical pages! Install additional memory\n");
        KePanic(0);
    }

    /* Store Page Map Level */
    MmPageMapLevel = KeInitializationBlock->LoaderInformation.PageMapLevel;

    /* Proceed with architecture specific initialization */
    MmpInitializeArchitecture();
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
MmpScanMemoryDescriptors(VOID)
{
    PLOADER_MEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY MemoryMappings;
    PFN_NUMBER FreePages;

    /* Initially, set number of free pages to 0 */
    FreePages = 0;

    /* Iterate through memory mappings provided by the boot loader */
    MemoryMappings = KeInitializationBlock->MemoryDescriptorListHead.Flink;
    while(MemoryMappings != &KeInitializationBlock->MemoryDescriptorListHead)
    {
        /* Get memory descriptor */
        MemoryDescriptor = CONTAIN_RECORD(MemoryMappings, LOADER_MEMORY_DESCRIPTOR, ListEntry);

        /* Check if memory type is invisible or cached */
        if(MmpVerifyMemoryTypeInvisible(MemoryDescriptor->MemoryType) ||
           (MemoryDescriptor->MemoryType == LoaderHALCachedMemory))
        {
            /* Skip this mapping */
            MemoryMappings = MemoryMappings->Flink;
            continue;
        }

        /* Make sure that memory type is not bad */
        if(MemoryDescriptor->MemoryType != LoaderBad)
        {
            /* Increment number of physical pages */
            MmNumberOfPhysicalPages += MemoryDescriptor->PageCount;
        }

        /* Find lowest physical page */
        if(MemoryDescriptor->BasePage < MmLowestPhysicalPage)
        {
            /* Update lowest physical page */
            MmLowestPhysicalPage = MemoryDescriptor->BasePage;
        }

        /* Find highest physical page */
        if(MemoryDescriptor->BasePage + MemoryDescriptor->PageCount > MmHighestPhysicalPage)
        {
            /* Update highest physical page */
            MmHighestPhysicalPage = (MemoryDescriptor->BasePage + MemoryDescriptor->PageCount) - 1;
        }

        /* Check if memory type should be considered as free */
        if(MmpVerifyMemoryTypeFree(MemoryDescriptor->MemoryType))
        {
            /* Check if this descriptor contains more free pages */
            if(MemoryDescriptor->PageCount >= FreePages)
            {
                /* Update free descriptor */
                FreePages = MemoryDescriptor->PageCount;
                MmFreeDescriptor = MemoryDescriptor;
            }
        }

        /* Get next memory descriptor */
        MemoryMappings = MemoryMappings->Flink;
    }

    /* Store original free descriptor */
    RtlCopyMemory(&MmOldFreeDescriptor, MmFreeDescriptor, sizeof(LOADER_MEMORY_DESCRIPTOR));
}

/** Checks whether the specified memory type should be considered as free.
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
MmpVerifyMemoryTypeFree(LOADER_MEMORY_TYPE MemoryType)
{
    return (MemoryType == LoaderFree) || (MemoryType == LoaderFirmwareTemporary) ||
           (MemoryType == LoaderLoadedProgram) || (MemoryType == LoaderOsloaderStack);
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
MmpVerifyMemoryTypeInvisible(LOADER_MEMORY_TYPE MemoryType)
{
    return (MemoryType == LoaderFirmwarePermanent) ||
           (MemoryType == LoaderSpecialMemory) ||
           (MemoryType == LoaderBBTMemory);
}
