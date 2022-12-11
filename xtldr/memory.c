/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/memory.c
 * DESCRIPTION:     EFI memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * Adds a physical to virtual address mapping to the linked list for future processing.
 *
 * @param MemoryMapping
 *        Supplies the head of the memory mapping list.
 *
 * @param VirtualAddress
 *        Supplies a virtual address where the physical address should be mapped.
 *
 * @param PhysicalAddress
 *        Supplies a physical address which will be mapped.
 *
 * @param NumberOfPages
 *        Supplies a number of pages which will be mapped.
 *
 * @param MemoryType
 *        Supplies the type of memory that will be assigned to the memory descriptor.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlAddVirtualMemoryMapping(IN PLIST_ENTRY MemoryMappings,
                          IN PVOID VirtualAddress,
                          IN PVOID PhysicalAddress,
                          IN UINT NumberOfPages,
                          LOADER_MEMORY_TYPE MemoryType)
{
    PLOADER_MEMORY_MAPPING Mapping1, Mapping2, Mapping3;
    PVOID PhysicalAddressEnd, PhysicalAddress2End;
    PLIST_ENTRY ListEntry, MappingListEntry;
    SIZE_T NumberOfMappedPages;
    EFI_STATUS Status;

    /* Allocate memory for new mapping */
    Status = BlEfiMemoryAllocatePool(sizeof(LOADER_MEMORY_MAPPING), (PVOID *)&Mapping1);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Set mapping fields */
    Mapping1->PhysicalAddress = PhysicalAddress;
    Mapping1->VirtualAddress = VirtualAddress;
    Mapping1->NumberOfPages = NumberOfPages;
    Mapping1->MemoryType = MemoryType;

    /* Calculate the end of the physical address */
    PhysicalAddressEnd = (PUINT8)PhysicalAddress + (NumberOfPages * EFI_PAGE_SIZE) - 1;

    /* Iterate through all the mappings already set to insert new mapping at the correct place */
    ListEntry = MemoryMappings->Flink;
    while(ListEntry != MemoryMappings)
    {
        /* Take a mapping from the list and calculate its end of physical address */
        Mapping2 = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_MAPPING, ListEntry);
        PhysicalAddress2End = (PUINT8)Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1 ;

        /* Check if they overlap */
        if(PhysicalAddressEnd > Mapping2->PhysicalAddress && PhysicalAddressEnd <= PhysicalAddress2End)
        {
            /* Make sure it's memory type is LoaderFree */
            if(Mapping2->MemoryType != LoaderFree)
            {
                /* LoaderFree memory type is strictly expected */
                return STATUS_EFI_INVALID_PARAMETER;
            }

            /* Calculate number of pages for this mapping */
            NumberOfMappedPages = ((PUINT8)PhysicalAddress2End - (PUINT8)PhysicalAddressEnd) / EFI_PAGE_SIZE;
            if(NumberOfMappedPages > 0)
            {
                /* Pages associated to the mapping, allocate memory for it */
                Status = BlEfiMemoryAllocatePool(sizeof(LOADER_MEMORY_MAPPING), (PVOID*)&Mapping3);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Memory allocation failure */
                    return Status;
                }

                /* Set mapping fields and insert it on the top */
                Mapping3->PhysicalAddress = (PUINT8)PhysicalAddressEnd + 1;
                Mapping3->VirtualAddress = NULL;
                Mapping3->NumberOfPages = NumberOfMappedPages;
                Mapping3->MemoryType = Mapping2->MemoryType;
                RtlInsertHeadList(&Mapping2->ListEntry, &Mapping3->ListEntry);
            }

            /* Calculate number of pages and the end of the physical address */
            Mapping2->NumberOfPages = ((PUINT8)PhysicalAddressEnd + 1 -
                                       (PUINT8)Mapping2->PhysicalAddress) / EFI_PAGE_SIZE;
            PhysicalAddress2End = (PUINT8)Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1;
        }

        /* Check if they overlap */
        if(Mapping1->PhysicalAddress > Mapping2->PhysicalAddress && Mapping1->PhysicalAddress < PhysicalAddress2End)
        {
            /* Make sure it's memory type is LoaderFree */
            if(Mapping2->MemoryType != LoaderFree)
            {
                /* LoaderFree memory type is strictly expected */
                return STATUS_EFI_INVALID_PARAMETER;
            }

            /* Calculate number of pages for this mapping */
            NumberOfMappedPages = ((PUINT8)PhysicalAddress2End + 1 - (PUINT8)Mapping1->PhysicalAddress) / EFI_PAGE_SIZE;
            if(NumberOfMappedPages > 0)
            {
                /* Pages associated to the mapping, allocate memory for it */
                Status = BlEfiMemoryAllocatePool(sizeof(LOADER_MEMORY_MAPPING), (PVOID*)&Mapping3);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Memory allocation failure */
                    return Status;
                }

                /* Set mapping fields and insert it on the top */
                Mapping3->PhysicalAddress = Mapping1->PhysicalAddress;
                Mapping3->VirtualAddress = NULL;
                Mapping3->NumberOfPages = NumberOfMappedPages;
                Mapping3->MemoryType = Mapping2->MemoryType;
                RtlInsertHeadList(&Mapping2->ListEntry, &Mapping3->ListEntry);
            }

            /* Calculate number of pages and the end of the physical address */
            Mapping2->NumberOfPages = ((PUINT8)Mapping1->PhysicalAddress -
                                       (PUINT8)Mapping2->PhysicalAddress) / EFI_PAGE_SIZE;
            PhysicalAddress2End = (PUINT8)Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1;
        }

        /* Check if mapping is really needed */
        if((Mapping2->PhysicalAddress >= Mapping1->PhysicalAddress && PhysicalAddress2End <= PhysicalAddressEnd) ||
           (Mapping2->NumberOfPages == 0))
        {
            /* If it is of LoaderFree type, then we can skip it */
            if(Mapping2->MemoryType == LoaderFree)
            {
                /* Store address of the next mapping */
                MappingListEntry = ListEntry->Flink;

                /* Remove mapping from the list and free up it's memory */
                RtlRemoveEntryList(&Mapping2->ListEntry);
                BlEfiMemoryFreePool(Mapping2);
                ListEntry = MappingListEntry;

                /* Go to the next mapping */
                continue;
            }
        }

        /* Determine phsical address order */
        if(Mapping2->PhysicalAddress > Mapping1->PhysicalAddress)
        {
            /* Insert new mapping in front */
            RtlInsertHeadList(Mapping2->ListEntry.Blink, &Mapping1->ListEntry);
            return STATUS_EFI_SUCCESS;
        }

        /* Get next mapping from the list */
        ListEntry = ListEntry->Flink;
    }

    /* Insert new mapping to the end of the list and return success */
    RtlInsertTailList(MemoryMappings, &Mapping1->ListEntry);
    return STATUS_EFI_SUCCESS;
}

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

