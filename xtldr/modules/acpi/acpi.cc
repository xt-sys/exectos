/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/acpi/acpi.cc
 * DESCRIPTION:     XTLDR ACPI Support Module
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <acpi.hh>


/* ACPI module information */
MODULE_AUTHOR(L"Rafal Kupiec <belliash@codingworkshop.eu.org>");
MODULE_DESCRIPTION(L"ACPI support");
MODULE_LICENSE(L"GPLv3");
MODULE_VERSION(L"0.1");


/**
 * Attempts to get XSDP. If it is not found or checksum mismatch, it will try to get RSDP instead.
 *
 * @param AcpiTable
 *        Suplies a pointer to memory area where XSDP or RSRP address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::GetAcpiDescriptionPointer(OUT PVOID *AcpiTable)
{
    PVOID Rsdp;

    /* Try to get XSDP (ACPI 2.0) from system configuration tables */
    if(GetXsdpTable(&Rsdp) == STATUS_EFI_SUCCESS)
    {
        /* XSDP found, return success */
        *AcpiTable = Rsdp;
        return STATUS_EFI_SUCCESS;
    }

    /* Try to get RSDP (ACPI 1.0) from system configuration tables */
    if(GetRsdpTable(&Rsdp) == STATUS_EFI_SUCCESS)
    {
        /* RSDP found, return success */
        *AcpiTable = Rsdp;
        return STATUS_EFI_SUCCESS;
    }

    /* Neither XSDP nor RSDP found */
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Finds ACPI description table with given signature.
 *
 * @param Signature
 *        Supplies the signature of the desired ACPI table.
 *
 * @param PreviousTable
 *        Supplies a pointer to the table to start searching from.
 *
 * @param AcpiTable
 *        Supplies a pointer to memory area where ACPI table address will be stored, or NULLPTR if not found.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::GetAcpiTable(IN CONST UINT Signature,
                   IN PVOID PreviousTable,
                   OUT PVOID *AcpiTable)
{
    PACPI_DESCRIPTION_HEADER TableHeader;
    SIZE_T RsdtIndex, TableIndex;
    EFI_STATUS Status;
    SIZE_T TableCount;
    PACPI_RSDP Rsdp;
    PACPI_RSDT Rsdt;
    BOOLEAN Xsdp;

    /* Return NULLPTR by default if requested table not found */
    *AcpiTable = NULLPTR;

    /* Get Root System Description Table Pointer */
    Status = GetAcpiDescriptionPointer((PVOID*)&Rsdp);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* ACPI tables not found, return error */
        return Status;
    }

    /* Check if it is XSDP (ACPI 2.0) or RSDP (ACPI 1.0) */
    if(Rsdp->Revision >= 2 && Rsdp->XsdtAddress)
    {
        /* XSDP (ACPI 2.0) */
        Xsdp = TRUE;
        Rsdt = (PACPI_RSDT)(UINT_PTR)Rsdp->XsdtAddress;
        TableCount = (Rsdt->Header.Length - sizeof(ACPI_DESCRIPTION_HEADER)) / 8;
    }
    else
    {
        /* RSDP (ACPI 1.0) */
        Xsdp = FALSE;
        Rsdt = (PACPI_RSDT)(UINT_PTR)Rsdp->RsdtAddress;
        TableCount = (Rsdt->Header.Length - sizeof(ACPI_DESCRIPTION_HEADER)) / 4;
    }

    /* Iterate over all ACPI tables */
    for(TableIndex = 0; TableIndex < TableCount; TableIndex++)
    {
        /* Get table headers in reverse order */
        RsdtIndex = TableCount - TableIndex - 1;

        /* Check if XSDP or RSDT is used */
        if(Xsdp)
        {
            /* Get table header from XSDT */
            TableHeader = (PACPI_DESCRIPTION_HEADER)(ULONG_PTR)((PULONGLONG)Rsdt->Tables)[RsdtIndex];
        }
        else
        {
            /* Get table header from RSDT */
            TableHeader = (PACPI_DESCRIPTION_HEADER)(ULONG_PTR)((PULONG)Rsdt->Tables)[RsdtIndex];
        }

        /* Check if previous table provided */
        if(PreviousTable != NULLPTR)
        {
            /* Check if this is a table previously found */
            if(TableHeader == (PVOID)PreviousTable)
            {
                /* Unset previous table */
                PreviousTable = NULLPTR;
            }

            /* Skip to next ACPI table */
            continue;
        }

        /* Verify table signature */
        if((TableHeader->Signature == Signature))
        {
            /* Found requested ACPI table */
            break;
        }
    }

    /* Make sure table was found */
    if(TableHeader->Signature != Signature)
    {
        /* ACPI table not found, return error */
        return STATUS_EFI_NOT_FOUND;
    }

    /* Don't validate FADT on old, broken firmwares with ACPI 2.0 or older */
    if(TableHeader->Signature != ACPI_FADT_SIGNATURE || TableHeader->Revision > 2)
    {
        /* Validate table checksum */
        if(!ValidateAcpiTable(TableHeader, TableHeader->Length))
        {
            /* Checksum mismatch, return error */
            return STATUS_EFI_CRC_ERROR;
        }
    }

    /* Found valid ACPI table, return success */
    *AcpiTable = TableHeader;
    return STATUS_EFI_SUCCESS;
}

