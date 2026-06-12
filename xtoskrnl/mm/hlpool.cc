/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/hlpool.cc
 * DESCRIPTION:     Hardware layer pool memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Allocates physical memory for kernel hardware layer before memory manager gets initialized.
 *
 * @param PageCount
 *        Supplies the number of pages to be allocated.
 *
 * @param Aligned
 *        Specifies whether allocated memory should be aligned to 64k boundary or not.
 *
 * @param MaximumAddress
 *        Supplies the maximum acceptable physical address for the allocation.
 *
 * @param Buffer
 *        Supplies a buffer that receives the physical address.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::HardwarePool::AllocateHardwareMemory(IN PFN_NUMBER PageCount,
                                         IN BOOLEAN Aligned,
                                         IN ULONGLONG MaximumAddress,
                                         OUT PPHYSICAL_ADDRESS Buffer)
{
    PLOADER_MEMORY_DESCRIPTOR Descriptor, ExtraDescriptor, HardwareDescriptor;
    PLIST_ENTRY ListEntry, LoaderMemoryDescriptors;
    PFN_NUMBER Alignment, MaxPage;
    ULONGLONG PhysicalAddress;

    /* Assume failure */
    (*Buffer).QuadPart = 0;

    /* Calculate maximum page address based on the requested limit */
    MaxPage = MaximumAddress >> MM_PAGE_SHIFT;

    /* Make sure there are at least 2 descriptors available */
    if((UsedHardwareAllocationDescriptors + 2) > MM_HARDWARE_ALLOCATION_DESCRIPTORS)
    {
        /* Not enough descriptors, return error */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Get a list of memory descriptors provided by the boot loader */
    LoaderMemoryDescriptors = KE::BootInformation::GetMemoryDescriptors();

    /* Scan memory descriptors provided by the boot loader */
    ListEntry = LoaderMemoryDescriptors->Blink;
    while(ListEntry != LoaderMemoryDescriptors)
    {
        Descriptor = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_DESCRIPTOR, ListEntry);

        /* Align memory to 64KB if needed */
        Alignment = Aligned ? (((Descriptor->BasePage + 0x0F) & ~0x0F) - Descriptor->BasePage) : 0;

        /* Ensure that memory type is free for this descriptor */
        if(Descriptor->MemoryType == LoaderFree)
        {
            /* Check if descriptor is big enough and if it fits under the maximum physical address */
            if(Descriptor->BasePage &&
               ((Descriptor->BasePage + PageCount + Alignment) < MaxPage) &&
               (Descriptor->PageCount >= (PageCount + Alignment)))
            {
                /* Set physical address */
                PhysicalAddress = (Descriptor->BasePage + Alignment) << MM_PAGE_SHIFT;
                break;
            }
        }

        /* Move to previous descriptor */
        ListEntry = ListEntry->Blink;
    }

    /* Make sure we found a descriptor */
    if(ListEntry == LoaderMemoryDescriptors)
    {
        /* Descriptor not found, return error */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Allocate new descriptor */
    HardwareDescriptor = &HardwareAllocationDescriptors[UsedHardwareAllocationDescriptors];
    HardwareDescriptor->BasePage = Descriptor->BasePage + Alignment;
    HardwareDescriptor->MemoryType = LoaderHardwareCachedMemory;
    HardwareDescriptor->PageCount = PageCount;

    /* Update hardware allocation descriptors count */
    UsedHardwareAllocationDescriptors++;

    /* Check if alignment was done */
    if(Alignment)
    {
        /* Check if extra descriptor is needed to describe the allocation */
        if(Descriptor->PageCount > (PageCount + Alignment))
        {
            /* Initialize extra descriptor */
            ExtraDescriptor = &HardwareAllocationDescriptors[UsedHardwareAllocationDescriptors];
            ExtraDescriptor->BasePage = Descriptor->BasePage + Alignment + (ULONG)PageCount;
            ExtraDescriptor->MemoryType = LoaderFree;
            ExtraDescriptor->PageCount = Descriptor->PageCount - (Alignment + (ULONG)PageCount);

            /* Update hardware allocation descriptors count */
            UsedHardwareAllocationDescriptors++;

            /* Insert extra descriptor in the list */
            RTL::LinkedList::InsertHeadList(&Descriptor->ListEntry, &ExtraDescriptor->ListEntry);
        }

        /* Trim source descriptor to the alignment */
        Descriptor->PageCount = Alignment;

        /* Insert new descriptor in the list */
        RTL::LinkedList::InsertHeadList(&Descriptor->ListEntry, &HardwareDescriptor->ListEntry);
    }
    else
    {
        /* Consume pages from the source descriptor */
        Descriptor->BasePage += (ULONG)PageCount;
        Descriptor->PageCount -= (ULONG)PageCount;

        /* Insert new descriptor in the list */
        RTL::LinkedList::InsertTailList(&Descriptor->ListEntry, &HardwareDescriptor->ListEntry);

        /* Check if source descriptor is fully consumed */
        if(Descriptor->PageCount == 0)
        {
            /* Remove descriptor from the list */
            RTL::LinkedList::RemoveEntryList(&Descriptor->ListEntry);
        }
    }

    /* Return physical address */
    (*Buffer).QuadPart = PhysicalAddress;
    return STATUS_SUCCESS;
}

/**
 * Allocates a physical page in low memory (addressable in real-mode) and maps it into the virtual address space.
 *
 * @param PhysicalAddress
 *        Supplies a pointer to a variable that receives the physical address of the allocated memory.
 *
 * @param VirtualAddress
 *        Supplies a pointer to a variable that receives the virtual address of the allocated memory.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::HardwarePool::AllocateLowMemory(OUT PPHYSICAL_ADDRESS PhysicalAddress,
                                    OUT PVOID *VirtualAddress)
{
    ULONG AllocationPages, TrampolineCodeSize;
    PVOID TrampolineCode;
    XTSTATUS Status;

    /* Check if low memory is already allocated and mapped */
    if(LowMemoryVirtualAddress && LowMemoryPhysicalAddress.QuadPart != 0)
    {
        /* Check if the caller requested the allocated addresses */
        if(PhysicalAddress != NULLPTR && VirtualAddress != NULLPTR)
        {
            /* Set the trampoline physical and virtual address and return success */
            *PhysicalAddress = LowMemoryPhysicalAddress;
            *VirtualAddress = LowMemoryVirtualAddress;
        }

        /* Return success */
        return STATUS_SUCCESS;
    }

    /* Get trampoline information */
    AR::ProcessorSupport::GetTrampolineInformation(TrampolineApStartup, &TrampolineCode, &TrampolineCodeSize);

    /* Verify trampoline information */
    if(TrampolineCode == NULLPTR || TrampolineCodeSize == 0)
    {
        /* Failed to get trampoline information, return error */
        return STATUS_UNSUCCESSFUL;
    }

    /* Compute number of pages for real-mode memory allocation (trampoline + processor start block + temporary stack) */
    AllocationPages = CalculateRealModeAllocationPages(TrampolineCodeSize);

    /* Allocate physical memory in first 1MB */
    Status = AllocateHardwareMemory(AllocationPages, TRUE, 0x100000, &LowMemoryPhysicalAddress);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to allocate memory, return error */
        return Status;
    }

    /* Map the memory to the virtual address */
    Status = MapHardwareMemory(LowMemoryPhysicalAddress, AllocationPages, FALSE, &LowMemoryVirtualAddress);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to map memory, free memory and return error */
        FreeHardwareMemory(LowMemoryPhysicalAddress, AllocationPages);
        return Status;
    }

    /* Check if the caller requested the allocated addresses */
    if(PhysicalAddress != NULLPTR && VirtualAddress != NULLPTR)
    {
        /* Set the trampoline physical and virtual address and return success */
        *PhysicalAddress = LowMemoryPhysicalAddress;
        *VirtualAddress = LowMemoryVirtualAddress;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Computes the total memory allocation size required for the real-mode trampoline execution environment.
 *
 * @param TrampolineCodeSize
 *        Supplies the size of the trampoline code in bytes.
 *
 * @return This routine returns the computed allocation size in pages.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
MM::HardwarePool::CalculateRealModeAllocationPages(IN ULONG TrampolineCodeSize)
{
    ULONG Size;

    /* Compute real-mode memory allocation size (trampoline + processor start block + temporary stack) */
    Size = TrampolineCodeSize + sizeof(PROCESSOR_START_BLOCK) + 512;

    /* Calculate and return number of pages */
    return (ULONG)(ROUND_UP(Size, MM_PAGE_SIZE) / MM_PAGE_SIZE);
}

/**
 * Releases physical memory allocated for kernel hardware layer.
 *
 * @param PhysicalAddress
 *        Supplies the physical address of the memory block to be freed.
 *
 * @param PageCount
 *        Supplies the number of pages associated with the allocation descriptor.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::HardwarePool::FreeHardwareMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                     IN PFN_NUMBER PageCount)
{
    PLOADER_MEMORY_DESCRIPTOR Descriptor;
    PFN_NUMBER BasePage;
    ULONG Index;

    /* Calculate base page from physical address */
    BasePage = PhysicalAddress.QuadPart >> MM_PAGE_SHIFT;

    /* Iterate through recorded hardware descriptors to find the matching allocation */
    for(Index = 0; Index < UsedHardwareAllocationDescriptors; Index++)
    {
        /* Get current hardware allocation descriptor */
        Descriptor = &HardwareAllocationDescriptors[Index];

        /* Verify descriptor properties */
        if(Descriptor->MemoryType == LoaderHardwareCachedMemory &&
           Descriptor->BasePage == BasePage &&
           Descriptor->PageCount == PageCount)
        {
            /* Make descriptor available again */
            Descriptor->MemoryType = LoaderFree;

            /* Return success */
            return STATUS_SUCCESS;
        }
    }

    /* Descriptors not found, return error */
    return STATUS_INVALID_PARAMETER;
}