/**
 * Initializes virtual memory by adding known and general mappings.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param MemoryMapAddress
 *        Supplies an address of the mapped virtual memory area.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlInitializeVirtualMemory(IN OUT PLIST_ENTRY MemoryMappings,
                          IN OUT PVOID *MemoryMapAddress)
{
    UINT_PTR MapKey, DescriptorSize, DescriptorCount;
    PEFI_MEMORY_DESCRIPTOR MemoryMap = NULL;
    LOADER_MEMORY_TYPE MemoryType;
    PUCHAR VirtualAddress;
    EFI_STATUS Status;
    BOOLEAN MapVRam;
    SIZE_T Index;

    /* Set initial VA and assume VRAM will be mapped */
    VirtualAddress = *MemoryMapAddress;
    MapVRam = TRUE;

    /* Get memory map */
    Status = BlGetMemoryMap(&MemoryMap, &MapKey, &DescriptorSize, &DescriptorCount);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    /* Iterate through all descriptors from memory map */
    for(Index = 0; Index < DescriptorCount; Index++)
    {
        /* Check memory type */
        switch(MemoryMap->Type)
        {
            case EfiACPIMemoryNVS:
            case EfiACPIReclaimMemory:
            case EfiPalCode:
            case EfiReservedMemoryType:
                MemoryType = LoaderSpecialMemory;
                break;
            case EfiLoaderCode:
                MemoryType = LoaderFirmwareTemporary;
                break;
            case EfiUnusableMemory:
                MemoryType = LoaderBad;
                break;
            default:
                MemoryType = LoaderFree;
                break;
        }

        /* Do initial memory mappings */
        if(MemoryType == LoaderFirmwareTemporary)
        {
            /* Map EFI firmware code */
            Status = BlAddVirtualMemoryMapping(MemoryMappings, (PVOID)MemoryMap->PhysicalStart,
                                               (PVOID)MemoryMap->PhysicalStart, MemoryMap->NumberOfPages, MemoryType);
        }
        else if(MemoryType != LoaderFree)
        {
            /* Add non-free memory mapping */
            Status = BlAddVirtualMemoryMapping(MemoryMappings, VirtualAddress,
                                               (PVOID)MemoryMap->PhysicalStart, MemoryMap->NumberOfPages, MemoryType);

            /* Calculate next valid virtual address */
            VirtualAddress += MemoryMap->NumberOfPages * EFI_PAGE_SIZE;

            /* Check if VRAM should be as well mapped */
            if((MemoryMap->PhysicalStart + (MemoryMap->NumberOfPages << EFI_PAGE_SHIFT) > 0xA0000) &&
               (MemoryMap->PhysicalStart <= 0xA0000))
            {
                /* No need to map VRAM */
                MapVRam = FALSE;
            }
        }
        else
        {
            /* Map all other memory as loader free */
            Status = BlAddVirtualMemoryMapping(MemoryMappings, NULL, (PVOID)MemoryMap->PhysicalStart,
                                               MemoryMap->NumberOfPages, LoaderFree);
        }

        /* Make sure memory mapping succeeded */
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Mapping failed */
            return Status;
        }

        /* Grab next descriptor */
        MemoryMap = (PEFI_MEMORY_DESCRIPTOR)((PUCHAR)MemoryMap + DescriptorSize);
    }

    /* Check if VRAM should mapped */
    if(MapVRam)
    {
        /* Add VRAM mapping */
        Status = BlAddVirtualMemoryMapping(MemoryMappings, NULL, (PVOID)0xA0000, 0x60, LoaderFirmwarePermanent);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* VRAM mapping failed */
            return Status;
        }
    }

    /* Store next valid virtual address and return success */
    *MemoryMapAddress = VirtualAddress;
    return STATUS_EFI_SUCCESS;
}
