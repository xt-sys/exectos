/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/memory.c
 * DESCRIPTION:     XT Boot Loader memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


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
XTCDECL
EFI_STATUS
BlAllocateMemoryPages(IN ULONGLONG Pages,
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
XTCDECL
EFI_STATUS
BlAllocateMemoryPool(IN UINT_PTR Size,
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
XTCDECL
EFI_STATUS
BlFreeMemoryPages(IN ULONGLONG Pages,
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
XTCDECL
EFI_STATUS
BlFreeMemoryPool(IN PVOID Memory)
{
    /* Free pool */
    return EfiSystemTable->BootServices->FreePool(Memory);
}

/**
 * Returns the number of mappings in the page mapping structure.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param NumberOfMappings
 *        Supplies a pointer to memory area where the number of mappings is returned.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlGetMappingsCount(IN PXTBL_PAGE_MAPPING PageMap,
                   OUT PULONG NumberOfMappings)
{
    /* Return number of mappings */
    *NumberOfMappings = PageMap->MapSize;
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
XTCDECL
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
                BlFreeMemoryPool(MemoryMap->Map);
            }
            return Status;
        }

        /* Allocate the desired amount of memory */
        MemoryMap->MapSize += 2 * MemoryMap->DescriptorSize;
        BlAllocateMemoryPool(MemoryMap->MapSize, (PVOID *)&MemoryMap->Map);
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
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param PhysicalAddress
 *        Supplies a physical address to search for in the mappings.
 *
 * @return This routine returns a corresponding virtual address found in the mappings.
 *
 * @since XT 1.0
 */
XTCDECL
PVOID
BlGetVirtualAddress(IN PXTBL_PAGE_MAPPING PageMap,
                    IN PVOID PhysicalAddress)
{
    PXTBL_MEMORY_MAPPING Mapping;
    PLIST_ENTRY ListEntry;

    /* Iterate over memory mappings in order to find descriptor containing a physical address */
    ListEntry = PageMap->MemoryMap.Flink;
    while(ListEntry != &PageMap->MemoryMap)
    {
        /* Get mapping from linked list */
        Mapping = CONTAIN_RECORD(ListEntry, XTBL_MEMORY_MAPPING, ListEntry);

        /* Make sure any virtual address is set */
        if(Mapping->VirtualAddress)
        {
            /* Check if provided physical address is in range of this mapping */
            if((PhysicalAddress >= Mapping->PhysicalAddress) &&
               (PhysicalAddress < Mapping->PhysicalAddress + (Mapping->NumberOfPages * EFI_PAGE_SIZE)))
            {
                /* Calculate virtual address based on the mapping and return it */
                return PhysicalAddress - Mapping->PhysicalAddress + Mapping->VirtualAddress;
            }
        }

        /* Get next element from the list */
        ListEntry = ListEntry->Flink;
    }

    /* Mapping not found, return 0 */
    return 0;
}

/**
 * Initializes the page mapping structures.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param PageMapLevel
 *        Specifies a number of of paging structures levels.
 *
 * @param PageSize
 *        Specifies a page size (currently it has no effect).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlInitializePageMap(OUT PXTBL_PAGE_MAPPING PageMap,
                    IN SHORT PageMapLevel,
                    IN PAGE_SIZE PageSize)
{
    /* Initialize memory mappings */
    RtlInitializeListHead(&PageMap->MemoryMap);
    PageMap->MapSize = 0;

    /* Set page map size/level and memory map address */
    PageMap->PageMapLevel = PageMapLevel;
    PageMap->PageSize = PageSize;
}

