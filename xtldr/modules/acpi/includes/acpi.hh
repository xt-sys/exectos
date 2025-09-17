/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/acpi/includes/acpi.hh
 * DESCRIPTION:     XTLDR ACPI module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_ACPI_ACPI_HH
#define __XTLDR_ACPI_ACPI_HH

#include <xtblapi.h>


/* ACPI module for XTLDR */
class Acpi
{
    private:
        STATIC XTBL_ACPI_PROTOCOL AcpiProtocol;
        STATIC PXTBL_LOADER_PROTOCOL XtLdrProtocol;

    public:
        STATIC XTCDECL EFI_STATUS GetAcpiDescriptionPointer(OUT PVOID *AcpiTable);
        STATIC XTCDECL EFI_STATUS GetAcpiTable(IN CONST UINT Signature,
                                               IN PVOID PreviousTable,
                                               OUT PVOID *AcpiTable);
        STATIC XTCDECL EFI_STATUS GetApicBase(OUT PVOID *ApicBase);
        STATIC XTCDECL EFI_STATUS GetRsdpTable(OUT PVOID *AcpiTable);
        STATIC XTCDECL EFI_STATUS GetSMBiosTable(OUT PVOID *SmBiosTable);
        STATIC XTCDECL EFI_STATUS GetSMBios3Table(OUT PVOID *SmBiosTable);
        STATIC XTCDECL EFI_STATUS GetXsdpTable(OUT PVOID *AcpiTable);
        STATIC XTCDECL EFI_STATUS InitializeModule(IN EFI_HANDLE ImageHandle,
                                                   IN PEFI_SYSTEM_TABLE SystemTable);

    private:
        STATIC XTCDECL BOOLEAN ValidateAcpiTable(IN PVOID Buffer,
                                                 IN UINT_PTR Size);
};

#endif /* __XTLDR_ACPI_ACPI_HH */
