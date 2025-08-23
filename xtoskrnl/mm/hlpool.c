/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/hlpool.c
 * DESCRIPTION:     Hardware layer pool memory management
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Allocates physical memory for kernel hardware layer before memory manager gets initialized.
 *
 * @param PageCount
 *        Supplies the number of pages to be allocated.
 *
 * @param Aligned
 *        Specifies whether allocated memory should be aligned to 64k boundary or not.
 *
 * @param Buffer
 *        Supplies a buffer that receives the physical address.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MmAllocateHardwareMemory(IN PFN_NUMBER PageCount,
                         IN BOOLEAN Aligned,
                         OUT PPHYSICAL_ADDRESS Buffer)
{
    PLOADER_MEMORY_DESCRIPTOR Descriptor, ExtraDescriptor, HardwareDescriptor;
    PFN_NUMBER Alignment, MaxPage;
    ULONGLONG PhysicalAddress;
    PLIST_ENTRY ListEntry;

    /* Assume failure */
    (*Buffer).QuadPart = 0;

    /* Calculate maximum page address */
    MaxPage = MM_MAXIMUM_PHYSICAL_ADDRESS >> MM_PAGE_SHIFT;

    /* Make sure there are at least 2 descriptors available */
    if((MmpUsedHardwareAllocationDescriptors + 2) > MM_HARDWARE_ALLOCATION_DESCRIPTORS)
    {
        /* Not enough descriptors, return error */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Scan memory descriptors provided by the boot loader */
    ListEntry = KeInitializationBlock->MemoryDescriptorListHead.Flink;
    while(ListEntry != &KeInitializationBlock->MemoryDescriptorListHead)
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

        /* Move to next descriptor */
        ListEntry = ListEntry->Flink;
    }

    /* Make sure we found a descriptor */
    if(ListEntry == &KeInitializationBlock->MemoryDescriptorListHead)
    {
        /* Descriptor not found, return error */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Allocate new descriptor */
    HardwareDescriptor = &MmpHardwareAllocationDescriptors[MmpUsedHardwareAllocationDescriptors];
    HardwareDescriptor->BasePage = Descriptor->BasePage + Alignment;
    HardwareDescriptor->MemoryType = LoaderHardwareCachedMemory;
    HardwareDescriptor->PageCount = PageCount;

    /* Update hardware allocation descriptors count */
    MmpUsedHardwareAllocationDescriptors++;

    /* Check if alignment was done */
    if(Alignment)
    {
        /* Check if extra descriptor is needed to describe the allocation */
        if(Descriptor->PageCount > (PageCount + Alignment))
        {
            /* Initialize extra descriptor */
            ExtraDescriptor = &MmpHardwareAllocationDescriptors[MmpUsedHardwareAllocationDescriptors];
            ExtraDescriptor->BasePage = Descriptor->BasePage + Alignment + (ULONG)PageCount;
            ExtraDescriptor->MemoryType = LoaderFree;
            ExtraDescriptor->PageCount = Descriptor->PageCount - (Alignment + (ULONG)PageCount);

            /* Update hardware allocation descriptors count */
            MmpUsedHardwareAllocationDescriptors++;

            /* Insert extra descriptor in the list */
            RtlInsertHeadList(&Descriptor->ListEntry, &ExtraDescriptor->ListEntry);
        }

        /* Trim source descriptor to the alignment */
        Descriptor->PageCount = Alignment;

        /* Insert new descriptor in the list */
        RtlInsertHeadList(&Descriptor->ListEntry, &HardwareDescriptor->ListEntry);
    }
    else
    {
        /* Consume pages from the source descriptor */
        Descriptor->BasePage += (ULONG)PageCount;
        Descriptor->PageCount -= (ULONG)PageCount;

        /* Insert new descriptor in the list */
        RtlInsertTailList(&Descriptor->ListEntry, &HardwareDescriptor->ListEntry);

        /* Check if source descriptor is fully consumed */
        if(Descriptor->PageCount == 0)
        {
            /* Remove descriptor from the list */
            RtlRemoveEntryList(&Descriptor->ListEntry);
        }
    }

    /* Return physical address */
    (*Buffer).QuadPart = PhysicalAddress;
    return STATUS_SUCCESS;
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
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MmMapHardwareMemory(IN PHYSICAL_ADDRESS PhysicalAddress,
                    IN PFN_NUMBER PageCount,
                    IN BOOLEAN FlushTlb,
                    OUT PVOID *VirtualAddress)
{
    PVOID BaseAddress, ReturnAddress;
    PFN_NUMBER MappedPages;
    PHARDWARE_PTE PtePointer;

    /* Initialize variables */
    BaseAddress = MmpHardwareHeapStart;
    MappedPages = 0;
    ReturnAddress = BaseAddress;
    *VirtualAddress = NULL;

    /* Iterate through all pages */
    while(MappedPages < PageCount)
    {
        /* Check if address overflows */
        if(BaseAddress == NULL)
        {
            /* Not enough free pages, return error */
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        /* Get PTE pointer and advance to next page */
        PtePointer = (PHARDWARE_PTE)MmpGetPteAddress(ReturnAddress);
        ReturnAddress = (PVOID)(ULONG_PTR)ReturnAddress + MM_PAGE_SIZE;

        /* Check if PTE is valid */
        if(MmpPageMapRoutines->PteValid(PtePointer))
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
    ReturnAddress = (PVOID)(ULONG_PTR)(BaseAddress + PAGE_OFFSET(PhysicalAddress.LowPart));

    /* Check if base address starts at the beginning of the heap */
    if(BaseAddress == MmpHardwareHeapStart)
    {
        /* Move heap beyond base address */
        MmpHardwareHeapStart = (PVOID)((ULONG_PTR)BaseAddress + ((ULONG_PTR)PageCount << MM_PAGE_SHIFT));
    }

    /* Iterate through mapped pages */
    while(MappedPages--)
    {
        /* Get PTE pointer */
        PtePointer = (PHARDWARE_PTE)MmpGetPteAddress(BaseAddress);

        /* Fill the PTE */
        MmpPageMapRoutines->SetPte(PtePointer, (PFN_NUMBER)(PhysicalAddress.QuadPart >> MM_PAGE_SHIFT), TRUE);

        /* Advance to the next address */
        PhysicalAddress.QuadPart += MM_PAGE_SIZE;
        BaseAddress = (PVOID)((ULONG_PTR)BaseAddress + MM_PAGE_SIZE);
    }

    /* Check if TLB needs to be flushed */
    if(FlushTlb)
    {
        /* Flush the TLB */
        MmFlushTlb();
    }

    /* Return virtual address */
    *VirtualAddress = ReturnAddress;
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
MmMarkHardwareMemoryWriteThrough(IN PVOID VirtualAddress,
                                 IN PFN_NUMBER PageCount)
{
    PHARDWARE_PTE PtePointer;
    PFN_NUMBER Page;

    /* Get PTE address from virtual address */
    PtePointer = (PHARDWARE_PTE)MmpGetPteAddress(VirtualAddress);

    /* Iterate through mapped pages */
    for(Page = 0; Page < PageCount; Page++)
    {
        /* Mark pages as CD/WT */
        MmpPageMapRoutines->SetPteCaching(PtePointer, TRUE, TRUE);
        PtePointer++;
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
MmRemapHardwareMemory(IN PVOID VirtualAddress,
                      IN PHYSICAL_ADDRESS PhysicalAddress,
                      IN BOOLEAN FlushTlb)
{
    PHARDWARE_PTE PtePointer;

    /* Get PTE address from virtual address */
    PtePointer = (PHARDWARE_PTE)MmpGetPteAddress(VirtualAddress);

    /* Remap the PTE */
    MmpPageMapRoutines->SetPte(PtePointer, (PFN_NUMBER)(PhysicalAddress.QuadPart >> MM_PAGE_SHIFT), TRUE);

    /* Check if TLB needs to be flushed */
    if(FlushTlb)
    {
        /* Flush the TLB */
        MmFlushTlb();
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
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MmUnmapHardwareMemory(IN PVOID VirtualAddress,
                      IN PFN_NUMBER PageCount,
                      IN BOOLEAN FlushTlb)
{
    PHARDWARE_PTE PtePointer;
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
    PtePointer = (PHARDWARE_PTE)MmpGetPteAddress(VirtualAddress);

    /* Iterate through mapped pages */
    for(Page = 0; Page < PageCount; Page++)
    {
        /* Unmap the PTE and get the next one */
        MmpPageMapRoutines->ClearPte(PtePointer);
        PtePointer++;
    }

    /* Check if TLB needs to be flushed */
    if(FlushTlb)
    {
        /* Flush the TLB */
        MmFlushTlb();
    }

    /* Check if heap can be reused */
    if(MmpHardwareHeapStart > VirtualAddress)
    {
        /* Free VA space */
        MmpHardwareHeapStart = VirtualAddress;
    }

    /* Return success */
    return STATUS_SUCCESS;
}