/**
 * Adds EFI memory mapping to the page mapping structure.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param MemoryMapAddress
 *        Supplies a virtual address, where EFI memory will be mapped.
 *
 * @param GetMemoryTypeRoutine
 *        Supplies a pointer to the routine which will be used to match EFI memory type to the OS memory type.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlMapEfiMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
               IN OUT PVOID *MemoryMapAddress,
               IN PBL_GET_MEMTYPE_ROUTINE GetMemoryTypeRoutine)
{
    PEFI_MEMORY_DESCRIPTOR Descriptor;
    LOADER_MEMORY_TYPE MemoryType;
    PEFI_MEMORY_MAP MemoryMap;
    SIZE_T DescriptorCount;
    PUCHAR VirtualAddress;
    EFI_STATUS Status;
    SIZE_T Index;

    /* Set virtual address as specified in argument */
    VirtualAddress = *MemoryMapAddress;

    /* Check if custom memory type routine is specified */
    if(GetMemoryTypeRoutine == NULL)
    {
        /* Use default memory type routine */
        GetMemoryTypeRoutine = (PBL_GET_MEMTYPE_ROUTINE)BlpGetLoaderMemoryType;
    }

    /* Allocate and zero-fill buffer for EFI memory map */
    BlAllocateMemoryPool(sizeof(EFI_MEMORY_MAP), (PVOID*)&MemoryMap);
    RtlZeroMemory(MemoryMap, sizeof(EFI_MEMORY_MAP));

    /* Get EFI memory map */
    Status = BlGetMemoryMap(MemoryMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get EFI memory map */
        return Status;
    }

    /* Calculate descriptors count and get first one */
    Descriptor = MemoryMap->Map;
    DescriptorCount = MemoryMap->MapSize / MemoryMap->DescriptorSize;

    /* Iterate through all descriptors from the memory map */
    for(Index = 0; Index < DescriptorCount; Index++)
    {
        /* Make sure descriptor does not start beyond lowest physical page */
        if(Descriptor->PhysicalStart <= MAXUINT_PTR)
        {
            /* Skip EFI reserved memory */
            if(Descriptor->Type == EfiReservedMemoryType)
            {
                /* Go to the next descriptor */
                Descriptor = (PEFI_MEMORY_DESCRIPTOR)((PUCHAR)Descriptor + MemoryMap->DescriptorSize);
                continue;
            }

            /* Check if memory descriptor exceeds the lowest physical page */
            if(Descriptor->PhysicalStart + (Descriptor->NumberOfPages << EFI_PAGE_SHIFT) > MAXUINT_PTR)
            {
                /* Truncate memory descriptor to the 4GB */
                Descriptor->NumberOfPages = ((MAXUINT_PTR + 1) - Descriptor->PhysicalStart) >> EFI_PAGE_SHIFT;
            }

            /* Convert EFI memory type into XTLDR memory type */
            MemoryType = GetMemoryTypeRoutine(Descriptor->Type);

            /* Do memory mappings depending on memory type */
            if(MemoryType == LoaderFirmwareTemporary)
            {
                /* Map EFI firmware code */
                Status = BlMapVirtualMemory(PageMap, (PVOID)Descriptor->PhysicalStart,
                                            (PVOID)Descriptor->PhysicalStart, Descriptor->NumberOfPages, MemoryType);
            }
            else if(MemoryType != LoaderFree)
            {
                /* Add any non-free memory mapping */
                Status = BlMapVirtualMemory(PageMap, VirtualAddress, (PVOID)Descriptor->PhysicalStart,
                                            Descriptor->NumberOfPages, MemoryType);

                /* Calculate next valid virtual address */
                VirtualAddress += Descriptor->NumberOfPages * EFI_PAGE_SIZE;
            }
            else
            {
                /* Map all other memory as loader free */
                Status = BlMapVirtualMemory(PageMap, NULL, (PVOID)Descriptor->PhysicalStart,
                                            Descriptor->NumberOfPages, LoaderFree);
            }

            /* Make sure memory mapping succeeded */
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Mapping failed */
                return Status;
            }
        }

        /* Grab next descriptor */
        Descriptor = (PEFI_MEMORY_DESCRIPTOR)((PUCHAR)Descriptor + MemoryMap->DescriptorSize);
    }

    /* Always map first page */
    Status = BlMapVirtualMemory(PageMap, NULL, (PVOID)0, 1, LoaderFirmwarePermanent);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping failed */
        return Status;
    }

    /* Map BIOS ROM and VRAM */
    Status = BlMapVirtualMemory(PageMap, NULL, (PVOID)0xA0000, 0x60, LoaderFirmwarePermanent);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping failed */
        return Status;
    }

    /* Store next valid virtual address and return success */
    *MemoryMapAddress = VirtualAddress;
    return STATUS_EFI_SUCCESS;
}