/**
 * Gets the Advanced Programmable Interrupt Controller (APIC) base address.
 *
 * @param ApicBase
 *        Supplies a pointer to memory area where APIC base address will be stored.
 *
 * @return This routine returns an EFI status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::GetApicBase(OUT PVOID *ApicBase)
{
    CPUID_REGISTERS CpuRegisters;

    /* Prepare CPUID registers to query for APIC support */
    XtLdrProtocol->Memory.ZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;

    /* Query CPUID */
    XtLdrProtocol->Cpu.CpuId(&CpuRegisters);

    /* Check if APIC present */
    if((CpuRegisters.Edx & CPUID_FEATURES_EDX_APIC) == 0)
    {
        /* APIC is not supported by the CPU */
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Get APIC base address */
    *ApicBase = (PVOID)((UINT_PTR)XtLdrProtocol->Cpu.ReadModelSpecificRegister(0x1B) & 0xFFFFF000);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Gets RSDP (ACPI 1.0) from EFI system configuration
 *
 * @param AcpiTable
 *        Suplies a pointer to memory area where RSDP address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::GetRsdpTable(OUT PVOID *AcpiTable)
{
    EFI_GUID AcpiGuid = EFI_CONFIG_TABLE_ACPI_TABLE_GUID;
    EFI_STATUS Status;
    PVOID RsdpTable;

    /* Get RSDP (ACPI 1.0) table from system configuration tables */
    Status = XtLdrProtocol->Utils.GetConfigurationTable(&AcpiGuid, &RsdpTable);
    if(Status != STATUS_EFI_SUCCESS || !ValidateAcpiTable(RsdpTable, 20))
    {
        /* RSDP not found or checksum mismatch */
        *AcpiTable = NULLPTR;
        return STATUS_EFI_NOT_FOUND;
    }

    /* RSDP found, return success */
    *AcpiTable = RsdpTable;
    return STATUS_EFI_SUCCESS;
}

/**
 * Gets SMBIOS from EFI system configuration
 *
 * @param SmBiosTable
 *        Suplies a pointer to memory area where SMBIOS address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::GetSMBiosTable(OUT PVOID *SmBiosTable)
{
    EFI_GUID SmBiosGuid = EFI_CONFIG_TABLE_SMBIOS_TABLE_GUID;
    PSMBIOS_TABLE_HEADER SmBios;
    EFI_STATUS Status;

    /* Get SMBIOS table from system configuration tables */
    Status = XtLdrProtocol->Utils.GetConfigurationTable(&SmBiosGuid, (PVOID*)&SmBios);
    if(Status != STATUS_EFI_SUCCESS || !ValidateAcpiTable(SmBios, SmBios->Length))
    {
        /* SMBIOS not found or checksum mismatch */
        *SmBiosTable = NULLPTR;
        return STATUS_EFI_NOT_FOUND;
    }

    /* SMBIOS found, return success */
    *SmBiosTable = SmBios;
    return STATUS_EFI_SUCCESS;
}

