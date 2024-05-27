/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/amd64/memory.c
 * DESCRIPTION:     EFI memory management for AMD64 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Maps the page table for hardware layer addess space.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtMapHalMemory(IN PXTBL_PAGE_MAPPING PageMap)
{
    PHARDWARE_PTE PdeBase, PpeBase, PxeBase;
    EFI_PHYSICAL_ADDRESS Address;
    XTSTATUS Status;

    /* Check page map level */
    if(PageMap->PageMapLevel > 4)
    {
        /* PML5 (LA57) is not supported yet */
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Get PXE (PML4) base address */
    PxeBase = ((PHARDWARE_PTE)(PageMap->PtePointer));

    /* Check if PXE entry already exists */
    if(!PxeBase[(MM_HAL_VA_START >> MM_PXI_SHIFT) & 0x1FF].Valid)
    {
        /* No valid PXE, allocate memory */
        Status = XtLdrProtocol->Memory.AllocatePages(1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure, return error */
            return Status;
        }

        /* Zero fill memory used by PXE */
        RtlZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

        /* Make PXE valid */
        PxeBase[(MM_HAL_VA_START >> MM_PXI_SHIFT) & 0x1FF].Valid = 1;
        PxeBase[(MM_HAL_VA_START >> MM_PXI_SHIFT) & 0x1FF].PageFrameNumber = Address / EFI_PAGE_SIZE;
        PxeBase[(MM_HAL_VA_START >> MM_PXI_SHIFT) & 0x1FF].Writable = 1;

        /* Set PPE base address */
        PpeBase = (PHARDWARE_PTE)(UINT_PTR)Address;
    }
    else
    {
        /* Set PPE base address based on existing PXE */
        PpeBase = (PHARDWARE_PTE)((PxeBase[(MM_HAL_VA_START >> MM_PXI_SHIFT) & 0x1FF].PageFrameNumber) << EFI_PAGE_SHIFT);
    }

    /* Check if PPE entry already exists */
    if(!PpeBase[(MM_HAL_VA_START >> MM_PPI_SHIFT) & 0x1FF].Valid)
    {
        /* No valid PPE, allocate memory */
        Status = XtLdrProtocol->Memory.AllocatePages(1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure, return error */
            return Status;
        }

        /* Zero fill memory used by PPE */
        RtlZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

        /* Make PPE valid */
        PpeBase[(MM_HAL_VA_START >> MM_PPI_SHIFT) & 0x1FF].Valid = 1;
        PpeBase[(MM_HAL_VA_START >> MM_PPI_SHIFT) & 0x1FF].PageFrameNumber = Address / EFI_PAGE_SIZE;
        PpeBase[(MM_HAL_VA_START >> MM_PPI_SHIFT) & 0x1FF].Writable = 1;

        /* Set PDE base address */
        PdeBase = (PHARDWARE_PTE)Address;
    }
    else
    {
        /* Set PDE base address, based on existing PPE */
        PdeBase = (PHARDWARE_PTE)((PpeBase[(MM_HAL_VA_START >> MM_PPI_SHIFT) & 0x1FF].PageFrameNumber) << EFI_PAGE_SHIFT);
    }

    /* Loop through 2 PDE entries */
    for(UINT Index = 0 ; Index < 2 ; Index++)
    {
        /* Check if PDE entry already exists */
        if(!PdeBase[((MM_HAL_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].Valid)
        {
            /* No valid PDE, allocate memory */
            Status = XtLdrProtocol->Memory.AllocatePages(1, &Address);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure, return error */
                return Status;
            }

            /* Zero fill memory used by PDE */
            RtlZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

            /* Make PDE valid */
            PdeBase[((MM_HAL_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].Valid = 1;
            PdeBase[((MM_HAL_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].PageFrameNumber = Address / EFI_PAGE_SIZE;
            PdeBase[((MM_HAL_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].Writable = 1;
        }
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Builds the actual memory mapping page table and enables paging. This routine exits EFI boot services as well.
 *
 * @param PageMap
 *        Supplies a pointer to the page mapping structure.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtEnablePaging(IN PXTBL_PAGE_MAPPING PageMap)
{
    EFI_STATUS Status;

    /* Build page map */
    Status = XtLdrProtocol->Memory.BuildPageMap(PageMap, 0xFFFFF6FB7DBED000);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to build page map */
        XtLdrProtocol->Debug.Print(L"Failed to build page map (Status code: %zX)\n", Status);
        return Status;
    }

    /* Map memory for hardware layer */
    Status = XtMapHalMemory(PageMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to map memory for hardware layer */
        XtLdrProtocol->Debug.Print(L"Failed to map memory for hardware leyer (Status code: %zX)\n", Status);
        return Status;
    }

    /* Exit EFI Boot Services */
    XtLdrProtocol->Debug.Print(L"Exiting EFI boot services\n");
    Status = XtLdrProtocol->Util.ExitBootServices();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to exit boot services */
        XtLdrProtocol->Debug.Print(L"Failed to exit boot services (Status code: %zX)\n", Status);
        return STATUS_EFI_ABORTED;
    }

    /* Write PML4 to CR3 */
    ArWriteControlRegister(3, (UINT_PTR)PageMap->PtePointer);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