/**
 * Adds a physical to virtual address mappings.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param VirtualAddress
 *        Supplies a virtual address where the physical address should be mapped.
 *
 * @param PhysicalAddress
 *        Supplies a physical address which will be mapped.
 *
 * @param NumberOfPages
 *        Supplies a number of pages that will be mapped.
 *
 * @param MemoryType
 *        Supplies the type of mapped memory that will be assigned to the memory descriptor.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlMapVirtualMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                   IN PVOID VirtualAddress,
                   IN PVOID PhysicalAddress,
                   IN ULONGLONG NumberOfPages,
                   IN LOADER_MEMORY_TYPE MemoryType)
{
    PXTBL_MEMORY_MAPPING Mapping1, Mapping2, Mapping3;
    PVOID PhysicalAddressEnd, PhysicalAddress2End;
    PLIST_ENTRY ListEntry, MappingListEntry;
    SIZE_T NumberOfMappedPages;
    EFI_STATUS Status;

    /* Allocate memory for new mapping */
    Status = BlAllocateMemoryPool(sizeof(XTBL_MEMORY_MAPPING), (PVOID *)&Mapping1);
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
    PhysicalAddressEnd = (PUCHAR)PhysicalAddress + (NumberOfPages * EFI_PAGE_SIZE) - 1;

    /* Iterate through all the mappings already set to insert new mapping at the correct place */
    ListEntry = PageMap->MemoryMap.Flink;
    while(ListEntry != &PageMap->MemoryMap)
    {
        /* Take a mapping from the list and calculate its end of physical address */
        Mapping2 = CONTAIN_RECORD(ListEntry, XTBL_MEMORY_MAPPING, ListEntry);
        PhysicalAddress2End = (PUCHAR)Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1 ;

        /* Check if new mapping is a subset of an existing mapping */
        if(Mapping1->PhysicalAddress >= Mapping2->PhysicalAddress && PhysicalAddressEnd <= PhysicalAddress2End)
        {
            /* Make sure it's memory type is the same */
            if(Mapping1->MemoryType == Mapping2->MemoryType)
            {
                /* It is already mapped */
                return STATUS_EFI_SUCCESS;
            }
        }

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
            NumberOfMappedPages = ((PUCHAR)PhysicalAddress2End - (PUCHAR)PhysicalAddressEnd) / EFI_PAGE_SIZE;
            if(NumberOfMappedPages > 0)
            {
                /* Pages associated to the mapping, allocate memory for it */
                Status = BlAllocateMemoryPool(sizeof(XTBL_MEMORY_MAPPING), (PVOID*)&Mapping3);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Memory allocation failure */
                    return Status;
                }

                /* Set mapping fields and insert it on the top */
                Mapping3->PhysicalAddress = (PUCHAR)PhysicalAddressEnd + 1;
                Mapping3->VirtualAddress = NULL;
                Mapping3->NumberOfPages = NumberOfMappedPages;
                Mapping3->MemoryType = Mapping2->MemoryType;
                RtlInsertHeadList(&Mapping2->ListEntry, &Mapping3->ListEntry);
            }

            /* Calculate number of pages and the end of the physical address */
            Mapping2->NumberOfPages = ((PUCHAR)PhysicalAddressEnd + 1 -
                                       (PUCHAR)Mapping2->PhysicalAddress) / EFI_PAGE_SIZE;
            PhysicalAddress2End = (PUCHAR)Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1;
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
            NumberOfMappedPages = ((PUCHAR)PhysicalAddress2End + 1 - (PUCHAR)Mapping1->PhysicalAddress) / EFI_PAGE_SIZE;
            if(NumberOfMappedPages > 0)
            {
                /* Pages associated to the mapping, allocate memory for it */
                Status = BlAllocateMemoryPool(sizeof(XTBL_MEMORY_MAPPING), (PVOID*)&Mapping3);
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
            Mapping2->NumberOfPages = ((PUCHAR)Mapping1->PhysicalAddress -
                                       (PUCHAR)Mapping2->PhysicalAddress) / EFI_PAGE_SIZE;
            PhysicalAddress2End = (PUCHAR)Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1;
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
            Status = BlFreeMemoryPool(Mapping2);
            ListEntry = MappingListEntry;

            /* Go to the next mapping */
            continue;
        }

        /* Determine physical address order */
        if(Mapping2->PhysicalAddress > Mapping1->PhysicalAddress)
        {
            /* Insert new mapping in front */
            RtlInsertHeadList(Mapping2->ListEntry.Blink, &Mapping1->ListEntry);
            return STATUS_EFI_SUCCESS;
        }

        /* Get next mapping from the list */
        ListEntry = ListEntry->Flink;
    }

    /* Insert new mapping to the list and increase page map size */
    RtlInsertTailList(&PageMap->MemoryMap, &Mapping1->ListEntry);
    PageMap->MapSize++;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Converts physical address to virtual address based on physical base and virtual base.
 *
 * @param PhysicalAddress
 *        Specifies physical address that will be converted to virtual address.
 *
 * @param PhysicalBase
 *        Supplies a physical base address.
 *
 * @param VirtualBase
 *        Supplies a virtual base address.
 *
 * @return This routine returns a mapped virtual address.
 *
 * @since XT 1.0
 */
XTCDECL
PVOID
BlPhysicalAddressToVirtual(IN PVOID PhysicalAddress,
                           IN PVOID PhysicalBase,
                           IN PVOID VirtualBase)
{
    /* Convert physical address to virtual address */
    return (PUCHAR)VirtualBase + ((PUCHAR)PhysicalAddress - (PUCHAR)PhysicalBase);
}

