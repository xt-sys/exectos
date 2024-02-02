/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/acpi/acpi.c
 * DESCRIPTION:     XTLDR ACPI Support Module
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <acpi.h>


/* Dummy module information */
XTBL_MODINFO = L"ACPI support";

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
AcGetAcpiDescriptionPointer(OUT PVOID *AcpiTable)
{
    PVOID Rsdp;

    /* Try to get XSDP (ACPI 2.0) from system configuration tables */
    if(AcGetXsdpTable(&Rsdp) == STATUS_EFI_SUCCESS)
    {
        /* XSDP found, return success */
        *AcpiTable = Rsdp;
        return STATUS_EFI_SUCCESS;
    }

    /* Try to get RSDP (ACPI 1.0) from system configuration tables */
    if(AcGetRsdpTable(&Rsdp) == STATUS_EFI_SUCCESS)
    {
        /* RSDP found, return success */
        *AcpiTable = Rsdp;
        return STATUS_EFI_SUCCESS;
    }

    /* Neither XSDP nor RSDP found */
    return STATUS_EFI_NOT_FOUND;
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
AcGetRsdpTable(OUT PVOID *AcpiTable)
{
    EFI_GUID AcpiGuid = EFI_CONFIG_TABLE_ACPI_TABLE_GUID;
    EFI_STATUS Status;
    PVOID RsdpTable;

    /* Get RSDP (ACPI 1.0) table from system configuration tables */
    Status = XtLdrProtocol->Util.GetConfigurationTable(&AcpiGuid, &RsdpTable);
    if(Status != STATUS_EFI_SUCCESS || AcpChecksumTable(RsdpTable, 20) != 0)
    {
        /* RSDP not found or checksum mismatch */
        *AcpiTable = NULL;
        return STATUS_EFI_NOT_FOUND;
    }

    /* RSDP found, return success */
    *AcpiTable = RsdpTable;
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
AcGetXsdpTable(OUT PVOID *AcpiTable)
{
    EFI_GUID AcpiGuid = EFI_CONFIG_TABLE_ACPI20_TABLE_GUID;
    EFI_STATUS Status;
    PVOID XsdpTable;

    /* Get XSDP (ACPI 2.0) from system configuration tables */
    Status = XtLdrProtocol->Util.GetConfigurationTable(&AcpiGuid, &XsdpTable);
    if(Status != STATUS_EFI_SUCCESS || AcpChecksumTable(XsdpTable, 36) != 0)
    {
        /* XSDP not found or checksum mismatch */
        *AcpiTable = NULL;
        return STATUS_EFI_NOT_FOUND;
    }

    /* XSDP found, return success */
    *AcpiTable = XsdpTable;
    return STATUS_EFI_SUCCESS;
}

/**
 * Checksums a given ACPI table.
 *
 * @param Buffer
 *        Supplies a pointer to the table to checksum.
 *
 * @param Size
 *        Supplies the size of the table, in bytes.
 *
 * @return This routine returns the calculated checksum.
 */
XTCDECL
UCHAR
AcpChecksumTable(IN PVOID Buffer,
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
    return Sum;
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
    AcpAcpiProtocol.GetAcpiDescriptionPointer = AcGetAcpiDescriptionPointer;
    AcpAcpiProtocol.GetRsdpTable = AcGetRsdpTable;
    AcpAcpiProtocol.GetXsdpTable = AcGetXsdpTable;

    /* Install ACPI protocol */
    return XtLdrProtocol->Protocol.Install(&AcpAcpiProtocol, &Guid);
}
