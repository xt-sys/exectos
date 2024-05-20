/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/acpi/includes/acpi.h
 * DESCRIPTION:     XTLDR ACPI module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_ACPI_ACPI_H
#define __XTLDR_ACPI_ACPI_H

#include <xtblapi.h>
#include <globals.h>


/* ACPI module routines forward references */
XTCDECL
EFI_STATUS
AcGetAcpiDescriptionPointer(OUT PVOID *AcpiTable);

XTCDECL
EFI_STATUS
AcGetAcpiTable(IN CONST UINT Signature,
               IN PVOID PreviousTable,
               OUT PVOID *AcpiTable);

XTCDECL
EFI_STATUS
AcGetApicBase(OUT PVOID *ApicBase);

XTCDECL
EFI_STATUS
AcGetRsdpTable(OUT PVOID *AcpiTable);

XTCDECL
EFI_STATUS
AcGetSMBiosTable(OUT PVOID *SmBiosTable);

XTCDECL
EFI_STATUS
AcGetSMBios3Table(OUT PVOID *SmBiosTable);

XTCDECL
EFI_STATUS
AcGetXsdpTable(OUT PVOID *AcpiTable);

XTCDECL
UCHAR
AcpChecksumTable(IN PVOID Buffer,
                 IN UINT_PTR Size);

XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

#endif/* __XTLDR_ACPI_ACPI_H */