/**
 * Converts whole linked list addressing from physical to virtual for future use after enabling paging.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param ListHead
 *        Supplies a pointer to a structure that serves as the list header.
 *
 * @param PhysicalBase
 *        Supplies a physical base address.
 *
 * @param VirtualBase
 *        Supplies a virtual base address.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlPhysicalListToVirtual(IN PXTBL_PAGE_MAPPING PageMap,
                        IN OUT PLIST_ENTRY ListHead,
                        IN PVOID PhysicalBase,
                        IN PVOID VirtualBase)
{
    PLIST_ENTRY ListEntry, NextEntry;

    /* Make sure list is properly initialized */
    if(ListHead->Flink == 0 || ListHead->Blink == 0)
    {
        /* List not initialized, return error code */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Iterate through all elements */
    ListEntry = ListHead->Flink;
    while(ListEntry != ListHead)
    {
        /* Save physical address of the next element */
        NextEntry = ListEntry->Flink;

        /* Convert the address of this element to VirtualAddress */
        if(ListEntry->Flink == ListHead)
        {
            /* Convert list head */
            ListEntry->Flink = ListHead->Flink->Blink;
        }
        else
        {
            /* Convert list entry */
            ListEntry->Flink = BlPhysicalAddressToVirtual(ListEntry->Flink, (PVOID)PhysicalBase, VirtualBase);
        }
        if(ListEntry->Blink == ListHead)
        {
            /* Find virtual address of list head */
            ListEntry->Blink = BlGetVirtualAddress(PageMap, ListEntry->Blink);
        }
        else
        {
            /* Convert list entry */
            ListEntry->Blink = BlPhysicalAddressToVirtual(ListEntry->Blink, (PVOID)PhysicalBase, VirtualBase);
        }

        /* Get to the next element*/
        ListEntry = NextEntry;
    }

    /* Convert list head */
    ListHead->Flink = BlPhysicalAddressToVirtual(ListHead->Flink, (PVOID)PhysicalBase, VirtualBase);
    ListHead->Blink = BlPhysicalAddressToVirtual(ListHead->Blink, (PVOID)PhysicalBase, VirtualBase);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Converts EFI memory type to XTLDR memory type.
 *
 * @param EfiMemoryType
 *        Specifies EFI memory type.
 *
 * @return This routine returns a mapped XTLDR memory type.
 *
 * @since XT 1.0
 */
XTCDECL
LOADER_MEMORY_TYPE
BlpGetLoaderMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType)
{
    LOADER_MEMORY_TYPE MemoryType;

    /* Check EFI memory type and convert to XTLDR memory type */
    switch(EfiMemoryType)
    {
        case EfiACPIMemoryNVS:
        case EfiACPIReclaimMemory:
        case EfiPalCode:
        case EfiReservedMemoryType:
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
        case EfiLoaderData:
            MemoryType = LoaderFirmwareTemporary;
            break;
        case EfiUnusableMemory:
            MemoryType = LoaderBad;
            break;
        default:
            MemoryType = LoaderFree;
            break;
    }

    /* Return XTLDR memory type */
    return MemoryType;
}

/**
 * Returns next level of the Page Table.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @param PageTable
 *        Supplies a pointer to the current Page Table.
 *
 * @param Entry
 *        Supplies an index of the current Page Table entry.
 *
 * @param NextPageTable
 *        Supplies a pointer to the memory area where the next Page Table level is returned.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpGetNextPageTable(IN PXTBL_PAGE_MAPPING PageMap,
                    IN PHARDWARE_PTE PageTable,
                    IN SIZE_T Entry,
                    OUT PHARDWARE_PTE *NextPageTable)
{
    EFI_PHYSICAL_ADDRESS Address;
    ULONGLONG PmlPointer;
    EFI_STATUS Status;

    /* Check if this is a valid table */
    if(PageTable[Entry].Valid)
    {
        /* Get PML pointer */
        PmlPointer = PageTable[Entry].PageFrameNumber;
        PmlPointer <<= EFI_PAGE_SHIFT;
    }
    else
    {
        /* Allocate pages for new PML entry */
        Status = BlAllocateMemoryPages(1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            return Status;
        }

        /* Add new memory mapping */
        Status = BlMapVirtualMemory(PageMap, NULL, (PVOID)(UINT_PTR)Address, 1, LoaderMemoryData);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory mapping failure */
            return Status;
        }

        /* Fill allocated memory with zeros */
        RtlZeroMemory((PVOID)(ULONGLONG)Address, EFI_PAGE_SIZE);

        /* Set paging entry settings */
        PageTable[Entry].PageFrameNumber = Address / EFI_PAGE_SIZE;
        PageTable[Entry].Valid = 1;
        PageTable[Entry].Write = 1;
        PmlPointer = (ULONGLONG)Address;
    }

    /* Set next Page Map Level (PML) */
    *NextPageTable = (PHARDWARE_PTE)(ULONGLONG)PmlPointer;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