/**
 * Maps physical address to the virtual memory area used by kernel hardware layer.
 *
 * @param PhysicalAddress
 *        Supplies the physical address to map.
 *
 * @param PageCount
 *        Supplies the number of pages to be mapped.
 *
 * @param FlushTlb
 *        Specifies whether to flush the TLB or not.
 *
 * @param VirtualAddress
 *        Supplies a buffer that receives the virtual address of the mapped pages.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::HardwarePool::MapHardwareMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                    IN PFN_NUMBER PageCount,
                                    IN BOOLEAN FlushTlb,
                                    OUT PVOID *VirtualAddress)
{
    PVOID BaseAddress, ReturnAddress;
    PFN_NUMBER MappedPages;
    PMMPTE PtePointer;

    /* Initialize variables */
    BaseAddress = HardwareHeapStart;
    MappedPages = 0;
    ReturnAddress = BaseAddress;
    *VirtualAddress = NULLPTR;

    /* Iterate through all pages */
    while(MappedPages < PageCount)
    {
        /* Check if address overflows */
        if(BaseAddress == NULLPTR)
        {
            /* Not enough free pages, return error */
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        /* Get PTE pointer and advance to next page */
        PtePointer = MM::Paging::GetPteAddress(ReturnAddress);
        ReturnAddress = (PVOID)((ULONG_PTR)ReturnAddress + MM_PAGE_SIZE);

        /* Check if PTE is valid */
        if(MM::Paging::PteValid(PtePointer))
        {
            /* PTE is not available, go to the next one */
            BaseAddress = ReturnAddress;
            MappedPages = 0;
            continue;
        }

        /* Increase number of mapped pages */
        MappedPages++;
    }

    /* Take the actual base address with an offset */
    ReturnAddress = (PVOID)((ULONG_PTR)BaseAddress + PAGE_OFFSET(PhysicalAddress.LowPart));

    /* Check if base address starts at the beginning of the heap */
    if(BaseAddress == HardwareHeapStart)
    {
        /* Move heap beyond base address */
        HardwareHeapStart = (PVOID)((ULONG_PTR)BaseAddress + ((ULONG_PTR)PageCount << MM_PAGE_SHIFT));
    }

    /* Iterate through mapped pages */
    while(MappedPages--)
    {
        /* Get PTE pointer */
        PtePointer = MM::Paging::GetPteAddress(BaseAddress);

        /* Fill the PTE */
        MM::Paging::SetPte(PtePointer, (PFN_NUMBER)(PhysicalAddress.QuadPart >> MM_PAGE_SHIFT), MM_PTE_READWRITE);

        /* Advance to the next address */
        PhysicalAddress.QuadPart += MM_PAGE_SIZE;
        BaseAddress = (PVOID)((ULONG_PTR)BaseAddress + MM_PAGE_SIZE);
    }

    /* Check if TLB needs to be flushed */
    if(FlushTlb)
    {
        /* Flush the TLB */
        MM::Paging::FlushTlb();
    }

    /* Return virtual address */
    *VirtualAddress = ReturnAddress;
    return STATUS_SUCCESS;
}

