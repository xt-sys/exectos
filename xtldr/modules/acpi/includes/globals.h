/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/acpi/includes/globals.h
 * DESCRIPTION:     ACPI Module global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_ACPI_GLOBALS_H
#define __XTLDR_ACPI_GLOBALS_H

#include <acpi.h>


/* ACPI Protocol */
EXTERN XTBL_ACPI_PROTOCOL AcpAcpiProtocol;

/* XTLDR protocol handler */
EXTERN PXTBL_LOADER_PROTOCOL XtLdrProtocol;

#endif/* __XTLDR_ACPI_GLOBALS_H */
