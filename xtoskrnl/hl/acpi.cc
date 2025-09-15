/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/acpi.cc
 * DESCRIPTION:     Advanced Configuration and Power Interface (ACPI) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Stores given ACPI table in the kernel local cache.
 *
 * @param AcpiTable
 *        Supplies a pointer to ACPI table that will be stored in the cache.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Acpi::CacheAcpiTable(IN PACPI_DESCRIPTION_HEADER AcpiTable)
{
    PACPI_CACHE_LIST AcpiCache;

    /* Create new ACPI table cache entry */
    AcpiCache = CONTAIN_RECORD(AcpiTable, ACPI_CACHE_LIST, Header);
    RTL::LinkedList::InsertTailList(&CacheList, &AcpiCache->ListEntry);
}

/**
 * Gets a pointer to the ACPI system description pointer (RSDP).
 *
 * @param Rsdp
 *        Supplies a pointer to the memory area, where RSDP virtual address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::GetAcpiSystemDescriptionPointer(OUT PACPI_RSDP *Rsdp)
{
    /* Get RSDP and return success */
    *Rsdp = RsdpStructure;
    return STATUS_SUCCESS;
}

/**
 * Gets an ACPI description table with given signature, available in the XSDT/RSDT.
 *
 * @param Signature
 *        Supplies the signature of the desired ACPI table.
 *
 * @param AcpiTable
 *        Supplies a pointer to memory area where ACPI table virtual address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::GetAcpiTable(IN ULONG Signature,
                       OUT PACPI_DESCRIPTION_HEADER *AcpiTable)
{
    PACPI_DESCRIPTION_HEADER Table;
    XTSTATUS Status;

    /* Assume ACPI table not found */
    *AcpiTable = nullptr;

    /* Attempt to get ACPI table from the cache */
    Status = QueryAcpiCache(Signature, &Table);
    if(Status != STATUS_SUCCESS)
    {
        /* Table not found in the cache, query ACPI tables */
        Status = QueryAcpiTables(Signature, &Table);
        if(Status != STATUS_SUCCESS)
        {
            /* ACPI table not found, return error */
            return STATUS_NOT_FOUND;
        }
    }

    /* Store ACPI table and return success */
    *AcpiTable = Table;
    return STATUS_SUCCESS;
}

