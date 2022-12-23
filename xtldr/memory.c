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
                          IN LOADER_MEMORY_TYPE MemoryType)
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
            /* Make sure it's memory type is LoaderFree */
            if(Mapping2->MemoryType != LoaderFree)
            {
                /* LoaderFree memory type is strictly expected */
                return STATUS_EFI_INVALID_PARAMETER;
            }

            /* Store address of the next mapping */
            MappingListEntry = ListEntry->Flink;

            /* Remove mapping from the list and free up it's memory */
            RtlRemoveEntryList(&Mapping2->ListEntry);
            BlEfiMemoryFreePool(Mapping2);
            ListEntry = MappingListEntry;

            /* Go to the next mapping */
            continue;
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
 * Converts an EFI memory type into an XTOS memory type.
 *
 * @param EfiMemoryType
 *        Supplies the EFI memory type.
 *
 * @return Returns a conversion of the memory type.
 *
 * @since XT 1.0
 */
LOADER_MEMORY_TYPE
BlConvertEfiMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType)
{
    LOADER_MEMORY_TYPE MemoryType;

    /* Check EFI memory type and convert to XTOS memory type */
    switch(EfiMemoryType)
    {
        case EfiACPIMemoryNVS:
        case EfiACPIReclaimMemory:
        case EfiPalCode:
            MemoryType = LoaderSpecialMemory;
            break;
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
            MemoryType = LoaderFirmwarePermanent;
            break;
        case EfiBootServicesData:
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

    /* Return XTOS memory type */
    return MemoryType;
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
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlGetMemoryMap(OUT PEFI_MEMORY_MAP MemoryMap)
{
    EFI_STATUS Status;

    if(MemoryMap == NULL)
    {
        return STATUS_EFI_INVALID_PARAMETER;
    }

    MemoryMap->Map = NULL;
    MemoryMap->MapSize = 0;

    /* Get memory map */
    do
    {
        /* Attempt do get EFI memory map */
        Status = EfiSystemTable->BootServices->GetMemoryMap(&MemoryMap->MapSize, MemoryMap->Map, &MemoryMap->MapKey,
                                                            &MemoryMap->DescriptorSize, &MemoryMap->DescriptorVersion);
        if(Status == STATUS_EFI_SUCCESS)
        {
            /* Go further if succeeded */
            break;
        }
        else if(Status != STATUS_EFI_BUFFER_TOO_SMALL)
        {
            /* Some error occurred */
            if(MemoryMap->Map)
            {
                /* Free allocated memory */
                BlEfiMemoryFreePool(MemoryMap->Map);
            }
            return Status;
        }

        /* Allocate the desired amount of memory */
        MemoryMap->MapSize += 2 * MemoryMap->DescriptorSize;
        BlEfiMemoryAllocatePool(MemoryMap->MapSize, (PVOID *)&MemoryMap->Map);
    }
    while(Status == STATUS_EFI_BUFFER_TOO_SMALL);

    /* Make sure memory map is set */
    if(MemoryMap->Map == NULL)
    {
        /* Something went wrong */
        return STATUS_EFI_NO_MAPPING;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Attempts to find a virtual address of the specified physical address in memory mappings.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param PhysicalAddress
 *        Supplies a physical address to search for in the mappings.
 *
 * @param VirtualAddress
 *        Supplies a buffer, where mapped virtual address of the found mapping will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlGetVirtualAddress(IN PLIST_ENTRY MemoryMappings,
                    IN PVOID PhysicalAddress,
                    OUT PVOID *VirtualAddress)
{
    PLOADER_MEMORY_MAPPING Mapping;
    PLIST_ENTRY ListEntry;

    /* NULLify virtual address */
    *VirtualAddress = NULL;

    /* Iterate over memory mappings in order to find descriptor containing a physical address */
    ListEntry = MemoryMappings->Flink;
    while(ListEntry != MemoryMappings)
    {
        /* Get mapping from linked list */
        Mapping = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_MAPPING, ListEntry);

        /* Make sure any virtual address is set */
        if(Mapping->VirtualAddress)
        {
            /* Check if provided physical address is in range of this mapping */
            if((PhysicalAddress >= Mapping->PhysicalAddress) &&
               (PhysicalAddress < Mapping->PhysicalAddress + (Mapping->NumberOfPages * EFI_PAGE_SIZE)))
            {
                /* Calculate virtual address based on the mapping */
                *VirtualAddress = PhysicalAddress - Mapping->PhysicalAddress + Mapping->VirtualAddress;
            }
        }

        /* Get next element from the list */
        ListEntry = ListEntry->Flink;
    }

    /* If virtual address is still NULL, then mapping was not found */
    if(*VirtualAddress == NULL)
    {
        /* Mapping not found */
        return STATUS_EFI_NOT_FOUND;
    }

    /* Mapping found, return success */
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
    PEFI_MEMORY_DESCRIPTOR Descriptor;
    LOADER_MEMORY_TYPE MemoryType;
    PEFI_MEMORY_MAP MemoryMap;
    SIZE_T DescriptorCount;
    PUCHAR VirtualAddress;
    EFI_STATUS Status;
    SIZE_T Index;

    /* Set initial virtual address */
    VirtualAddress = *MemoryMapAddress;

    /* Allocate and zero-fill buffer for EFI memory map */
    BlEfiMemoryAllocatePool(sizeof(EFI_MEMORY_MAP), (PVOID*)&MemoryMap);
    RtlZeroMemory(MemoryMap, sizeof(EFI_MEMORY_MAP));

    /* Get EFI memory map */
    Status = BlGetMemoryMap(MemoryMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        return Status;
    }

    /* Calculate descriptors count and get first one */
    Descriptor = MemoryMap->Map;
    DescriptorCount = MemoryMap->MapSize / MemoryMap->DescriptorSize;

    /* Iterate through all descriptors from the memory map */
    for(Index = 0; Index < DescriptorCount; Index++)
    {
        /* Make sure descriptor does not go beyond lowest physical page */
        if((Descriptor->PhysicalStart + (Descriptor->NumberOfPages * EFI_PAGE_SIZE)) <= (UINT_PTR)-1)
        {
            /* Convert EFI memory type into XTOS memory type */
            MemoryType = BlConvertEfiMemoryType(Descriptor->Type);

            /* Do memory mappings depending on memory type */
            if(MemoryType == LoaderFirmwareTemporary)
            {
                /* Map EFI firmware code */
                Status = BlAddVirtualMemoryMapping(MemoryMappings, (PVOID)Descriptor->PhysicalStart,
                                                   (PVOID)Descriptor->PhysicalStart, Descriptor->NumberOfPages, MemoryType);
            }
            else if(MemoryType != LoaderFree)
            {
                /* Add any non-free memory mapping */
                Status = BlAddVirtualMemoryMapping(MemoryMappings, VirtualAddress, (PVOID)Descriptor->PhysicalStart,
                                                   Descriptor->NumberOfPages, MemoryType);

                /* Calculate next valid virtual address */
                VirtualAddress += Descriptor->NumberOfPages * EFI_PAGE_SIZE;
            }
            else
            {
                /* Map all other memory as loader free */
                Status = BlAddVirtualMemoryMapping(MemoryMappings, NULL, (PVOID)Descriptor->PhysicalStart,
                                                   Descriptor->NumberOfPages, LoaderFree);
            }

            /* Make sure memory mapping succeeded */
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Mapping failed */
                return Status;
            }

            /* Grab next descriptor */
            Descriptor = (PEFI_MEMORY_DESCRIPTOR)((PUCHAR)Descriptor + MemoryMap->DescriptorSize);
        }
    }

    /* Store next valid virtual address and return success */
    *MemoryMapAddress = VirtualAddress;
    return STATUS_EFI_SUCCESS;
}
