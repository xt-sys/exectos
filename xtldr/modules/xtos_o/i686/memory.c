/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/i686/memory.c
 * DESCRIPTION:     EFI memory management for i686 target
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
    EFI_PHYSICAL_ADDRESS Address;
    PHARDWARE_PTE PdeBase;
    XTSTATUS Status;

    /* Allocate memory */
    Status = XtLdrProtocol->Memory.AllocatePages(1, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, return error */
        return Status;
    }

    /* Zero fill allocated memory */
    RtlZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

    /* Check page map level */
    if(PageMap->PageMapLevel == 3)
    {
        /* Get PDE base address (PAE enabled) */
        PdeBase = (PHARDWARE_PTE)(((PHARDWARE_PTE)PageMap->PtePointer)[MM_HAL_VA_START >> MM_PPI_SHIFT].PageFrameNumber << MM_PAGE_SHIFT);

        /* Make PDE valid */
        PdeBase[(MM_HAL_VA_START >> MM_PDI_SHIFT) & 0x1FF].PageFrameNumber = Address >> MM_PAGE_SHIFT;
        PdeBase[(MM_HAL_VA_START >> MM_PDI_SHIFT) & 0x1FF].Valid = 1;
        PdeBase[(MM_HAL_VA_START >> MM_PDI_SHIFT) & 0x1FF].Writable = 1;
    }
    else
    {
        /* Make PDE valid (PAE disabled) */
        ((PHARDWARE_LEGACY_PTE)PageMap->PtePointer)[MM_HAL_VA_START >> MM_PDI_LEGACY_SHIFT].Valid = 1;
        ((PHARDWARE_LEGACY_PTE)PageMap->PtePointer)[MM_HAL_VA_START >> MM_PDI_LEGACY_SHIFT].PageFrameNumber = Address >> MM_PAGE_SHIFT;
        ((PHARDWARE_LEGACY_PTE)PageMap->PtePointer)[MM_HAL_VA_START >> MM_PDI_LEGACY_SHIFT].Writable = 1;
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
    PCPUID_REGISTERS CpuRegisters = NULL;
    EFI_STATUS Status;

    /* Prepare CPUID registers */
    CpuRegisters->Leaf = CPUID_GET_CPU_FEATURES;
    CpuRegisters->SubLeaf = 0;
    CpuRegisters->Eax = 0;
    CpuRegisters->Ebx = 0;
    CpuRegisters->Ecx = 0;
    CpuRegisters->Edx = 0;

    /* Get CPUID */
    ArCpuId(CpuRegisters);

    /* Store PAE status from the CPUID results */
    if(!(CpuRegisters->Edx & CPUID_FEATURES_EDX_PAE))
    {
        /* No PAE support */
        XtLdrProtocol->Debug.Print(L"ERROR: PAE extension not supported by the CPU\n");
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Build page map */
    Status = XtLdrProtocol->Memory.BuildPageMap(PageMap, 0xC0000000);
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

    /* Enable Physical Address Extension (PAE) */
    ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_PAE);

    /* Write page mappings to CR3 */
    ArWriteControlRegister(3, (UINT_PTR)PageMap->PtePointer);

    /* Enable paging */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_PG);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