/**
 * Performs an initialization of the ACPI subsystem.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::InitializeAcpi(VOID)
{
    PACPI_FADT Fadt;
    XTSTATUS Status;

    /* Initialize ACPI cache */
    Status = InitializeAcpiCache();
    if(Status != STATUS_SUCCESS)
    {
        /* ACPI cache initialization failed, return error */
        return Status;
    }

    /* Get Fixed ACPI Description Table (FADT) */
    Status = GetAcpiTable(ACPI_FADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Fadt);
    if(Status != STATUS_SUCCESS || !Fadt)
    {
        /* Failed to get FADT, return error */
        return STATUS_NOT_FOUND;
    }

    /* Initialize ACPI timer */
    InitializeAcpiTimer();

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the kernel's local ACPI cache storage.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::InitializeAcpiCache(VOID)
{
    PACPI_DESCRIPTION_HEADER Rsdt;
    XTSTATUS Status;

    /* Initialize ACPI cache list */
    RTL::LinkedList::InitializeListHead(&CacheList);

    /* Get XSDT/RSDT */
    Status = InitializeAcpiSystemDescriptionTable(&Rsdt);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to get XSDT/RSDT, return error */
        return Status;
    }

    /* Cache XSDT/RSDT table */
    CacheAcpiTable(Rsdt);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes ACPI System Description Table (XSDT/RSDT) by finding proper table and mapping its virtual address.
 *
 * @param AcpiTable
 *        Supplies a pointer to memory area where ACPI table virtual address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::InitializeAcpiSystemDescriptionTable(OUT PACPI_DESCRIPTION_HEADER *AcpiTable)
{
    PHYSICAL_ADDRESS RsdpAddress, RsdtAddress;
    PSYSTEM_RESOURCE_HEADER ResourceHeader;
    PSYSTEM_RESOURCE_ACPI AcpiResource;
    ULONG RsdtPages;
    PACPI_RSDT Rsdt;
    XTSTATUS Status;

    /* Assume ACPI table not found */
    *AcpiTable = nullptr;

    /* Get ACPI system resource */
    Status = KeGetSystemResource(SystemResourceAcpi, &ResourceHeader);
    if(Status != STATUS_SUCCESS)
    {
        /* Resource not found */
        return STATUS_NOT_FOUND;
    }

    /* Cast system resource to ACPI resource and store RSDP physical address */
    AcpiResource = (PSYSTEM_RESOURCE_ACPI)ResourceHeader;
    RsdpAddress.QuadPart = (LONGLONG)AcpiResource->Header.PhysicalAddress;

    /* Map RSDP and mark it as CD/WT to avoid delays in write-back cache */
    Status = MM::HardwarePool::MapHardwareMemory(RsdpAddress, 1, TRUE, (PVOID *)&RsdpStructure);
    MM::HardwarePool::MarkHardwareMemoryWriteThrough(RsdpStructure, 1);

    /* Validate RSDP signature */
    if(Status != STATUS_SUCCESS || RsdpStructure->Signature != ACPI_RSDP_SIGNATURE)
    {
        /* Not mapped correctly or invalid RSDP signature, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Check RSDP revision to determine RSDT/XSDT address */
    if(RsdpStructure->Revision >= 2)
    {
        /* Get XSDT address */
        RsdtAddress.QuadPart = (LONGLONG)RsdpStructure->XsdtAddress;
    }
    else
    {
        /* Get RSDT address */
        RsdtAddress.QuadPart = (LONGLONG)RsdpStructure->RsdtAddress;
    }

    /* Map RSDT/XSDT as CD/WT */
    Status = MM::HardwarePool::MapHardwareMemory(RsdtAddress, 2, TRUE, (PVOID *)&Rsdt);
    MM::HardwarePool::MarkHardwareMemoryWriteThrough(Rsdt, 2);

    /* Validate RSDT/XSDT signature */
    if((Status != STATUS_SUCCESS) ||
       (Rsdt->Header.Signature != ACPI_RSDT_SIGNATURE &&
        Rsdt->Header.Signature != ACPI_XSDT_SIGNATURE))
    {
        /* Not mapped correctly or invalid RSDT/XSDT signature, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Calculate the length of all available ACPI tables and remap it if needed */
    RsdtPages = ((RsdtAddress.LowPart & (MM_PAGE_SIZE - 1)) + Rsdt->Header.Length + (MM_PAGE_SIZE - 1)) >> MM_PAGE_SHIFT;
    if(RsdtPages != 2)
    {
        /* RSDT/XSDT needs less or more than 2 pages, remap it */
        MM::HardwarePool::UnmapHardwareMemory(Rsdt, 2, TRUE);
        Status = MM::HardwarePool::MapHardwareMemory(RsdtAddress, RsdtPages, TRUE, (PVOID *)&Rsdt);
        MM::HardwarePool::MarkHardwareMemoryWriteThrough(Rsdt, RsdtPages);

        /* Make sure remapping was successful */
        if(Status != STATUS_SUCCESS)
        {
            /* Remapping failed, return error */
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    /* Get ACPI table header and return success */
    *AcpiTable = &Rsdt->Header;
    return STATUS_SUCCESS;
}

/**
 * Initializes System Information structure based on the ACPI provided data.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::InitializeAcpiSystemInformation(VOID)
{
    PACPI_MADT_LOCAL_X2APIC LocalX2Apic;
    PACPI_MADT_LOCAL_APIC LocalApic;
    ULONG_PTR MadtTable;
    PACPI_MADT Madt;
    XTSTATUS Status;
    USHORT CpuCount;

    /* Allocate memory for ACPI system information structure */
    Status = InitializeAcpiSystemStructure();
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to allocate memory, return error */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Get Multiple APIC Description Table (MADT) */
    Status = GetAcpiTable(ACPI_MADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Madt);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to get MADT, return error */
        return STATUS_NOT_FOUND;
    }

    /* Set APIC table traverse pointer and initialize number of CPUs */
    MadtTable = (ULONG_PTR)Madt->ApicTables;
    CpuCount = 0;

    /* Traverse all MADT tables to get system information */
    while(MadtTable <= ((ULONG_PTR)Madt + Madt->Header.Length))
    {
        /* Check if this is a local APIC subtable */
        if((((PACPI_SUBTABLE_HEADER)MadtTable)->Type == ACPI_MADT_TYPE_LOCAL_APIC) &&
           (((PACPI_SUBTABLE_HEADER)MadtTable)->Length == sizeof(ACPI_MADT_LOCAL_APIC)))
        {
            /* Get local APIC subtable */
            LocalApic = (PACPI_MADT_LOCAL_APIC)MadtTable;

            /* Make sure, this CPU can be enabled */
            if(LocalApic->Flags & ACPI_MADT_PLAOC_ENABLED)
            {
                /* Store CPU number, APIC ID and CPU ID */
                SystemInfo.CpuInfo[CpuCount].AcpiId = LocalApic->AcpiId;
                SystemInfo.CpuInfo[CpuCount].ApicId = LocalApic->ApicId;
                SystemInfo.CpuInfo[CpuCount].CpuNumber = CpuCount;

                /* Increment number of CPUs */
                CpuCount++;
            }

            /* Go to the next MADT table */
            MadtTable += ((PACPI_SUBTABLE_HEADER)MadtTable)->Length;
        }
        else if((((PACPI_SUBTABLE_HEADER)MadtTable)->Type == ACPI_MADT_TYPE_LOCAL_X2APIC) &&
                (((PACPI_SUBTABLE_HEADER)MadtTable)->Length == sizeof(ACPI_MADT_LOCAL_X2APIC)))
        {
            /* Get local X2APIC subtable */
            LocalX2Apic = (PACPI_MADT_LOCAL_X2APIC)MadtTable;

            /* Make sure, this CPU can be enabled */
            if(LocalX2Apic->Flags & ACPI_MADT_PLAOC_ENABLED)
            {
                /* Store CPU number, APIC ID and CPU ID */
                SystemInfo.CpuInfo[CpuCount].AcpiId = LocalX2Apic->AcpiId;
                SystemInfo.CpuInfo[CpuCount].ApicId = LocalX2Apic->ApicId;
                SystemInfo.CpuInfo[CpuCount].CpuNumber = CpuCount;

                /* Increment number of CPUs */
                CpuCount++;
            }

            /* Go to the next MADT table */
            MadtTable += ((PACPI_SUBTABLE_HEADER)MadtTable)->Length;
        }
        else
        {
            /* Any other MADT table, try to go to the next one byte-by-byte */
            MadtTable += 1;
        }
    }

    /* Store number of CPUs */
    SystemInfo.CpuCount = CpuCount;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes ACPI System Information data structure based on the size of available ACPI data.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::InitializeAcpiSystemStructure(VOID)
{
    PHYSICAL_ADDRESS PhysicalAddress;
    PFN_NUMBER PageCount;
    ULONG_PTR MadtTable;
    PACPI_MADT Madt;
    XTSTATUS Status;
    ULONG CpuCount;

    /* Get Multiple APIC Description Table (MADT) */
    Status = GetAcpiTable(ACPI_MADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Madt);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to get MADT, return error */
        return STATUS_NOT_FOUND;
    }

    /* Set APIC table traverse pointer and initialize number of CPUs */
    MadtTable = (ULONG_PTR)Madt->ApicTables;
    CpuCount = 0;

    /* Traverse all MADT tables to get number of processors */
    while(MadtTable <= ((ULONG_PTR)Madt + Madt->Header.Length))
    {
        /* Check if this is a local APIC subtable */
        if((((PACPI_SUBTABLE_HEADER)MadtTable)->Type == ACPI_MADT_TYPE_LOCAL_APIC) &&
           (((PACPI_SUBTABLE_HEADER)MadtTable)->Length == sizeof(ACPI_MADT_LOCAL_APIC)))
        {
            /* Make sure, this CPU can be enabled */
            if(((PACPI_MADT_LOCAL_APIC)MadtTable)->Flags & ACPI_MADT_PLAOC_ENABLED)
            {
                /* Increment number of CPUs */
                CpuCount++;
            }

            /* Go to the next MADT table */
            MadtTable += ((PACPI_SUBTABLE_HEADER)MadtTable)->Length;
        }
        else if((((PACPI_SUBTABLE_HEADER)MadtTable)->Type == ACPI_MADT_TYPE_LOCAL_X2APIC) &&
                (((PACPI_SUBTABLE_HEADER)MadtTable)->Length == sizeof(ACPI_MADT_LOCAL_X2APIC)))
        {
            /* Make sure, this CPU can be enabled */
            if(((PACPI_MADT_LOCAL_X2APIC)MadtTable)->Flags & ACPI_MADT_PLAOC_ENABLED)
            {
                /* Increment number of CPUs */
                CpuCount++;
            }

            /* Go to the next MADT table */
            MadtTable += ((PACPI_SUBTABLE_HEADER)MadtTable)->Length;
        }
        else
        {
            /* Any other MADT table, try to go to the next one byte-by-byte */
            MadtTable += 1;
        }
    }

    /* Zero the ACPI system information structure */
    RTL::Memory::ZeroMemory(&SystemInfo, sizeof(ACPI_SYSTEM_INFO));

    /* Calculate number of pages needed to store CPU information */
    PageCount = SIZE_TO_PAGES(CpuCount * sizeof(PROCESSOR_IDENTITY));

    /* Allocate memory for CPU information */
    Status = MM::HardwarePool::AllocateHardwareMemory(PageCount, TRUE, &PhysicalAddress);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to allocate memory, return error */
        return Status;
    }

    /* Map physical address to the virtual memory area */
    Status = MM::HardwarePool::MapHardwareMemory(PhysicalAddress, PageCount, TRUE, (PVOID *)&SystemInfo.CpuInfo);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to map memory, return error */
        return Status;
    }

    /* Zero the CPU information structure */
    RTL::Memory::ZeroMemory(SystemInfo.CpuInfo, PAGES_TO_SIZE(PageCount));

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the ACPI Timer.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::InitializeAcpiTimer(VOID)
{
    PACPI_FADT Fadt;
    XTSTATUS Status;

    /* Get Fixed ACPI Description Table (FADT) */
    Status = GetAcpiTable(ACPI_FADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Fadt);
    if(Status != STATUS_SUCCESS || !Fadt)
    {
        /* Failed to get FADT, return error */
        return STATUS_NOT_FOUND;
    }

    /* Set ACPI timer port address */
    TimerInfo.TimerPort = Fadt->PmTmrBlkIoPort;

    /* Determine whether 32-bit or 24-bit timer is used */
    if(Fadt->Flags & ACPI_FADT_32BIT_TIMER)
    {
        /* 32-bit timer */
        TimerInfo.MsbMask = ACPI_FADT_TIMER_32BIT;
    }
    else
    {
        /* 24-bit timer */
        TimerInfo.MsbMask = ACPI_FADT_TIMER_24BIT;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Queries kernel local ACPI cache in attempt to find a requested ACPI table.
 *
 * @param Signature
 *        Supplies the signature of the desired ACPI table.
 *
 * @param AcpiTable
 *        Supplies a pointer to memory area where ACPI table virtual address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::QueryAcpiCache(IN ULONG Signature,
                         OUT PACPI_DESCRIPTION_HEADER *AcpiTable)
{
    PACPI_DESCRIPTION_HEADER TableHeader;
    PACPI_CACHE_LIST AcpiCache;
    PLIST_ENTRY ListEntry;

    /* Initialize variables */
    TableHeader = nullptr;

    /* Iterate through ACPI tables cache list */
    ListEntry = CacheList.Flink;
    while(ListEntry != &CacheList)
    {
        /* Get cached ACPI table header */
        AcpiCache = CONTAIN_RECORD(ListEntry, ACPI_CACHE_LIST, ListEntry);

        /* Check if ACPI table signature matches */
        if(AcpiCache->Header.Signature == Signature)
        {
            /* ACPI table found in cache, return it */
            TableHeader = &AcpiCache->Header;
            break;
        }

        /* Go to the next cache entry */
        ListEntry = ListEntry->Flink;
    }

    /* Check if the requested ACPI table was found in the cache */
    if(TableHeader == NULL)
    {
        /* ACPI table not found in cache, return error */
        return STATUS_NOT_FOUND;
    }

    /* Return table header and status code */
    *AcpiTable = TableHeader;
    return STATUS_SUCCESS;
}

/**
 * Queries XSDT/RSDT in order to find a requested ACPI table.
 * Once the desired ACPI table is found, it is being mapped and cached.
 *
 * @param Signature
 *        Supplies the signature of the desired ACPI table.
 *
 * @param AcpiTable
 *        Supplies a pointer to memory area where ACPI table virtual address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Acpi::QueryAcpiTables(IN ULONG Signature,
                          OUT PACPI_DESCRIPTION_HEADER *AcpiTable)
{
    ULONG TableCount, TableIndex, TablePages;
    PACPI_DESCRIPTION_HEADER TableHeader;
    PHYSICAL_ADDRESS TableAddress;
    PACPI_FADT Fadt;
    PACPI_RSDT Rsdt;
    PACPI_XSDT Xsdt;
    XTSTATUS Status;

    /* Check if requesting a valid ACPI table */
    if(Signature == ACPI_RSDT_SIGNATURE || Signature == ACPI_XSDT_SIGNATURE)
    {
        /* Cannot provide RSDP/XSDP table, it should be cached; return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Ensure that table header is not set before attempting to find ACPI table */
    TableHeader = nullptr;

    /* Check if DSDT or FACS table requested */
    if(Signature == ACPI_DSDT_SIGNATURE || Signature == ACPI_FACS_SIGNATURE)
    {
        /* Get FADT as it contains a pointer to DSDT and FACS */
        Status = GetAcpiTable(ACPI_FADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Fadt);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to get FADT, return error */
            return Status;
        }

        /* Check if DSDT or FACS table requested */
        if(Signature == ACPI_DSDT_SIGNATURE)
        {
            /* Get DSDT address */
            TableAddress.LowPart = Fadt->Dsdt;
        }
        else
        {
            /* Get FACS address */
            TableAddress.LowPart = Fadt->FirmwareCtrl;
        }

        /* Fill in high part of ACPI table address */
        TableAddress.HighPart = 0;

        /* Map table using hardware memory pool */
        Status = MM::HardwarePool::MapHardwareMemory(TableAddress, 2, TRUE, (PVOID*)&TableHeader);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to map table, return error */
            return Status;
        }
    }
    else
    {
        /* Query cache for XSDP table */
        Status = QueryAcpiCache(ACPI_XSDT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Xsdt);
        if(Status != STATUS_SUCCESS)
        {
            /* XSDP not found, query cache for RSDP table */
            Status = QueryAcpiCache(ACPI_RSDT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Rsdt);
        }

        /* Check if XSDT or RSDT table found */
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to get XSDT/RSDT, return error */
            return Status;
        }

        /* Get table count depending on root table type */
        if(Xsdt != NULL)
        {
            /* Get table count from XSDT */
            TableCount = (Xsdt->Header.Length - sizeof(ACPI_DESCRIPTION_HEADER)) / 8;
        }
        else
        {
            /* Get table count from RSDT */
            TableCount = (Rsdt->Header.Length - sizeof(ACPI_DESCRIPTION_HEADER)) / 4;
        }

        /* Iterate over all ACPI tables */
        for(TableIndex = 0; TableIndex < TableCount; TableIndex++)
        {
            /* Check if XSDP or RSDT is used */
            if(Xsdt != NULL)
            {
                /* Get table header physical address from XSDT */
                TableAddress.QuadPart = Xsdt->Tables[TableIndex];
            }
            else
            {
                /* Get table header physical address from RSDT */
                TableAddress.LowPart = Rsdt->Tables[TableIndex];
                TableAddress.HighPart = 0;
            }

            /* Check whether some table is already mapped */
            if(TableHeader != NULL)
            {
                /* Unmap previous table */
                MM::HardwarePool::UnmapHardwareMemory(TableHeader, 2, TRUE);
            }

            /* Map table using hardware memory pool */
            Status = MM::HardwarePool::MapHardwareMemory(TableAddress, 2, TRUE, (PVOID*)&TableHeader);
            if(Status != STATUS_SUCCESS)
            {
                /* Failed to map table, return error */
                return Status;
            }

            /* Verify table signature */
            if(TableHeader->Signature == Signature)
            {
                /* Found requested ACPI table */
                break;
            }
        }
    }

    /* Make sure table was found */
    if(TableHeader->Signature != Signature)
    {
        /* ACPI table not found, check if cleanup is needed */
        if(TableHeader != NULL)
        {
            /* Unmap non-matching ACPI table */
            MM::HardwarePool::UnmapHardwareMemory(TableHeader, 2, TRUE);
        }

        /* Return error */
        return STATUS_NOT_FOUND;
    }

    /* Don't validate FADT on old, broken firmwares with ACPI 2.0 or older */
    if(TableHeader->Signature != ACPI_FADT_SIGNATURE || TableHeader->Revision > 2)
    {
        /* Validate table checksum */
        if(!ValidateAcpiTable(TableHeader, TableHeader->Length))
        {
            /* Checksum mismatch, unmap table and return error */
            MM::HardwarePool::UnmapHardwareMemory(TableHeader, 2, TRUE);
            return STATUS_CRC_ERROR;
        }
    }

    /* Calculate the length of ACPI table and remap it if needed */
    TablePages = (((ULONG_PTR)TableHeader & (MM_PAGE_SIZE - 1)) + TableHeader->Length + (MM_PAGE_SIZE - 1)) >> MM_PAGE_SHIFT;
    if(TablePages != 2)
    {
        /* ACPI table needs less or more than 2 pages, remap it */
        MM::HardwarePool::UnmapHardwareMemory(TableHeader, 2, FALSE);
        Status = MM::HardwarePool::MapHardwareMemory(TableAddress, TablePages, TRUE, (PVOID*)&TableHeader);

        /* Make sure remapping was successful */
        if(Status != STATUS_SUCCESS)
        {
            /* Remapping failed, return error */
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    /* Mark table as write through and store it in local cache */
    MM::HardwarePool::MarkHardwareMemoryWriteThrough(TableHeader, TablePages);
    CacheAcpiTable(TableHeader);

    /* Store ACPI table and return success */
    *AcpiTable = TableHeader;
    return STATUS_SUCCESS;
}

/**
 * Validates given ACPI table by calculating its checksum.
 *
 * @param Buffer
 *        Supplies a pointer to the table to checksum.
 *
 * @param Size
 *        Supplies the size of the table, in bytes.
 *
 * @return This routine returns TRUE if the table is valid, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
HL::Acpi::ValidateAcpiTable(IN PVOID Buffer,
                            IN UINT_PTR Size)
{
    PUCHAR Pointer;
    UCHAR Sum;

    /* Initialize variables */
    Sum = 0;
    Pointer = (PUCHAR)Buffer;

    /* Calculate checksum of given table */
    while(Size != 0)
    {
        Sum = (UCHAR)(Sum + *Pointer);
        Pointer += 1;
        Size -= 1;
    }

    /* Return calculated checksum */
    return (Sum == 0) ? TRUE : FALSE;
}
