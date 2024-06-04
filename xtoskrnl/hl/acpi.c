/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/acpi.c
 * DESCRIPTION:     Advanced Configuration and Power Interface (ACPI) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
HlGetAcpiSystemDescriptionPointer(OUT PACPI_RSDP *Rsdp)
{
    /* Get RSDP and return success */
    *Rsdp = HlpAcpiRsdp;
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
HlGetAcpiTable(IN ULONG Signature,
               OUT PACPI_DESCRIPTION_HEADER *AcpiTable)
{
    PACPI_DESCRIPTION_HEADER Table;
    XTSTATUS Status;

    /* Assume ACPI table not found */
    *AcpiTable = NULL;

    /* Attempt to get ACPI table from the cache */
    Status = HlpQueryAcpiCache(Signature, &Table);
    if(Status != STATUS_SUCCESS)
    {
        /* Table not found in the cache, query ACPI tables */
        Status = HlpQueryAcpiTables(Signature, &Table);
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
HlpCacheAcpiTable(IN PACPI_DESCRIPTION_HEADER AcpiTable)
{
    PACPI_CACHE_LIST AcpiCache;

    /* Create new ACPI table cache entry */
    AcpiCache = CONTAIN_RECORD(AcpiTable, ACPI_CACHE_LIST, Header);
    RtlInsertTailList(&HlpAcpiCacheList, &AcpiCache->ListEntry);
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
HlpInitializeAcpi(VOID)
{
    PACPI_FADT Fadt;
    XTSTATUS Status;

    /* Initialize ACPI cache */
    Status = HlpInitializeAcpiCache();
    if(Status != STATUS_SUCCESS)
    {
        /* ACPI cache initialization failed, return error */
        return Status;
    }

    /* Get Fixed ACPI Description Table (FADT) */
    Status = HlGetAcpiTable(ACPI_FADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Fadt);
    if(Status != STATUS_SUCCESS || !Fadt)
    {
        /* Failed to get FADT, return error */
        return STATUS_NOT_FOUND;
    }

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
HlpInitializeAcpiCache(VOID)
{
    PACPI_DESCRIPTION_HEADER Rsdt;
    XTSTATUS Status;

    /* Initialize ACPI cache list */
    RtlInitializeListHead(&HlpAcpiCacheList);

    /* Get XSDT/RSDT */
    Status = HlpInitializeAcpiSystemDescriptionTable(&Rsdt);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to get XSDT/RSDT, return error */
        return Status;
    }

    /* Cache XSDT/RSDT table */
    HlpCacheAcpiTable(Rsdt);

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
HlpInitializeAcpiSystemDescriptionTable(OUT PACPI_DESCRIPTION_HEADER *AcpiTable)
{
    PHYSICAL_ADDRESS RsdpAddress, RsdtAddress;
    PSYSTEM_RESOURCE_HEADER ResourceHeader;
    PSYSTEM_RESOURCE_ACPI AcpiResource;
    ULONG RsdtPages;
    PACPI_RSDT Rsdt;
    XTSTATUS Status;

    /* Assume ACPI table not found */
    *AcpiTable = NULL;

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
    Status = MmMapHardwareMemory(RsdpAddress, 1, TRUE, (PVOID *)&HlpAcpiRsdp);
    MmMarkHardwareMemoryWriteThrough(HlpAcpiRsdp, 1);

    /* Validate RSDP signature */
    if(Status != STATUS_SUCCESS || HlpAcpiRsdp->Signature != ACPI_RSDP_SIGNATURE)
    {
        /* Not mapped correctly or invalid RSDP signature, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Check RSDP revision to determine RSDT/XSDT address */
    if(HlpAcpiRsdp->Revision >= 2)
    {
        /* Get XSDT address */
        RsdtAddress.QuadPart = (LONGLONG)HlpAcpiRsdp->XsdtAddress;
    }
    else
    {
        /* Get RSDT address */
        RsdtAddress.QuadPart = (LONGLONG)HlpAcpiRsdp->RsdtAddress;
    }

    /* Map RSDT/XSDT as CD/WT */
    Status = MmMapHardwareMemory(RsdtAddress, 2, TRUE, (PVOID *)&Rsdt);
    MmMarkHardwareMemoryWriteThrough(Rsdt, 2);

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
        MmUnmapHardwareMemory(Rsdt, 2, TRUE);
        Status = MmMapHardwareMemory(RsdtAddress, RsdtPages, TRUE, (PVOID *)&Rsdt);
        MmMarkHardwareMemoryWriteThrough(Rsdt, RsdtPages);

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
HlpQueryAcpiCache(IN ULONG Signature,
                  OUT PACPI_DESCRIPTION_HEADER *AcpiTable)
{
    PACPI_CACHE_LIST AcpiCache;
    PLIST_ENTRY ListEntry;

    /* Assume ACPI table not cached */
    *AcpiTable = NULL;

    /* Iterate through ACPI tables cache list */
    ListEntry = HlpAcpiCacheList.Flink;
    while(ListEntry != &HlpAcpiCacheList)
    {
        /* Get cached ACPI table header */
        AcpiCache = CONTAIN_RECORD(ListEntry, ACPI_CACHE_LIST, ListEntry);

        /* Check if ACPI table signature matches */
        if(AcpiCache->Header.Signature == Signature)
        {
            /* ACPI table found in cache, return it */
            *AcpiTable = &AcpiCache->Header;
            break;
        }

        /* Go to the next cache entry */
        ListEntry = ListEntry->Flink;
    }

    /* Check if the requested ACPI table was found in the cache */
    if(*AcpiTable == NULL)
    {
        /* ACPI table not found in cache, return error */
        return STATUS_NOT_FOUND;
    }

    /* Return success */
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
HlpQueryAcpiTables(IN ULONG Signature,
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

    /* Check if DSDT or FACS table requested */
    if(Signature == ACPI_DSDT_SIGNATURE || Signature == ACPI_FACS_SIGNATURE)
    {
        /* Get FADT as it contains a pointer to DSDT and FACS */
        Status = HlGetAcpiTable(ACPI_FADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Fadt);
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
        Status = MmMapHardwareMemory(TableAddress, 2, TRUE, (PVOID*)&TableHeader);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to map table, return error */
            return Status;
        }
    }
    else
    {
        /* Query cache for XSDP table */
        Status = HlpQueryAcpiCache(ACPI_XSDT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Xsdt);
        if(Status != STATUS_SUCCESS)
        {
            /* XSDP not found, query cache for RSDP table */
            Status = HlpQueryAcpiCache(ACPI_RSDT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Rsdt);
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
                MmUnmapHardwareMemory(TableHeader, 2, TRUE);
            }

            /* Map table using hardware memory pool */
            Status = MmMapHardwareMemory(TableAddress, 2, TRUE, (PVOID*)&TableHeader);
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
            MmUnmapHardwareMemory(TableHeader, 2, TRUE);
        }

        /* Return error */
        return STATUS_NOT_FOUND;
    }

    /* Don't validate FADT on old, broken firmwares with ACPI 2.0 or older */
    if(TableHeader->Signature != ACPI_FADT_SIGNATURE || TableHeader->Revision > 2)
    {
        /* Validate table checksum */
        if(!HlpValidateAcpiTable(TableHeader, TableHeader->Length))
        {
            /* Checksum mismatch, unmap table and return error */
            MmUnmapHardwareMemory(TableHeader, 2, TRUE);
            return STATUS_CRC_ERROR;
        }
    }

    /* Calculate the length of ACPI table and remap it if needed */
    TablePages = (((ULONG_PTR)TableHeader & (MM_PAGE_SIZE - 1)) + TableHeader->Length + (MM_PAGE_SIZE - 1)) >> MM_PAGE_SHIFT;
    if(TablePages != 2)
    {
        /* ACPI table needs less or more than 2 pages, remap it */
        MmUnmapHardwareMemory(TableHeader, 2, FALSE);
        Status = MmMapHardwareMemory(TableAddress, TablePages, TRUE, (PVOID*)&TableHeader);

        /* Make sure remapping was successful */
        if(Status != STATUS_SUCCESS)
        {
            /* Remapping failed, return error */
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    /* Mark table as write through and store it in local cache */
    MmMarkHardwareMemoryWriteThrough(TableHeader, TablePages);
    HlpCacheAcpiTable(TableHeader);

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
HlpValidateAcpiTable(IN PVOID Buffer,
                     IN UINT_PTR Size)
{
    PUCHAR Pointer;
    UCHAR Sum;

    /* Initialize variables */
    Sum = 0;
    Pointer = Buffer;

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
