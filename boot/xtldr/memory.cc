/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/memory.cc
 * DESCRIPTION:     XT Boot Loader memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


/**
 * This routine allocates one or more 4KB pages.
 *
 * @param NumberOfPages
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
Memory::AllocatePages(IN EFI_ALLOCATE_TYPE AllocationType,
                      IN ULONGLONG NumberOfPages,
                      OUT PEFI_PHYSICAL_ADDRESS Memory)
{
    return XtLoader::GetEfiSystemTable()->BootServices->AllocatePages(AllocationType, EfiLoaderData,
                                                                      NumberOfPages, Memory);
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
Memory::AllocatePool(IN UINT_PTR Size,
                     OUT PVOID *Memory)
{
    /* Allocate pool */
    return XtLoader::GetEfiSystemTable()->BootServices->AllocatePool(EfiLoaderData, Size, Memory);
}

/**
 * This routine frees memory pages.
 *
 * @param NumberOfPages
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
Memory::FreePages(IN ULONGLONG NumberOfPages,
                  IN EFI_PHYSICAL_ADDRESS Memory)
{
    return XtLoader::GetEfiSystemTable()->BootServices->FreePages(Memory, NumberOfPages);
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
Memory::FreePool(IN PVOID Memory)
{
    /* Free pool */
    return XtLoader::GetEfiSystemTable()->BootServices->FreePool(Memory);
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
Memory::GetLoaderMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType)
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
Memory::GetMappingsCount(IN PXTBL_PAGE_MAPPING PageMap,
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
Memory::GetMemoryMap(OUT PEFI_MEMORY_MAP MemoryMap)
{
    EFI_STATUS Status;

    if(MemoryMap == NULLPTR)
    {
        return STATUS_EFI_INVALID_PARAMETER;
    }

    MemoryMap->Map = NULLPTR;
    MemoryMap->MapSize = 0;

    /* Get memory map */
    do
    {
        /* Attempt do get EFI memory map */
        Status = XtLoader::GetEfiSystemTable()->BootServices->GetMemoryMap(&MemoryMap->MapSize,
                                                                           MemoryMap->Map,
                                                                           &MemoryMap->MapKey,
                                                                           &MemoryMap->DescriptorSize,
                                                                           &MemoryMap->DescriptorVersion);
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
                FreePool(MemoryMap->Map);
            }
            return Status;
        }

        /* Allocate the desired amount of memory */
        MemoryMap->MapSize += 2 * MemoryMap->DescriptorSize;
        AllocatePool(MemoryMap->MapSize, (PVOID *)&MemoryMap->Map);
    }
    while(Status == STATUS_EFI_BUFFER_TOO_SMALL);

    /* Make sure memory map is set */
    if(MemoryMap->Map == NULLPTR)
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
Memory::GetVirtualAddress(IN PXTBL_PAGE_MAPPING PageMap,
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
            if(((UINT_PTR)PhysicalAddress >= (UINT_PTR)Mapping->PhysicalAddress) &&
               ((UINT_PTR)PhysicalAddress < ((UINT_PTR)Mapping->PhysicalAddress + (Mapping->NumberOfPages * EFI_PAGE_SIZE))))
            {
                /* Calculate virtual address based on the mapping and return it */
                return (PVOID)(((UINT_PTR)PhysicalAddress - (UINT_PTR)Mapping->PhysicalAddress) + (UINT_PTR)Mapping->VirtualAddress);
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
Memory::InitializePageMap(OUT PXTBL_PAGE_MAPPING PageMap,
                          IN SHORT PageMapLevel,
                          IN PAGE_SIZE PageSize)
{
    /* Initialize memory mappings */
    RTL::LinkedList::InitializeListHead(&PageMap->MemoryMap);
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
Memory::MapEfiMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                     IN OUT PVOID *MemoryMapAddress,
                     IN PBL_GET_MEMTYPE_ROUTINE GetMemoryTypeRoutine)
{
    ULONGLONG MaxAddress, VirtualAddress;
    PEFI_MEMORY_DESCRIPTOR Descriptor;
    LOADER_MEMORY_TYPE MemoryType;
    PEFI_MEMORY_MAP MemoryMap;
    SIZE_T DescriptorCount;
    EFI_STATUS Status;
    SIZE_T Index;

    /* Set virtual address as specified in argument */
    VirtualAddress = (ULONGLONG)*MemoryMapAddress;

    /* Check if custom memory type routine is specified */
    if(GetMemoryTypeRoutine == NULLPTR)
    {
        /* Use default memory type routine */
        GetMemoryTypeRoutine = GetLoaderMemoryType;
    }

    /* Allocate and zero-fill buffer for EFI memory map */
    AllocatePool(sizeof(EFI_MEMORY_MAP), (PVOID*)&MemoryMap);
    RTL::Memory::ZeroMemory(MemoryMap, sizeof(EFI_MEMORY_MAP));

    /* Get EFI memory map */
    Status = GetMemoryMap(MemoryMap);
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
        /* Check page map level */
        if(PageMap->PageMapLevel == 2)
        {
            /* Limit physical address to 4GB in legacy mode */
            MaxAddress = 0xFFFFFFFF;
        }
        else if(PageMap->PageMapLevel == 3)
        {
            /* Limit physical address to 64GB in PAE mode */
            MaxAddress = 0xFFFFFFFFF;
        }

        /* Check page map level */
        if(PageMap->PageMapLevel == 2 || PageMap->PageMapLevel == 3)
        {
            /* Check if physical address starts beyond limit */
            if(Descriptor->PhysicalStart > MaxAddress)
            {
                /* Go to the next descriptor */
                Descriptor = (PEFI_MEMORY_DESCRIPTOR)((PUCHAR)Descriptor + MemoryMap->DescriptorSize);
                continue;
            }

            /* Check if memory descriptor exceeds the lowest physical page */
            if(Descriptor->PhysicalStart + (Descriptor->NumberOfPages << EFI_PAGE_SHIFT) > MaxAddress)
            {
                /* Truncate memory descriptor to the lowest supported physical page */
                Descriptor->NumberOfPages = (((ULONGLONG)MaxAddress) - Descriptor->PhysicalStart) >> EFI_PAGE_SHIFT;
            }
        }

        {
            /* Skip EFI reserved memory */
            if(Descriptor->Type == EfiReservedMemoryType)
            {
                /* Go to the next descriptor */
                Descriptor = (PEFI_MEMORY_DESCRIPTOR)((PUCHAR)Descriptor + MemoryMap->DescriptorSize);
                continue;
            }

            /* Convert EFI memory type into XTLDR memory type */
            MemoryType = GetMemoryTypeRoutine((EFI_MEMORY_TYPE)Descriptor->Type);

            /* Do memory mappings depending on memory type */
            if(MemoryType == LoaderFirmwareTemporary)
            {
                /* Map EFI firmware code */
                Status = MapVirtualMemory(PageMap, Descriptor->PhysicalStart,
                                          Descriptor->PhysicalStart, Descriptor->NumberOfPages, MemoryType);
            }
            else if(MemoryType != LoaderFree)
            {
                /* Add any non-free memory mapping */
                Status = MapVirtualMemory(PageMap, VirtualAddress, Descriptor->PhysicalStart,
                                          Descriptor->NumberOfPages, MemoryType);

                /* Update virtual address */
                VirtualAddress = VirtualAddress + (Descriptor->NumberOfPages * MM_PAGE_SIZE);
            }
            else
            {
                /* Map all other memory as loader free */
                Status = MapVirtualMemory(PageMap, (ULONGLONG)NULLPTR, Descriptor->PhysicalStart,
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
    Status = MapVirtualMemory(PageMap, (ULONGLONG)NULLPTR, 0, 1, LoaderFirmwarePermanent);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping failed */
        return Status;
    }

    /* Map BIOS ROM and VRAM */
    Status = MapVirtualMemory(PageMap, (ULONGLONG)NULLPTR, 0xA0000, 0x60, LoaderFirmwarePermanent);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping failed */
        return Status;
    }

    /* Store next valid virtual address and return success */
    *MemoryMapAddress = (PVOID)VirtualAddress;
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
Memory::MapVirtualMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                         IN ULONGLONG VirtualAddress,
                         IN ULONGLONG PhysicalAddress,
                         IN ULONGLONG NumberOfPages,
                         IN LOADER_MEMORY_TYPE MemoryType)
{
    PXTBL_MEMORY_MAPPING Mapping1, Mapping2, Mapping3;
    ULONGLONG PhysicalAddressEnd, PhysicalAddress2End;
    PLIST_ENTRY ListEntry, MappingListEntry;
    SIZE_T NumberOfMappedPages;
    EFI_STATUS Status;

    /* Allocate memory for new mapping */
    Status = AllocatePool(sizeof(XTBL_MEMORY_MAPPING), (PVOID *)&Mapping1);
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
    PhysicalAddressEnd = PhysicalAddress + (NumberOfPages * EFI_PAGE_SIZE) - 1;

    /* Iterate through all the mappings already set to insert new mapping at the correct place */
    ListEntry = PageMap->MemoryMap.Flink;
    while(ListEntry != &PageMap->MemoryMap)
    {
        /* Take a mapping from the list and calculate its end of physical address */
        Mapping2 = CONTAIN_RECORD(ListEntry, XTBL_MEMORY_MAPPING, ListEntry);
        PhysicalAddress2End = Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1;

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
            NumberOfMappedPages = (PhysicalAddress2End - PhysicalAddressEnd) / EFI_PAGE_SIZE;
            if(NumberOfMappedPages > 0)
            {
                /* Pages associated to the mapping, allocate memory for it */
                Status = AllocatePool(sizeof(XTBL_MEMORY_MAPPING), (PVOID*)&Mapping3);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Memory allocation failure */
                    return Status;
                }

                /* Set mapping fields and insert it on the top */
                Mapping3->PhysicalAddress = PhysicalAddressEnd + 1;
                Mapping3->VirtualAddress = (ULONGLONG)NULLPTR;
                Mapping3->NumberOfPages = NumberOfMappedPages;
                Mapping3->MemoryType = Mapping2->MemoryType;
                RTL::LinkedList::InsertHeadList(&Mapping2->ListEntry, &Mapping3->ListEntry);
            }

            /* Calculate number of pages and the end of the physical address */
            Mapping2->NumberOfPages = ((PUCHAR)PhysicalAddressEnd + 1 -
                                       (PUCHAR)Mapping2->PhysicalAddress) / EFI_PAGE_SIZE;
            PhysicalAddress2End = Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1;
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
                Status = AllocatePool(sizeof(XTBL_MEMORY_MAPPING), (PVOID*)&Mapping3);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Memory allocation failure */
                    return Status;
                }

                /* Set mapping fields and insert it on the top */
                Mapping3->PhysicalAddress = Mapping1->PhysicalAddress;
                Mapping3->VirtualAddress = (ULONGLONG)NULLPTR;
                Mapping3->NumberOfPages = NumberOfMappedPages;
                Mapping3->MemoryType = Mapping2->MemoryType;
                RTL::LinkedList::InsertHeadList(&Mapping2->ListEntry, &Mapping3->ListEntry);
            }

            /* Calculate number of pages and the end of the physical address */
            Mapping2->NumberOfPages = ((PUCHAR)Mapping1->PhysicalAddress -
                                       (PUCHAR)Mapping2->PhysicalAddress) / EFI_PAGE_SIZE;
            PhysicalAddress2End = Mapping2->PhysicalAddress + (Mapping2->NumberOfPages * EFI_PAGE_SIZE) - 1;
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
            RTL::LinkedList::RemoveEntryList(&Mapping2->ListEntry);
            Status = FreePool(Mapping2);
            ListEntry = MappingListEntry;

            /* Go to the next mapping */
            continue;
        }

        /* Determine physical address order */
        if(Mapping2->PhysicalAddress > Mapping1->PhysicalAddress)
        {
            /* Insert new mapping in front */
            RTL::LinkedList::InsertHeadList(Mapping2->ListEntry.Blink, &Mapping1->ListEntry);
            return STATUS_EFI_SUCCESS;
        }

        /* Get next mapping from the list */
        ListEntry = ListEntry->Flink;
    }

    /* Insert new mapping to the list and increase page map size */
    RTL::LinkedList::InsertTailList(&PageMap->MemoryMap, &Mapping1->ListEntry);
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
Memory::PhysicalAddressToVirtual(IN PVOID PhysicalAddress,
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
Memory::PhysicalListToVirtual(IN PXTBL_PAGE_MAPPING PageMap,
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
        if(ListEntry->Blink == ListHead)
        {
            /* Find virtual address of list head */
            ListEntry->Blink = (PLIST_ENTRY)GetVirtualAddress(PageMap, ListEntry->Blink);
        }
        else
        {
            /* Convert list entry */
            ListEntry->Blink = (PLIST_ENTRY)PhysicalAddressToVirtual(ListEntry->Blink, (PVOID)PhysicalBase, VirtualBase);
        }
        if(ListEntry->Flink == ListHead)
        {
            /* Convert list head */
            ListEntry->Flink = ListHead->Flink->Blink;
        }
        else
        {
            /* Convert list entry */
            ListEntry->Flink = (PLIST_ENTRY)PhysicalAddressToVirtual(ListEntry->Flink, (PVOID)PhysicalBase, VirtualBase);
        }

        /* Get to the next element*/
        ListEntry = NextEntry;
    }

    /* Convert list head */
    ListHead->Blink = (PLIST_ENTRY)PhysicalAddressToVirtual(ListHead->Blink, (PVOID)PhysicalBase, VirtualBase);
    ListHead->Flink = (PLIST_ENTRY)PhysicalAddressToVirtual(ListHead->Flink, (PVOID)PhysicalBase, VirtualBase);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
