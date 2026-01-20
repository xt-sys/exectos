/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/i686/memory.cc
 * DESCRIPTION:     EFI memory management for i686 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Determines the appropriate paging level (PML) for the i686 architecture.
 *
 * @param Parameters
 *        A pointer to the wide character string containing the kernel boot parameters.
 *
 * @return This routine returns the appropriate page map level (3 if PAE is enabled, 2 otherwise).
 *
 * @since XT 1.0
 */
XTCDECL
ULONG
Xtos::DeterminePagingLevel(IN CONST PWCHAR Parameters)
{
    CPUID_REGISTERS CpuRegisters;

    /* Prepare CPUID registers to query for PAE support */
    XtLdrProtocol->Memory.ZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;

    /* Query CPUID */
    XtLdrProtocol->Cpu.CpuId(&CpuRegisters);

    /* Check if eXtended Physical Addressing (XPA) is enabled and if PAE is supported by the CPU */
    if((CpuRegisters.Edx & CPUID_FEATURES_EDX_PAE) &&
       !(XtLdrProtocol->BootUtils.GetBooleanParameter(Parameters, L"NOXPA")))
    {
        /* Enable PAE (PML3) */
        return 3;
    }

    /* Disable PAE and use PML2 by default */
    return 2;
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
Xtos::EnablePaging(IN PXTBL_PAGE_MAPPING PageMap)
{
    EFI_STATUS Status;
    ULONG_PTR SelfMapAddress;

    /* Initialize self map address */
    if(PageMap->PageMapLevel == 3)
    {
        /* For PML3 (PAE) use PTE base address */
        SelfMapAddress = MM_PTE_BASE;
    }
    else
    {
        /* For PML2 (PAE disabled) use legacy PDE base address */
        SelfMapAddress = MM_PDE_LEGACY_BASE;
    }

    /* Build page map */
    Status = XtLdrProtocol->Memory.BuildPageMap(PageMap, SelfMapAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to build page map */
        XtLdrProtocol->Debug.Print(L"Failed to build page map (Status code: %zX)\n", Status);
        return Status;
    }

    /* Map memory for hardware layer */
    Status = MapHardwareMemoryPool(PageMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to map memory for hardware layer */
        XtLdrProtocol->Debug.Print(L"Failed to map memory for hardware layer (Status code: %zX)\n", Status);
        return Status;
    }

    /* Exit EFI Boot Services */
    XtLdrProtocol->Debug.Print(L"Exiting EFI boot services\n");
    Status = XtLdrProtocol->Utils.ExitBootServices();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to exit boot services */
        XtLdrProtocol->Debug.Print(L"Failed to exit boot services (Status code: %zX)\n", Status);
        return STATUS_EFI_ABORTED;
    }

    /* Disable paging */
    XtLdrProtocol->Cpu.WriteControlRegister(0, XtLdrProtocol->Cpu.ReadControlRegister(0) & ~CR0_PG);

    /* Check the configured page map level to set the PAE state accordingly */
    if(PageMap->PageMapLevel == 3)
    {
        /* Enable Physical Address Extension (PAE) */
        XtLdrProtocol->Debug.Print(L"Enabling Physical Address Extension (PAE)\n");
        XtLdrProtocol->Cpu.WriteControlRegister(4, XtLdrProtocol->Cpu.ReadControlRegister(4) | CR4_PAE);
    }
    else
    {
        /* Disable Physical Address Extension (PAE) */
        XtLdrProtocol->Debug.Print(L"Disabling Physical Address Extension (PAE)\n");
        XtLdrProtocol->Cpu.WriteControlRegister(4, XtLdrProtocol->Cpu.ReadControlRegister(4) & ~CR4_PAE);
    }

    /* Write page mappings to CR3 */
    XtLdrProtocol->Cpu.WriteControlRegister(3, (UINT_PTR)PageMap->PtePointer);

    /* Enable paging */
    XtLdrProtocol->Cpu.WriteControlRegister(0, XtLdrProtocol->Cpu.ReadControlRegister(0) | CR0_PG);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

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
Xtos::MapHardwareMemoryPool(IN PXTBL_PAGE_MAPPING PageMap)
{
    EFI_PHYSICAL_ADDRESS Address;
    PHARDWARE_LEGACY_PTE LegacyPdeBase;
    PHARDWARE_MODERN_PTE PdeBase;
    EFI_STATUS Status;

    /* Allocate memory */
    Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, 1, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, return error */
        return Status;
    }

    /* Zero fill allocated memory */
    XtLdrProtocol->Memory.ZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

    /* Check if PAE is enabled (3-level paging) */
    if(PageMap->PageMapLevel == 3)
    {
        /* Get PDE base address (PAE enabled) */
        PdeBase = (PHARDWARE_MODERN_PTE)(((PHARDWARE_MODERN_PTE)PageMap->PtePointer)[MM_HARDWARE_VA_START >> MM_PPI_SHIFT].PageFrameNumber << MM_PAGE_SHIFT);

        /* Make PDE valid */
        XtLdrProtocol->Memory.ZeroMemory(&PdeBase[(MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF], sizeof(HARDWARE_MODERN_PTE));
        PdeBase[(MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF].PageFrameNumber = Address >> MM_PAGE_SHIFT;
        PdeBase[(MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF].Valid = 1;
        PdeBase[(MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF].Writable = 1;
    }
    else
    {
        /* Get PDE base address (PAE disabled) */
        LegacyPdeBase = (PHARDWARE_LEGACY_PTE)PageMap->PtePointer;

        /* Check for a conflicting PDE */
        if(LegacyPdeBase[MM_HARDWARE_VA_START >> MM_PDI_LEGACY_SHIFT].Valid)
        {
            /* PDE already exists and is valid, nothing to do */
            return STATUS_EFI_SUCCESS;
        }

        /* Make PDE valid  */
        XtLdrProtocol->Memory.ZeroMemory(&LegacyPdeBase[MM_HARDWARE_VA_START >> MM_PDI_LEGACY_SHIFT], sizeof(HARDWARE_LEGACY_PTE));
        LegacyPdeBase[MM_HARDWARE_VA_START >> MM_PDI_LEGACY_SHIFT].Valid = 1;
        LegacyPdeBase[MM_HARDWARE_VA_START >> MM_PDI_LEGACY_SHIFT].PageFrameNumber = Address >> MM_PAGE_SHIFT;
        LegacyPdeBase[MM_HARDWARE_VA_START >> MM_PDI_LEGACY_SHIFT].Writable = 1;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
