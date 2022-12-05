/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/memory.c
 * DESCRIPTION:     EFI memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>

/**
 * This routine allocates one or more 4KB pages.
 *
 * @param Pages
 *        The number of contiguous 4KB pages to allocate.
 *
 * @param Memory
 *        The pointer to a physical address.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryAllocatePages(IN UINT64 Pages,
                         OUT PEFI_PHYSICAL_ADDRESS Memory)
{
    return EfiSystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, Pages, Memory);
}

/**
 * This routine allocates a pool memory.
 *
 * @param Size
 *        The number of bytes to allocate from the pool.
 *
 * @param Memory
 *        The pointer to a physical address.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryAllocatePool(IN UINT_PTR Size,
                        OUT PVOID *Memory)
{
    /* Allocate pool */
    return EfiSystemTable->BootServices->AllocatePool(EfiLoaderData, Size, Memory);
}

/**
 * This routine frees memory pages.
 *
 * @param Pages
 *        The number of contiguous 4 KB pages to free.
 *
 * @param Memory
 *        The base physical address of the pages to be freed.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryFreePages(IN UINT64 Pages,
                     IN EFI_PHYSICAL_ADDRESS Memory)
{
    return EfiSystemTable->BootServices->FreePages(Memory, Pages);
}

/**
 * Returns pool memory to the system.
 *
 * @param Memory
 *        The pointer to the buffer to free.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlEfiMemoryFreePool(IN PVOID Memory)
{
    /* Free pool */
    return EfiSystemTable->BootServices->FreePool(Memory);
}

/**
 * Returns the memory descriptors which define a memory map of all the physical memory ranges reserved by the UEFI.
 *
 * @param MemoryMap
 *        Supplies a pointer to the buffer where memory map will be written.
 *
 * @param MapKey
 *        Supplies a pointer where the firmware stores the map key.
 *
 * @param DescriptorSize
 *        Supplies a pointer where the size of EFI_MEMORY_DESCRIPTOR will be stored.
 *
 * @param DescriptorCount
 *        Supplies a pointer where number of written descriptors will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlGetMemoryMap(OUT PEFI_MEMORY_DESCRIPTOR *MemoryMap,
               OUT PUINT_PTR MapKey,
               OUT PUINT_PTR DescriptorSize,
               OUT PUINT_PTR DescriptorCount)
{
    PEFI_MEMORY_DESCRIPTOR LocalMemoryMap = NULL;
    UINT_PTR MemoryMapSize = 0, LocalMapKey, LocalDescriptorSize;
    UINT32 DescriptorVersion;
    EFI_STATUS Status;

    /* Get memory map */
    do
    {
        /* Attempt do get EFI memory map */
        Status = EfiSystemTable->BootServices->GetMemoryMap(&MemoryMapSize, LocalMemoryMap, &LocalMapKey,
                                                            &LocalDescriptorSize, &DescriptorVersion);
        if(Status == STATUS_EFI_SUCCESS)
        {
            /* Go further if succeeded */
            break;
        }
        else if(Status != STATUS_EFI_BUFFER_TOO_SMALL)
        {
            /* Some error occurred */
            if(MemoryMap)
            {
                /* Free allocated memory */
                BlEfiMemoryFreePool(MemoryMap);
            }
            return Status;
        }

        /* Allocate the desired amount of memory */
        MemoryMapSize += 2 * LocalDescriptorSize;
        BlEfiMemoryAllocatePool(MemoryMapSize, (PVOID *)&LocalMemoryMap);
    }
    while(Status == STATUS_EFI_BUFFER_TOO_SMALL);

    /* Make sure memory map is set */
    if(LocalMemoryMap == NULL)
    {
        /* Something went wrong */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Store memory map details */
    *MemoryMap = LocalMemoryMap;
    *MapKey = LocalMapKey;
    *DescriptorSize = LocalDescriptorSize;
    *DescriptorCount = MemoryMapSize / LocalDescriptorSize;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
