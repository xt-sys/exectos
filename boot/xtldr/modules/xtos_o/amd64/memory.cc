/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/amd64/memory.cc
 * DESCRIPTION:     EFI memory management for AMD64 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Determines the appropriate paging level (PML) for the AMD64 architecture.
 *
 * @param Parameters
 *        A pointer to the wide character string containing the kernel boot parameters.
 *
 * @return This routine returns the appropriate page map level (5 if LA57 is enabled, 4 otherwise).
 *
 * @since XT 1.0
 */
XTCDECL
ULONG
Xtos::DeterminePagingLevel(IN CONST PWCHAR Parameters)
{
    CPUID_REGISTERS CpuRegisters;

    /* Prepare CPUID registers to query for STD7 features */
    XtLdrProtocol->Memory.ZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_VENDOR_STRING;

    /* Query CPUID */
    XtLdrProtocol->Cpu.CpuId(&CpuRegisters);

    /* Verify if the CPU supports the STD7 feature leaf (0x00000007) */
    if(CpuRegisters.Eax >= CPUID_GET_STANDARD7_FEATURES)
    {
        /* Prepare CPUID registers to query for LA57 support */
        XtLdrProtocol->Memory.ZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
        CpuRegisters.Leaf = CPUID_GET_STANDARD7_FEATURES;

        /* Query CPUID */
        XtLdrProtocol->Cpu.CpuId(&CpuRegisters);

        /* Check if eXtended Physical Addressing (XPA) is enabled and if LA57 is supported by the CPU */
        if((CpuRegisters.Ecx & CPUID_FEATURES_ECX_LA57) &&
           !(XtLdrProtocol->BootUtils.GetBooleanParameter(Parameters, L"NOXPA")))
        {
            /* Enable LA57 (PML5) */
            return 5;
        }
    }

    /* Disable LA57 and use PML4 by default */
    return 4;
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
    EFI_PHYSICAL_ADDRESS TrampolineAddress;
    PXT_TRAMPOLINE_ENTRY TrampolineEntry;
    ULONG_PTR TrampolineSize;
    PVOID TrampolineCode;

    /* Build page map */
    Status = XtLdrProtocol->Memory.BuildPageMap(PageMap, (PageMap->PageMapLevel > 4) ? MM_P5E_LA57_BASE : MM_PXE_BASE);
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
        XtLdrProtocol->Debug.Print(L"Failed to map memory for hardware leyer (Status code: %zX)\n", Status);
        return Status;
    }

    /* Check the configured page map level to set the LA57 state accordingly */
    if(PageMap->PageMapLevel == 5)
    {
        /* Get the trampoline code information */
        XtLdrProtocol->BootUtils.GetTrampolineInformation(TrampolineEnableXpa, &TrampolineCode, &TrampolineSize);
        if(TrampolineCode == NULLPTR || TrampolineSize == 0)
        {
            /* Failed to get trampoline information */
            XtLdrProtocol->Debug.Print(L"Failed to get trampoline information\n");
            return STATUS_EFI_INVALID_PARAMETER;
        }

        /* Set the address of the trampoline code below 1MB */
        TrampolineAddress = MM_TRAMPOLINE_ADDRESS;

        /* Allocate pages for the trampoline */
        Status = XtLdrProtocol->Memory.AllocatePages(AllocateAddress, EFI_SIZE_TO_PAGES(TrampolineSize), &TrampolineAddress);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to allocate memory for trampoline code */
            XtLdrProtocol->Debug.Print(L"Failed to allocate memory for trampoline code (Status code: %zX)\n", Status);
            return Status;
        }

        /* Set the trampoline entry point and copy its code into the allocated buffer */
        TrampolineEntry = (PXT_TRAMPOLINE_ENTRY)(UINT_PTR)TrampolineAddress;
        XtLdrProtocol->Memory.CopyMemory((PVOID)TrampolineEntry, TrampolineCode, TrampolineSize);
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

    /* Check the configured page map level to set the LA57 state accordingly */
    if(PageMap->PageMapLevel == 5)
    {
        /* Enable Linear Address 57-bit (LA57) extension */
        XtLdrProtocol->Debug.Print(L"Enabling Linear Address 57-bit (LA57)\n");

        /* Execute the trampoline to enable LA57 and write PML5 to CR3 */
        TrampolineEntry((UINT64)PageMap->PtePointer);
    }
    else
    {
        /* Disable Linear Address 57-bit (LA57) extension */
        XtLdrProtocol->Debug.Print(L"Disabling Linear Address 57-bit (LA57)\n");

        /* Write PML4 to CR3 and enable paging */
        XtLdrProtocol->Cpu.WriteControlRegister(3, (UINT_PTR)PageMap->PtePointer);
        XtLdrProtocol->Cpu.WriteControlRegister(0, XtLdrProtocol->Cpu.ReadControlRegister(0) | CR0_PG);
    }

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
    PHARDWARE_PTE P5eBase, PdeBase, PpeBase, PxeBase;
    EFI_PHYSICAL_ADDRESS Address;
    EFI_STATUS Status;

    if(PageMap->PageMapLevel == 5)
    {
        /* Get P5E (PML5) base address */
        P5eBase = (PHARDWARE_PTE)PageMap->PtePointer;

        /* Check if P5E entry already exists */
        if(!P5eBase[(MM_HARDWARE_VA_START >> MM_P5I_SHIFT) & 0x1FF].Valid)
        {
            /* No valid P5E, allocate memory */
            Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, 1, &Address);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure, return error */
                return Status;
            }

            /* Zero fill memory used by P5E */
            XtLdrProtocol->Memory.ZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

            /* Make P5E valid */
            P5eBase[(MM_HARDWARE_VA_START >> MM_P5I_SHIFT) & 0x1FF].Valid = 1;
            P5eBase[(MM_HARDWARE_VA_START >> MM_P5I_SHIFT) & 0x1FF].PageFrameNumber = Address / EFI_PAGE_SIZE;
            P5eBase[(MM_HARDWARE_VA_START >> MM_P5I_SHIFT) & 0x1FF].Writable = 1;

            /* Set PXE base address */
            PxeBase = (PHARDWARE_PTE)(UINT_PTR)Address;
        }
        else
        {
            /* Set PXE base address based on existing P5E */
            PxeBase = (PHARDWARE_PTE)((P5eBase[(MM_HARDWARE_VA_START >> MM_P5I_SHIFT) & 0x1FF].PageFrameNumber) << EFI_PAGE_SHIFT);
        }
    }
    else
    {
        /* Get PXE (PML4) base address */
        PxeBase = (PHARDWARE_PTE)PageMap->PtePointer;
    }

    /* Check if PXE entry already exists */
    if(!PxeBase[(MM_HARDWARE_VA_START >> MM_PXI_SHIFT) & 0x1FF].Valid)
    {
        /* No valid PXE, allocate memory */
        Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, 1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure, return error */
            return Status;
        }

        /* Zero fill memory used by PXE */
        XtLdrProtocol->Memory.ZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

        /* Make PXE valid */
        PxeBase[(MM_HARDWARE_VA_START >> MM_PXI_SHIFT) & 0x1FF].Valid = 1;
        PxeBase[(MM_HARDWARE_VA_START >> MM_PXI_SHIFT) & 0x1FF].PageFrameNumber = Address / EFI_PAGE_SIZE;
        PxeBase[(MM_HARDWARE_VA_START >> MM_PXI_SHIFT) & 0x1FF].Writable = 1;

        /* Set PPE base address */
        PpeBase = (PHARDWARE_PTE)(UINT_PTR)Address;
    }
    else
    {
        /* Set PPE base address based on existing PXE */
        PpeBase = (PHARDWARE_PTE)((PxeBase[(MM_HARDWARE_VA_START >> MM_PXI_SHIFT) & 0x1FF].PageFrameNumber) << EFI_PAGE_SHIFT);
    }

    /* Check if PPE entry already exists */
    if(!PpeBase[(MM_HARDWARE_VA_START >> MM_PPI_SHIFT) & 0x1FF].Valid)
    {
        /* No valid PPE, allocate memory */
        Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, 1, &Address);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure, return error */
            return Status;
        }

        /* Zero fill memory used by PPE */
        XtLdrProtocol->Memory.ZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

        /* Make PPE valid */
        PpeBase[(MM_HARDWARE_VA_START >> MM_PPI_SHIFT) & 0x1FF].Valid = 1;
        PpeBase[(MM_HARDWARE_VA_START >> MM_PPI_SHIFT) & 0x1FF].PageFrameNumber = Address / EFI_PAGE_SIZE;
        PpeBase[(MM_HARDWARE_VA_START >> MM_PPI_SHIFT) & 0x1FF].Writable = 1;

        /* Set PDE base address */
        PdeBase = (PHARDWARE_PTE)Address;
    }
    else
    {
        /* Set PDE base address, based on existing PPE */
        PdeBase = (PHARDWARE_PTE)((PpeBase[(MM_HARDWARE_VA_START >> MM_PPI_SHIFT) & 0x1FF].PageFrameNumber) << EFI_PAGE_SHIFT);
    }

    /* Loop through 2 PDE entries */
    for(UINT Index = 0 ; Index < 2 ; Index++)
    {
        /* Check if PDE entry already exists */
        if(!PdeBase[((MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].Valid)
        {
            /* No valid PDE, allocate memory */
            Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, 1, &Address);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure, return error */
                return Status;
            }

            /* Zero fill memory used by PDE */
            XtLdrProtocol->Memory.ZeroMemory((PVOID)Address, EFI_PAGE_SIZE);

            /* Make PDE valid */
            PdeBase[((MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].Valid = 1;
            PdeBase[((MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].PageFrameNumber = Address / EFI_PAGE_SIZE;
            PdeBase[((MM_HARDWARE_VA_START >> MM_PDI_SHIFT) & 0x1FF) + Index].Writable = 1;
        }
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
