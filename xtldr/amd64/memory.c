/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/amd64/memory.c
 * DESCRIPTION:     EFI memory management for AMD64 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * Creates and switches to a new stack.
 *
 * @param StackPtr
 *        Supplies a pointer to memory area, where the stack will be created.
 *
 * @param StackSize
 *        Specifies a size (in bytes) of the new stack.
 *
 * @param Callback
 *        Supplies a pointer to a callback function that will be executed on top of new stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlCreateStack(IN PVOID *StackPtr,
              IN ULONG StackSize,
              IN PVOID Callback)
{
    EFI_PHYSICAL_ADDRESS Address;
    PVOID StackEnd;

    /* Allocate pages for new stack and calculate its end */
    BlEfiMemoryAllocatePages(EFI_SIZE_TO_PAGES(StackSize), &Address);
    *StackPtr = (PVOID)(UINT_PTR)Address;
    StackEnd = (PUINT8)*StackPtr + (StackSize - EFI_PAGE_SIZE);

    /* Create new stack and switch to it immediatelly by calling callback function */
    asm volatile("mov %1, %%rax\n"
                 "mov %%rsp, %%rbx\n"
                 "mov %0, %%rsp\n"
                 "push %%rbp\n"
                 "mov %%rsp, %%rbp\n"
                 "push %%rbx\n"
                 "sub $32, %%rsp\n"
                 "call *%%rax\n"
                 :
                 : "m" (StackEnd), "m" (Callback)
                 : "rax", "rbx");
}