/**
 * Establishes a temporary identity mapping for a specified physical memory range to facilitate real-mode execution.
 *
 * @param PhysicalAddress
 *        Supplies the physical address of the trampoline memory.
 *
 * @param Size
 *        Supplies the size of the trampoline memory allocation.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::HardwarePool::MapRealModeMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                    IN ULONG Pages)
{
    PFN_NUMBER Index;
    XTSTATUS Status;

    /* Identity map each page of the real-mode memory allocation */
    for(Index = 0; Index < Pages; Index++)
    {
        /* Map the current physical page */
        Status = MM::Paging::MapVirtualAddress((PVOID)(PhysicalAddress.QuadPart + (Index * MM_PAGE_SIZE)),
                                               (PhysicalAddress.QuadPart >> MM_PAGE_SHIFT) + Index,
                                               MM_PTE_EXECUTE_READWRITE);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to map the page, unmap previously mapped pages and return the error code */
            UnmapRealModeMemory(PhysicalAddress, Index * MM_PAGE_SIZE);
            return Status;
        }
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Marks existing mapping as CD/WT to avoid delays in write-back cache.
 *
 * @param VirtualAddress
 *        Supplies the virtual address region to mark as CD/WT.
 *
 * @param PageCount
 *        Supplies the number of mapped pages.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::HardwarePool::MarkHardwareMemoryWriteThrough(IN PVOID VirtualAddress,
                                                 IN PFN_NUMBER PageCount)
{
    PMMPTE PtePointer;
    PFN_NUMBER Page;

    /* Get PTE address from virtual address */
    PtePointer = MM::Paging::GetPteAddress(VirtualAddress);

    /* Iterate through mapped pages */
    for(Page = 0; Page < PageCount; Page++)
    {
        /* Mark pages as CD/WT */
        MM::Paging::SetPteCaching(PtePointer, TRUE, TRUE);
        MM::Paging::GetNextEntry(PtePointer);
    }
}