/**
 * Gets SMBIOS3 from EFI system configuration
 *
 * @param SmBiosTable
 *        Suplies a pointer to memory area where SMBIOS3 address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::GetSMBios3Table(OUT PVOID *SmBiosTable)
{
    EFI_GUID SmBios3Guid = EFI_CONFIG_TABLE_SMBIOS3_TABLE_GUID;
    PSMBIOS3_TABLE_HEADER SmBios;
    EFI_STATUS Status;

    /* Get SMBIOS3 table from system configuration tables */
    Status = XtLdrProtocol->Utils.GetConfigurationTable(&SmBios3Guid, (PVOID*)&SmBios);
    if(Status != STATUS_EFI_SUCCESS || !ValidateAcpiTable(SmBios, SmBios->Length))
    {
        /* SMBIOS3 not found or checksum mismatch */
        *SmBiosTable = NULLPTR;
        return STATUS_EFI_NOT_FOUND;
    }

    /* SMBIOS3 found, return success */
    *SmBiosTable = SmBios;
    return STATUS_EFI_SUCCESS;
}

/**
 * Gets XSDP (ACPI 2.0) from EFI system configuration
 *
 * @param AcpiTable
 *        Suplies a pointer to memory area where XSDP address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::GetXsdpTable(OUT PVOID *AcpiTable)
{
    EFI_GUID AcpiGuid = EFI_CONFIG_TABLE_ACPI20_TABLE_GUID;
    EFI_STATUS Status;
    PVOID XsdpTable;

    /* Get XSDP (ACPI 2.0) from system configuration tables */
    Status = XtLdrProtocol->Utils.GetConfigurationTable(&AcpiGuid, &XsdpTable);
    if(Status != STATUS_EFI_SUCCESS || !ValidateAcpiTable(XsdpTable, 36))
    {
        /* XSDP not found or checksum mismatch */
        *AcpiTable = NULLPTR;
        return STATUS_EFI_NOT_FOUND;
    }

    /* XSDP found, return success */
    *AcpiTable = XsdpTable;
    return STATUS_EFI_SUCCESS;
}

/**
 * Initializes ACPI module by opening XTLDR protocol and installing ACPI protocol.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
Acpi::InitializeModule(IN EFI_HANDLE ImageHandle,
                       IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_ACPI_PROTOCOL_GUID;
    EFI_STATUS Status;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open the protocol, return error */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set routines available via ACPI protocol */
    AcpiProtocol.GetAcpiDescriptionPointer = GetAcpiDescriptionPointer;
    AcpiProtocol.GetAcpiTable = GetAcpiTable;
    AcpiProtocol.GetApicBase = GetApicBase;
    AcpiProtocol.GetRsdpTable = GetRsdpTable;
    AcpiProtocol.GetSMBiosTable = GetSMBiosTable;
    AcpiProtocol.GetSMBios3Table = GetSMBios3Table;
    AcpiProtocol.GetXsdpTable = GetXsdpTable;

    /* Install ACPI protocol */
    return XtLdrProtocol->Protocol.Install(&AcpiProtocol, &Guid);
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
XTCDECL
BOOLEAN
Acpi::ValidateAcpiTable(IN PVOID Buffer,
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

/**
 * This routine is the entry point of the XT EFI boot loader module.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    /* Initialize ACPI module */
    return Acpi::InitializeModule(ImageHandle, SystemTable);
}
