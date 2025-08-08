/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/i686/memory.c
 * DESCRIPTION:     EFI memory management for i686 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/**
 * Allocates and maps the 4MB hardware layer memory pool.
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
XtpMapHardwareMemoryPool(IN PXTBL_PAGE_MAPPING PageMap)
{
    EFI_PHYSICAL_ADDRESS Address;
    EFI_STATUS Status;

    /* Allocate a contiguous 4MB block of physical memory */
    Status = XtLdrProtocol->Memory.AllocatePages(MM_HARDWARE_POOL_PAGE_COUNT, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failed, return error */
        return Status;
    }

    /* Map all the memory for the hardware layer */
    Status = XtLdrProtocol->Memory.MapPage(PageMap, MM_HARDWARE_VA_START, Address, MM_HARDWARE_POOL_PAGE_COUNT);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory mapping failed, free memory and return error */
        XtLdrProtocol->Memory.FreePages(MM_HARDWARE_POOL_PAGE_COUNT, Address);
        return Status;
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
    CPUID_REGISTERS CpuRegisters;
    EFI_STATUS Status;

    /* Prepare CPUID registers */
    RtlZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_CPU_FEATURES;

    /* Get CPUID */
    ArCpuId(&CpuRegisters);

    /* Store PAE status from the CPUID results */
    if(!(CpuRegisters.Edx & CPUID_FEATURES_EDX_PAE))
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
    Status = XtpMapHardwareMemoryPool(PageMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to map memory for hardware layer */
        XtLdrProtocol->Debug.Print(L"Failed to map memory for hardware layer (Status code: %zX)\n", Status);
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

    /* Disable paging */
    ArWriteControlRegister(0, ArReadControlRegister(0) & ~CR0_PG);

    /* Check the configured page map level to set the PAE state accordingly */
    if(PageMap->PageMapLevel == 3)
    {
        /* Enable Physical Address Extension (PAE) */
        XtLdrProtocol->Debug.Print(L"Enabling Physical Address Extension (PAE)\n");
        ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_PAE);
    }
    else
    {
        /* Disable Physical Address Extension (PAE) */
        XtLdrProtocol->Debug.Print(L"Disabling Physical Address Extension (PAE)\n");
        ArWriteControlRegister(4, ArReadControlRegister(4) & ~CR4_PAE);
    }

    /* Write page mappings to CR3 */
    ArWriteControlRegister(3, (UINT_PTR)PageMap->PtePointer);

    /* Enable paging */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_PG);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