/**
 * Remaps the PTE to new physical address.
 *
 * @param VirtualAddress
 *        Supplies the virtual address to remap.
 *
 * @param PhysicalAddress
 *        Supplies a new physical address.
 *
 * @param FlushTlb
 *        Specifies whether to flush the TLB or not.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::HardwarePool::RemapHardwareMemory(IN PVOID VirtualAddress,
                                      IN PHYSICAL_ADDRESS PhysicalAddress,
                                      IN BOOLEAN FlushTlb)
{
    PMMPTE PtePointer;

    /* Get PTE address from virtual address */
    PtePointer = MM::Paging::GetPteAddress(VirtualAddress);

    /* Remap the PTE */
    MM::Paging::SetPte(PtePointer, (PFN_NUMBER)(PhysicalAddress.QuadPart >> MM_PAGE_SHIFT), MM_PTE_READWRITE);

    /* Check if TLB needs to be flushed */
    if(FlushTlb)
    {
        /* Flush the TLB */
        MM::Paging::FlushTlb();
    }
}

/**
 * Unmaps a Page Table Entry corresponding to the given virtual address.
 *
 * @param VirtualAddress
 *        Supplies the virtual address to unmap.
 *
 * @param PageCount
 *        Supplies the number of mapped pages.
 *
 * @param FlushTlb
 *        Specifies whether to flush the TLB or not.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::HardwarePool::UnmapHardwareMemory(IN PVOID VirtualAddress,
                                      IN PFN_NUMBER PageCount,
                                      IN BOOLEAN FlushTlb)
{
    PMMPTE PtePointer;
    PFN_NUMBER Page;

    /* Check if address is valid hardware memory */
    if(VirtualAddress < (PVOID)MM_HARDWARE_VA_START)
    {
        /* Invalid address, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Align virtual address down to page boundary */
    VirtualAddress = (PVOID)((ULONG_PTR)VirtualAddress & ~(MM_PAGE_SIZE - 1));

    /* Get PTE address from virtual address */
    PtePointer = MM::Paging::GetPteAddress(VirtualAddress);

    /* Iterate through mapped pages */
    for(Page = 0; Page < PageCount; Page++)
    {
        /* Unmap the PTE and get the next one */
        MM::Paging::ClearPte(PtePointer);
        PtePointer++;
    }

    /* Check if TLB needs to be flushed */
    if(FlushTlb)
    {
        /* Flush the TLB */
        MM::Paging::FlushTlb();
    }

    /* Check if heap can be reused */
    if(HardwareHeapStart > VirtualAddress)
    {
        /* Free VA space */
        HardwareHeapStart = VirtualAddress;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Removes the temporary identity mapping for a real-mode memory region.
 *
 * @param PhysicalAddress
 *        Supplies the physical address of the trampoline memory.
 *
 * @param Size
 *        Supplies the size of the trampoline memory allocation.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::HardwarePool::UnmapRealModeMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                                      IN ULONG Size)
{
    PFN_NUMBER AllocationPages, Index;

    /* Calculate number of pages to unmap */
    AllocationPages = (ULONG)(ROUND_UP(Size, MM_PAGE_SIZE) / MM_PAGE_SIZE);

    /* Iterate over the allocation pages to remove the identity mapping */
    for(Index = 0; Index < AllocationPages; Index++)
    {
        /* Clear the page table entry for the current virtual address */
        MM::Paging::ClearPte(MM::Paging::GetPteAddress((PVOID)(PhysicalAddress.QuadPart + (Index * MM_PAGE_SIZE))));
    }
}