/**
 * Builds the actual memory mapping page table and enables paging. This routine exits EFI boot services as well.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a pointer to the next valid, free and available virtual address.
 *
 * @param ImageProtocol
 *        A pointer to the EFI loaded image protocol with information about where in memory the loader code was placed.
 *
 * @param PtePointer
 *        Supplies a pointer to memory area containing a Page Table Entries (PTE).
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlEnablePaging(IN PLIST_ENTRY MemoryMappings,
               IN PVOID VirtualAddress,
               IN PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol,
               IN PVOID *PtePointer)
{
    PLOADER_MEMORY_MAPPING Mapping;
    EFI_PHYSICAL_ADDRESS Address;
    PEFI_MEMORY_MAP MemoryMap;
    PLIST_ENTRY ListEntry;
    EFI_STATUS Status;
    PVOID Stack;

    /* Allocate and zero-fill buffer for EFI memory map */
    BlEfiMemoryAllocatePool(sizeof(EFI_MEMORY_MAP), (PVOID*)&MemoryMap);
    RtlZeroMemory(MemoryMap, sizeof(EFI_MEMORY_MAP));

    /* Get EFI memory map */
    Status = BlGetMemoryMap(MemoryMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to get memory map */
        return Status;
    }

    /* Allocate pages for PML4 */
    Status = BlEfiMemoryAllocatePages(1, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Assign and zero-fill memory used by page mappings */
    *PtePointer = (PVOID)(UINT_PTR)Address;
    RtlZeroMemory(*PtePointer, EFI_PAGE_SIZE);

    /* Map the stack */
    BlGetStackPointer(&Stack);
    Status = BlAddVirtualMemoryMapping(MemoryMappings, Stack, Stack, EFI_SIZE_TO_PAGES(KERNEL_STACK_SIZE),
                                       LoaderOsloaderStack);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping the stack failed */
        return Status;
    }

    /* Map XTLDR code */
    Status = BlAddVirtualMemoryMapping(MemoryMappings, ImageProtocol->ImageBase, ImageProtocol->ImageBase,
                                       EFI_SIZE_TO_PAGES(ImageProtocol->ImageSize), LoaderFirmwareTemporary);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping the boot loader code failed */
        return Status;
    }

    /* Add page mapping itself to memory mapping */
    Status = BlAddVirtualMemoryMapping(MemoryMappings, NULL, *PtePointer, 1, LoaderMemoryData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping PML4 failed */
        return Status;
    }

    /* Iterate through all mappings */
    ListEntry = MemoryMappings->Flink;
    while(ListEntry != MemoryMappings)
    {
        /* Take mapping from the list */
        Mapping = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_MAPPING, ListEntry);

        /* Check if virtual address is set */
        if(Mapping->VirtualAddress)
        {
            /* Map memory */
            Status = BlMapVirtualMemory(MemoryMappings, (UINT_PTR)Mapping->VirtualAddress,
                                        (UINT_PTR)Mapping->PhysicalAddress, Mapping->NumberOfPages, FALSE, PtePointer);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory mapping failed */
                return Status;
            }
        }

        /* Take next element */
        ListEntry = ListEntry->Flink;
    }

    /* Exit EFI Boot Services */
    BlDbgPrint(L"Exiting EFI boot services\n");
    EfiSystemTable->BootServices->ExitBootServices(EfiImageHandle, MemoryMap->MapKey);

    /* No runtime services should touch boot services code, so get rid of it all at this point */
    EfiSystemTable->RuntimeServices->SetVirtualAddressMap(MemoryMap->MapSize, MemoryMap->DescriptorSize,
                                                          MemoryMap->DescriptorVersion, MemoryMap->Map);

    /* Write PML4 to CR3 */
    HlWriteControlRegister(3, (UINT_PTR)*PtePointer);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine does the actual virtual memory mapping.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a virtual address of the mapping.
 *
 * @param PhysicalAddress
 *        Supplies a physical address of the mapping.
 *
 * @param NumberOfPages
 *        Supplies a number of the pages of the mapping.
 *
 * @param PaeExtension
 *        Specifies whether Physical Address Extension (PAE) is supported by the hardware. Not used on AMD64.
 *
 * @param PtePointer
 *        Supplies a pointer to an array of pointers to page table entries.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlMapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                   IN UINT_PTR VirtualAddress,
                   IN UINT_PTR PhysicalAddress,
                   IN UINT NumberOfPages,
                   IN BOOLEAN PaeExtension,
                   IN OUT PVOID *PtePointer)
{
    PHARDWARE_PTE PageDirectoryPointTable, PageDirectory, PageTable;
    UINT Pml4Index, PdpIndex, PdIndex, PtIndex;
    EFI_PHYSICAL_ADDRESS Address;
    UINT_PTR PageFrameNumber;
    EFI_STATUS Status;
    UINT64 Pointer;

    /* Set the PFN */
    PageFrameNumber = PhysicalAddress >> EFI_PAGE_SHIFT;

    /* Do the recursive mapping */
    while(NumberOfPages > 0)
    {
        /* Calculate indices from a virtual address */
        Pml4Index = (VirtualAddress >> 39) & 0x1FF;
        PdpIndex = (VirtualAddress >> 30) & 0x1FF;
        PdIndex = (VirtualAddress >> 21) & 0x1FF;
        PtIndex = (VirtualAddress >> 12) & 0x1FF;

        /* Validate Page Map Level 4 (PML4) */
        if(!((PHARDWARE_PTE)(*PtePointer))[Pml4Index].Valid)
        {
            /* Allocate pages for the PDPT */
            Status = BlEfiMemoryAllocatePages(1, &Address);
            if (Status != STATUS_EFI_SUCCESS) {
                /* Memory allocation failure */
                return Status;
            }

            /* Add new memory mapping */
            Status = BlAddVirtualMemoryMapping(MemoryMappings, NULL, (PVOID)(UINT_PTR)Address, 1, LoaderMemoryData);
            if(Status != STATUS_EFI_SUCCESS) {
                /* Memory mapping failed */
                return Status;
            }

            /* Fill allocated memory with zeros */
            RtlZeroMemory((PVOID)(UINT_PTR)Address, EFI_PAGE_SIZE);

            /* Set paging entry settings */
            ((PHARDWARE_PTE)(*PtePointer))[Pml4Index].PageFrameNumber = Address / EFI_PAGE_SIZE;
            ((PHARDWARE_PTE)(*PtePointer))[Pml4Index].Valid = 1;
            ((PHARDWARE_PTE)(*PtePointer))[Pml4Index].Write = 1;
            PageDirectoryPointTable = (PHARDWARE_PTE)(UINT_PTR)Address;
        }
        else
        {
            /* Find Page Directory Point Table (PDPT) */
            Pointer = ((PHARDWARE_PTE)(*PtePointer))[Pml4Index].PageFrameNumber;
            Pointer <<= EFI_PAGE_SHIFT;
            PageDirectoryPointTable = (PHARDWARE_PTE)(UINT_PTR)Pointer;
        }

        /* Validate Page Directory Point Table (PDPT)*/
        if(!PageDirectoryPointTable[PdpIndex].Valid)
        {
            /* Allocate pages for the PD */
            Status = BlEfiMemoryAllocatePages(1, &Address);
            if (Status != STATUS_EFI_SUCCESS) {
                /* Memory allocation failure */
                return Status;
            }

            /* Add new memory mapping */
            Status = BlAddVirtualMemoryMapping(MemoryMappings, NULL, (PVOID)(UINT_PTR)Address, 1, LoaderMemoryData);
            if (Status != STATUS_EFI_SUCCESS) {
                /* Memory mapping failed */
                return Status;
            }

            /* Fill allocated memory with zeros */
            RtlZeroMemory((PVOID)(UINT_PTR)Address, EFI_PAGE_SIZE);

            /* Set paging entry settings */
            PageDirectoryPointTable[PdpIndex].PageFrameNumber = Address / EFI_PAGE_SIZE;
            PageDirectoryPointTable[PdpIndex].Valid = 1;
            PageDirectoryPointTable[PdpIndex].Write = 1;
            PageDirectory = (PHARDWARE_PTE)(UINT_PTR)Address;
        }
        else
        {
            /* Find Page Directory (PD) */
            Pointer = PageDirectoryPointTable[PdpIndex].PageFrameNumber;
            Pointer <<= EFI_PAGE_SHIFT;
            PageDirectory = (PHARDWARE_PTE)(UINT_PTR)Pointer;
        }

        /* Validate Page Directory (PD)*/
        if(!PageDirectory[PdIndex].Valid)
        {
            /* Allocate pages for the PT */
            Status = BlEfiMemoryAllocatePages(1, &Address);
            if (Status != STATUS_EFI_SUCCESS) {
                /* Memory allocation failure */
                return Status;
            }

            /* Add new memory mapping */
            Status = BlAddVirtualMemoryMapping(MemoryMappings, NULL, (PVOID)(UINT_PTR)Address, 1, LoaderMemoryData);
            if (Status != STATUS_EFI_SUCCESS) {
                /* Memory mapping failed */
                return Status;
            }

            /* Fill allocated memory with zeros */
            RtlZeroMemory((PVOID)(UINT_PTR)Address, EFI_PAGE_SIZE);

            /* Set paging entry settings */
            PageDirectory[PdIndex].PageFrameNumber = Address / EFI_PAGE_SIZE;
            PageDirectory[PdIndex].Valid = 1;
            PageDirectory[PdIndex].Write = 1;
            PageTable = (PHARDWARE_PTE)(UINT_PTR)Address;
        }
        else
        {
            /* Find Page Table (PT) */
            Pointer = PageDirectory[PdIndex].PageFrameNumber;
            Pointer <<= EFI_PAGE_SHIFT;
            PageTable = (PHARDWARE_PTE)(UINT_PTR)Pointer;
        }

        /* Set paging entry settings */
        PageTable[PtIndex].PageFrameNumber = PageFrameNumber;
        PageTable[PtIndex].Valid = 1;
        PageTable[PtIndex].Write = 1;

        /* Take next virtual address and PFN */
        VirtualAddress += EFI_PAGE_SIZE;
        PageFrameNumber++;

        /* Decrease number of pages left */
        NumberOfPages--;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
