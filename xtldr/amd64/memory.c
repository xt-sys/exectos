/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/amd64/memory.c
 * DESCRIPTION:     EFI memory management for AMD64 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


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
 * @param PtePointer
 *        Supplies a pointer to an array of pointers to page table entries.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
BlMapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                   IN UINT_PTR VirtualAddress,
                   IN UINT_PTR PhysicalAddress,
                   IN UINT NumberOfPages,
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

            /* Zero memory */
            RtlZeroMemory((PVOID)(UINT_PTR)Address, EFI_PAGE_SIZE);

            /* Set paging entry setting */
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

            /* Zero memory */
            RtlZeroMemory((PVOID)(UINT_PTR)Address, EFI_PAGE_SIZE);

            /* Set paging entry setting */
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

            /* Zero memory */
            RtlZeroMemory((PVOID)(UINT_PTR)Address, EFI_PAGE_SIZE);

            /* Set paging entry setting */
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

        /* Set paging entry setting */
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
